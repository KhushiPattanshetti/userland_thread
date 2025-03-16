#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

long long factorial_result;
pthread_mutex_t mutex;

void *factorial(void *arg) {
    int n = *(int *)arg;
    long long res = 1;
    for (int i = 1; i <= n; i++) {
        res *= i;
    }
    pthread_mutex_lock(&mutex);
    factorial_result = res;
    pthread_mutex_unlock(&mutex);
    pthread_exit(0);
}

int main() {
    pthread_t fact_thread;
    pthread_mutex_init(&mutex, NULL);
    int n;
    printf("Enter Number : ");
    scanf("%d",&n);
    pthread_create(&fact_thread, NULL, factorial, &n);
    pthread_join(fact_thread, NULL);
    pthread_mutex_destroy(&mutex);
    printf("Factorial of %d is %lld\n", n, factorial_result);
    return 0;
}

