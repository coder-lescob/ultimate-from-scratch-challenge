#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "interpreter.h"
#include "lexer.h"
#include "parser.h"

int main(void) {
    char input[512]; // why 512? no clue
    
    // print a greating message
    printf("Welcome to the ultimate programming challenge!\n\n");
    int last_return_value = 0;

    while (1) {
        
        // display the cwd indicator
        display_cwd();
        fflush(stdout);

        // read input and interprete it!
        fgets(input, sizeof(input) - 1, stdin);
        struct Lexer lexer = { input };
        struct Command cmd = parse_cmd(&lexer);
        last_return_value = run_cmd(cmd);

        // reset style to text mode
        printf("\017");
    }   

    return 0;
}