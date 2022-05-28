#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

const int SIZE = 100;

int main(int argc, char **argv) {
    int pipe_fd[2];
    pid_t pid;

    if (pipe(pipe_fd) < 0) {
        printf("pipe create error");
        exit(0);
    }
    char *buff = (char *) malloc(sizeof(char) * SIZE);

    pid = fork();

    if (pid == 0) {
        close(pipe_fd[0]);
        write(pipe_fd[1], "Child process 1 is sending a message!", SIZE);
        exit(0);
    } else {
        waitpid(pid, NULL, 0);//Hang
//        close(pipe_fd[1]);
        read(pipe_fd[0], buff, SIZE);
        printf("%s", buff);
    }

    pid = fork();

    if (pid == 0) {
        close(pipe_fd[0]);
        write(pipe_fd[1], "Child process 2 is sending a message!", SIZE);
        exit(0);
    } else {
        waitpid(pid, NULL, 0);//Hang
        close(pipe_fd[1]);
        read(pipe_fd[0], buff, SIZE);
        printf("%s", buff);
    }

//    close(pipe_fd[1]);
//    read(pipe_fd[0], buff, SIZE);
//    printf("%s", buff);
//    read(pipe_fd[0], buff, SIZE);
//    printf("%s", buff);
    return 0;
}