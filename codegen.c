#include "codegen.h"
#include <stdlib.h>
#include <string.h>

CodeGenerator* codegen_create(FILE *output) {
    CodeGenerator *codegen = malloc(sizeof(CodeGenerator));
    codegen->output = output;
    codegen->symbol_table = symbol_table_create();
    codegen->label_counter = 0;
    return codegen;
}

void codegen_free(CodeGenerator *codegen) {
    symbol_table_free(codegen->symbol_table);
    free(codegen);
}

SymbolTable* symbol_table_create() {
    SymbolTable *table = malloc(sizeof(SymbolTable));
    table->symbols = malloc(sizeof(Symbol) * 10);
    table->count = 0;
    table->capacity = 10;
    table->current_offset = 0;
    return table;
}

void symbol_table_free(SymbolTable *table) {
    for (int i = 0; i < table->count; i++) {
        free(table->symbols[i].name);
    }
    free(table->symbols);
    free(table);
}

void symbol_table_add(SymbolTable *table, const char *name, int offset) {
    if (table->count >= table->capacity) {
        table->capacity *= 2;
        table->symbols = realloc(table->symbols, sizeof(Symbol) * table->capacity);
    }
    table->symbols[table->count].name = strdup(name);
    table->symbols[table->count].stack_offset = offset;
    table->count++;
}

Symbol* symbol_table_lookup(SymbolTable *table, const char *name) {
    for (int i = 0; i < table->count; i++) {
        if (strcmp(table->symbols[i].name, name) == 0) {
            return &table->symbols[i];
        }
    }
    return NULL;
}

void codegen_expression(CodeGenerator *codegen, ASTNode *node);
void codegen_statement(CodeGenerator *codegen, ASTNode *node);

void codegen_generate(CodeGenerator *codegen, ASTNode *ast) {
    fprintf(codegen->output, "section .data\n");
    fprintf(codegen->output, "    newline db 10, 0\n");
    fprintf(codegen->output, "    output_buffer db '                    ', 0  ; Buffer for number conversion\n");
    
    fprintf(codegen->output, "\nsection .text\n");
    fprintf(codegen->output, "    global _start\n\n");
    
    // Helper function to print a floating point number
    fprintf(codegen->output, "print_float:\n");
    fprintf(codegen->output, "    ; Simple float printing (prints integer part only for now)\n");
    fprintf(codegen->output, "    cvttsd2si rax, xmm0     ; Convert float to integer\n");
    fprintf(codegen->output, "    \n");
    fprintf(codegen->output, "    ; Convert integer to string\n");
    fprintf(codegen->output, "    mov rdi, output_buffer + 19  ; Point to end of buffer\n");
    fprintf(codegen->output, "    mov byte [rdi], 0       ; Null terminate\n");
    fprintf(codegen->output, "    dec rdi\n");
    fprintf(codegen->output, "    mov rbx, 10\n");
    fprintf(codegen->output, "    \n");
    fprintf(codegen->output, "convert_loop:\n");
    fprintf(codegen->output, "    xor rdx, rdx\n");
    fprintf(codegen->output, "    div rbx\n");
    fprintf(codegen->output, "    add dl, '0'\n");
    fprintf(codegen->output, "    mov [rdi], dl\n");
    fprintf(codegen->output, "    dec rdi\n");
    fprintf(codegen->output, "    test rax, rax\n");
    fprintf(codegen->output, "    jnz convert_loop\n");
    fprintf(codegen->output, "    \n");
    fprintf(codegen->output, "    ; Print the string\n");
    fprintf(codegen->output, "    inc rdi                 ; Point to first digit\n");
    fprintf(codegen->output, "    mov rax, 1              ; sys_write\n");
    fprintf(codegen->output, "    mov rsi, rdi            ; String to print\n");
    fprintf(codegen->output, "    mov rdi, 1              ; stdout\n");
    fprintf(codegen->output, "    mov rdx, output_buffer + 20\n");
    fprintf(codegen->output, "    sub rdx, rsi            ; Calculate length\n");
    fprintf(codegen->output, "    syscall\n");
    fprintf(codegen->output, "    \n");
    fprintf(codegen->output, "    ; Print newline\n"); 
    fprintf(codegen->output, "    mov rax, 1              ; sys_write\n");
    fprintf(codegen->output, "    mov rdi, 1              ; stdout\n");
    fprintf(codegen->output, "    mov rsi, newline        ; newline character\n");
    fprintf(codegen->output, "    mov rdx, 1              ; length\n");
    fprintf(codegen->output, "    syscall\n");
    fprintf(codegen->output, "    ret\n\n");
    
    fprintf(codegen->output, "_start:\n");
    fprintf(codegen->output, "    push rbp\n");
    fprintf(codegen->output, "    mov rbp, rsp\n");
    fprintf(codegen->output, "    sub rsp, 256    ; Reserve stack space for variables\n\n");
    
    // Generate code for all statements
    for (int i = 0; i < ast->data.program.statement_count; i++) {
        codegen_statement(codegen, ast->data.program.statements[i]);
    }
    
    fprintf(codegen->output, "\n    ; Exit program\n");
    fprintf(codegen->output, "    mov rax, 60     ; sys_exit\n");
    fprintf(codegen->output, "    mov rdi, 0      ; exit status\n");
    fprintf(codegen->output, "    syscall\n");
}

