#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_SIZE 100

void tokenize(const char *input) {
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
                printf("[LET] ");
            } else if (strcmp(word, "print") == 0) {
                printf("[PRINT] ");
            } else {
                printf("[IDENTIFIER(%s)] ", word);
            }
        } else if (isdigit(input[i])) {
            // Read number
            char number[20];
            int j = 0;
            while (isdigit(input[i])) {
                number[j++] = input[i++];
            }
            number[j] = '\0';
            printf("[NUMBER(%s)] ", number);
        } else {
            // Single-character tokens
            switch (input[i]) {
                case '=': printf("[EQUALS] "); break;
                case '+': printf("[PLUS] "); break;
                case '-': printf("[MINUS] "); break;
                case '*': printf("[STAR] "); break;
                case '/': printf("[SLASH] "); break;
                case '(': printf("[LPAREN] "); break;
                case ')': printf("[RPAREN] "); break;
                case ';': printf("[SEMICOLON] "); break;
                default:  printf("[UNKNOWN(%c)] ", input[i]); break;
            }
            i++;
        }
    }

    printf("\n");
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
    tokenize(data);

    fclose(file);

    return 0;
}