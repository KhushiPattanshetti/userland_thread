#include <stdio.h>
#include "uthread.h"

uthread_mutex_t mutex; 


void *print_message(void *arg) {
    char *msg = (char *)arg;

    uthread_mutex_lock(&mutex);
    for (int i = 0; i < 5; i++) {
        printf("%s: iteration %d\n", msg, i);
        sleep(1);
    }
    uthread_mutex_unlock(&mutex);

    return NULL;
}

int main() {
    uthread_t thread1, thread2;

    uthread_mutex_init(&mutex);

    uthread_create(&thread1, print_message, "Thread 1");
    uthread_create(&thread2, print_message, "Thread 2");

    uthread_join(thread1, NULL);
    uthread_join(thread2, NULL);

    printf("All threads finished\n");
    return 0;
}

