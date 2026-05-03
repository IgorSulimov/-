#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define BUFFER_SIZE 5
#define MAX_ITEMS 20

int buffer[BUFFER_SIZE];
int count = 0;      // Текущее количество элементов в буфере
int fill_ptr = 0;   // Индекс для добавления элементов
int use_ptr = 0;    // Индекс для извлечения элементов

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t fill = PTHREAD_COND_INITIALIZER;

void put(int value) {
	buffer[fill_ptr] = value;
	fill_ptr = (fill_ptr + 1) % BUFFER_SIZE;
	count++;
}

int get() {
	 int tmp = buffer[use_ptr];
	 use_ptr = (use_ptr + 1) % BUFFER_SIZE;
	 count--;
	 return tmp;
}

void *producer(void *arg) {
    for (int i = 0; i < MAX_ITEMS; i++) {

        pthread_mutex_lock(&mutex);
        // Ждем, если буфер полон
        while (count == BUFFER_SIZE) {
            printf("Производитель ждет - буфер полон\n");
            pthread_cond_wait(&empty, &mutex);
        }
        // Производим элемент
        int item = rand() % 100;
		printf("Производитель добавил %d в позицию %d\n", item, fill_ptr);
		put(item);
        // Сигнализируем потребителю
        pthread_cond_signal(&fill);
        pthread_mutex_unlock(&mutex);
        
        sleep(rand() % 2);  // Имитируем работу
    }
    return NULL;
}

void *consumer(void *arg) {
    for (int i = 0; i < MAX_ITEMS; i++) {

        pthread_mutex_lock(&mutex);
        // Ждем, если буфер пуст
        while (count == 0) {
            printf("Потребитель ждет - буфер пуст\n");
            pthread_cond_wait(&fill, &mutex);
        }
        // Извлекаем элемент
        int item = get();
        printf("Потребитель извлек %d\n", item);
        // Сигнализируем производителю
        pthread_cond_signal(&empty);
        pthread_mutex_unlock(&mutex);
        
        sleep(rand() % 3);  // Имитируем работу
    }
    return NULL;
}

int main() {
    pthread_t producer_thread, consumer_thread;
    
    // Создаем потоки
    pthread_create(&producer_thread, NULL, producer, NULL);
    pthread_create(&consumer_thread, NULL, consumer, NULL);
    
    // Ждем завершения потоков
    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);
    
    // Уничтожаем мьютекс и условные переменные
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&empty);
    pthread_cond_destroy(&fill);
    
    return 0;
}
