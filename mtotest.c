#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ucontext.h>
#include <stdint.h>
#include <string.h>

#define STACK_SIZE (1024 * 1024) // 1MB stack per thread
#define MAX_THREADS 64

// Thread states
enum thread_state {
    THREAD_RUNNING,
    THREAD_READY,
    THREAD_WAITING,
    THREAD_TERMINATED
};

// Thread Control Block (TCB)
typedef struct {
    int tid;                    // Thread ID
    void *stack;                // Stack pointer
    ucontext_t context;         // Execution context
    enum thread_state state;    // Thread state
    void *(*start_routine)(void *); // Thread function
    void *arg;                  // Thread argument
    void *retval;               // Thread return value
} thread_t;

// Global variables
static thread_t threads[MAX_THREADS];
static int current_thread = -1;
static int num_threads = 0;
static int scheduler_running = 0;

// Timer setup for preemption
static struct itimerval timer;

// Function prototypes
static void scheduler(int signum);
static void thread_exit(void *retval);
static void thread_start_wrapper();

// Initialize the threading system
void init_threading() {
    // Setup timer for round-robin scheduling
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = &scheduler;
    sigaction(SIGALRM, &sa, NULL);
    
    // Set up timer to fire every 10ms (quantum)
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 10000; // 10ms
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 10000; // 10ms
    
    // Initialize main thread
    threads[0].tid = 0;
    threads[0].state = THREAD_RUNNING;
    threads[0].stack = NULL; // Main thread uses system stack
    num_threads = 1;
    current_thread = 0;
}

// Create a new thread
int thread_create(void *(*start_routine)(void *), void *arg) {
    if (num_threads >= MAX_THREADS) {
        return -1;
    }
    
    int tid = num_threads++;
    thread_t *t = &threads[tid];
    
    // Allocate stack
    t->stack = malloc(STACK_SIZE);
    if (!t->stack) {
        return -1;
    }
    
    // Initialize context
    if (getcontext(&t->context) == -1) {
        free(t->stack);
        return -1;
    }
    
    // Set up context
    t->context.uc_stack.ss_sp = t->stack;
    t->context.uc_stack.ss_size = STACK_SIZE;
    t->context.uc_link = &threads[0].context; // Link to main thread
    
    // Set up the thread function and arguments
    t->start_routine = start_routine;
    t->arg = arg;
    t->tid = tid;
    t->state = THREAD_READY;
    
    // Make the context point to our wrapper function
    makecontext(&t->context, (void (*)(void))thread_start_wrapper, 0);
    
    return tid;
}

// Wrapper function that calls the thread's start routine
static void thread_start_wrapper() {
    thread_t *t = &threads[current_thread];
    t->retval = t->start_routine(t->arg);
    thread_exit(t->retval);
}

// Thread exit function
static void thread_exit(void *retval) {
    thread_t *t = &threads[current_thread];
    t->retval = retval;
    t->state = THREAD_TERMINATED;
    
    // Free the stack (except for main thread)
    if (current_thread != 0) {
        free(t->stack);
        t->stack = NULL;
    }
    
    // Schedule another thread
    scheduler(0);
}

// Round-robin scheduler
static void scheduler(int signum) {
    if (!scheduler_running) return;
    
    // Save current context if we're preempting a running thread
    if (current_thread != -1 && threads[current_thread].state == THREAD_RUNNING) {
        if (swapcontext(&threads[current_thread].context, &threads[0].context) == -1) {
            perror("swapcontext");
            exit(EXIT_FAILURE);
        }
        return;
    }
    
    // Find next ready thread
    int next_thread = -1;
    for (int i = 1; i < num_threads; i++) {
        int idx = (current_thread + i) % num_threads;
        if (threads[idx].state == THREAD_READY) {
            next_thread = idx;
            break;
        }
    }
    
    // If no threads are ready, return to main thread
    if (next_thread == -1) {
        if (current_thread != 0) {
            current_thread = 0;
            threads[0].state = THREAD_RUNNING;
            if (swapcontext(&threads[0].context, &threads[0].context) == -1) {
                perror("swapcontext");
                exit(EXIT_FAILURE);
            }
        }
        return;
    }
    
    // Switch to next thread
    threads[current_thread].state = THREAD_READY;
    current_thread = next_thread;
    threads[current_thread].state = THREAD_RUNNING;
    
    if (swapcontext(&threads[0].context, &threads[current_thread].context) == -1) {
        perror("swapcontext");
        exit(EXIT_FAILURE);
    }
}

// Wait for a thread to finish
void thread_join(int tid, void **retval) {
    if (tid <= 0 || tid >= num_threads) {
        return;
    }
    
    while (threads[tid].state != THREAD_TERMINATED) {
        // Enable scheduler and yield
        scheduler_running = 1;
        setitimer(ITIMER_REAL, &timer, NULL);
        scheduler(0);
    }
    
    if (retval) {
        *retval = threads[tid].retval;
    }
}

// Example thread functions
void *thread_function1(void *arg) {
    for (int i = 0; i < 5; i++) {
        printf("Thread 1: %d\n", i);
        for (volatile int j = 0; j < 10000000; j++); // Busy wait
    }
    return (void *)1;
}

void *thread_function2(void *arg) {
    for (int i = 0; i < 5; i++) {
        printf("Thread 2: %d\n", i);
        for (volatile int j = 0; j < 10000000; j++); // Busy wait
    }
    return (void *)2;
}

int main() {
    init_threading();
    
    // Create threads
    int tid1 = thread_create(thread_function1, NULL);
    int tid2 = thread_create(thread_function2, NULL);
    
    printf("Threads created: %d, %d\n", tid1, tid2);
    
    // Start scheduling
    scheduler_running = 1;
    setitimer(ITIMER_REAL, &timer, NULL);
    
    // Wait for threads to finish
    void *retval1, *retval2;
    thread_join(tid1, &retval1);
    thread_join(tid2, &retval2);
    
    printf("Thread 1 returned: %ld\n", (intptr_t)retval1);
    printf("Thread 2 returned: %ld\n", (intptr_t)retval2);
    
    // Disable scheduler
    scheduler_running = 0;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &timer, NULL);
    
    printf("Main thread exiting\n");
    return 0;
}
