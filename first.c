#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>

#define STACK_SIZE 1024*64

typedef struct user_thread {
    ucontext_t context;
    void (*function)(void);
} user_thread;

void thread_start(user_thread *t) {
    t->function();
}

void create_thread(user_thread *t, void (*function)(void)) {
    getcontext(&t->context);
    t->function = function;
    t->context.uc_stack.ss_sp = malloc(STACK_SIZE);
    t->context.uc_stack.ss_size = STACK_SIZE;
    t->context.uc_link = NULL;
    makecontext(&t->context, (void (*)(void)) thread_start, 1, t);
}

void switch_thread(user_thread *t) {
    setcontext(&t->context);
+}

void thread_function() {
    printf("Hello from user-land thread!\n");
}

int main() {
    user_thread t1;
    create_thread(&t1, thread_function);
    printf("Switching to user-land thread...\n");
    switch_thread(&t1);
    printf("Back to main thread.\n");
    return 0;
}
