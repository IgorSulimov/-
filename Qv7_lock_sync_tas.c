#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <linux/futex.h>
#include <sys/syscall.h>
#include <stdlib.h>

volatile int _lock = 0;
long long all_in_spin = 0;
int iterations;
long long counter = 0;

int futex_wait(volatile int *futexp){
    return syscall(SYS_futex, futexp, FUTEX_WAIT, 1, NULL, NULL, 0);
}

void futex_wake(volatile int *futexp){
    syscall(SYS_futex, futexp, FUTEX_WAKE, 1, NULL, NULL, 0);
}

int lock(volatile int *lock) {
    int spin = 0;
    while (__sync_lock_test_and_set(lock, 1)) {
        spin++;
        futex_wait(lock);
    }
    return spin;
}

void unlock(volatile int *lock) {
    *lock = 0;
    futex_wake(lock);
}

void* thread_function(void* arg) {
    long long local_spin_sum = 0;
    for (int i = 0; i < iterations; i++) {
        local_spin_sum += lock(&_lock);
        counter++;
        unlock(&_lock);
    }
    __sync_fetch_and_add(&all_in_spin, local_spin_sum);
    return NULL;
}

int main(int argc, char* argv[]) {
    pthread_t thread1, thread2;
    
    if (argc != 2) return 1;
    iterations = atoi(argv[1]);
    
    pthread_create(&thread1, NULL, thread_function, NULL);
    pthread_create(&thread2, NULL, thread_function, NULL);
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    
    printf("counter = %lld\n", counter);
    printf("all in spin = %lld \n", all_in_spin);
    return 0;
}
