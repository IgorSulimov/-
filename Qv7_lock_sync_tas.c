#include <stdio.h>
#include <pthread.h>
// Глобальная переменная для блокировки
volatile int _lock = 0;
volatile int all_in_spin = 0;
// Функция для захвата блокировки
void lock(volatile int *lock) {
    while (__sync_lock_test_and_set(lock, 1)) {
        // Активное ожидание (busy-waiting), 
		// пока блокировка не освободится
    }
}
// Функция для освобождения блокировки
void unlock(volatile int *lock) {
	*lock = 0;// Освобождаем ресурс
}
// Общий ресурс
int counter = 0;
// Функция, которую выполняют потоки
void* thread_function(void* arg) {
    for (int i = 0; i < 1000000000; i++) {
        all_in_spin += lock(&_lock); // Захватываем блокировку
        counter++;      // Работаем с общим ресурсом
        unlock(&_lock); // Освобождаем блокировку
    }
    return NULL;
}
int main(int argc, char*argv[]) {
    pthread_t thread1, thread2;
    // Создаем два потока
    if(argc != 2) {
        printf("Использование: %s <колтчество_инкрементов_на_поток>\n",argv[0]);
        return 1;
    }
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
