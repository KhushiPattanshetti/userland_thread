#define _GNU_SOURCE
#include <stdio.h> //for printf 
#include <stdlib.h>//for malloc and free
#include <sched.h>//for clone
#include <unistd.h>//for fork and getpid
#include <signal.h>  // For sigchild
#include <sys/wait.h>  // For waitpid
//threads require separate stacks because they need space to manage their own local variables, function calls, and return addresses.
#define STACK_SIZE 1024 * 1024
//two thread functions are created

int threadFunc1(void* arg) {
    printf("hello from thread 1!\n");
    return 0;
}

int threadFunc2(void* arg) {
    printf("hello from thread 2!\n");
    return 0;
}

int main() {
    //allocate memory to the stacks
    char* stack1 = malloc(STACK_SIZE);
    char* stack2 = malloc(STACK_SIZE);
    if (!stack1 || !stack2) { //checks if the memory allocation failed. If it did, the program prints an error message and exits.
        perror("failed to allocate memory");
        return 1;
    }

    //stacks grow downward in memory , so we set the starting point of each stack to the top of the allocated memory.
    char* stack1_top = stack1 + STACK_SIZE;
    char* stack2_top = stack2 + STACK_SIZE;

    // create threads using clone
    pid_t thread1 = clone(threadFunc1, stack1_top, SIGCHLD, NULL);
    if (thread1 == -1) {
        perror("error creating thread 1");
        free(stack1);
        free(stack2);
        return 1;
    }

    pid_t thread2 = clone(threadFunc2, stack2_top, SIGCHLD, NULL);
    if (thread2 == -1) {
        perror("error creating thread 2");
        free(stack1);
        free(stack2);
        return 1;
    }

    //wait for threads to finish
    if (waitpid(thread1, NULL, 0) == -1) {
        perror("error waiting for thread 1");
    }
    if (waitpid(thread2, NULL, 0) == -1) {
        perror("error waiting for thread 2");
    }

    //free allocated memory
    free(stack1);
    free(stack2);

    printf("all threads have finished execution.\n");
    return 0;
}

