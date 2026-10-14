#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

static int is_blank(char c) {
    return c == ' ' || c == '\n' || c == '\t';
}

/**
 * get the new token from the cursor's stream
 */
struct Token lexer_next_token(struct Lexer *lexer) {
    if (lexer == NULL) {
        // return NULL token
        return token_alloc(NULL);
    }

    if (is_blank(*lexer->cursor)) {
        while (*lexer->cursor != 0 && is_blank(*(++lexer->cursor)));
    }

    if (*lexer->cursor == 0) {
        // end of stream
        return token_alloc(NULL);
    }

    size_t buf_len = 512;
    char *buf = calloc(buf_len + 1, sizeof(char));
    bool quote_opened = false;
    bool escaped      = false;

    for (size_t i = 0; *lexer->cursor != 0 && (!is_blank(*lexer->cursor) || quote_opened); i++, lexer->cursor++) {
        if (*lexer->cursor == '\\') {
            escaped = true;
            lexer->cursor++;
        }
        
        if (*lexer->cursor == '"' && !escaped) {
            quote_opened = !quote_opened;
            lexer->cursor++;
        }
        
        if (i >= buf_len) {
            // we shall reallocate buffer
            size_t new_size = 2 * buf_len + 1;
            char *new_buf = realloc(buf, new_size + 1);

            if (new_buf == NULL) {
                // reallocation failed
                perror("sh: failed to tokenize");
                exit(1);
            }

            buf_len = new_size;
            buf = new_buf;
        }

        // push to buffer
        buf[i] = *lexer->cursor;
        escaped = false;
    }

    if (is_blank(*lexer->cursor)) {
        while (*lexer->cursor != 0 && is_blank(*(++lexer->cursor)));
    }

    // create the token
    struct Token token = token_alloc(buf);

    // free the temporary buffer
    free(buf); 
    buf = NULL;

    return token;
}