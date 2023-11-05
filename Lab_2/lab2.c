#include <pthread.h> 
#include <stdlib.h> 
#include <stdio.h>    
#include <unistd.h>   

#define MAX_INCREMENTS 15  // Define a constant for the maximum counter increments

int counter = 0;           
pthread_mutex_t mutex;     // Declare a mutex for synchronizing access to the counter

// Thread function that increments the counter
void* run(void *arg) {
    // Allocate memory to keep track of how many times the thread has incremented the counter
    int* times = malloc(sizeof(int));
    if (times == NULL) {  // Check for successful memory allocation
        perror("Failed to allocate memory"); 
        pthread_exit(NULL); 
    }
    *times = 0;  // Initialize the number of increments to 0

    // Loop infinitely
    while (1) {
        pthread_mutex_lock(&mutex); 

        // Check if the counter has reached the maximum number of increments
        if (counter >= MAX_INCREMENTS) {
            pthread_mutex_unlock(&mutex); 
            break; 
        }

        // Increment the times counter and the global counter
        (*times)++;
        counter++;
      
        printf("TID: %lu, PID: %d, Buffer: %d\n", pthread_self(), getpid(), counter);

        pthread_mutex_unlock(&mutex); // Unlock the mutex
    }

   
    pthread_exit(times);
}

int main() {
    pthread_mutex_init(&mutex, NULL); 

    pthread_t threads[3]; 
    int sum = 0;         

    // Create 3 threads
    for (int i = 0; i < 3; i++) {
        if (pthread_create(&threads[i], NULL, run, NULL) != 0) {
            perror("Failed to create thread"); 
            return 1; 
        }
    }

    // Join the threads and sum their increments
    for (int i = 0; i < 3; i++) {
        int* retval; // Pointer to receive the return value from each thread

        if (pthread_join(threads[i], (void**)&retval) != 0) {
            perror("Failed to join thread"); 
            continue; 
        }

        sum += *retval; // Add the number of increments by the thread to the sum
        
        printf("Thread %d worked on the buffer %d times\n", i+1, *retval);
        free(retval); // Free the memory allocated for the return value
    }


    printf("Total buffer accesses: %d\n", sum);
    pthread_mutex_destroy(&mutex); 

    return 0; 
}

