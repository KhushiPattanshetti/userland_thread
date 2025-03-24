#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>

#define STACK_SIZE 8192
#define MAX_TASKS 2
#define TASK_DURATION 5

ucontext_t contexts[MAX_TASKS];
ucontext_t main_context;
int current_task = 0;

void task1() {
    while (1) {
        printf("Task 1 is running...\n");
        sleep(1);
    }
}

void task2() {
    while (1) {
        printf("Task 2 is running...\n");
        sleep(1);
    }
}

void signal_handler(int sig) {
    int prev_task = current_task;
    current_task = (current_task + 1) % MAX_TASKS;
    printf("Switching to Task %d...\n", current_task + 1);
    swapcontext(&contexts[prev_task], &contexts[current_task]);
}

void setup_timer() {
    struct itimerval timer;
    timer.it_value.tv_sec = TASK_DURATION;   
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = TASK_DURATION;  
    timer.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &timer, NULL);
}

void create_task(void (*func)(), ucontext_t *context) {
    char *stack = malloc(STACK_SIZE);
    getcontext(context);
    context->uc_stack.ss_sp = stack;
    context->uc_stack.ss_size = STACK_SIZE;
    context->uc_link = &main_context;
    makecontext(context, func, 0);
}

void setup_signals() {
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sa.sa_flags = SA_RESTART;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGALRM, &sa, NULL);
}

int main() {
    printf("Initializing tasks...\n");
    create_task(task1, &contexts[0]);
    create_task(task2, &contexts[1]);
    setup_signals();
    setup_timer();
    printf("Starting Task 1...\n");
    swapcontext(&main_context, &contexts[0]);
    printf("Back to main. All tasks completed!\n");
    return 0;
}

