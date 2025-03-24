#define _GNU_SOURCE // for std gnu lib
#include <stdio.h>  // for printf, perror 
#include <stdlib.h> // for malloc, free
#include <unistd.h> // for sleep, getpid
#include <sys/types.h> // for pid_t
#include <sched.h> // for clone
#define STACK_SIZE 1024 * 1024 // stack size is assigned to the user threads

typedef int (*uthread_func_t)(void*); // makes a function pointer with type void and return type int

int uthread_create(uthread_func_t func, void *arg) {
    void *stack = malloc(STACK_SIZE); // dynamically assigns a stack to the thread
    if (!stack) { // check if memory allocation for the stack failed
        perror("Failed to allocate stack");
        return -1;
    }

    // creating the thread using clone
    pid_t thread_pid = clone(
        func,
        (char *)stack + STACK_SIZE,
        CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD,
        arg
    );

    // stack+STACK_SIZE is done so as to point to the top of the stack and add entries in downward direction as stack grows downwards
    // CLONE_VM: shares memory space with parent process
    // CLONE_FS: shares file system info
    // CLONE_FILES: shares file descriptors
    // CLONE_SIGHAND: shares signal handlers
    // CLONE_THREAD: threads are part of the same thread group

    if (thread_pid == -1) {
        perror("Failed to create thread");
        free(stack);
        return -1;
    }

    printf("Parent thread: Created child thread with PID: %d\n", thread_pid);
    return thread_pid;
}

int thread_function(void *arg) {
    printf("Child thread has started. PID: %d, Argument: %s\n", getpid(), (char *)arg);
    sleep(2); // simulate some work
    printf("Child thread exiting. PID: %d\n", getpid());
    return 0;
}

int main() {
    printf("Parent thread PID: %d\n", getpid()); // Print the PID of the main thread

    const char *message = "Hello from uthread";
    if (uthread_create(thread_function, (void *)message) == -1) {
        fprintf(stderr, "Failed to create thread\n");
        return 1;
    }

    printf("Parent thread: Waiting for child thread to complete...\n");
    sleep(3); // wait to ensure the child thread finishes execution
    printf("Parent thread exiting. PID: %d\n", getpid());
    return 0;
}

