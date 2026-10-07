#include "interpreter.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
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
int run_cmd(char *command) {

    // create a "lexer"
    char **lexer = &command;

    // get the command name
    char cmd_name[64]; // why??
    int success = get_next_token(lexer, cmd_name, sizeof(cmd_name) - 1);
    if (success == -1) {
        fprintf(stderr, "commands names shall be less than or 63 long\n");
        return -1;
    }
    // end of stream
    else if (success == -2) {
        return 0;
    }

    if (strncmp(cmd_name, "cd", sizeof(cmd_name) - 1) == 0) {
        builtin_run_cd(lexer);
        return 0;
    }

    if (strncmp(cmd_name, "ls", sizeof(cmd_name) - 1) == 0) {
        builtin_run_ls(lexer);
        return 0;
    }

    if (strncmp(cmd_name, "exit", sizeof(cmd_name) - 1) == 0) {
        printf("[ EXIT SH ]\n");
        exit(0);
    }

    if (strncmp(cmd_name, "clear", sizeof(cmd_name) - 1) == 0) {
        // go to home
        // erase the buffer
        // erase the scroll buffer
        printf("\033[H\033[2J\033[3J");
        return 0;
    }

    if (run_binary_from_bin(cmd_name, lexer) == 0) {
        return 0; // success!!
    }

    // unknown command
    fprintf(stderr, "unknown command: '%s'\n", cmd_name);
    return 1;
}

/**
 * try to run the command from a binary in /bin
 */
int run_binary_from_bin(char *cmd, char **lexer) {
    if (cmd == NULL || lexer == NULL) return -1;

    // create the path
    char path[1024];
    snprintf(path, sizeof(path) - 1, "/bin/%s", cmd);

    // try open the file:
    struct stat statbuffer;
    if (stat(path, &statbuffer) != 0) {
        // file doesn't exist!!
        return -1;
    }

    if (S_ISDIR(statbuffer.st_mode)) {
        return -1; // connot execute directory!
    }

    struct ArgList arglist = { 0 };
    push_arg(&arglist, path, sizeof(path));
    if (get_arglist(lexer, &arglist) != 0) {
        free_arglist(&arglist);
        return -1;
    }

    pid_t child;
    if ((child = fork()) == 0) {
        // new process here
        // fine linux will free it up for the child
        execv(path, arglist.argv);

        // oopsi
        fprintf(stderr, "sh: couldn't execute %s: %s", path, strerror(errno));
        exit(1);
    }
    else if (child == -1) {
        // fork failed
        perror("sh: fork failed");
        return 0;
    }

    // wait for child to terminate
    waitpid(child, NULL, 0);

    // free the arglist
    free_arglist(&arglist);

    return 0;
}

/**
 * get the next token 0 success -1 overflow or invalid
 */
int get_next_token(char **cursor, char *buf, size_t buf_len) {
    // set buf to 0
    memset(buf, 0, buf_len);
    
    if (cursor == NULL) return -1;
    if (*cursor == NULL || **cursor == '\n') return -2;

    // skip until next word
    if (**cursor == ' ') {
        while (*(++(*cursor)) == ' ');
    }

    for (size_t i = 0; **cursor != ' ' && **cursor != 0 && **cursor != '\n'; (*cursor)++, i++) {
        if (i >= buf_len) {
            return -1; // overflow
        }
        // copy to buffer
        buf[i] = **cursor;
    }

    // go to next word
    if (**cursor == ' ') {
        while (*(++(*cursor)) == ' ');
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

    if (strncmp(buf, "/home", sizeof(buf) - 1) == 0) {
        // replace buffer content with '#'
        memset(buf, 0, sizeof(buf));
        buf[0] = '#';
    }

    printf("/ %s ", buf);
}

static int realloc_arglist(struct ArgList *arglist, int new_argcapacity) {
    if (arglist == NULL || new_argcapacity <= arglist->argcapacity) { 
        errno = EINVAL;
        return -1; 
    }

    // try reallocating
    int new_capacity = 2 * arglist->argcapacity + 1; // +1 to avoid the problem with 0 
    char **new_argv = realloc(arglist->argv, new_capacity * sizeof(char *));

    if (new_argv == NULL) {
        return -1;
    }

    // success!!
    arglist->argv = new_argv;
    arglist->argcapacity = new_argcapacity;

    return 0;
}

void free_arglist(struct ArgList *arglist) {
    if (arglist == NULL) return;

    for (int i = 0; i < arglist->argc; i++) {
        if (arglist->argv[i] != NULL) {
            free(arglist->argv[i]);
            arglist->argv[i] = NULL;
        }
    }

    free(arglist->argv);
    arglist->argv = NULL;

    arglist->argc        = 0;
    arglist->argcapacity = 0;
}

int push_arg(struct ArgList *arglist, char *buf, size_t buf_len) {
    if (arglist->argc + 1 >= arglist->argcapacity) {
        if (realloc_arglist(arglist, arglist->argcapacity * 2 + 1) != 0) {
            perror("sh: could not reallocate arglist");
            return -1;
        }
    }

    // push it to the argv
    if (buf == NULL) {
        arglist->argv[arglist->argc++] = NULL;
        return 0;
    }

    char *arg = calloc(buf_len, sizeof(char));
    strncpy(arg, buf, buf_len-1);
    arglist->argv[arglist->argc++] = arg;
    return 0;
}

/**
 * get the arguments as a list
 */
int get_arglist(char **lexer, struct ArgList *arglist) {
    if (arglist == NULL) {
        errno = EINVAL;
        return -1;
    }

    // no clue why...
    char buf[512];

    while (get_next_token(lexer, buf, sizeof(buf) - 1) == 0) {
        if (push_arg(arglist, buf, sizeof(buf)) != 0) {
            return -1;
        }
    }

    if (push_arg(arglist, NULL, 0) != 0) {
        return -1;
    }

    return 0;
}

/*******************************************************************************
 *                  BUILTIN COMMANDS SUCH AS CD                                *
 *******************************************************************************/

/**
 * change the cwd to the one given in argument
 */
void builtin_run_cd(char **lexer) {
    char path[512]; // 512 max??
    int success = get_next_token(lexer, path, sizeof(path) - 1);

    if (success == -1) {
        fprintf(stderr, "cd: path shall be at most 511 characters long.\n");
        return;
    }

    // if end of stream or path == '#' then replace the path by /home
    if (success == -2 || strncmp(path, "#", sizeof(path) - 1) == 0) {
        // '#' is alias for /home
        strcpy(path, "/home"); // 6 < 512 so no overflow possible!
    }

    // change the directory!
    if (chdir(path) == -1) {
        perror("cd: unable to change directory");
    }
}

/**
 * list the current working directory
 */
void builtin_run_ls(char **lexer) {
    char path[512]; // 512 max??
    int success = get_next_token(lexer, path, sizeof(path) - 1);

    if (success == -1) {
        fprintf(stderr, "path shall be at most 511 characters long.\n");
        return;
    }
    // end of stream (aka no argument)
    else if (success == -2) {
        // list cwd
        path[0] = '.';
    }

    if (strncmp(path, "#", sizeof(path) - 1) == 0) {
        // '#' is alias for /home
        strcpy(path, "/home"); // 6 < 512 so no overflow possible!
    }

    // open the directory
    DIR *dir = opendir(path);
    if (dir == NULL) {
        perror("ls: could not open directory");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        printf("%s\n", entry->d_name);
    }

    closedir(dir);
}