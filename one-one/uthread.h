#ifndef UTHREAD_H
#define UTHREAD_H

#include <sys/types.h>


typedef struct {
    pid_t tid;                 
    void *stack;               
} uthread_t;

typedef struct {
    int locked;                
} uthread_mutex_t;

int uthread_create(uthread_t *thread, void *(*start_routine)(void *), void *arg);
int uthread_join(uthread_t thread, void **retval);
void uthread_exit(void *retval);
int uthread_mutex_init(uthread_mutex_t *mutex);
int uthread_mutex_lock(uthread_mutex_t *mutex);
int uthread_mutex_unlock(uthread_mutex_t *mutex);

#endif

