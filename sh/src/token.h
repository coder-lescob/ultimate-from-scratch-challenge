#ifndef TOKEN_H
#define TOKEN_H

#include <stddef.h>

enum TokenType {
    TOKEN_NULL,
    TOKEN_ID,
    TOKEN_REDIR_STDOUT,
    TOKEN_REDIR_STDIN,
};

struct Token {
    enum TokenType type;
    char *word;
    size_t len;
};

/**
 * allocates a token
 */
struct Token token_alloc(char *word);

/**
 * free a token
 */
void token_free(struct Token *token);

#endif