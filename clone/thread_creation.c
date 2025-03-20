#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/syscall.h>

#define STACK_SIZE (1024 * 1024)

int child_function(void *arg) {
    printf("Thread: PID = %d, TID = %ld\n", getpid(), syscall(SYS_gettid));
    return 0;
}

int main() {
    void *child_stack = malloc(STACK_SIZE);
    if (!child_stack) {
        perror("Failed to allocate memory");
        exit(1);
    }

    pid_t thread_pid = clone(child_function, child_stack + STACK_SIZE, 
                             CLONE_VM | CLONE_SIGHAND | CLONE_THREAD, NULL);
    if (thread_pid == -1) {
        perror("clone failed");
        exit(1);
    }

    printf("Main Thread: PID = %d, TID = %ld\n", getpid(), syscall(SYS_gettid));
    sleep(1);
    free(child_stack);
    return 0;
}

