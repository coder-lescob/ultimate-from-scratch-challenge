#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <stdio.h>
#include <unistd.h>
#include <stddef.h>

struct ArgList {
    char **argv;
    int argc;
    int argcapacity;
};

/**
 * runs a command in the terminal
 */
int run_cmd(char *command);

/**
 * try to run the command from a binary in /bin
 */
int run_binary(char *cmd, char **lexer);

/**
 * get the next token 0 success -1 overflow or invalid
 */
int get_next_token(char **cursor, char *buf, size_t buf_len);

/**
 * display the current working directory
 */
void display_cwd();

/**
 * get the arguments as a list
 */
int get_arglist(char **lexer, struct ArgList *arglist);

/**
 * free an arglist
 */
void free_arglist(struct ArgList *arglist);

/**
 * push arg to arg list
 */
int push_arg(struct ArgList *arglist, char *arg, size_t buf_len);

/*******************************************************************************
 *                  BUILTIN COMMANDS SUCH AS CD                                *
 *******************************************************************************/

/**
 * change the cwd to the one given in argument
 */
void builtin_run_cd(char **lexer);

/**
 * list the current working directory
 */
void builtin_run_ls(char **lexer);

#endif