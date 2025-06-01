#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "token.h"

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