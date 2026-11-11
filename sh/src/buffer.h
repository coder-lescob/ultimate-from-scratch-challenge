#ifndef BUFFER_H
#define BUFFER_H

#include <sys/ioctl.h>
#include <asm/termbits.h>
#include <asm/ioctls.h>
#include <stddef.h>

struct RawBuffer {
    struct termios2 original_conf;
    struct termios2 new_conf;
};

int setup_raw_mode(struct RawBuffer *buf);
int restore_normal_mode(struct RawBuffer *buf);

#define CTRL_C (3)
#define ANSI_ESC (033)

struct InputBuf {
    char *buf;
    size_t len, cap;
};

int read_input_buf(struct InputBuf *buf);
void free_buf(struct InputBuf *buf);

#endif
