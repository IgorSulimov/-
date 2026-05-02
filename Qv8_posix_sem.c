// posix_semaphore_assignment.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_THREADS 5

// TODO 1: объявите POSIX семафор
sem_t sem;

void *thread_func(void *arg) {
    int id = *(int *)arg;

    printf("Thread %d waiting...\n", id);

    // TODO 2: захват семафора (P)
    sem_wait(&sem);

    printf("Thread %d entered critical section!\n", id);

    // TODO 3: имитация работы потока
    sleep(1);

    printf("Thread %d leaving critical section\n", id);

    // TODO 4: освобождение семафора (V)
    sem_post(&sem);

    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    int ids[NUM_THREADS];

    // TODO 5: инициализация семафора
    sem_init(&sem, 0, 2); // 0 — локальный для потоков, 2 — количество ресурсов

    // запуск потоков
    for (int i = 0; i < NUM_THREADS; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, thread_func, &ids[i]);
    }

    // ожидание завершения всех потоков
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // TODO 6: очистка ресурсов семафора
    sem_destroy(&sem);

    return 0;
}
