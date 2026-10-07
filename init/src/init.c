#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <sys/mount.h>
#include <sys/reboot.h>

#define LOG_NAME "[ INIT ] "

pid_t sh = -1;

void reap_zombie_child(int sig) {
    (void)sig;

    int saved_status;
    // reap zombie child!
    pid_t child_pid = waitpid(-1, &saved_status, WNOHANG);

    if (sh != -1 && child_pid == sh) {
        // sh dies so end of session
        printf(LOG_NAME "power off\n");
        sleep(2);
        reboot(RB_POWER_OFF);
    }
}

int main(void) {

#define DUMMY_STR "none"

    // try to mount everything
    if (mount(DUMMY_STR, "/sys", "sysfs", 0, "") != 0) goto failure; 
    printf(LOG_NAME "mounted /sys\n");
    if (mount(DUMMY_STR, "/proc", "proc", 0, "") != 0) goto failure; 
    printf(LOG_NAME "mounted /proc\n");
    if (mount(DUMMY_STR, "/dev", "devtmpfs", 0, "") != 0) goto failure; 
    printf(LOG_NAME "mounted /dev\n");
    if (mount("/dev/vda", "/home", "ext4", 0, "") != 0) goto failure;
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

    // create sigaction to stop zombie process eating this compute's brain!!
    struct sigaction action;
    action.sa_handler = reap_zombie_child;
    sigemptyset(&action.sa_mask);
    action.sa_flags = SA_RESTART | SA_NOCLDSTOP;

    if (sigaction(SIGCHLD, &action, NULL) == -1) {
        perror(LOG_NAME "couldn't lauch zombie cleaning...");
        return -1;
    }

    printf(LOG_NAME "Zombie reaping actiavted!\n");
    printf(LOG_NAME "lauching shell\n");

    // launch a shell
    if ((sh = fork()) == 0) {
        // change the current working directory to /mnt
        if (chdir("/home") != 0) {
            perror("unable to change directory");
        };

        // child process
#define SHELL "/bin/sh"
        execl(SHELL, SHELL, (char *)NULL);
#undef SHELL

        // error
        perror(LOG_NAME "unable to launch shell");
        sleep(2);
        return 1; // trigger kernel panic!
    }
    else if (sh == -1) {
        perror(LOG_NAME "forking to sh failed");
        sleep(2);
        return 1;
    }

    while (1)
        pause();

failure:
    perror(LOG_NAME "critical failure");
    sleep(2);

    return 0;
}