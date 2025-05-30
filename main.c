#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "token.h"

#define MAX_SIZE 100

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
                default:
                    char unknown[2] = {input[i], '\0'};
                    token = create_identifier_token(unknown);
                    token.type = TOKEN_UNKNOWN;
                    break;
            }
            token_array_add(tokens, token);
            i++;
        }
    }

    return tokens;
}

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
    printf("Source Code:\n%s\n", data);
    
    printf("\nTokens\n");
    TokenArray* tokens = tokenize(data);
    for (int i = 0; i < tokens->count; i++) {
        Token token = tokens->tokens[i];
        char *token_str = token_to_string(token);
        printf("%s ", token_str);
        free(token_str);
    }

    printf("\n");

    token_array_free(tokens);

    fclose(file);

    return 0;
}