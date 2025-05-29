#include <stdio.h>

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

    printf("%s", data);

    fclose(file);

    return 0;
}