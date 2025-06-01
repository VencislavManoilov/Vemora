#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ASTNode* ast_node_create(ASTNodeType type) {
    ASTNode *node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = type;
    memset(&node->data, 0, sizeof(node->data));
    return node;
}

void ast_node_free(ASTNode *node) {
    if (!node) return;
    
    switch (node->type) {
        case AST_PROGRAM:
            for (int i = 0; i < node->data.program.statement_count; i++) {
                ast_node_free(node->data.program.statements[i]);
            }
            free(node->data.program.statements);
            break;
        case AST_VARIABLE_DECLARATION:
            free(node->data.variable_declaration.name);
            ast_node_free(node->data.variable_declaration.value);
            break;
        case AST_PRINT_STATEMENT:
            ast_node_free(node->data.print_statement.expression);
            break;
        case AST_BINARY_EXPRESSION:
            ast_node_free(node->data.binary_expression.left);
            ast_node_free(node->data.binary_expression.right);
            break;
        case AST_IDENTIFIER:
            free(node->data.identifier.name);
            break;
        case AST_NUMBER:
            break;
    }
    free(node);
}

void ast_print(ASTNode *node, int indent) {
    if (!node) return;
    
    for (int i = 0; i < indent; i++) printf("  ");
    
    switch (node->type) {
        case AST_PROGRAM:
            printf("Program\n");
            for (int i = 0; i < node->data.program.statement_count; i++) {
                ast_print(node->data.program.statements[i], indent + 1);
            }
            break;
        case AST_VARIABLE_DECLARATION:
            printf("VariableDeclaration: %s\n", node->data.variable_declaration.name);
            ast_print(node->data.variable_declaration.value, indent + 1);
            break;
        case AST_PRINT_STATEMENT:
            printf("PrintStatement\n");
            ast_print(node->data.print_statement.expression, indent + 1);
            break;
        case AST_BINARY_EXPRESSION:
            printf("BinaryExpression: %s\n", token_type_to_string(node->data.binary_expression.operator));
            ast_print(node->data.binary_expression.left, indent + 1);
            ast_print(node->data.binary_expression.right, indent + 1);
            break;
        case AST_IDENTIFIER:
            printf("Identifier: %s\n", node->data.identifier.name);
            break;
        case AST_NUMBER:
            printf("Number: %g\n", node->data.number.value);
            break;
    }
}

Parser* parser_create(TokenArray *tokens) {
    Parser *parser = (Parser*)malloc(sizeof(Parser));
    parser->tokens = tokens;
    parser->current = 0;
    return parser;
}

void parser_free(Parser *parser) {
    free(parser);
}

Token parser_current_token(Parser *parser) {
    if (parser->current >= parser->tokens->count) {
        Token eof_token;
        eof_token.type = TOKEN_UNKNOWN;
        return eof_token;
    }
    return parser->tokens->tokens[parser->current];
}

Token parser_consume(Parser *parser, TokenType expected) {
    Token token = parser_current_token(parser);
    if (token.type == expected) {
        parser->current++;
        return token;
    } else {
        printf("Parser error: Expected %s but got %s\n", 
               token_type_to_string(expected), token_type_to_string(token.type));
        exit(1);
    }
}

int parser_match(Parser *parser, TokenType type) {
    if (parser_current_token(parser).type == type) {
        parser->current++;
        return 1;
    }
    return 0;
}

ASTNode* parse_program(Parser *parser) {
    ASTNode *program = ast_node_create(AST_PROGRAM);
    program->data.program.statements = (ASTNode**)malloc(sizeof(ASTNode*) * 10);
    program->data.program.statement_count = 0;
    program->data.program.statement_capacity = 10;
    
    while (parser->current < parser->tokens->count) {
        ASTNode *stmt = parse_statement(parser);
        if (stmt) {
            if (program->data.program.statement_count >= program->data.program.statement_capacity) {
                program->data.program.statement_capacity *= 2;
                program->data.program.statements = (ASTNode**)realloc(
                    program->data.program.statements, 
                    sizeof(ASTNode*) * program->data.program.statement_capacity);
            }
            program->data.program.statements[program->data.program.statement_count++] = stmt;
        }
    }
    
    return program;
}

