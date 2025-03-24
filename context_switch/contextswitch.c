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
int task_time_elapsed = 0;

void task1() {
    while (1) {
        sleep(1);
    }
}

void task2() {
    while (1) {
        sleep(1);
    }
}

void signal_handler(int sig) {
    task_time_elapsed++;
    printf("Task %d is running... %d seconds elapsed\n", current_task + 1, task_time_elapsed);
    if (task_time_elapsed >= TASK_DURATION) {
        printf("Task %d completed %d seconds. Switching tasks...\n", current_task + 1, TASK_DURATION);
        int prev_task = current_task;
        current_task = (current_task + 1) % MAX_TASKS;
        task_time_elapsed = 0;
        swapcontext(&contexts[prev_task], &contexts[current_task]);
    }
}

void setup_timer(int use_virtual) {
    struct itimerval timer;
    timer.it_value.tv_sec = 1;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = 1;
    timer.it_interval.tv_usec = 0;
    if (use_virtual) {
        setitimer(ITIMER_VIRTUAL, &timer, NULL);
    } else {
        setitimer(ITIMER_REAL, &timer, NULL);
    }
}

void create_task(void (*func)(), ucontext_t *context) {
    char *stack = malloc(STACK_SIZE);
    getcontext(context);
    context->uc_stack.ss_sp = stack;
    context->uc_stack.ss_size = STACK_SIZE;
    context->uc_link = &main_context;
    makecontext(context, func, 0);
}

void setup_signals(int use_virtual) {
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sa.sa_flags = SA_RESTART;
    sigemptyset(&sa.sa_mask);
    if (use_virtual) {
        sigaction(SIGVTALRM, &sa, NULL);
    } else {
        sigaction(SIGALRM, &sa, NULL);
    }
}

int main(int argc, char *argv[]) {
    int use_virtual = 0;
    if (argc > 1 && atoi(argv[1]) == 1) {
        use_virtual = 1;
    }
    printf("Initializing tasks...\n");
    create_task(task1, &contexts[0]);
    create_task(task2, &contexts[1]);
    setup_signals(use_virtual);
    setup_timer(use_virtual);
    printf("Starting Task 1...\n");
    swapcontext(&main_context, &contexts[0]);
    printf("Back to main. All tasks completed!\n");
    return 0;
}

