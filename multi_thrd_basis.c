#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <syscall.h>

int conc_amnt;
const int MAX_LENGTH = 70;

struct message {
    bool first;
    char *str;
};
typedef struct message message;

void *threadFunc(void *arg) {
    message *m = (message *) arg;
    printf("[spid](%ld) [tgid](%d) [ptid](%lu) start\n", syscall(SYS_gettid), getpid(), pthread_self());
    if (!m->first)
        pthread_detach(pthread_self());
    printf("[spid](%ld) [read](%s)\n", syscall(SYS_gettid), m->str);
    printf("[spid](%ld) [tgid](%d) [ptid](%lu) [func](pthread_exit)\n", syscall(SYS_gettid), getpid(), pthread_self());
    pthread_exit(0);
    return NULL;
}


int main(int argc, char **argv) {
    if (argc == 3 && strcmp(argv[1], "-c") == 0)
        conc_amnt = (int) strtol(argv[2], (char **) NULL, 10);
    else
        return 0;

    printf("[spid](%ld) [tgid](%d) [ptid](%lu) start\n", syscall(SYS_gettid), getpid(), pthread_self());
    FILE *file = fopen("td.txt", "r");

    pthread_t *threadId = (pthread_t *) malloc(sizeof(pthread_t) * (conc_amnt - 1));

    for (int i = 0; i < conc_amnt - 1; ++i) {
        char *buff = (char *) malloc(sizeof(char) * MAX_LENGTH);
        fgets(buff, MAX_LENGTH, file);
        buff[strlen(buff) - 1] = '\0';
        printf("[spid](%ld) [read](%s)\n", syscall(SYS_gettid), buff);
        message *m = (message *) malloc(sizeof(message));
        m->str = buff;
        if (i == 0) {
            m->first = true;
            pthread_attr_t attr;
            pthread_attr_init(&attr);
            pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
            pthread_create(&threadId[i], &attr, threadFunc, m);
            pthread_attr_destroy(&attr);
        } else {
            m->first = false;
            pthread_create(&threadId[i], NULL, threadFunc, m);
        }
    }
    fclose(file);
    sleep(1);
    printf("[spid](%ld) [tgid](%d) [ptid](%lu) [func](exit)\n", syscall(SYS_gettid), getpid(), pthread_self());
    exit(0);
}