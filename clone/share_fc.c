#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>    
#include <sys/wait.h> 

#define STACK_SIZE (1024 * 1024)

int child_function(void *arg) {
    printf("Child Process: Changing directory to /tmp\n");
    chdir("/tmp");
    system("pwd");
    return 0;
}

int main() {
    printf("Parent Process: Initial directory\n");
    system("pwd");

    void *child_stack = malloc(STACK_SIZE);
    if (!child_stack) {
        perror("Failed to allocate memory");
        exit(1);
    }

    pid_t child_pid = clone(child_function, child_stack + STACK_SIZE, CLONE_FS | SIGCHLD, NULL);
    if (child_pid == -1) {
        perror("clone failed");
        exit(1);
    }

    wait(NULL);
    printf("Parent Process: After child directory change\n");
    system("pwd");

    free(child_stack);
    return 0;
}

