#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int child_function(void *arg) {
    printf("Child process (PID: %d)\n", getpid());
    return 0;
}

int main() {
    const int STACK_SIZE = 1024 * 1024;
    void *stack = malloc(STACK_SIZE);

    if (!stack) {
        perror("malloc");
        exit(1);
    }

    pid_t child_pid = clone(child_function, stack + STACK_SIZE, SIGCHLD, NULL);

    if (child_pid == -1) {
        perror("clone");
        exit(1);
    }

    printf("Parent process (PID: %d), child PID: %d\n", getpid(), child_pid);
    wait(NULL);
    free(stack);
    return 0;
}

