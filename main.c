#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "token.h"
#include "ast.h"
#include "codegen.h"

#define MAX_SIZE 100

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("No file selected\n");
        return 1;
    }
    
    char *file_path = argv[1];
    
    // Now you can use file_path to open and process the file
    FILE *file = fopen(file_path, "r");
    if (file == NULL) {
        printf("Error opening file\n");
        return 1;
    }

    char data[MAX_SIZE];
    int n = fread(data, 1, MAX_SIZE - 1, file);

    data[n] = '\0';
    printf("\nSource Code:\n%s\n", data);
    
    printf("\nv v v\n");
    
    printf("\nTokens:\n");
    TokenArray* tokens = tokenize(data);
    for (int i = 0; i < tokens->count; i++) {
        Token token = tokens->tokens[i];
        char *token_str = token_to_string(token);
        printf("%s ", token_str);
        free(token_str);
    }
    printf("\n");
    
    printf("\nv v v\n");

    printf("\nAST:\n");
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parse_program(parser);
    ast_print(ast, 0);

    printf("\nv v v\n");

    // Generate assembly
    printf("\nGenerating assembly...\n");
    FILE *asm_file = fopen("output.asm", "w");
    if (asm_file == NULL) {
        printf("Error creating assembly file\n");
        return 1;
    }
    
    CodeGenerator *codegen = codegen_create(asm_file);
    codegen_generate(codegen, ast);
    fclose(asm_file);
    
    printf("Assembly generated in output.asm\n");
    // Compile the generated assembly
    printf("Compiling the assembly...\n");
    int nasm_result = system("nasm -f elf64 output.asm -o output.o");
    if (nasm_result != 0) {
        printf("Error running nasm command\n");
        return 1;
    }
    
    int ld_result = system("ld output.o -o output");
    if (ld_result != 0) {
        printf("Error running ld command\n");
        return 1;
    }
    
    printf("Compilation successful. Executable created as 'output'\n");

    // Cleanup
    codegen_free(codegen);
    ast_node_free(ast);
    parser_free(parser);
    token_array_free(tokens);
    fclose(file);

    return 0;
}