#include <stdio.h>
#include <string.h>

struct Flags {
  unsigned int force: 1;
  unsigned int recusive: 1;
};

struct Flags parse_flags(int argc, char **argv);

int main(int argc, char **argv) {
  // parse the flags
  struct Flags flags = parse_flags(argc, argv);

  printf("f: %d, r: %d\n", flags.force, flags.recusive);

  return 0;
}

struct Flags parse_flags(int argc, char **argv) {
  struct Flags flags = { 0 };

  void parse_flag_str(char *strf, struct Flags *flags);
  
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] != '-' || strlen(argv[i]) <= 1) continue;
    
    // parse the flag str
    // len > 1 thus adding one to the string pointer just skips the first letter safely
    parse_flag_str(argv[i] + 1, &flags);
  }

  return flags;
}

void parse_flag_str(char *strf, struct Flags *flags) {
  if (strf == NULL || flags == NULL) return;

  for (; *strf != 0; strf++) {
    switch (*strf) {
      case 'r': 
        flags->recusive = 1;
        break;
      case 'f':
        flags->force = 1;
         break;
    }
  }
}
