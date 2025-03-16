#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int child_func(void *arg) {
    printf("Inside new UTS namespace\n");
    sethostname("NewHost", 7);
    system("hostname");
    return 0;
}

int main() {
    const int STACK_SIZE = 1024 * 1024;
    void *stack = malloc(STACK_SIZE);

    if (!stack) {
        perror("malloc");
        exit(1);
    }

    pid_t child_pid = clone(child_func, stack + STACK_SIZE, CLONE_NEWUTS | SIGCHLD, NULL);

    if (child_pid == -1) {
        perror("clone");
        exit(1);
    }

    wait(NULL);
    printf("Parent process exiting\n");
    free(stack);
    return 0;
}

