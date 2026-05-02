#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <linux/futex.h>
#include <sys/time.h>
#include <pthread.h>

typedef struct {
    atomic_int value;
} sem_t;

// --- Обёртки для futex ---
int futex_wait(atomic_int *addr, int expected) {
    // TODO 1: вызвать системный вызов futex с FUTEX_WAIT
    return syscall(SYS_futex, addr, FUTEX_WAIT, expected, NULL, NULL, 0);
}

int futex_wake(atomic_int *addr, int n) {
    // TODO 2: вызвать системный вызов futex с FUTEX_WAKE
    return syscall(SYS_futex, addr, FUTEX_WAKE, n, NULL, NULL, 0);
}

// --- Инициализация ---
void sem_init(sem_t *s, int init_val) {
    // TODO 3: инициализировать atomic_int
    atomic_init(&s->value, init_val);
}

// --- sem_wait (P) с отрицательными значениями ---
void sem_wait(sem_t *s) {
    // TODO 4: уменьшить значение на 1 атомарно
    int val = atomic_fetch_sub(&s->value, 1);
    
    // TODO 5: если val <= 0, дождаться через futex_wait
    if (val <= 0) {
        futex_wait(&s->value, val - 1);
    }
}

// --- sem_post (V) с отрицательными значениями ---
void sem_post(sem_t *s) {
    // TODO 6: увеличить значение на 1 атомарно
    int val = atomic_fetch_add(&s->value, 1);
    
    // TODO 7: если val < 0, пробудить один поток через futex_wake
    if (val < 0) {
        futex_wake(&s->value, 1);
    }
}

// ----------- Тест семафора ------------
#define NUM_THREADS 5

sem_t sem;

void *thread_func(void *arg) {
    int id = *(int *)arg;
    
    printf("Thread %d waiting...\n", id);
    
    // TODO 8: захват семафора
    sem_wait(&sem);
    
    printf("Thread %d entered critical section!\n", id);
    
    // TODO 9: имитация работы
    sleep(1);
    
    printf("Thread %d leaving critical section\n", id);
    
    // TODO 10: освобождение семафора
    sem_post(&sem);
    
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    int ids[NUM_THREADS];
    
    // TODO 11: инициализация семафора с количеством ресурсов 2
    sem_init(&sem, 2);
    
    for (int i = 0; i < NUM_THREADS; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, thread_func, &ids[i]);
    }
    
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    return 0;
}
