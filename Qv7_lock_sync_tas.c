#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <linux/futex.h>
#include <sys/syscall.h>
int spin_count[2] = { 0, 0 };
// Глобальная переменная для блокировки
volatile int _lock = 0;
volatile int all_in_spin = 0;
int iterations;

int futex_wait(volatile int* futexp) {
    return syscall(SYS_futex, futexp, FUTEX_WAIT, 1, NULL, NULL, 0);
}

void futex_wake(volatile int* futexp) {
    syscall(SYS_futex, futexp, FUTEX_WAKE, 1, NULL, NULL, 0);
}
// Функция для захвата блокировки
int lock(volatile int* lock,int thread_id) {
    int spin = 0;
    while (__sync_lock_test_and_set(lock, 1)) {
        // Активное ожидание (busy-waiting), 
        // пока блокировка не освободится
        spin++;
        futex_wait(lock);
    }
    spin_count[thread_id] += spin;
    return spin;
}
// Функция для освобождения блокировки
void unlock(volatile int* lock) {
    *lock = 0;// Освобождаем ресурс
    futex_wake(lock);
}
// Общий ресурс
int counter = 0;
// Функция, которую выполняют потоки
void* thread_function(void* arg) {
    for (int i = 0; i < iterations; i++) {
        int id = *(int*)arg;
        all_in_spin += lock(&_lock,id); // Захватываем блокировку
        counter++;      // Работаем с общим ресурсом
        unlock(&_lock); // Освобождаем блокировку
    }
    return NULL;
}
int main(int argc, char* argv[]) {
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
    printf("all in spin = %d \n", all_in_spin);
    return 0;
}
