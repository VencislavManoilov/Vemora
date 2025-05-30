#ifndef TOKEN_H
#define TOKEN_H

#include <stdlib.h>
#include <string.h>

// Token types
typedef enum {
    TOKEN_LET,
    TOKEN_PRINT,
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_EQUALS,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_STAR,
    TOKEN_SLASH,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_SEMICOLON,
    TOKEN_UNKNOWN
} TokenType;

// Token structure
typedef struct {
    TokenType type;
    union {
        char *string_value;  // For identifiers and unknown tokens
        double number_value; // For numbers
        char char_value;     // For single-character tokens
    } value;
} Token;

// Token array
typedef struct {
    Token *tokens;
    int count;
    int capacity;
} TokenArray;

// Initialize token array
TokenArray* token_array_init(int initial_capacity);

// Add token to array
void token_array_add(TokenArray *array, Token token);

// Free token array and its contents
void token_array_free(TokenArray *array);

// Helper functions to create different types of tokens
Token create_keyword_token(TokenType type);
Token create_identifier_token(const char *identifier);
Token create_number_token(double number);
Token create_char_token(TokenType type, char value);

// Get string representation of token type
const char* token_type_to_string(TokenType type);

// Get string representation of token (for debugging)
char* token_to_string(Token token);

#endif // TOKEN_H