#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
int main()
{
    pid_t pid = fork();
    if (pid == -1)
    {
        printf("error\n");
        exit(0);
    }
    else if (pid == 0)
    {
        sleep(1);
        printf("i am child!\n");
        return 0;
    }
    else
    {
        printf("i am father pid:%d\n", pid);
        printf("waiting for child\n");
        pid_t wpid = wait(NULL);
        printf("child finish!\n");
        return 0;
    }
    return 1;
}