ASTNode* parse_statement(Parser *parser) {
    Token current = parser_current_token(parser);
    
    if (current.type == TOKEN_LET) {
        return parse_variable_declaration(parser);
    } else if (current.type == TOKEN_PRINT) {
        return parse_print_statement(parser);
    }
    
    return NULL;
}

ASTNode* parse_variable_declaration(Parser *parser) {
    parser_consume(parser, TOKEN_LET);
    Token name_token = parser_consume(parser, TOKEN_IDENTIFIER);
    parser_consume(parser, TOKEN_EQUALS);
    ASTNode *value = parse_expression(parser);
    parser_consume(parser, TOKEN_SEMICOLON);
    
    ASTNode *var_decl = ast_node_create(AST_VARIABLE_DECLARATION);
    var_decl->data.variable_declaration.name = strdup(name_token.value.string_value);
    var_decl->data.variable_declaration.value = value;
    
    return var_decl;
}

ASTNode* parse_print_statement(Parser *parser) {
    parser_consume(parser, TOKEN_PRINT);
    parser_consume(parser, TOKEN_LPAREN);
    ASTNode *expression = parse_expression(parser);
    parser_consume(parser, TOKEN_RPAREN);
    parser_consume(parser, TOKEN_SEMICOLON);
    
    ASTNode *print_stmt = ast_node_create(AST_PRINT_STATEMENT);
    print_stmt->data.print_statement.expression = expression;
    
    return print_stmt;
}

ASTNode* parse_expression(Parser *parser) {
    ASTNode *left = parse_term(parser);
    
    while (parser_current_token(parser).type == TOKEN_PLUS || 
           parser_current_token(parser).type == TOKEN_MINUS) {
        TokenType op = parser_current_token(parser).type;
        parser->current++;
        ASTNode *right = parse_term(parser);
        
        ASTNode *binary = ast_node_create(AST_BINARY_EXPRESSION);
        binary->data.binary_expression.left = left;
        binary->data.binary_expression.right = right;
        binary->data.binary_expression.operator = op;
        left = binary;
    }
    
    return left;
}

ASTNode* parse_term(Parser *parser) {
    ASTNode *left = parse_factor(parser);
    
    while (parser_current_token(parser).type == TOKEN_STAR || 
           parser_current_token(parser).type == TOKEN_SLASH) {
        TokenType op = parser_current_token(parser).type;
        parser->current++;
        ASTNode *right = parse_factor(parser);
        
        ASTNode *binary = ast_node_create(AST_BINARY_EXPRESSION);
        binary->data.binary_expression.left = left;
        binary->data.binary_expression.right = right;
        binary->data.binary_expression.operator = op;
        left = binary;
    }
    
    return left;
}

ASTNode* parse_factor(Parser *parser) {
    Token current = parser_current_token(parser);
    
    if (current.type == TOKEN_NUMBER) {
        parser->current++;
        ASTNode *number = ast_node_create(AST_NUMBER);
        number->data.number.value = current.value.number_value;
        return number;
    } else if (current.type == TOKEN_IDENTIFIER) {
        parser->current++;
        ASTNode *identifier = ast_node_create(AST_IDENTIFIER);
        identifier->data.identifier.name = strdup(current.value.string_value);
        return identifier;
    } else if (current.type == TOKEN_LPAREN) {
        parser->current++;
        ASTNode *expression = parse_expression(parser);
        parser_consume(parser, TOKEN_RPAREN);
        return expression;
    }
    
    printf("Parser error: Unexpected token %s\n", token_type_to_string(current.type));
    exit(1);
}
