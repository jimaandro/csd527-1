#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include "thread.h"
#include "sem.h"

// Preemption interval in microseconds
#define PREEMPT_INTERVAL 100000 // 100 ms

// Semaphore and threads example
Sem_T semaphore;

// Preemption handler function
void preempt_handler(int signum) {
    Thread_pause();  // Switch to the next thread
}

// Setup the timer for preemption
void setup_preemption() {
    struct sigaction sa;
    struct itimerval timer;

    // Install preemption signal handler for SIGALRM
    sa.sa_handler = &preempt_handler;
    sigaction(SIGALRM, &sa, NULL);

    // Set up timer to trigger every PREEMPT_INTERVAL microseconds
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = PREEMPT_INTERVAL;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = PREEMPT_INTERVAL;
    
    setitimer(ITIMER_REAL, &timer, NULL);
}

// Thread function 1
void *thread_func1(void *arg) {
    printf("Thread 1 waiting for semaphore...\n");
    Sem_wait(&semaphore);  // Wait on semaphore
    printf("Thread 1 acquired semaphore!\n");
    Thread_exit(0);        // Exit thread
    return NULL;
}

// Thread function 2
void *thread_func2(void *arg) {
    printf("Thread 2 waiting for semaphore...\n");
    Sem_wait(&semaphore);  // Wait on semaphore
    printf("Thread 2 acquired semaphore!\n");
    Thread_exit(0);        // Exit thread
    return NULL;
}

// Main function
int main(void) {
    Thread_init();  // Initialize thread system
    Sem_init(&semaphore, 1);  // Initialize semaphore with a count of 1

    setup_preemption();  // Set up timer for preemption

    Thread_new(thread_func1, NULL, 0);  // Create thread 1
    Thread_new(thread_func2, NULL, 0);  // Create thread 2

    // Main thread signals the semaphore after some time
    for (int i = 0; i < 5; ++i) {
        printf("Main thread signaling semaphore in 1 second...\n");
        sleep(1);
        Sem_signal(&semaphore);
    }

    Thread_join(0);  // Wait for all threads to finish
    return 0;
}
