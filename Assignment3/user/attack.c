#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "kernel/riscv.h"

int valid(char c) {
  return (c >= 'a' && c <= 'z') ||
          (c >= 'A' && c <= 'Z') ||
          (c >= '0' && c <= '9');
}

int main(int argc, char *argv[]) {

  char* memory = sbrk(32 * 4096);

  for (int i = 0; i < 32 * 4096; i++) {

    if (valid(memory[i])) {

      int len = 0;

      while (valid(memory[i + len])) {
        len++;
      } 

      if (len > 2 && memory[i + len] == '\0') {
        printf("%s\n", &memory[i]);
      }

      i += len;

    }

  }

  exit(0);

}
