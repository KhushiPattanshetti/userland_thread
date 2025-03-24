#define _GNU_SOURCE
#include <ucontext.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>


#define STACK_SIZE 8192
#define NUM_THREADS 2
#define MAX_SWITCHES 10  // Stop after 10 switches

ucontext_t main_ctx, thread_ctx[NUM_THREADS];
int current_thread = 0;
int switch_count = 0;

void thread1() {
    for (int i = 0; i < 5; i++) {
        printf("Thread 1 is running: Iteration %d\n", i + 1);
        sleep(1);
    }
    printf("Thread 1 finished\n");
}

void thread2() {
    for (int i = 0; i < 5; i++) {
        printf("Thread 2 is running: Iteration %d\n", i + 1);
        sleep(1);
    }
    printf("Thread 2 finished\n");
}

void switch_threads(int signum) {
    if (switch_count >= MAX_SWITCHES) {
        printf("Max context switches reached. Stopping execution.\n");
        setcontext(&main_ctx);  // Return to main
    }
    int next_thread = (current_thread + 1) % NUM_THREADS;
    switch_count++;
    swapcontext(&thread_ctx[current_thread], &thread_ctx[next_thread]);
    current_thread = next_thread;
}

int main() {
    char stack1[STACK_SIZE], stack2[STACK_SIZE];
    struct sigaction sa;
    struct itimerval timer;

    // Create thread 1 context
    getcontext(&thread_ctx[0]);
    thread_ctx[0].uc_link = &main_ctx;
    thread_ctx[0].uc_stack.ss_sp = stack1;
    thread_ctx[0].uc_stack.ss_size = STACK_SIZE;
    makecontext(&thread_ctx[0], thread1, 0);

    // Create thread 2 context
    getcontext(&thread_ctx[1]);
    thread_ctx[1].uc_link = &main_ctx;
    thread_ctx[1].uc_stack.ss_sp = stack2;
    thread_ctx[1].uc_stack.ss_size = STACK_SIZE;
    makecontext(&thread_ctx[1], thread2, 0);

    // Setup timer and signal handler
    sa.sa_handler = switch_threads;
    sigaction(SIGALRM, &sa, NULL);

    // Set timer to switch threads every 1 second
    timer.it_value.tv_sec = 1;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = 1;
    timer.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &timer, NULL);

    // Start first thread
    swapcontext(&main_ctx, &thread_ctx[0]);

    printf("Back to main\n");
    return 0;
}

