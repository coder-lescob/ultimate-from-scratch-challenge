#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mount.h>

int main(void) {

#define DUMMY_STR "none"

    // try to mount everything
    if (mount(DUMMY_STR, "/sys", "sysfs", 0, "") != 0) goto failure; 
    printf("[ INIT ] mounted /sys\n");
    if (mount(DUMMY_STR, "/proc", "proc", 0, "") != 0) goto failure; 
    printf("[ INIT ] mounted /proc\n");
    if (mount(DUMMY_STR, "/dev", "devtmpfs", 0, "") != 0) goto failure; 
    printf("[ INIT ] mounted /dev\n");

#undef DUMMY_STR

    // open tty1
    int fd = open("/dev/tty1", O_RDWR);
    if (fd < 0) goto failure;

    // use tty1 as stdio
    dup2(fd, 0);
    dup2(fd, 1);
    dup2(fd, 2);

    // close fd
    if (fd > 2) {
        close(fd);
    }

    // log success
    printf("[ INIT ] tty1 enabled\n");
    printf("[ INIT ] system initialization successful !\n");
    printf("[ INIT ] TODO: now launch a shell\n");

    while (1)
        pause();

failure:
    perror("[ INIT ] critical failure");

    return 0;
}