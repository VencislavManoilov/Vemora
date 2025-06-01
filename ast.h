#ifndef AST_H
#define AST_H

#include "token.h"

// AST Node types
typedef enum {
    AST_PROGRAM,
    AST_VARIABLE_DECLARATION,
    AST_PRINT_STATEMENT,
    AST_BINARY_EXPRESSION,
    AST_IDENTIFIER,
    AST_NUMBER
} ASTNodeType;

// Forward declaration
struct ASTNode;

// AST Node structure
typedef struct ASTNode {
    ASTNodeType type;
    union {
        struct {
            struct ASTNode **statements;
            int statement_count;
            int statement_capacity;
        } program;
        
        struct {
            char *name;
            struct ASTNode *value;
        } variable_declaration;
        
        struct {
            struct ASTNode *expression;
        } print_statement;
        
        struct {
            struct ASTNode *left;
            struct ASTNode *right;
            TokenType operator;
        } binary_expression;
        
        struct {
            char *name;
        } identifier;
        
        struct {
            double value;
        } number;
    } data;
} ASTNode;

// Parser structure
typedef struct {
    TokenArray *tokens;
    int current;
} Parser;

// AST functions
ASTNode* ast_node_create(ASTNodeType type);
void ast_node_free(ASTNode *node);
void ast_print(ASTNode *node, int indent);

// Parser functions
Parser* parser_create(TokenArray *tokens);
void parser_free(Parser *parser);
ASTNode* parse_program(Parser *parser);
ASTNode* parse_statement(Parser *parser);
ASTNode* parse_variable_declaration(Parser *parser);
ASTNode* parse_print_statement(Parser *parser);
ASTNode* parse_expression(Parser *parser);
ASTNode* parse_term(Parser *parser);
ASTNode* parse_factor(Parser *parser);

// Helper functions
Token parser_current_token(Parser *parser);
Token parser_consume(Parser *parser, TokenType expected);
int parser_match(Parser *parser, TokenType type);

#endif // AST_H
