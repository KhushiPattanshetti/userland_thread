#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h> 

int thread_func(void *arg) {
    printf("Thread running in process %d (Parent: %d)\n", getpid(), getppid());
    return 0;
}

int main() {
    const int STACK_SIZE = 1024 * 1024;
    void *stack = malloc(STACK_SIZE);

    if (!stack) {
        perror("malloc");
        exit(1);
    }

    pid_t thread_pid = clone(thread_func, stack + STACK_SIZE, CLONE_THREAD | CLONE_PARENT | CLONE_SIGHAND | SIGCHLD, NULL);

    if (thread_pid == -1) {
        perror("clone");
        exit(1);
    }

    sleep(1);
    free(stack);
    return 0;
}

