#include <stdio.h>
#include "thread.h"
#include "sem.h"

#define NUM_THREADS 5

Sem_T sem;  

void *thread_func(void *arg) {
    int id = *(int *)arg;
    
    Sem_wait(&sem);  

    printf("Thread %d entering critical section.\n", id);
    
    for (volatile int i = 0; i < 1000000; ++i);

    printf("Thread %d exiting critical section.\n", id);

    Sem_signal(&sem);  

    return NULL;
}

int main(void) {
    Thread_init(); 
    Sem_init(&sem, 1);

    int thread_ids[NUM_THREADS];
   
    for (int i = 0; i < NUM_THREADS; ++i) {
        thread_ids[i] = i + 1;  
        Thread_new(thread_func, &thread_ids[i], sizeof(int));  
    }

    Thread_join(0);
    // Thread_join(5);  

    return 0;
}