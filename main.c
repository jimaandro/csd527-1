// #include <stdio.h>
// #include "thread.h"
// #include "sem.h"

// 
// int thread_function(void *arg) {
//     int tid = Thread_self();
//     printf("Thread %d is running with argument: %d\n", tid, *(int *)arg);
//     return tid;
// }

// int main() {
//     Thread_init();  

//     int args[3] = {1, 2, 3};
    
//     
//     int tid1 = Thread_new(thread_function, &args[0], sizeof(int));
//     printf("Thread %d started with return value\n", tid1);
//     int tid2 = Thread_new(thread_function, &args[1], sizeof(int));
//     int tid3 = Thread_new(thread_function, &args[2], sizeof(int));

//     // 
//     int ret1 = Thread_join(tid1);
//     int ret2 = Thread_join(tid2);
//     int ret3 = Thread_join(tid3);

//     printf("Thread %d finished with return value %d\n", tid1, ret1);
//     printf("Thread %d finished with return value %d\n", tid2, ret2);
//     printf("Thread %d finished with return value %d\n", tid3, ret3);

//     return 0;
// }


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