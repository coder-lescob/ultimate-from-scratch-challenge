#include <stdio.h>

int main(int argc, char **argv) {
    printf("Welcome to the ultimate bootstrapping challenge!\n");

    for (int i = 0; i < argc; i++) {
        printf("[ ARG %d ] %s\n", i, argv[i]);
    }

    return 0;
}