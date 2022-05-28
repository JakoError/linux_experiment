#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int conc_amnt;

void pa_handle_sigint(int sig) {
    printf("[pid](%d): SIGINT is handled in pa_handle_sigint()\n", getpid());
    printf("[pid](%d): [func](exit) with [conc_amnt](%d)\n", getpid(), conc_amnt);
    exit(0);
}

void pa_handle_sigchld(int sig) {
    printf("[pid](%d): SIGCHLD is handled in pa_handle_sigchld()\n", getpid());
    int pid;
    while ((pid = waitpid(-1, NULL, WNOHANG)) > 0) {
        printf("[pid](%d): [cpid](%d) is waited in pa_handle_sigchld()\n", getpid(), pid);
        conc_amnt--;
    }
}

int main(int argc, char **argv) {
    int _conc_amnt;
    if (argc == 3 && strcmp(argv[1], "-c") == 0)
        _conc_amnt = conc_amnt = (int) strtol(argv[2], (char **) NULL, 10);
    else
        return 0;

    printf("[pid](%d): start\n", getpid());

    struct sigaction sigact_int, sigact_chld;
    memset(&sigact_int, 0, sizeof(struct sigaction));
    memset(&sigact_chld, 0, sizeof(struct sigaction));
    sigemptyset(&sigact_int.sa_mask);
    sigemptyset(&sigact_chld.sa_mask);
    sigact_int.sa_handler = pa_handle_sigint;
    sigact_chld.sa_handler = pa_handle_sigchld;

    sigaction(SIGINT, &sigact_int, NULL);

    pid_t pid = 0;
    pid_t *pids = (pid_t *) malloc(sizeof(pid_t) * conc_amnt);
    for (int i = 0; i < _conc_amnt - 1; ++i) {
        pid = fork();
        if (pid == 0)
            break;
        else
            pids[i] = pid;
    }

    if (pid == 0) {
        //child
        printf("[pid](%d): start\n", getpid());
        pause();

    } else {
        //parent
        sigaction(SIGCHLD, &sigact_chld, NULL);
        sleep(1);
        for (int i = 0; i < _conc_amnt - 1; ++i) {
            kill(pids[i], SIGINT);
        }
        while (conc_amnt != 1) {
            printf("[pid](%d): [func](pause) with [conc_amnt](%d)\n", getpid(), conc_amnt);
            pause();
        }
        pause();
    }


    return 0;
}