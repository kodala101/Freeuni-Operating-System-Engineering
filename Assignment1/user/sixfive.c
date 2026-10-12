#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

void helper(int descriptor) {
    char ch;
    int num = 0;
    int finished = 1;
    char* separators = " -\r\t\n./,";
    int validSep = 1;

    while(read(descriptor, &ch, 1) == 1) {
        if (ch >= '0' && ch <= '9') {
            num = num * 10 + (ch - '0');
            finished = 0;
        } else if (strchr(separators, ch) != 0) {
            if (!finished && validSep) {
                if (num % 5 == 0 || num % 6 == 0) {
                    printf("%d\n", num);
                }
            }
            num = 0;
            finished = 1;
            validSep = 1;
        } else {
            num = 0;
            finished = 1;
            validSep = 0;
        }
    }

    if (!finished) {
        if (num % 5 == 0 || num % 6 == 0) {
                printf("%d\n", num);
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        helper(0);
    } else {
        for (int i = 1; i < argc; i++) {
            int descriptor = open(argv[i], O_RDONLY);
            if (descriptor < 0) {
                fprintf(2, "sixfive: cannot open %s\n", argv[i]);
                exit(1);
            }
            helper(descriptor);
            close(descriptor);
        }
    }

    exit(0);
}