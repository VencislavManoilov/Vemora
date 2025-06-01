#include "token.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

TokenArray* tokenize(const char *input) {
    TokenArray *tokens = token_array_init(50);
    int i = 0;
    while (input[i] != '\0') {
        if (isspace(input[i])) {
            i++;
        } else if (isalpha(input[i])) {
            char word[50];
            int j = 0;
            while (isalnum(input[i])) {
                word[j++] = input[i++];
            }
            word[j] = '\0';

            if (strcmp(word, "let") == 0) {
                token_array_add(tokens, create_keyword_token(TOKEN_LET));
            } else if (strcmp(word, "print") == 0) {
                token_array_add(tokens, create_keyword_token(TOKEN_PRINT));
            } else {
                token_array_add(tokens, create_identifier_token(word));
            }
        } else if (isdigit(input[i])) {
            // Read number
            char number[20];
            int j = 0;
            while (isdigit(input[i])) {
                number[j++] = input[i++];
            }
            number[j] = '\0';
            token_array_add(tokens, create_number_token(atof(number)));
        } else {
            // Single-character tokens
            Token token;
            switch (input[i]) {
                case '=':
                    token = create_keyword_token(TOKEN_EQUALS);
                    break;
                case '+':
                    token = create_keyword_token(TOKEN_PLUS);
                    break;
                case '-':
                    token = create_keyword_token(TOKEN_MINUS);
                    break;
                case '*':
                    token = create_keyword_token(TOKEN_STAR);
                    break;
                case '/':
                    token = create_keyword_token(TOKEN_SLASH);
                    break;
                case '(':
                    token = create_keyword_token(TOKEN_LPAREN);
                    break;
                case ')':
                    token = create_keyword_token(TOKEN_RPAREN);
                    break;
                case ';':
                    token = create_keyword_token(TOKEN_SEMICOLON);
                    break;
                default: {
                    char unknown[2] = {input[i], '\0'};
                    token = create_identifier_token(unknown);
                    token.type = TOKEN_UNKNOWN;
                    break;
                }
            }
            token_array_add(tokens, token);
            i++;
        }
    }

    return tokens;
}