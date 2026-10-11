#include "parser.h"
#include "lexer.h"
#include "token.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

/**
 * parses a command
 */
struct Command parse_cmd(struct Lexer *lexer) {
    if (lexer == NULL) {
        perror("sh: NULL lexer");
        exit(1);
    }

    struct Token cmd_name = lexer_next_token(lexer);
    struct ArgList arglist = { 0 };

    // push the cmd name
    push_arg(&arglist, cmd_name.word, strlen(cmd_name.word));

    char *fstdin = NULL, *fstdout = NULL;
    struct Token token;
    while (1) {
        // get next token
        token = lexer_next_token(lexer);

        if (token.type == TOKEN_NULL) {
            // end of stream
            break;
        }

        switch (token.type) {
            case TOKEN_REDIR_STDIN:
                // which file? 
                // if eos then here it's going to say NULL which will be interpreted as stdio
                token = lexer_next_token(lexer);

                // please don't free that token we need it!
                fstdin = token.word;
                break;
            case TOKEN_REDIR_STDOUT:
                // which file? 
                // if eos then here it's going to say NULL which will be interpreted as stdio
                token = lexer_next_token(lexer);

                // please don't free that token we need it!
                fstdout = token.word;
                break;
            default:
                // just normal argument...
                push_arg(&arglist, token.word, strlen(token.word) + 1); // +1 for null
                token_free(&token); // token has been copied to arglist free it!
                break;
        }
    }

    // return syntax
    return (struct Command) { cmd_name.word, arglist, fstdin, fstdout };
}

/**
 * free a command
 */
void free_command(struct Command *cmd) {
    if (cmd == NULL) return;

    if (cmd->cmd != NULL) {
        free(cmd->cmd);
        cmd->cmd = NULL;
    }

    free_arglist(&cmd->args);

    if (cmd->fstdin != NULL) {
        free(cmd->fstdin);
        cmd->fstdin = NULL;
    }

    if (cmd->fstdout != NULL) {
        free(cmd->fstdout);
        cmd->fstdout = NULL;
    }
}

static int realloc_arglist(struct ArgList *arglist, int new_argcapacity) {
    if (arglist == NULL || new_argcapacity <= arglist->argcapacity) { 
        errno = EINVAL;
        return -1; 
    }

    // try reallocating
    int new_capacity = 2 * arglist->argcapacity + 1; // +1 to avoid the problem with 0 
    char **new_argv = realloc(arglist->argv, new_capacity * sizeof(char *));

    if (new_argv == NULL) {
        return -1;
    }

    // success!!
    arglist->argv = new_argv;
    arglist->argcapacity = new_argcapacity;

    return 0;
}

void free_arglist(struct ArgList *arglist) {
    if (arglist == NULL) return;

    for (int i = 0; i < arglist->argc; i++) {
        if (arglist->argv[i] != NULL) {
            free(arglist->argv[i]);
            arglist->argv[i] = NULL;
        }
    }

    if (arglist->argv != NULL) {
        free(arglist->argv);
        arglist->argv = NULL;
    }

    arglist->argc        = 0;
    arglist->argcapacity = 0;
}

int push_arg(struct ArgList *arglist, char *buf, size_t buf_len) {
    if (arglist->argc + 1 > arglist->argcapacity) {
        if (realloc_arglist(arglist, arglist->argcapacity * 2 + 1) != 0) {
            perror("sh: could not reallocate arglist");
            exit(1);
        }
    }

    // push it to the argv
    if (buf == NULL) {
        arglist->argv[arglist->argc++] = NULL;
        return 0;
    }

    // alloc arg
    char *arg = calloc(buf_len, sizeof(char));
    if (arg == NULL) {
        perror("sh: could not push arg to arglist");
        exit(1);
    }
    strncpy(arg, buf, buf_len-1);
    
    // push it!
    arglist->argv[arglist->argc++] = arg;
    return 0;
}