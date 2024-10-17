#include <stdlib.h>
#include <assert.h>
#include "thread.h"

#define MAX_THREADS 1024
#define STACK_SIZE  4096


static Thread_T *thread_pool[MAX_THREADS];  // Array to hold all created threads
int thread_state_pool[MAX_THREADS];
// static Thread_T *previous_thread = NULL;     // Pointer to currently running thread previous_thread
static int num_threads = 0;

// External assembly context switch function
extern void _swtch(void *from, void *to);
extern void _thrstart();

// Internal function prototypes
// static void enqueue(Thread_T **queue, Thread_T *t);
// static Thread_T *dequeue(Thread_T **queue);
static void thread_stub(void);


void thread_start(int (*func)(void *), void *args) {
    func(args);      // Call the thread function
    Thread_exit(0);  // Exit the thread when done
}

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

    // previous_thread = malloc(sizeof(Thread_T));
    // assert(previous_thread);
    // previous_thread->id = 0;
    // previous_thread->stack = NULL;
    // previous_thread->func = NULL;
    // previous_thread->args = NULL;
    // previous_thread->return_value = 0;
    // previous_thread->next = NULL;

    thread_pool[current_thread->id] = current_thread;
    thread_state_pool[current_thread->id] = 0; // this is the running thread (not free) 
}

// Create a new thread and add it to the ready queue
int Thread_new(int func(void *), void *args, size_t nbytes, ...) {
    Thread_T *new_thread = malloc(sizeof(Thread_T));
    assert(new_thread);

    new_thread->id = num_threads++;
    new_thread->func = func;
    new_thread->args = args;
    new_thread->stack = malloc(STACK_SIZE);  // Allocate memory for the stack

    assert(new_thread->stack);

    // Set up the stack pointer and initialize the thread context
    void **stack = (void **)((char *)new_thread->stack + STACK_SIZE);
    
    // // Reserve space for the general-purpose registers that will be saved by _swtch
    
    *(--stack) = (void *)_thrstart; //16
    *(--stack) = 3;  // Space for %ebp (saved by _swtch)    12
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

// Exit the current thread
void Thread_exit(int code) {
    current_thread->return_value = code;

    // printf("current_thread return_value: %d\n", current_thread->return_value);
Thread_T *previous_thread = current_thread;

    if (previous_thread) {
        // free(previous_thread->stack);  // Free allocated stack space
        // printf("current_thread free\n");
        free(previous_thread);
    }
    previous_thread = current_thread;

    // Switch to the next available thread
    current_thread = dequeue(&ready_queue);
    // printf(" exit\n");
    if (current_thread) {
        // printf("current_thread switch\n");
        thread_state_pool[current_thread->id] = 0;
        _swtch(&previous_thread->stack, &current_thread->stack);  // Switch to the next thread
        
    } else {
        printf("current_thread exit\n");
        thread_state_pool[current_thread->id] = 0;
        exit(0);  // No more threads to run, exit the process
    }
}

// Return the ID of the currently running thread
int Thread_self(void) {
    return current_thread ? current_thread->id : -1;
}

// Wait for the specified thread to finish
int Thread_join(int tid) {
    Thread_T *target = thread_pool[tid];
    assert(target);
    // print_thread_queue(&ready_queue);
// printf("Thread started with return value %d id %d\n", target->return_value, target->id);
int state = target->id<MAX_THREADS+1 ? thread_state_pool[target->id]==1 : 0;
    while ( state && target->stack) {
        Thread_pause();
        state = target->id<MAX_THREADS+1 ? thread_state_pool[target->id]==1 : 0;
        printf("Thread started with return value %d id %d\n", target->return_value);
    }
    // printf("Thread started with return value %d id %d\n", target->return_value, target->id);
    return target->return_value;
}

// Voluntarily yield the CPU to another thread
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

// Enqueue a thread in the queue
void enqueue(Thread_T **queue, Thread_T *t) {
    t->next = NULL;
    if (*queue == NULL) {
        *queue = t;
    } else {
        Thread_T *temp = *queue;
        while (temp->next) {
            temp = temp->next;
        }
        temp->next = t;
    }
}

// Dequeue a thread from the queue
Thread_T *dequeue(Thread_T **queue) {
    if (*queue == NULL) {
        return NULL;
    } else {
        Thread_T *t = *queue;
        *queue = (*queue)->next;
        return t;
    }
}

// // A function to be used as a stub for new threads
// static void thread_stub(void) {
//     Thread_exit(current_thread->func(current_thread->args));
// }
