#include <stdio.h>
#include "threads.h" 

void *mythread(void *arg) { 
    printf("Hello from thread %d\n", *(int *)arg); //it prints "Hello from thread X", where X is the thread’s ID.
    return NULL;                                    the *(int *)arg part dereferences the pointer to get the integer value.
}

int main() {
    pthread_t t1, t2;                               // t1 and t2, will store thread identifiers.
    int id1 = 1, id2 = 2;

    printf("Main: Creating threads\n");

    Pthread_create(&t1, NULL, mythread, &id1);     //t1 runs mythread() with id1 (value 1).                            
    Pthread_create(&t2, NULL, mythread, &id2);     //t2 runs mythread() with id2 (value 2).
    Pthread_join(t1, NULL);                        //ensures that the main thread waits for both threads (t1 and t2) to finish
    Pthread_join(t2, NULL);

    printf("Main: Threads finished\n");
    return 0;
}

