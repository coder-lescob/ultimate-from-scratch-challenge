#include "interpreter.h"
#include "token.h"
#include "lexer.h"
#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>
#include <errno.h>
#include <dirent.h>
#include <sys/pidfd.h>
#include <sys/wait.h>
#include <sys/reboot.h>
#include <sys/stat.h>
#include <sys/stat.h>

/**
 * runs a command in the terminal
 */
int run_cmd(struct Command command) {

    // EOS nothing there to show
    if (command.cmd == NULL) {
        return 0;
    }

    // get the command name
    size_t cmdlen = strlen(command.cmd);
    char cmd_name[cmdlen + 1];
    strncpy(cmd_name, command.cmd, cmdlen);

    if (strcmp(cmd_name, "cd") == 0) {
        builtin_run_cd(&command);
        return 0;
    }

    if (strcmp(cmd_name, "ls") == 0) {
        builtin_run_ls(&command);
        return 0;
    }

    if (strcmp(cmd_name, "exit") == 0) {
        printf("[ EXIT SH ]\n");
        exit(0);
    }

    if (strcmp(cmd_name, "clear") == 0) {
        // go to home
        // erase the buffer
        // erase the scroll buffer
        printf("\033[H\033[2J\033[3J");
        return 0;
    }

    if (run_binary(&command) == 0) {
        free_command(&command);
        return 0; // success!!
    }

    // unknown command
    fprintf(stderr, "unknown command: '%s'\n", cmd_name);
    free_command(&command);
    return 1;
}

/**
 * try to run the command from a binary in /bin
 */
int run_binary(struct Command *cmd) {
    if (cmd == NULL) return -1;

    // get the path
    char *path;
    size_t pathlen = strlen(cmd->cmd);
    bool in_cwd = false;
    if (pathlen > 2 && cmd->cmd[0] == '.' && cmd->cmd[1] == '/') {
        // binary in the cwd
        in_cwd = true;
        path = cmd->cmd;
    }
    else {
        // binary in /bin
        path = calloc(pathlen + 6, sizeof(char)); // "/bin/" is 5 characters long + null is 6
        snprintf(path, pathlen + 6, "/bin/%s", cmd->cmd); // write to path /bin/cmd_name

        // free the cmd because it's copied to path
        free(cmd->cmd);
        cmd->cmd = NULL;

        pathlen += 5;
    }

    // try open the file:
    struct stat statbuffer;
    if (stat(path, &statbuffer) != 0) {
        
        // file doesn't exist!!
        if (in_cwd) {
            fprintf(stderr, "sh: cannot execute %s: no such file or directory\n", path);
            return 0;
        }
        free(path);
        return -1;
    }

    if (S_ISDIR(statbuffer.st_mode)) {
        if (in_cwd) {
            fprintf(stderr, "sh: cannot execute %s: is a directory\n", path);
            free(path);
            return 0;
        }
        free(path);
        return -1; // connot execute directory! but since /bin/dirname, just say that you don't know it's a commmand
    }

    // push NULL to statisfy execve
    push_arg(&cmd->args, NULL, 0);

    pid_t child;
    if ((child = fork()) == 0) {
        // redirection
        if (cmd->fstdin != NULL) {
            // redirect to there!
            int fd = open(cmd->fstdin, O_RDONLY);
            if (fd == -1) {
                perror("sh: unable to redirect stdin");
                exit(1);
            }
            dup2(fd, 0); // copy fd to stdin

            // close fd
            if (fd > 0) close(fd);
        }
        if (cmd->fstdout != NULL) {
            // redirect to there!
            int fd = open(cmd->fstdout, O_WRONLY | O_CREAT | O_TRUNC);
            if (fd == -1) {
                perror("sh: unable to redirect stdout/stderr");
                exit(1);
            }
            dup2(fd, 1); // copy fd to stdout
            dup2(fd, 2); // copy fd to stderr

            // close fd
            if (fd > 2) close(fd);
        }

        // fine linux will free it up for the child
        execv(path, cmd->args.argv);

        // oopsi
        fprintf(stderr, "sh: couldn't execute %s: %s\n", path, strerror(errno));
        exit(1); // linux will still free the resources for the child
    }
    else if (child == -1) {
        // fork failed
        perror("sh: fork failed");
        goto cleanup;
    }

    // wait for child to terminate
    waitpid(child, NULL, 0);

cleanup:
    // free path
    if (!in_cwd) {
        free(path);
    }

    return 0;
}

