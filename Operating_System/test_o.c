#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

int main(int argc, char **argv) {
    struct stat status;
    char *path = "/home/jakoerror/linux_c_workspace/Operating_System/src";
    int i = stat(path, &status);
    printf("%d %u", i, status.st_mode);
    if (!S_ISDIR(status.st_mode)) {
        printf("\"%s\" is not a directory!\n", path);
    }else{
        printf("\"%s\" is a directory!\n", path);

    }
    return 0;
}