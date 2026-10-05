#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    printf("Welcome to the ultimate bootstrapping challenge!\n\n");

    char input[512]; // why 512? no clue

    while (1) {

        // show the little indicator
        printf("/ # ");
        fflush(stdout);

        // read input TODO: interprete it!
        fgets(input, sizeof(input) - 1, stdin);
        printf("%s\n", input);
    }   

    return 0;
}