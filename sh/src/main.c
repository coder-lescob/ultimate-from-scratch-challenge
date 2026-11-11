#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>

#include "interpreter.h"
#include "lexer.h"
#include "parser.h"
#include "buffer.h"

int main(void) {
    struct RawBuffer buf;
    if (setup_raw_mode(&buf) != 0) {
        perror("sh: could not setup raw mode");
        return -1; // error
    }

    // print a greating message
    printf("Welcome to the ultimate programming challenge!\n\n");
    int last_return_value = 0;
    
    struct InputBuf input;

    while (1) {
        // display the cwd indicator
        display_cwd();
        fflush(stdout);

        // read input
        int success = read_input_buf(&input);
        if (success < 0) {
            perror("sh: could not get input buffer");
            return -1;
        }
        if (success > 0) {
            continue;
        }

        if (input.buf == NULL) continue;

        // interprete it!
        struct Lexer lexer = { input.buf };
        struct Command cmd = parse_cmd(&lexer);
        last_return_value = run_cmd(cmd);

        // reset style to text mode
        printf("\017");
    }
    
    free_buf(&input);
    if (restore_normal_mode(&buf) != 0) {
        perror("sh: could not restore normal mode");
        return -1;
    }

    return 0;
}
