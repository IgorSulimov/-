// barrier_semaphore_assignment.c
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

#define N 3
#define ITER 2

int data[N];
int next_data[N];

// --- переменные барьера ---
int count = 0;
sem_t mutex;
sem_t turnstile;
sem_t turnstile2;

// --- барьер (reusable, 2 turnstile) ---
void barrier_wait() {
    // --- TODO 1: ФАЗА 1 ---
    sem_wait(&mutex);
    count++;
    if (count == N) {
        // TODO 1a: отпустить N потоков через turnstile
        for (int i = 0; i < N; i++) {
            sem_post(&turnstile);
        }
    }
    sem_post(&mutex);

    // TODO 1b: дождаться turnstile
    sem_wait(&turnstile);

    // --- TODO 2: ФАЗА 2 ---
    sem_wait(&mutex);
    count--;
    if (count == 0) {
        // TODO 2a: отпустить N потоков через turnstile2
        for (int i = 0; i < N; i++) {
            sem_post(&turnstile2);
        }
    }
    sem_post(&mutex);

    // TODO 2b: дождаться turnstile2
    sem_wait(&turnstile2);
}

// --- поток ---
void* thread_func(void* arg) {
    int id = *(int*)arg;

    for (int i = 1; i <= ITER; i++) {
        // Фаза 1: каждый поток обновляет свой элемент
        data[id] += id + i;
        printf("Поток %d: шаг %d, data[%d] = %d (фаза 1)\n", id, i, id, data[id]);

        // TODO 3: барьер после фазы 1
        barrier_wait();

        // TODO 4: добавить случайную задержку (имитация работы)
        usleep((rand() % 2500) * 1000);

        // Фаза 2: суммирование всех элементов data[]
        int sum = 0;
        for (int j = 0; j < N; j++) {
            // TODO 5: суммировать элементы массива data[]
            sum += data[j];
        }
        next_data[id] = sum;
        printf("Поток %d: шаг %d, next_data[%d] = %d (фаза 2)\n", id, i, id, sum);

        // TODO 6: барьер перед обновлением основного массива
        barrier_wait();

        // TODO 7: обновить основной массив для следующего шага
        data[id] = next_data[id];
    }

    return NULL;
}

// --- main ---
int main() {
    pthread_t threads[N];
    int ids[N];

    // начальные значения
    for (int i = 0; i < N; i++)
        data[i] = 0;

    // TODO 8: инициализация семафоров
    sem_init(&mutex, 0, 1);
    sem_init(&turnstile, 0, 0);
    sem_init(&turnstile2, 0, 0);

    // запуск потоков
    for (int i = 0; i < N; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, thread_func, &ids[i]);
    }

    // ожидание завершения
    for (int i = 0; i < N; i++)
        pthread_join(threads[i], NULL);

    // результат
    printf("Итоговое состояние массива: ");
    for (int i = 0; i < N; i++)
        printf("%d ", data[i]);
    printf("\n");

    // TODO 9: очистка семафоров
    sem_destroy(&mutex);
    sem_destroy(&turnstile);
    sem_destroy(&turnstile2);

    return 0;
}
