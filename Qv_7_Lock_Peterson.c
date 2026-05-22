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
	};//spin 
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
}
int main(int argc, char* argv[]) {
	pthread_t one_p, two_p;
	if (argc != 2) return 1;
	iterations = atoi(argv[1]);
	int tid[2];
	tid[0] = 0;
	    printf("Введите начальное значение counter: ");
    scanf("%d", &counter);
	pthread_create(&one_p, NULL, increment, &tid[0]);
	tid[1] = 1;
	pthread_create(&two_p, NULL, increment, &tid[1]);
	pthread_join(one_p, NULL);
	pthread_join(two_p, NULL);
	printf("counter = %d \n", counter);
	return 0;
}
