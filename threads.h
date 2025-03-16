#ifndef THREADS_H
#define THREADS_H
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
// code for: Pthread_create
// purpose: Creates a new thread and runs a function in it
// parameters:
//   - thread: pointer to the thread variable
//   - attr: thread attributes 
//   - start_routine: function that the thread will execute
//   - arg: argument to be passed to the function
// if thread creation fails, the program prints an error and exits.

static inline void Pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void *), void *arg) {
    int rc = pthread_create(thread, attr, start_routine, arg);
    if (rc != 0) { //if thread creation fails return error 
        fprintf(stderr, "Error: pthread_create failed\n");
        exit(1);
    }
}
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------

// code: Pthread_join
// purpose: waits for a thread to finish executing
// parameters:
//   - thread: the thread to wait for
//   - retval: pointer to store the return value of the thread
// if joining the thread fails, the program prints an error and exits.
static inline void Pthread_join(pthread_t thread, void **retval) {
    int rc = pthread_join(thread, retval);
    if (rc != 0) {
        fprintf(stderr, "Error: pthread_join failed\n");
        exit(1);
    }
}
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------

// code for: Pthread_exit
// purpose: Ends the calling thread and optionally returns a value
// parameter:
//   - retval: The value to return when the thread exits
// this function does not return because it terminates the thread immediately.
static inline void Pthread_exit(void *retval) {   //retval is a pointer to the return value of a thread. 
    pthread_exit(retval); // ends the calling thread and returns retval
}
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // THREADS_H