void codegen_statement(CodeGenerator *codegen, ASTNode *node) {
    switch (node->type) {
        case AST_VARIABLE_DECLARATION: {
            // Evaluate the expression and store result on stack
            codegen_expression(codegen, node->data.variable_declaration.value);
            
            // Allocate stack space for variable
            codegen->symbol_table->current_offset += 8;
            symbol_table_add(codegen->symbol_table, 
                           node->data.variable_declaration.name, 
                           codegen->symbol_table->current_offset);
            
            fprintf(codegen->output, "    ; Store variable %s\n", 
                   node->data.variable_declaration.name);
            fprintf(codegen->output, "    movsd qword [rbp-%d], xmm0\n\n", 
                   codegen->symbol_table->current_offset);
            break;
        }
        case AST_PRINT_STATEMENT: {
            fprintf(codegen->output, "    ; Print statement\n");
            codegen_expression(codegen, node->data.print_statement.expression);
            
            fprintf(codegen->output, "    ; Call print function\n");
            fprintf(codegen->output, "    call print_float\n\n");
            break;
        }
        case AST_PROGRAM:
        case AST_BINARY_EXPRESSION:
        case AST_IDENTIFIER:
        case AST_NUMBER:
            fprintf(stderr, "Error: Invalid node type for statement: %d\n", node->type);
            exit(1);
    }
}

void codegen_expression(CodeGenerator *codegen, ASTNode *node) {
    switch (node->type) {
        case AST_NUMBER: {
            fprintf(codegen->output, "    ; Load number %g\n", node->data.number.value);
            
            // Create a unique label for this constant
            char label[32];
            snprintf(label, sizeof(label), "float_const_%d", codegen->label_counter++);
            
            // Store the constant in .rodata section (we'll add it at the end)
            // For now, use a temporary approach with manual bit manipulation
            union {
                double d;
                uint64_t i;
            } converter;
            converter.d = node->data.number.value;
            
            fprintf(codegen->output, "    mov rax, 0x%lx    ; Load float bits\n", converter.i);
            fprintf(codegen->output, "    movq xmm0, rax\n");
            break;
        }
        case AST_IDENTIFIER: {
            Symbol *symbol = symbol_table_lookup(codegen->symbol_table, 
                                                node->data.identifier.name);
            if (symbol) {
                fprintf(codegen->output, "    ; Load variable %s\n", node->data.identifier.name);
                fprintf(codegen->output, "    movsd xmm0, qword [rbp-%d]\n", 
                       symbol->stack_offset);
            } else {
                fprintf(stderr, "Error: Undefined variable %s\n", node->data.identifier.name);
                exit(1);
            }
            break;
        }
        case AST_BINARY_EXPRESSION: {
            // Generate code for left operand (result in xmm0)
            codegen_expression(codegen, node->data.binary_expression.left);
            fprintf(codegen->output, "    movsd qword [rsp-8], xmm0  ; Save left operand\n");
            
            // Generate code for right operand (result in xmm0)
            codegen_expression(codegen, node->data.binary_expression.right);
            fprintf(codegen->output, "    movsd xmm1, qword [rsp-8]  ; Restore left operand\n");
            
            // Perform operation
            switch (node->data.binary_expression.operator) {
                case TOKEN_PLUS:
                    fprintf(codegen->output, "    addsd xmm1, xmm0\n");
                    break;
                case TOKEN_MINUS:
                    fprintf(codegen->output, "    subsd xmm1, xmm0\n");
                    break;
                case TOKEN_STAR:
                    fprintf(codegen->output, "    mulsd xmm1, xmm0\n");
                    break;
                case TOKEN_SLASH:
                    fprintf(codegen->output, "    divsd xmm1, xmm0\n");
                    break;
                case TOKEN_LET:
                case TOKEN_PRINT:
                case TOKEN_IDENTIFIER:
                case TOKEN_NUMBER:
                case TOKEN_EQUALS:
                case TOKEN_LPAREN:
                case TOKEN_RPAREN:
                case TOKEN_SEMICOLON:
                case TOKEN_UNKNOWN:
                    fprintf(stderr, "Error: Invalid operator for binary expression: %d\n", 
                           node->data.binary_expression.operator);
                    exit(1);
            }
            fprintf(codegen->output, "    movsd xmm0, xmm1  ; Result in xmm0\n");
            break;
        }
        case AST_PROGRAM:
        case AST_VARIABLE_DECLARATION:
        case AST_PRINT_STATEMENT:
            fprintf(stderr, "Error: Invalid node type for expression: %d\n", node->type);
            exit(1);
    }
}