/**
 * display the current working directory
 */
void display_cwd() {

    // get the cwd
    char buf[512]; // 512??
    getcwd(buf, sizeof(buf) - 1);

    #define COLOR_RED "\x1b[38;5;196m"
    #define RESET_COL "\x1b[0m"

    if (strlen(buf) >= 2 && buf[0] == '/' && buf[1] == '~') {
        printf(COLOR_RED "root" RESET_COL "@challenge %s # ", buf+1);
    }
    else {
        printf(COLOR_RED "root" RESET_COL "@challenge %s # ", buf);
    }
}

/*******************************************************************************
 *                  BUILTIN COMMANDS SUCH AS CD                                *
 *******************************************************************************/

/**
 * change the cwd to the one given in argument
 */
void builtin_run_cd(struct Command *cmd) {
    if (cmd == NULL) return;

    // if end of stream
    if (cmd->args.argc <= 1) {
        // default ~ aka home
        if (chdir("/~") == -1) {
            perror("cd: unable to go home, you screwed home up!");
        }
        goto end_cd;
    }

    // get path
    char *path;
    size_t pathlen = strlen(cmd->args.argv[1]);
    bool free_path = false;

    if (pathlen >= 1 && cmd->args.argv[1][0] == '~') {
        path = calloc(pathlen + 2, sizeof(char)); // +1 for '/' +1 for null
        if (path == NULL) {
            perror("cd: unable to change directory");
            goto end_cd;
        }
        snprintf(path, pathlen + 2, "/%s", cmd->args.argv[1]);
        free_path = true;
    }
    else {
        path = cmd->args.argv[1];
    }

    // change the directory!
    if (chdir(path) == -1) {
        perror("cd: unable to change directory");
    }

    if (cmd->args.argc > 2) {
        fprintf(stderr, "cd: warning: cannot change to more than one directory\n");
    }

    if (free_path) {
        free(path);
    }

end_cd:
    free_command(cmd);
}

static void list_dir(char *path, FILE *fp) {
    // open the directory
    DIR *dir = opendir(path);
    if (dir == NULL) {
        fprintf(fp, "ls: could not open directory: %s", strerror(errno));
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        fprintf(fp, "%s\n", entry->d_name);
    }

    closedir(dir);
}

/**
 * list the current working directory
 */
void builtin_run_ls(struct Command *cmd) {
    if (cmd == NULL) return;

    FILE *fp = stdout;
    if (cmd->fstdout != NULL) {
        fp = fopen(cmd->fstdout, "w");
        if (fp == NULL) {
            fprintf(stderr, "sh: unable to redirect stdout to %s: %s", cmd->fstdout, strerror(errno));
            goto end_ls;
        }
    }

    if (cmd->args.argc <= 1) {
        // list cwd
        list_dir(".", fp);
        goto end_ls;
    }

    bool multiple_dirs = cmd->args.argc > 2;

    for (int i = 1; i < cmd->args.argc; i++) {
        char *path = cmd->args.argv[i];
        size_t pathlen = strlen(path);

        bool alloced = false;
        if (pathlen >= 1 && path[0] == '~') {
            path = calloc(pathlen + 2, sizeof(char)); // +1 for '/' +1 for null
            if (path == NULL) {
                fprintf(fp, "ls: couldn't format path: %s", strerror(errno));
                goto end_ls;
            }
            snprintf(path, pathlen + 2, "/%s", cmd->args.argv[i]);
            alloced = true;
        }

        if (multiple_dirs) {
            fprintf(fp, "%s:\n", path);
        }

        list_dir(path, fp);

        if (multiple_dirs) {
            fprintf(fp, "\n");
        }

        if (alloced) {
            free(path);
        }
    }

end_ls:
    // close fp
    if (cmd->fstdout != NULL) {
        fclose(fp);
    }

    free_command(cmd);
}