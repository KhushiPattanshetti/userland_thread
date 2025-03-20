#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#define STACK_SIZE (1024 * 1024)

int shared_variable = 0;

int child_function(void *arg) {
    printf("Child Process: Modifying shared variable\n");
    shared_variable = 42;
    return 0;
}

int main() {
    printf("Parent: Initial shared variable = %d\n", shared_variable);

    void *child_stack = malloc(STACK_SIZE);
    if (!child_stack) {
        perror("Failed to allocate memory");
        exit(1);
    }

    pid_t child_pid = clone(child_function, child_stack + STACK_SIZE, CLONE_VM | SIGCHLD, NULL);
    if (child_pid == -1) {
        perror("clone failed");
        exit(1);
    }

    wait(NULL);
    printf("Parent: After child modification, shared variable = %d\n", shared_variable);
    free(child_stack);
    return 0;
}

