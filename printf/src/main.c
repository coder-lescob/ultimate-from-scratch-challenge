#include <stdio.h>
#include <stdlib.h>

int hex_to_num(char ch) {
    if (ch >= '0' && ch <= '9') return ch - '0';
    if (ch >= 'a' && ch <= 'f') return 10 + ch - 'a';
    if (ch >= 'A' && ch <= 'F') return 10 + ch - 'A';
    printf("oopsie, not valid! %c\n", ch);
    return 0;
}

void print_formatted_str(char *str) {
    int escaped = 0;
    
    for (; *str != 0; str++) {
        if (*str == '\\') {
            escaped = 1;
            
            str++;

            // avoid overflowing the buffer
            if (*str == 0) break;
        }

        if (!escaped) {
            putchar(*str);
        }
        else {
            switch (*str) {
                case 'n':
                    putchar('\n');
                    break;

                case 'r':
                    putchar('\r');
                    break;

                case 'x':
                    // interprete as hex the next two chars
                    str++; // skip x
                    char a = hex_to_num(*(str++)) & 0xF;
                    char b = hex_to_num(*str) & 0xF;

                    // quick and dirty way
                    int hexn = (a << 4) + b;

                    // print the exact code asked for
                    putchar(hexn);
                    break;

                default:
                    putchar(*str);
                    break;
            }
        }

        escaped = 0;
    }
}

int main(int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
        print_formatted_str(argv[i]);
    }
    return 0;
}
