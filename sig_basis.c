#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

void catcher(int sig) {
    char *sig_name = NULL;
    if (sig == SIGUSR1)
        sig_name = "SIGUSR1";
    else if (sig == SIGUSR2)
        sig_name = "SIGUSR2";
    else if (sig == SIGINT)
        sig_name = "SIGINT";
    printf("[pid](%d): [sig](%s) is captured\n", getpid(), sig_name);

    if (sig == SIGINT)
        exit(0);
}

int main(int argc, char **argv) {
    printf("[pid](%d): start\n", getpid());

    struct sigaction sigact;
    memset(&sigact, 0, sizeof(struct sigaction));
    sigemptyset(&sigact.sa_mask);
    sigact.sa_handler = catcher;

    sigaction(SIGUSR1, &sigact, NULL);
    sigaction(SIGUSR2, &sigact, NULL);
    sigaction(SIGINT, &sigact, NULL);
    while (1)
        pause();
}