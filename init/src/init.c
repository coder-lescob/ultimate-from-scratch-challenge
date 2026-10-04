#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

int main(void) {
    printf("Welcome to the ultimate bootstrapping challenge !\n");

    while (1)
        pause();

    return 0;
}