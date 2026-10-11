#include "token.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * allocates a token
 */
struct Token token_alloc(char *word) {
    if (word == NULL) {
        return (struct Token) { .type = TOKEN_NULL, .word = NULL, .len = 0 };
    }

    if (strcmp(word, ">") == 0) {
        return (struct Token) { .type = TOKEN_REDIR_STDOUT, .word = NULL, .len = 0 };
    }

    if (strcmp(word, "<") == 0) {
        return (struct Token) { .type = TOKEN_REDIR_STDIN, .word = NULL, .len = 0 };
    }

    // allocate and copy
    size_t len       = strlen(word);
    char *word_alloc = calloc(len + 1, sizeof(char)); // +1 for 0 terminator
    
    if (word_alloc == NULL) {
        // allocation failed
        perror("sh: could not allocate token");
        exit(1);
    }
    
    // copy the string inside the allocation
    strncpy(word_alloc, word, len);

    return (struct Token) { .type = TOKEN_ID, .word = word_alloc, .len = len };
}

/**
 * free a token
 */
void token_free(struct Token *token) {
    if (token == NULL) return;
    
    token->type = TOKEN_NULL;
    token->len = 0;
    
    if (token->word != NULL) {
        free(token->word);
        token->word = NULL;
    }
}