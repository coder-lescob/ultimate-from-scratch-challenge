#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

void print_content(char *path) {
    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        fprintf(stderr, "cat: could not open file %s: %s\n", path, strerror(errno));
        return;
    }

    // get file length
    fseek(fp, 0, SEEK_END);
    size_t len = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // +1 for null
    char *file_content = calloc(len + 1, sizeof(char));
    fread(file_content, 1, len, fp);

    // close file
    fclose(fp);
    
    // print file content
    printf("%s", file_content);
}

int main(int argc, char **argv) {
    if (argc <= 1) {
        fprintf(stderr, "cat: no file given\n");
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        print_content(argv[i]);
    }
}