#include <stdio.h>
#include "threads.h" // Include the custom header file

void *mythread(void *arg) {
    printf("Hello from thread %d\n", *(int *)arg);
    return NULL;
}

int main() {
    pthread_t t1, t2;
    int id1 = 1, id2 = 2;

    printf("Main: Creating threads\n");

    Pthread_create(&t1, NULL, mythread, &id1);
    Pthread_create(&t2, NULL, mythread, &id2);

    Pthread_join(t1, NULL);
    Pthread_join(t2, NULL);

    printf("Main: Threads finished\n");
    return 0;
}

