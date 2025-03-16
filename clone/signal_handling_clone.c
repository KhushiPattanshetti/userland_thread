#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void signal_handler(int sig) {
    printf("Caught signal %d in process %d\n", sig, getpid());
}

int child_func(void *arg) {
    printf("Child process running, sending SIGUSR1\n");
    kill(getppid(), SIGUSR1);
    return 0;
}

int main() {
    signal(SIGUSR1, signal_handler);

    const int STACK_SIZE = 1024 * 1024;
    void *stack = malloc(STACK_SIZE);

    if (!stack) {
        perror("malloc");
        exit(1);
    }

    pid_t child_pid = clone(child_func, stack + STACK_SIZE, CLONE_SIGHAND | SIGCHLD, NULL);

    if (child_pid == -1) {
        perror("clone");
        exit(1);
    }

    wait(NULL);
    free(stack);
    return 0;
}

