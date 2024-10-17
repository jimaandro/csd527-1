#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include "thread.h"
#include "sem.h"


#define PREEMPT_INTERVAL 100000 // 100 ms


Sem_T semaphore;


void preempt_handler(int signum) {
    Thread_pause();  
}


void setup_preemption() {
    struct sigaction sa;
    struct itimerval timer;


    sa.sa_handler = &preempt_handler;
    sigaction(SIGALRM, &sa, NULL);


    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = PREEMPT_INTERVAL;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = PREEMPT_INTERVAL;
    
    setitimer(ITIMER_REAL, &timer, NULL);
}


void *thread_func1(void *arg) {
    printf("Thread 1 waiting for semaphore...\n");
    Sem_wait(&semaphore);  // Wait on semaphore
    printf("Thread 1 acquired semaphore!\n");
    Thread_exit(0);        // Exit thread
    return NULL;
}


void *thread_func2(void *arg) {
    printf("Thread 2 waiting for semaphore...\n");
    Sem_wait(&semaphore);  // Wait on semaphore
    printf("Thread 2 acquired semaphore!\n");
    Thread_exit(0);        // Exit thread
    return NULL;
}


int main(void) {
    Thread_init(); 
    Sem_init(&semaphore, 1);  

    setup_preemption(); 

    Thread_new(thread_func1, NULL, 0);  
    Thread_new(thread_func2, NULL, 0);  


    for (int i = 0; i < 5; ++i) {
        printf("Main thread signaling semaphore in 1 second...\n");
        sleep(1);
        Sem_signal(&semaphore);
    }

    Thread_join(2); 
    return 0;
}
