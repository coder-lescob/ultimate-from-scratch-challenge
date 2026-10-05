#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv) {
    printf("Welcome to the ultimate bootstrapping challenge!\n\n");

    // show the little indicator
    printf("/ # ");
    fflush(stdout);

    while (1)
        pause();

    return 0;
}