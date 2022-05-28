#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

const char *TD1_FILE = "td1.txt";
const char *TD2_FILE = "td2.txt";

int main(int argc, char **argv) {
    int num = 0;
    if (argc == 3 && strcmp(argv[1], "-c") == 0)
        num = (int) strtol(argv[2], (char **) NULL, 10);
    else
        return 0;

    printf("[pid](%d) [ppid](%d): start\n", getpid(), getppid());
    int fd1_td1 = open(TD1_FILE, O_RDONLY);
    printf("[pid](%d) [ppid](%d): [fd1_td1](%d) is got\n", getpid(), getppid(), fd1_td1);

    pid_t pid = 0;
    for (int i = 0; i < num - 1; ++i) {
        pid = fork();
        if (pid == 0) break;
    }

    char *buf = (char *) malloc(sizeof(char) * 10);
    if (pid == 0) {
        //child
        printf("[pid](%d) [ppid](%d): start\n", getpid(), getppid());
        read(fd1_td1, buf, 5);
        printf("[pid](%d) [ppid](%d): [read](%5.5s) by [fd1_td1](%d)\n", getpid(), getppid(), buf, fd1_td1);

        int fd2_td1 = open(TD1_FILE, O_RDONLY);
        printf("[pid](%d) [ppid](%d): [fd2_td1](%d) is got\n", getpid(), getppid(), fd2_td1);
        read(fd2_td1, buf, 5);
        printf("[pid](%d) [ppid](%d): [read](%5.5s) by [fd2_td1](%d)\n", getpid(), getppid(), buf, fd2_td1);

        close(fd2_td1);
        close(fd1_td1);
    } else {
        //parent
        read(fd1_td1, buf, 5);
        printf("[pid](%d) [ppid](%d): [read](%5.5s) by [fd1_td1](%d)\n", getpid(), getppid(), buf, fd1_td1);
        close(fd1_td1);

        int fd1_td2 = open(TD2_FILE, O_RDONLY);
        printf("[pid](%d) [ppid](%d): [fd1_td2](%d) is got\n", getpid(), getppid(), fd1_td2);
        read(fd1_td2, buf, 5);
        printf("[pid](%d) [ppid](%d): [read](%5.5s) by [fd1_td2](%d)\n", getpid(), getppid(), buf, fd1_td2);
        close(fd1_td2);

        sleep(2);
    }
    printf("[pid](%d) [ppid](%d): exit\n", getpid(), getppid());
    exit(0);
}
