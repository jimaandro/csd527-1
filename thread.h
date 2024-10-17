
#ifndef THREAD_INCLUDED
#define THREAD_INCLUDED

#include <stddef.h>

typedef struct Thread_T {
    int id;                    // Thread ID
    void *stack;               // Stack pointer
    int (*func)(void *);       // Function to be executed by the thread
    void *args;                // Arguments passed to the function
    int return_value;          // Return value after thread execution
    struct Thread_T *next;     // Pointer to the next thread (for linked list)
} Thread_T;

static Thread_T *current_thread = NULL;     // Pointer to currently running thread 

static Thread_T *ready_queue = NULL;        // Queue of threads ready to run

extern void Thread_init(void);
extern int Thread_new(int func(void *), void *args, size_t nbytes, ...);
extern void Thread_exit(int code);
extern int Thread_self(void);
extern int Thread_join(int tid);
extern void Thread_pause(void);

extern void enqueue(Thread_T **queue, Thread_T *t);
extern Thread_T *dequeue(Thread_T **queue);

#endif
