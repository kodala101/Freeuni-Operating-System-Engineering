#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

void memdump(char *fmt, char *data, int len);

int
main(int argc, char *argv[])
{
  if (argc == 1) {
    printf("Example 1:\n");
    int a[2] = {61810, 2026};
    memdump("ii", (char *)a, sizeof(a));

    printf("Example 2:\n");
    memdump("S", "a string", sizeof("a string"));

    printf("Example 3:\n");
    char *s = "another";
    memdump("s", (char *)&s, sizeof(s));

    struct sss {
      char *ptr;
      int num1;
      short num2;
      char byte;
      char bytes[8];
    } example;

    example.ptr = "hello";
    example.num1 = 1819438967;
    example.num2 = 100;
    example.byte = 'z';
    strcpy(example.bytes, "xyzzy");

    printf("Example 4:\n");
    memdump("pihcS", (char *)&example, sizeof(example));

    printf("Example 5:\n");
    memdump("sccccc", (char *)&example, sizeof(example));
  } else if (argc == 2) {
    // format in argv[1], up to 512 bytes of data from standard input.
    char data[512];
    int n = 0;
    memset(data, '\0', sizeof(data));
    while (n < sizeof(data)) {
      int nn = read(0, data + n, sizeof(data) - n);
      if (nn <= 0)
        break;
      n += nn;
    }
    memdump(argv[1], data, n);
  } else {
    printf("Usage: memdump [format]\n");
    exit(1);
  }
  exit(0);
}

void
memdump(char *fmt, char *data, int len)
{
int offset = 0;

    for(int i = 0; fmt[i] != '\0'; i++) {
        char ch = fmt[i];
        int bites = 0;

        if (ch == 'i') {
            bites = 4;
        } else if (ch == 'p' || ch == 's') {
            bites = 8;
        } else if (ch == 'h') {
            bites = 2;
        } else if (ch == 'c') {
            bites = 1;
        } else if (ch == 'S') {
            bites = 0;
        } else {
            continue;
        }
      
        if (ch != 'S' && bites + offset > len) {
            fprintf(2, "memdump: not enough data for '%c'\n", ch);
            return;
        }

        if (ch == 'i') {
            int value = *(int*)(data + offset);
            printf("%d\n", value);
            offset += 4;
        } else if (ch == 'p') {
            uint64 value = *(uint64*)(data + offset);
            printf("%lx", value);
            printf("\n");
            offset += 8;
        } else if (ch == 'h') {
            short value = *(short*)(data + offset);
            printf("%d\n", (int)value);
            offset += 2;
        } else if (ch == 'c') {
            char value = data[offset];
            printf("%c\n", value);
            offset += 1;
        } else if (ch == 's') {
            char* strinPointer = *(char**)(data + offset);
            printf("%s\n", strinPointer);
            offset += 8;
        } else if (ch == 'S') {
            int remaining = len - offset;
            int printed = 0;
            
            while(printed < remaining && data[offset + printed] != '\0') {
                printf("%c", data[offset + printed]);
                printed++;
            }

            printf("\n");
            return;
        }
    }      
} 