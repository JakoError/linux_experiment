#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>

const int MAX_COUNT = 300;
sem_t table[5];

void *eat(void *index) {
    //pthread_detach(pthread_self());
    int i = *(int *) index;
    int count = MAX_COUNT;
    printf("%d philosopher start!%d\n", i + 1, count);
    while (count > 0) {
        if (i % 2) {
            sem_wait(&table[i]);
            sem_wait(&table[(i + 1) % 5]);
            printf("%d philosopher eat!%d\n", i + 1, count);
            usleep(100);
            sem_post(&table[(i + 1) % 5]);
            sem_post(&table[i]);
        } else {
            sem_wait(&table[(i + 1) % 5]);
            sem_wait(&table[i]);
            printf("%d philosopher eat!%d\n", i + 1, count);
            usleep(100);
            sem_post(&table[i]);
            sem_post(&table[(i + 1) % 5]);
        }
        count--;
        printf("%d philosopher think!\n", i + 1);
        usleep(100);
    }
    return NULL;
}

int main(int argc, char **argv) {

    pthread_t *philosophers = (pthread_t *) malloc(sizeof(pthread_t) * 5);

    for (int i = 0; i < 5; ++i) {
        sem_init(&table[i], 0, 1);
    }

    int label[5] = {0, 1, 2, 3, 4};
    for (int i = 0; i < 5; ++i) {
        pthread_create(&philosophers[i], NULL, eat, &label[i]);
    }
    for (int i = 0; i < 5; ++i) {
        pthread_join(philosophers[i], NULL);
    }
    for (int i = 0; i < 5; ++i) {
        sem_destroy(&table[i]);
    }
    printf("\n---finish---\n");
    return 0;
}