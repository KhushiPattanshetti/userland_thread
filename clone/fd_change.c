#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>    
#include <sys/wait.h>  

#define STACK_SIZE (1024 * 1024)

int child_function(void *arg) {
    printf("Child Process: Modifying shared file\n");
    int fd = *(int *)arg;
    write(fd, "Child writes\n", 13);
    return 0;
}

int main() {
    int fd = open("shared_file.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd == -1) {
        perror("Failed to open file");
        exit(1);
    }

    void *child_stack = malloc(STACK_SIZE);
    if (!child_stack) {
        perror("Failed to allocate memory");
        exit(1);
    }

    pid_t child_pid = clone(child_function, child_stack + STACK_SIZE, CLONE_FILES | SIGCHLD, &fd);
    if (child_pid == -1) {
        perror("clone failed");
        exit(1);
    }

    write(fd, "Parent writes\n", 14);
    wait(NULL);
    close(fd);
    free(child_stack);
    return 0;
}

