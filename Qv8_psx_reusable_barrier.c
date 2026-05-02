// barrier_assignment.c
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define N 3       // количество потоков
#define ITER 2    // количество шагов

int data[N];       // массив текущих значений
int next_data[N];  // массив для записи фазной суммы
pthread_barrier_t barrier;

void* thread_func(void* arg) {
    int id = *(int*)arg;

    for (int i = 1; i <= ITER; i++) {
        // --- ФАЗА 1: обновление своего элемента ---
        data[id] += id + i;
        printf("Поток %d: шаг %d, data[%d] = %d (фаза 1)\n", id, i, id, data[id]);

        // TODO 1: вставьте первый барьер, чтобы дождаться всех потоков после фазы 1
        pthread_barrier_wait(&barrier);

        // TODO 2: добавьте случайную задержку, имитирующую работу
        usleep((rand() % 2500) * 1000);

        // --- ФАЗА 2: суммирование всех элементов в next_data[id] ---
        int sum = 0;
        for (int j = 0; j < N; j++) {
            // TODO 3: суммируйте элементы массива data[]
            sum += data[j];
        }
        next_data[id] = sum;
        printf("Поток %d: шаг %d, next_data[%d] = %d (фаза 2)\n", id, i, id, sum);

        // TODO 4: вставьте второй барьер, чтобы дождаться всех потоков перед копированием
        pthread_barrier_wait(&barrier);

        // TODO 5: обновите основной массив для следующего шага
        data[id] = next_data[id];
    }

    return NULL;
}

int main() {
    pthread_t threads[N];
    int ids[N];

    for (int i = 0; i < N; i++) data[i] = 0;

    // TODO 6: инициализация барьера для N потоков
    pthread_barrier_init(&barrier, NULL, N);

    // запуск потоков
    for (int i = 0; i < N; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, thread_func, &ids[i]);
    }

    for (int i = 0; i < N; i++)
        pthread_join(threads[i], NULL);

    // TODO 7: уничтожение барьера
    pthread_barrier_destroy(&barrier);

    printf("Итоговое состояние массива: ");
    for (int i = 0; i < N; i++)
        printf("%d ", data[i]);
    printf("\n");

    return 0;
}
