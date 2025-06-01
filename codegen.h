#ifndef CODEGEN_H
#define CODEGEN_H

#include "ast.h"
#include <stdio.h>
#include <stdint.h>

// Symbol table entry
typedef struct {
    char *name;
    int stack_offset;
} Symbol;

// Symbol table
typedef struct {
    Symbol *symbols;
    int count;
    int capacity;
    int current_offset;
} SymbolTable;

// Code generator
typedef struct {
    FILE *output;
    SymbolTable *symbol_table;
    int label_counter;
} CodeGenerator;

// Code generator functions
CodeGenerator* codegen_create(FILE *output);
void codegen_free(CodeGenerator *codegen);
void codegen_generate(CodeGenerator *codegen, ASTNode *ast);

// Symbol table functions
SymbolTable* symbol_table_create();
void symbol_table_free(SymbolTable *table);
void symbol_table_add(SymbolTable *table, const char *name, int offset);
Symbol* symbol_table_lookup(SymbolTable *table, const char *name);

#endif // CODEGEN_H
