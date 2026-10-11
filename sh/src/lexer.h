#ifndef LEXER_H
#define LEXER_H

#include "token.h"

// a lexer consist of a cursor
struct Lexer {
    char *cursor;
};

/**
 * get the new token from the cursor's stream
 */
struct Token lexer_next_token(struct Lexer *lexer);

#endif