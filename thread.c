#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include "thread.h"

#define MAX_THREADS 1024
#define STACK_SIZE  4096


static Thread_T *thread_pool[MAX_THREADS];  // Array of created threads
int thread_state_pool[MAX_THREADS];
static Thread_T *previous_thread = NULL;     // Pointer to previous_thread
static int num_threads = 0;


// External assembly context switch function
extern void _swtch(void *from, void *to);
extern void _thrstart();

// Internal function prototypes
// static void enqueue(Thread_T **queue, Thread_T *t);
// static Thread_T *dequeue(Thread_T **queue);
static void thread_stub(void);


// void thread_start(int (*func)(void *), void *args) {
//     func(args);      // Call the thread function
//     Thread_exit(0);  // Exit the thread when done
// }

// Function to print the thread queue
void print_thread_queue(Thread_T **queue) {
    printf("Thread Queue:\n");
    if (*queue == NULL) {
        printf("Queue is empty.\n");
        return;
    }

    Thread_T *temp = *queue;
    while (temp != NULL) {
        printf("Thread ID: %d\n", temp->id);
        temp = temp->next;
    }
}

// Initialize the thread library
void Thread_init(void) {
    current_thread = malloc(sizeof(Thread_T));
    assert(current_thread);
    current_thread->id = num_threads++;
    current_thread->stack = NULL;
    current_thread->func = NULL;
    current_thread->args = NULL;
    current_thread->return_value = 0;
    current_thread->next = NULL;

    thread_pool[current_thread->id] = current_thread;
    thread_state_pool[current_thread->id] = 1; // this is the running thread (not free) 
}

int Thread_new(int func(void *), void *args, size_t nbytes, ...) {
    Thread_T *new_thread = malloc(sizeof(Thread_T));
    assert(new_thread);

    new_thread->id = num_threads++;
    new_thread->func = func;
    new_thread->args = args;
    new_thread->stack = malloc(STACK_SIZE);  // Allocate memory for the stack

    assert(new_thread->stack);

    void **stack = (void **)((char *)new_thread->stack + STACK_SIZE);
    
    
    *(--stack) = (void *)_thrstart; //16
    *(--stack) = 0;  // Space for %ebp (saved by _swtch)    12
    *(--stack) = (void *)args;  // Space for %edi (saved by _swtch)    8
    *(--stack) = (void *)func;  // Space for %esi (saved by _swtch)    4
    *(--stack) = 0;  // Space for %ebx (saved by _swtch)    0

    new_thread->stack = stack;

    new_thread->next = NULL;
    enqueue(&ready_queue, new_thread);

    thread_pool[new_thread->id] = new_thread;
    thread_state_pool[new_thread->id] = 1;
    return new_thread->id;
}

void Thread_exit(int code) {
    current_thread->return_value = code;

    // printf("current_thread return_value: %d\n", current_thread->return_value);


    if (previous_thread) {
        // free(previous_thread->stack); 
        // printf("current_thread free\n");
        free(previous_thread);
    }
    if (current_thread->id!=0)  // if not the thread 0 (root thread)
    {
        previous_thread = current_thread;
            thread_state_pool[current_thread->id] = 0;
    
    }
    current_thread = dequeue(&ready_queue);
    
    // printf(" exit\n");
    if (current_thread) {
        // if(previous_thread)
        // printf("current_thread %d switch\n", current_thread->id);
        
        _swtch(&previous_thread->stack, &current_thread->stack);  // Switch to the next thread
        
    } else {
        printf("current_thread exit\n");
        // thread_state_pool[current_thread->id] = 0;
        exit(code);  // No more threads to run, exit the process
    }
}

// Return the ID 
int Thread_self(void) {
    return current_thread ? current_thread->id : -1;
}

// Wait for  thread to finish
int Thread_join(int tid) {
    //  printf("Thread %d exiting critical section.\n", tid);
    Thread_T *target = thread_pool[tid];

    assert(target);


    // print_thread_queue(&ready_queue);
// printf("Thread started with return value %d id %d\n", target->return_value, target->id);
bool state = (target->id<MAX_THREADS+1 && target->id >=0 ) ? thread_state_pool[target->id]==1 : false;
    if ( target) {
        // printf("Thread started with state value %b id %d\n", state, target->id);
        Thread_pause();
        // printf("Thread started with state value %b id %d\n", state, target->id);
        state = (target->id<MAX_THREADS+1 && target->id >=0 ) ? thread_state_pool[target->id]==1 : false;
    }
    // printf("Thread started with return value %d id %d\n", target->return_value, target->id);
    return target->return_value;
}

void Thread_pause(void) {
    Thread_T *old_thread = current_thread;
    // print_thread_queue(&ready_queue);

    current_thread = dequeue(&ready_queue);

    //  printf("After dequeue_______\n");
    // print_thread_queue(&ready_queue);

    // printf("current_thread ID: %d\n", current_thread->id);

    if (current_thread) {
        enqueue(&ready_queue, old_thread);  // Re-enqueue the old thread

        //  printf("After enqueue___________\n");
        // print_thread_queue(&ready_queue);

        _swtch(&old_thread->stack, &current_thread->stack);  // Switch to the new thread
        // printf("After swutch\n");
    }
}

void enqueue(Thread_T **queue, Thread_T *t) {
    t->next = NULL;
    if (*queue == NULL) {
        *queue = t;
    } else {
        Thread_T *temp = *queue;
        while (temp->next) {
            temp = temp->next;  // find the end of the queue
        }
        temp->next = t;
    }
}

Thread_T *dequeue(Thread_T **queue) {
    if (*queue == NULL) {
        return NULL;
    } else {
        Thread_T *t = *queue;
        *queue = (*queue)->next;
        return t;
    }
}
