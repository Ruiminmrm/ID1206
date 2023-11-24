#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define MAX_INCREMENTS 15

/*In the first part of the assignment, you will write a program that creates three
threads. These threads access a shared integer, buffer, one at a time. The
buffer will initially be set to 0. Each thread should print its thread ID, process
ID, and the buffer’s current value in one statement, then increment the buffer
by one. Use a mutex to ensure this whole process is not interrupted. Have the
threads modify the buffer 15 times. When each thread is done, it should
return the number of times it changed the buffer to the main thread. */


int counter = 0;
pthread_mutex_t mutex;

void* run(void *arg) {
    int* times = malloc(sizeof(int));
    if (times == NULL) {
        perror("Failed to allocate memory");
        pthread_exit(NULL);
    }
    *times = 0;

    while (1) {
        pthread_mutex_lock(&mutex);
        if (counter >= MAX_INCREMENTS) {
            pthread_mutex_unlock(&mutex);
            break;
        }
        (*times)++;
        counter++;
        printf("TID: %lu, PID: %d, Buffer: %d\n", pthread_self(), getpid(), counter);
        pthread_mutex_unlock(&mutex);
    }

    pthread_exit(times);
}

int main() {
    pthread_mutex_init(&mutex, NULL);

    pthread_t threads[3];
    int sum = 0;

    for (int i = 0; i < 3; i++) {
        if (pthread_create(&threads[i], NULL, run, NULL) != 0) {
            perror("Failed to create thread");
            return 1;
        }
    }

    for (int i = 0; i < 3; i++) {
        int* retval;
        if (pthread_join(threads[i], (void**)&retval) != 0) {
            perror("Failed to join thread");
            continue;
        }
        sum += *retval;
        printf("Thread %d worked on the buffer %d times\n", i+1, *retval);
        free(retval);
    }

    printf("Total buffer accesses: %d\n", sum);
    pthread_mutex_destroy(&mutex);

    return 0;
}

