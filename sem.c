#include "sem.h"
#include "thread.h"
#include <stdlib.h>
#include <assert.h>

void Sem_init(Sem_T *s, int count) {
    s->count = count;
    s->queue = NULL;
}

void Sem_wait(Sem_T *s) {
    while (s->count < 0) {
        // Block the current thread and add to the semaphore's wait queue
        enqueue((Thread_T **)&s->queue, current_thread);
        Thread_pause();
    }
     s->count--;
}

void Sem_signal(Sem_T *s) {
    s->count++;
    if (s->count <= 0) {
        // Unblock a thread from the semaphore's wait queue
        Thread_T *t = dequeue((Thread_T **)&s->queue);
        if (t) {    //check not empty queue
            enqueue(&ready_queue, t);
        }
    }
}
