#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mount.h>

#define LOG_NAME "[ INIT ] "

int main(void) {

#define DUMMY_STR "none"

    // try to mount everything
    if (mount(DUMMY_STR, "/sys", "sysfs", 0, "") != 0) goto failure; 
    printf(LOG_NAME "mounted /sys\n");
    if (mount(DUMMY_STR, "/proc", "proc", 0, "") != 0) goto failure; 
    printf(LOG_NAME "mounted /proc\n");
    if (mount(DUMMY_STR, "/dev", "devtmpfs", 0, "") != 0) goto failure; 
    printf(LOG_NAME "mounted /dev\n");
    if (mount("/dev/vda", "/mnt", "ext4", 0, "") != 0) goto failure;
    printf(LOG_NAME "mounted disk\n");

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
    printf(LOG_NAME "tty1 enabled\n");
    printf(LOG_NAME "system initialization successful !\n");
    printf(LOG_NAME "lauching shell\n");

    // launch a shell
    if (fork() == 0) {
        // child process
#define SHELL "/bin/sh"
        execl(SHELL, SHELL, (char *)NULL);
#undef SHELL

        // error
        perror(LOG_NAME "unable to launch shell");
        return 1; // trigger kernel panic!
    }

    while (1)
        pause();

failure:
    perror(LOG_NAME "critical failure");

    return 0;
}