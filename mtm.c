#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <unistd.h>
#include <string.h>
#include <stdatomic.h>
#include <sys/syscall.h>
#include <linux/futex.h>
#include <stdint.h>

#define MAX_THREADS 10
#define STACK_SIZE 8192

// --------------------------- Structures ---------------------------

typedef struct {
    ucontext_t ctx;
    void *stack;
    int tid;
} thread_t;

typedef struct {
    atomic_int lock;
} futex_mutex_t;

// --------------------------- Globals ---------------------------

static thread_t *threads[MAX_THREADS];
static int thread_count = 0;
static ucontext_t scheduler_ctx;
static int current_thread = 0;

static futex_mutex_t log_mutex;
char *log_store[100];
int log_index = 0;

// --------------------------- Logging ---------------------------

void add_log(const char *msg) {
    if (log_index < 100) {
        log_store[log_index++] = strdup(msg);
    }
}

void show_log(void) {
    printf("\n--- Thread Log ---\n");
    for (int i = 0; i < log_index; i++) {
        printf("%s\n", log_store[i]);
        free(log_store[i]);
    }
    log_index = 0;
}

// --------------------------- Futex ---------------------------

int futex_wait(int *addr, int val) {
    return syscall(SYS_futex, addr, FUTEX_WAIT, val, NULL, NULL, 0);
}

int futex_wake(int *addr, int n) {
    return syscall(SYS_futex, addr, FUTEX_WAKE, n, NULL, NULL, 0);
}

void futex_mutex_init(futex_mutex_t *mutex) {
    atomic_store(&mutex->lock, 0);
}

void mutex_lock(futex_mutex_t *mutex) {
    int expected = 0;
    while (!atomic_compare_exchange_weak(&mutex->lock, &expected, 1)) {
        expected = 0;
        futex_wait(&mutex->lock, 1);
    }
}

void mutex_unlock(futex_mutex_t *mutex) {
    atomic_store(&mutex->lock, 0);
    futex_wake(&mutex->lock, 1);
}

// --------------------------- Thread Logic ---------------------------

void many_to_one_task(int tid) {
    for (int i = 0; i < 5; i++) {
        mutex_lock(&log_mutex);

        char msg[100];
        snprintf(msg, sizeof(msg), "Thread %d: iteration %d", tid, i + 1);
        add_log(msg);
        printf("%s\n", msg);

        mutex_unlock(&log_mutex);

        usleep(50000);  // simulate work
    }

    setcontext(&scheduler_ctx);
}

void many_to_one_create_thread() {
    if (thread_count >= MAX_THREADS) {
        printf("Max thread limit reached!\n");
        return;
    }

    thread_t *t = malloc(sizeof(thread_t));
    t->tid = thread_count;
    t->stack = malloc(STACK_SIZE);

    getcontext(&t->ctx);
    t->ctx.uc_stack.ss_sp = t->stack;
    t->ctx.uc_stack.ss_size = STACK_SIZE;
    t->ctx.uc_link = &scheduler_ctx;

    makecontext(&t->ctx, (void (*)(void))many_to_one_task, 1, t->tid);

    threads[thread_count++] = t;
    printf("Created thread %d\n", t->tid);
}

void many_to_one_run() {
    if (thread_count == 0) {
        printf("No threads to run.\n");
        return;
    }

    for (int i = 0; i < thread_count; i++) {
        current_thread = i;
        swapcontext(&scheduler_ctx, &threads[i]->ctx);
    }
}

// --------------------------- Menu ---------------------------

void many_to_one_menu() {
    int choice;
    thread_count = 0;
    log_index = 0;
    futex_mutex_init(&log_mutex);

    while (1) {
        printf("\nModel: Many-to-One (M:1) with Futex Mutex\n");
        printf("1. Create thread\n");
        printf("2. Run threads\n");
        printf("3. Show log\n");
        printf("4. Exit\n");
        printf("Choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: many_to_one_create_thread(); break;
            case 2: many_to_one_run(); break;
            case 3: show_log(); break;
            case 4: return;
            default: printf("Invalid choice.\n");
        }
    }
}

// --------------------------- Main ---------------------------

int main() {
    many_to_one_menu();
    return 0;
}

