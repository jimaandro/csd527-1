
#ifndef THREAD_INCLUDED
#define THREAD_INCLUDED

#include <stddef.h>

typedef struct Thread_T {
    int id;                    
    void *stack;               
    int (*func)(void *);       
    void *args;                
    int return_value;          
    struct Thread_T *next;     
} Thread_T;

static Thread_T *current_thread = NULL;     


static Thread_T *ready_queue = NULL;        

extern void Thread_init(void);
extern int Thread_new(int func(void *), void *args, size_t nbytes, ...);
extern void Thread_exit(int code);
extern int Thread_self(void);
extern int Thread_join(int tid);
extern void Thread_pause(void);

extern void enqueue(Thread_T **queue, Thread_T *t);
extern Thread_T *dequeue(Thread_T **queue);

#endif
