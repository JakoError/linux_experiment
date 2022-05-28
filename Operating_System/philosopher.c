#include <stdio.h>
#include <stdlib.h>
#include<sys/sem.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

int table[5];
int count[5];//count eat

struct sembuf p;
struct sembuf v;

void *eat(void *index) {
    //pthread_detach(pthread_self());

    int i = *(int *) index;
    while (count[i] > 0) {
        if (i % 2) {
            semop(table[i], &p, 1);
            semop(table[(i + 1) % 5], &p, 1);
            printf("%d philosopher eat!%d\n", i, count[i]);
            sleep(1);
            //semop(table[(i + 1) % 5], &v, 1);
            //semop(table[i], &v, 1);
        } else {
            semop(table[(i + 1) % 5], &p, 1);
            semop(table[i], &p, 1);
            printf("%d philosopher eat!%d\n", i, count[i]);
            sleep(1);
            //semop(table[i], &v, 1);
            //semop(table[(i + 1) % 5], &v, 1);
        }
        count[i]--;
    }
}

int main(int argc, char **argv) {

    pthread_t *philosophers = (pthread_t *) malloc(sizeof(pthread_t) * 5);

    p.sem_num = v.sem_num = 0;
    p.sem_op = -1;
    v.sem_op = 1;
    p.sem_flg = v.sem_flg = SEM_UNDO;

    union semun sem_args;
    unsigned short array[1] = {1};
    sem_args.array = array;

    for (int i = 0; i < 5; ++i) {
        table[i] = semget(0, 1, IPC_CREAT);
        semctl(table[i], 0, SETVAL, sem_args);
        printf("%d\n", table[i]);
    }

    int label[5] = {1, 2, 3, 4, 5};
    for (int i = 0; i < 5; ++i) {
        count[i] = 20;
        pthread_create(&philosophers[i], NULL, eat, &label[i]);
    }
    for (int i = 0; i < 5; ++i) {
        pthread_join(philosophers[i], NULL);
    }
    for (int i = 0; i < 5; ++i) {
        semctl(table[i], 0, IPC_RMID);
    }

    return 0;
}