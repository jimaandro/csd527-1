#include <stdio.h>
#include "thread.h"
#include "sem.h"

#define NUM_THREADS 5

Sem_T sem;  // Define the semaphore

void *thread_func(void *arg) {
    int id = *(int *)arg;
    
    Sem_wait(&sem);  // Wait for the semaphore

    // Critical section (simulating access to a shared resource)
    printf("Thread %d entering critical section.\n", id);
    
    // Simulate some work by the thread
    for (volatile int i = 0; i < 1000000; ++i);

    printf("Thread %d exiting critical section.\n", id);

    Sem_signal(&sem);  // Signal the semaphore

    return NULL;
}

int main(void) {
    Thread_init();  // Initialize thread system

    // Initialize the semaphore with a count of 1 (binary semaphore)
    Sem_init(&sem, 1);

    int thread_ids[NUM_THREADS];
    
    // Create and start threads
    for (int i = 0; i < NUM_THREADS; ++i) {
        thread_ids[i] = i + 1;  // Assign thread ID
        Thread_new(thread_func, &thread_ids[i], sizeof(int));  // Create new thread
    }

    // Wait for all threads to finish
    Thread_join(1);
    // Thread_join(5);  // Wait for all threads

    return 0;
}