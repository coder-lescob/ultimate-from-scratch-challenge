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

    int ch;
    while ((ch = fgetc(fp)) != EOF) {
        putc(ch, stdout);
    }

    // close file
    fclose(fp);
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