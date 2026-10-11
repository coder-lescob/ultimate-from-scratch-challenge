#include <stdio.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

int main(int argc, char **argv) {
    if (argc <= 1) {
        fprintf(stderr, "mkdir: missing directories to make\n");
        return 1;
    }

    bool ignore_existing = false;
    bool ignore[argc];

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-p") == 0) {
            ignore[i] = true;
            ignore_existing = true;
        }
        else {
            ignore[i] = false;
        }

        if (strcmp(argv[i], "--version") == 0) {
            printf("mkdir version 0.5 for challenge\n");
            return 0;
        }

        if (strcmp(argv[i], "--help") == 0) {
            printf("mkdir [OPTIONS] DIRECTORIES\nmakes directories with options specified.\n");
            return 0;
        }
    }

    for (int i = 1; i < argc; i++) {
        if (ignore[i]) continue;

        if (mkdir(argv[i], 0) == -1) {
            if (!ignore_existing || errno != EEXIST) {
                perror("mkdir: cannot make directory");
                return 1;
            }
        }
    }

    return 0;
}