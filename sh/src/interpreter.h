#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "parser.h"

#include <stdio.h>
#include <unistd.h>
#include <stddef.h>

/**
 * runs a command in the terminal
 */
int run_cmd(struct Command command);

/**
 * try to run the command from a binary in /bin
 */
int run_binary(struct Command *cmd);

/**
 * display the current working directory
 */
void display_cwd();

/*******************************************************************************
 *                  BUILTIN COMMANDS SUCH AS CD                                *
 *******************************************************************************/

/**
 * change the cwd to the one given in argument
 */
void builtin_run_cd(struct Command *cmd);

/**
 * list the current working directory
 */
void builtin_run_ls(struct Command *cmd);

#endif