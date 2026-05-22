#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

volatile int counter;
volatile int request[2] = { 0, 0 };
volatile int turn = 0;
int iterations;

void lock(int id) {
    request[id] = 1;
    int other = 1 - id;
    turn = other;
    while (request[other] == 1 && turn == other) {
        // spin
    }
}

void unlock(int id) {
    request[id] = 0;
}

void* increment(void* arg) {
    int id = *(int*)arg;
    for (int i = 0; i < iterations; i++) {
        lock(id);
        counter++;
        unlock(id);
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    pthread_t one_p, two_p;
    
    // Проверка аргументов командной строки
    if (argc != 2) {
        printf("Использование: %s <количество_инкрементов_на_поток>\n", argv[0]);
        return 1;
    }
    
    iterations = atoi(argv[1]);
    
    printf("Введите начальное значение counter: ");
    scanf("%d", &counter);
    
    int tid[2];
    tid[0] = 0;
    pthread_create(&one_p, NULL, increment, &tid[0]);
    
    tid[1] = 1;
    pthread_create(&two_p, NULL, increment, &tid[1]);
    
    pthread_join(one_p, NULL);
    pthread_join(two_p, NULL);
    
    printf("counter = %d\n", counter);
    return 0;
}
