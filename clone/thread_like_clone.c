#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int thread_function(void *arg) {
    int *shared_var = (int *)arg;
    (*shared_var)++;
    printf("Thread: Shared var = %d\n", *shared_var);
    return 0;
}

int main() {
    int shared_var = 10;
    const int STACK_SIZE = 1024 * 1024;
    void *stack = malloc(STACK_SIZE);

    if (!stack) {
        perror("malloc");
        exit(1);
    }

    pid_t thread_pid = clone(thread_function, stack + STACK_SIZE, CLONE_VM | SIGCHLD, &shared_var);

    if (thread_pid == -1) {
        perror("clone");
        exit(1);
    }

    wait(NULL);
    printf("Parent: Shared var = %d\n", shared_var);
    free(stack);
    return 0;
}

