#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdlib.h>
int iterations;
long spin_count[2] = { 0, 0 };
// Глобальная переменная для блокировки
atomic_int _lock = 0;

// Функция для захвата блокировки
void lock(atomic_int* lock,int thread_id) {
    while (atomic_exchange(lock, 1) == 1) {
        // Активное ожидание (busy-waiting),
        // пока блокировка не освободится
    }
    spin_count[thread_id]++;
}

// Функция для освобождения блокировки
void unlock(atomic_int* lock) {
    atomic_store(lock, 0); // Освобождаем ресурс
}

// Общий ресурс
int counter = 0;
// Функция, которую выполняют потоки
void* thread_function(void* arg) {
    int id = *(int*)arg;
    for (int i = 0; i < iterations; i++) {
        //lock(&_lock);   // Захватываем блокировку
        counter++;      // Работаем с общим ресурсом
        //unlock(&_lock); // Освобождаем блокировку
    }
    return NULL;
}
int main() {
    if (argc != 2) {
        printf("Usage: %s <iterations>\n", argv[0]);
        return 1;
    }
    iterations = atoi(argv[1]);
    pthread_t thread1, thread2;
    // Создаем два потока
    pthread_create(&thread1, NULL, thread_function, NULL);
    pthread_create(&thread2, NULL, thread_function, NULL);
    // Ждем завершения потоков
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    // Выводим результат
    printf("counter = %d\n", counter);
    return 0;
}
