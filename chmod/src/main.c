#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>

typedef unsigned int mode_t;
mode_t parse_permissions(char *permissions, mode_t original_perms);
mode_t add_perm(mode_t perms, mode_t perm);
mode_t remove_perm(mode_t perms, mode_t perm);
mode_t get_mode_from_char(char ch);

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "chmod: expected 2 arguments, the mode and the file.\n");
        return 1;
    }

    char *path = argv[2];
    struct stat statbuf;

    // get the permition from the file
    if (stat(path, &statbuf) != 0) {
        perror("chmod: unable to read permitions of the file");
        return -1;
    }

    // parse the permitions
    mode_t perm = parse_permissions(argv[1], statbuf.st_mode);

    // modify the mode of the file
    if (chmod(path, perm) != 0) {
        perror("chmod: unable to change permitions");
        return -1;
    }

    return 0;
}

mode_t parse_permissions(char *permissions, mode_t original_perms) {
    mode_t perms = original_perms;
    
    do {
        // get the '+' or '-' for add or remove permission
        char add_or_rm = *(permissions++);
        mode_t perm    = get_mode_from_char(*permissions);

        if (add_or_rm == '+') {
            perms = add_perm(perms, perm);
        }
        else if (add_or_rm == '-') {
            perms = remove_perm(perms, perm);
        }
        else {
            // user might have a bad experience if it parses the mode with an offset of 1...
            fprintf(stderr, "chmod: unknown mode modifier '%c', not modifying permissions\n", add_or_rm);
            return original_perms; 
        }
    } 
    while (*(++permissions) != 0);

    return perms;
}

mode_t add_perm(mode_t perms, mode_t perm) {
    return perms | perm;
}

mode_t remove_perm(mode_t perms, mode_t perm) {
    return perms & (~perm);
}

mode_t get_mode_from_char(char ch) {
    switch (ch) {
        case 'r': return S_IRUSR; // Owner read
        case 'w': return S_IWUSR; // Owner write
        case 'x': return S_IXUSR; // Owner execute

        default:
            fprintf(stderr, "chmod: warning: unknown permission '%c', ignored\n", ch);
            return 0; // won't change the permission
    }
}