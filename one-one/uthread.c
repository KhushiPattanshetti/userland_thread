#define _GNU_SOURCE
#include <sched.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include "uthread.h"

#define SS (1024 * 1024)  

int child_function(void *arg) {
    if (arg == NULL) {
        fprintf(stderr, "Error: Argument to child_function is NULL\n");
        _exit(1);
    }

    void **args = (void **)arg;
    void *(*start_routine)(void *) = args[0];
    void *routine_arg = args[1];

    if (start_routine == NULL) {
        fprintf(stderr, "Error: start_routine is NULL\n");
        _exit(1);
    }

    start_routine(routine_arg);
    _exit(0);
}

int uthread_create(uthread_t *thread, void *(*start_routine)(void *), void *arg) {
    if (thread == NULL || start_routine == NULL) {
        fprintf(stderr, "Error: Invalid thread or start_routine\n");
        return -1;
    }

    thread->stack = malloc(SS);
    if (thread->stack == NULL) {
        perror("Failed to allocate stack");
        return -1;
    }

    void *stack_top = (char *)thread->stack + SS - sizeof(void *);

    void **args = malloc(2 * sizeof(void *));
    if (args == NULL) {
        perror("Failed to allocate memory for args");
        free(thread->stack);
        return -1;
    }

    args[0] = start_routine;
    args[1] = arg;

    thread->tid = clone(child_function, stack_top,
                        SIGCHLD | CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND,
                        args);

    if (thread->tid == -1) {
        perror("clone failed");
        free(thread->stack);
        free(args);
        return -1;
    }

    return 0;  
}

int uthread_join(uthread_t thread, void **retval) {
    int status;
    if (thread.tid <= 0) {
        fprintf(stderr, "Error: Invalid thread ID\n");
        return -1;
    }

    if (waitpid(thread.tid, &status, 0) == -1) {
        perror("waitpid failed");
        return -1;
    }
    if (thread.stack != NULL) {
        free(thread.stack);
        thread.stack = NULL;
    }

    if (retval) {
        *retval = NULL;
    }
    return 0;
}

void uthread_exit(void *retval) {
    _exit(0); 
}

int uthread_mutex_init(uthread_mutex_t *mutex) {
    if (mutex == NULL) {
        fprintf(stderr, "Error: Mutex is NULL\n");
        return -1;
    }
    mutex->locked = 0;
    return 0;
}

int uthread_mutex_lock(uthread_mutex_t *mutex) {
    if (mutex == NULL) {
        fprintf(stderr, "Error: Mutex is NULL\n");
        return -1;
    }

    while (__sync_lock_test_and_set(&mutex->locked, 1)) {
        while (mutex->locked) {
           
            sched_yield();
        }
    }
    return 0;
}

int uthread_mutex_unlock(uthread_mutex_t *mutex) {
    if (mutex == NULL) {
        fprintf(stderr, "Error: Mutex is NULL\n");
        return -1;
    }

    
    __sync_lock_release(&mutex->locked);
    return 0;
}

