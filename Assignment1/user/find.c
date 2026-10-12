#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/param.h"
#include "kernel/fcntl.h"

char* fmtname(char *path) {
  char *p;

  for (p = path + strlen(path); p >= path && *p != '/'; p--){}
  p++;

  return p;
}

void find(char* path, char* filename, char* execArgv[], int execArgc) {
    int fds;
    struct stat st;
    char fullPath[512];
    char* p;
    struct dirent de;

    if ((fds = open(path, O_RDONLY)) < 0) {
        fprintf(2, "File cannot be opened\n");
        return;
    }

    if (fstat(fds, &st) < 0) {
        fprintf(2, "ls: cannot stat %s\n", path);
        close(fds);
        return;
    }

    if (strcmp(fmtname(path), filename) == 0) {
        if (execArgc > 0) {
            char* arguments[MAXARG];
            for (int i = 0; i < execArgc; i++) {
                arguments[i] = execArgv[i];
            }

            arguments[execArgc] = path;
            arguments[execArgc + 1] = 0;

            int pid = fork();
            if (pid < 0) {
                fprintf(2, "find: fork failed\n");
                close(fds);
                exit(1);
            }
            if (pid == 0) {
                exec(arguments[0], arguments);
                fprintf(2, "find: exec %s failed\n", arguments[0]);
                exit(1);
            }

            wait(0);
        } else {
            printf("%s\n", path);
        }
    }

    switch (st.type) {
        case T_FILE:
            break;
        
        case T_DIR:
            if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(fullPath)) {
                fprintf(2, "Path too long\n");
                break;
            }

            strcpy(fullPath, path);
            p = fullPath + strlen(fullPath);
            *p = '/';
            p++;
            
            while(read(fds, &de, sizeof(de)) == sizeof(de)) {
                if (de.inum == 0) continue;
                if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) continue;

                memmove(p, de.name, DIRSIZ);
                p[DIRSIZ] = 0;

                find(fullPath, filename, execArgv, execArgc);
            }
    }

    close(fds);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(2, "Error: find <path> <filename> [-exec <cmd...>]\n");
        exit(1);
    }

    char* execArgv[MAXARG];
    int execArgc = 0; 

    if (argc > 3 && strcmp(argv[3], "-exec") == 0) {
        for (int j = 4; j < argc; j++) {
            execArgv[execArgc++] = argv[j];
        }
    }

    find(argv[1], argv[2], execArgv, execArgc);
    exit(0);
}