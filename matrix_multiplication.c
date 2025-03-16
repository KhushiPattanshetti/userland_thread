#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int A[3][3] = { {1, 2, 3}, {4, 5, 6}, {7, 8, 9} };
int B[3][3] = { {1, 2, 3}, {4, 5, 6}, {7, 8, 9} };
int C[3][3];  


typedef struct {
    int row;
    int col;
} MatrixCell;

void *multiply_cell(void *param) {
    MatrixCell *cell = (MatrixCell *)param;
    int sum = 0;
    for (int i = 0; i < 3; i++) {
        sum += A[cell->row][i] * B[i][cell->col];
    }
    C[cell->row][cell->col] = sum;
    free(cell);
    pthread_exit(0);
}

void matrix_multiplication() {
    pthread_t threads[3][3];
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            MatrixCell *cell = (MatrixCell *)malloc(sizeof(MatrixCell));
            cell->row = i;
            cell->col = j;
            pthread_create(&threads[i][j], NULL, multiply_cell, cell);
        }
    }
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            pthread_join(threads[i][j], NULL);
        }
    }
}

void print_matrix(int matrix[3][3]) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}

int main() {
    printf("Matrix A:\n");
    print_matrix(A);
    printf("Matrix B:\n");
    print_matrix(B);
    
    matrix_multiplication();
    
    printf("Resultant Matrix C (A * B):\n");
    print_matrix(C);
    return 0;
}


