#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int done = 0; // Флаг условия

// Функция потока-сигнализатора
void* child(void* arg) {
    sleep(2); // Имитация работы

    pthread_mutex_lock(&mutex);
    printf("[поток child] Заблокировал мьютекс, изменяю условие...\n");
    done = 1; // Устанавливаем флаг
    pthread_cond_signal(&cond); // Сигнализируем об изменении условия
    printf("[поток child] Условие изменено, сигнал отправлен.\n");
    pthread_mutex_unlock(&mutex);
    return NULL;
}

int main() {
    pthread_t waiter, signaler;

    // Создаем потоки
    pthread_create(&waiter, NULL, child, NULL);
    
	// ожидание сигнала от child
	pthread_mutex_lock(&mutex);
    printf("[поток main] Заблокировал мьютекс, жду условие...\n");

    while (!done) {
        printf("[поток main] Условие не выполнено, жду...\n");
        pthread_cond_wait(&cond, &mutex); // Ожидаем сигнал
    }

    printf("[поток main] Условие выполнено! Продолжаю работу.\n");
    pthread_mutex_unlock(&mutex);

    // Уничтожаем мьютекс и условную переменную
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    return 0;
}
