#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define N 3  // Number of rows
#define M 3  // Number of columns (Matrix A)
#define P 3  // Number of columns (Matrix B)

// Matrices
int A[N][M] = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
int B[M][P] = {{9, 8, 7}, {6, 5, 4}, {3, 2, 1}};
int C[N][P];  // Resultant Matrix

// Thread function to compute one row of the resultant matrix
void* multiply_row(void* arg) {
    int row = *(int*)arg;
    free(arg); // Free allocated memory for thread argument

    for (int j = 0; j < P; j++) {
        C[row][j] = 0;
        for (int k = 0; k < M; k++) {
            C[row][j] += A[row][k] * B[k][j];
        }
    }

    pthread_exit(NULL);
}

int main() {
    pthread_t threads[N];

    // Creating threads to compute each row
    for (int i = 0; i < N; i++) {
        int* row = malloc(sizeof(int));  // Allocate memory to pass row index
        *row = i;
        pthread_create(&threads[i], NULL, multiply_row, row);
    }

    // Wait for all threads to finish
    for (int i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
    }

    // Print the resultant matrix
    printf("Resultant Matrix:\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < P; j++) {
            printf("%d ", C[i][j]);
        }
        printf("\n");
    }

    return 0;
}

