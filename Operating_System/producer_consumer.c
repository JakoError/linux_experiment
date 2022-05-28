#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>

const int N = 10;
const int PRODUCER_NUM = 3;
const int CONSUMER_NUM = 4;
const int MAX_ROUND = 3000;
const int MAX_RES_SIZE = 40;

void **resource;
int start, end;

sem_t empty, full, mutex;

void *consume(void *arg) {
    int label = *(int *) arg;
    for (int i = 0; i < MAX_ROUND; ++i) {
        sem_wait(&full);
        sem_wait(&mutex);

        start %= N;
        void *res = resource[start++];

        sem_post(&mutex);
        sem_post(&empty);

        usleep(100);
        printf("consume: [%s] by Con:%d\n", (char *) res, label);
        free(res);

    }
}

void *produce(void *arg) {

    int label = *(int *) arg;
    for (int i = 0; i < MAX_ROUND; ++i) {
        char *res = (char *) malloc(sizeof(char) * MAX_RES_SIZE);
        usleep(100);
        sprintf(res, "Res.%d by Pro.%d", rand(), label);
        sem_wait(&empty);
        sem_wait(&mutex);

        end %= N;
        resource[end++] = res;
        printf("produce: %s\n", res);

        sem_post(&mutex);
        sem_post(&full);
    }
}

int max(const int a, const int b) {
    return a > b ? a : b;
}

int main(int argc, char **argv) {
    srand(time(NULL));

    sem_init(&empty, 0, N);
    sem_init(&full, 0, 0);
    sem_init(&mutex, 0, 1);

    resource = (void **) malloc(sizeof(void *) * N);
    start = end = 0;

    pthread_t *producers = (pthread_t *) malloc(sizeof(pthread_t) * PRODUCER_NUM);
    pthread_t *consumers = (pthread_t *) malloc(sizeof(pthread_t) * CONSUMER_NUM);

    int label_length = max(PRODUCER_NUM, CONSUMER_NUM);
    int *label = (int *) malloc(sizeof(int) * label_length);
    for (int i = 0; i < label_length; ++i) {
        label[i] = i + 1;
    }

    for (int i = 0; i < CONSUMER_NUM; ++i) {
        pthread_create(&consumers[i], NULL, consume, &label[i]);
    }
    for (int i = 0; i < PRODUCER_NUM; ++i) {
        pthread_create(&producers[i], NULL, produce, &label[i]);
    }
    for (int i = 0; i < PRODUCER_NUM; ++i) {
        pthread_join(producers[i], NULL);
    }

    while (end != start) {
        usleep(100);
    }

    for (int i = 0; i < CONSUMER_NUM; ++i) {
        pthread_cancel(consumers[i]);
    }
    sem_destroy(&empty);
    sem_destroy(&full);
    sem_destroy(&mutex);

    printf("\n---finish---\n");
    return 0;
}