#ifndef THREADS_H
#define THREADS_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

// Wrapper for pthread_create
static inline void Pthread_create(pthread_t *thread, const pthread_attr_t *attr, 
                                  void *(*start_routine)(void *), void *arg) {
    int rc = pthread_create(thread, attr, start_routine, arg);
    if (rc != 0) {
        fprintf(stderr, "Error: pthread_create failed\n");
        exit(1);
    }
}

// Wrapper for pthread_join
static inline void Pthread_join(pthread_t thread, void **retval) {
    int rc = pthread_join(thread, retval);
    if (rc != 0) {
        fprintf(stderr, "Error: pthread_join failed\n");
        exit(1);
    }
}

// Wrapper for pthread_exit
static inline void Pthread_exit(void *retval) {
    pthread_exit(retval);
}

#endif // THREADS_H

