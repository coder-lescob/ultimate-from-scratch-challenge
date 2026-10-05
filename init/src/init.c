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

    // let the challenge begin
    printf("Welcome to the ultimate bootstrapping challenge !\n");

    while (1)
        pause();

failure:
    perror("[ INIT ] critical failure");

    return 0;
}