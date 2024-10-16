#include <stdio.h>
#include "thread.h"
#include "sem.h"


int thread_function(void *arg) {
    int tid = Thread_self();
    printf("Thread %d is running with argument: %d\n", tid, *(int *)arg);
    return tid;
}

int main() {
    Thread_init();  

    int args[3] = {1, 2, 3};
    
    
    int tid1 = Thread_new(thread_function, &args[0], sizeof(int));
    printf("Thread %d started with return value\n", tid1);
    int tid2 = Thread_new(thread_function, &args[1], sizeof(int));
    int tid3 = Thread_new(thread_function, &args[2], sizeof(int));

    // 
    int ret1 = Thread_join(tid1);
    int ret2 = Thread_join(tid2);
    int ret3 = Thread_join(tid3);

    printf("Thread %d finished with return value %d\n", tid1, ret1);
    printf("Thread %d finished with return value %d\n", tid2, ret2);
    printf("Thread %d finished with return value %d\n", tid3, ret3);

    return 0;
}