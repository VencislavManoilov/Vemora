#include "token.h"
#include <stdio.h>

TokenArray* token_array_init(int initial_capacity) {
    TokenArray *array = (TokenArray*)malloc(sizeof(TokenArray));
    array->tokens = (Token*)malloc(sizeof(Token) * initial_capacity);
    array->count = 0;
    array->capacity = initial_capacity;
    return array;
}

void token_array_add(TokenArray *array, Token token) {
    // Resize array if needed
    if (array->count >= array->capacity) {
        array->capacity *= 2;
        array->tokens = (Token*)realloc(array->tokens, sizeof(Token) * array->capacity);
    }
    
    array->tokens[array->count++] = token;
}

void token_array_free(TokenArray *array) {
    // Free string values in tokens
    for (int i = 0; i < array->count; i++) {
        if (array->tokens[i].type == TOKEN_IDENTIFIER || 
            array->tokens[i].type == TOKEN_UNKNOWN) {
            free(array->tokens[i].value.string_value);
        }
    }
    
    free(array->tokens);
    free(array);
}

Token create_keyword_token(TokenType type) {
    Token token;
    token.type = type;
    return token;
}

Token create_identifier_token(const char *identifier) {
    Token token;
    token.type = TOKEN_IDENTIFIER;
    token.value.string_value = strdup(identifier);
    return token;
}

Token create_number_token(double number) {
    Token token;
    token.type = TOKEN_NUMBER;
    token.value.number_value = number;
    return token;
}

Token create_char_token(TokenType type, char value) {
    Token token;
    token.type = type;
    token.value.char_value = value;
    return token;
}

const char* token_type_to_string(TokenType type) {
    switch (type) {
        case TOKEN_LET: return "LET";
        case TOKEN_PRINT: return "PRINT";
        case TOKEN_IDENTIFIER: return "IDENTIFIER";
        case TOKEN_NUMBER: return "NUMBER";
        case TOKEN_EQUALS: return "EQUALS";
        case TOKEN_PLUS: return "PLUS";
        case TOKEN_MINUS: return "MINUS";
        case TOKEN_STAR: return "STAR";
        case TOKEN_SLASH: return "SLASH";
        case TOKEN_LPAREN: return "LPAREN";
        case TOKEN_RPAREN: return "RPAREN";
        case TOKEN_SEMICOLON: return "SEMICOLON";
        case TOKEN_UNKNOWN: return "UNKNOWN";
        default: return "INVALID_TOKEN";
    }
}

char* token_to_string(Token token) {
    char buffer[100];
    
    switch (token.type) {
        case TOKEN_IDENTIFIER:
            sprintf(buffer, "[%s(%s)]", token_type_to_string(token.type), token.value.string_value);
            break;
        case TOKEN_NUMBER:
            sprintf(buffer, "[%s(%g)]", token_type_to_string(token.type), token.value.number_value);
            break;
        case TOKEN_UNKNOWN:
            sprintf(buffer, "[%s(%s)]", token_type_to_string(token.type), token.value.string_value);
            break;
        default:
            sprintf(buffer, "[%s]", token_type_to_string(token.type));
    }
    
    return strdup(buffer);
}