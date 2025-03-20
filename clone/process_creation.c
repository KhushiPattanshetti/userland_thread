#define _GNU_SOURCE
#include <sched.h>     
#include <stdio.h>   
#include <stdlib.h>    
#include <unistd.h>  
#include <signal.h>    
#include <sys/wait.h>  

#define STACK_SIZE (1024 * 1024) // stack size of the child process


// child process function that runs after clone() is called and prints the PID and PPID 
int child_function(void *arg) {
    printf("Child Process: PID = %d, PPID = %d\n", getpid(), getppid());
    return 0; 
}

int main() {
    void *child_stack = malloc(STACK_SIZE);
    if (!child_stack) {
        perror("Failed to allocate memory");  
        exit(1);  
    }

    pid_t child_pid = clone(child_function, child_stack + STACK_SIZE, SIGCHLD, NULL);
    if (child_pid == -1) {
        perror("clone failed");  
        exit(1);  
    }

    printf("Parent Process: PID = %d, Child PID = %d\n", getpid(), child_pid);

    wait(NULL);
    free(child_stack);
    return 0;
}
// clone() flag: SIGCHLD: Sends a signal to the parent when the child process terminates.

