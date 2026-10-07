#include <stdio.h>
#include <sys/stat.h>

int main(int argc, char **argv) {
    if (argc <= 1) {
        fprintf(stderr, "mkdir: missing directory to make\n");
        return 1;
    }

    if (mkdir(argv[1], 0) == -1) {
        perror("mkdir: cannot make directory");
        return 1;
    }

    return 0;
}