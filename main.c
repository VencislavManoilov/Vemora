#include <stdio.h>

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

    char code[100];
    fgets(code, 100, file);

    printf("%s", code);

    fclose(file);

    return 0;
}