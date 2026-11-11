#include "buffer.h"
#include <sys/ioctl.h>
#include <asm/termbits.h>
#include <asm/ioctls.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

int setup_raw_mode(struct RawBuffer *buf) {

    // read the original configuaration
    if (ioctl(0 /* stdin */, TCGETS2, &buf->original_conf) != 0) {
        return -1; // error
    }

    // copy it in the new configuaration and modifies it
    buf->new_conf = buf->original_conf;

    // first disable line buffering and echo
    buf->new_conf.c_lflag &= ~(ICANON | ECHO | ISIG);
    buf->new_conf.c_iflag &= ~(IXON   | ICRNL);
    buf->new_conf.c_lflag &= ~(OPOST);

    // then set the buffer reading threshold to 1 byte
    buf->new_conf.c_cc[VMIN] = 1;
    buf->new_conf.c_cc[VTIME] = 0; // 0 timeout
    
    // set the new settings
    if (ioctl(0 /* stdin */, TCSETS2, &buf->new_conf) != 0) {
        return -1; // error
    }
    
    // success
    return 0;
}

int restore_normal_mode(struct RawBuffer *buf) {
    return ioctl(0 /* stdin */, TCSETS2, &buf->original_conf) != 0;
}

int read_input_buf(struct InputBuf *buf) {
    if (buf == NULL) return -1;

    // clear buf
    free_buf(buf);
    
    int ch;
    size_t cursor = 0;
    bool escape_code = false;
    while ((ch = getchar()) != EOF) {
        if (ch == '\n' || ch == '\r') { 
            putchar('\n'); 
            break; 
        }
        if (ch == CTRL_C) {
            puts("^C");
            return 1;
        }

        if (ch == 127) {
            // backspace
            if (cursor == 0 || buf->len == 0) continue;

            putchar('\b');
            putchar(' ');
            putchar('\b');

            cursor -= 1;
            if (cursor < buf->len) {
                memmove(buf->buf + cursor, buf->buf + cursor + 1, buf->len - cursor);

                for (size_t i = cursor; i < buf->len - 1; i++) {
                    putchar(buf->buf[i]);
                }
                putchar(' ');
                putchar('\b');
                for (size_t i = cursor; i < buf->len - 1; i++) {
                    putchar('\x1b');
                    putchar('[');
                    putchar('D');
                }
            }
            else {
                // place a new null
                buf->buf[cursor] = 0;
            }
            buf->len --;
            
            continue;
        }

        if (ch == ANSI_ESC) { 
            escape_code = true; 
            continue; 
        }

        // not printable? no worry!
        if (ch <= 0x1f || ch >= 127) { continue; }
        
        if (escape_code && ch == '[') {
            switch (getchar()) {
                // left
                case 'D':
                    if (cursor > 0) {
                        cursor --; 
                        putchar('\x1b');
                        putchar('[');
                        putchar('D');
                    }
                    escape_code = false;
                    break;

                case 'C':
                    if (cursor < buf->len) {
                        cursor++;
                        putchar('\x1b');
                        putchar('[');
                        putchar('C');
                    }
                    escape_code = false;
                    break;
                default:
                    escape_code = false;
                    break;
            }
            continue;
        }

        putchar(ch);

        if (buf->len + 1 >= buf->cap) {
            // must reallocate
            size_t new_cap = 2 * buf->cap + 1;
            char *new_buf = realloc(buf->buf, new_cap);
            
            // rallocation failed
            if (new_buf == NULL) return -1;

            buf->buf = new_buf;
            buf->cap = new_cap;
        }

        // push the char
        if (cursor < buf->len) {
            memmove(buf->buf + cursor + 1, buf->buf + cursor, buf->len - cursor);
            for (size_t i = cursor + 1; i < buf->len + 1; i++) {
                putchar(buf->buf[i]);
            }

            for (size_t i = cursor + 1; i < buf->len + 1; i++) {
                putchar('\x1b');
                putchar('[');
                putchar('D');
            }
        }

        buf->buf[cursor++] = ch;
        buf->len++;
    }

    return 0;
}

void free_buf(struct InputBuf *buf) {
    if (buf == NULL) return;

    if (buf->buf != NULL) {
        free(buf->buf);
        buf->buf = NULL;
    }

    buf->len = 0;
    buf->cap = 0;
}
