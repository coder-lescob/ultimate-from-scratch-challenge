#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "token.h"
#include <stddef.h>

struct ArgList {
    char **argv;
    int argc;
    int argcapacity;
};

struct Command {

    // command name and arguments
    char *cmd;
    struct ArgList args;

    // redirection:
    // NULL if stdio, otherwise path to file
    char *fstdin; 
    char *fstdout;
};

/**
 * parses a command
 */
struct Command parse_cmd(struct Lexer *lexer);

/**
 * free a command
 */
void free_command(struct Command *cmd);

/**
 * free an arglist
 */
void free_arglist(struct ArgList *arglist);

/**
 * push arg to arg list
 */
int push_arg(struct ArgList *arglist, char *arg, size_t buf_len);

#endif