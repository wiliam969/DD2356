#include <stdio.h>
#include <stdlib.h>
#include <cblas.h>

#ifndef MATRIX_SIZE
#define MATRIX_SIZE 1000  // Matrix size
#endif

#ifndef IO_ON_OFF
#define IO_ON_OFF 1 // IO flag
#endif

void initialize(double *matrix, double *vector) {
    for (int i = 0; i < MATRIX_SIZE * MATRIX_SIZE; i++) {
        matrix[i] = (double)(i % 100) / 10.0;
    }
    for (int i = 0; i < MATRIX_SIZE; i++) {
        vector[i] = (double)(i % 50) / 5.0;
    }
}

int main() {
    double *matrix = (double*) malloc(MATRIX_SIZE * MATRIX_SIZE * sizeof(double));
    double *vector = (double*) malloc(MATRIX_SIZE * sizeof(double));
    double *result = (double*) malloc(MATRIX_SIZE * sizeof(double));
    
    initialize(matrix, vector);
    
    // Perform matrix-vector multiplication using BLAS
    cblas_dgemv(CblasRowMajor, CblasNoTrans, MATRIX_SIZE, MATRIX_SIZE, 1.0, matrix, MATRIX_SIZE, vector, 1, 0.0, result, 1);
    
    // Write output to file
    if (IO_ON_OFF == 1)
        FILE *f = fopen("blas_serial_output.txt", "w");
        for (int i = 0; i < MATRIX_SIZE; i++) {
            fprintf(f, "%f\n", result[i]);
        }
        fclose(f);
    }
    
    free(matrix);
    free(vector);
    free(result);
    printf("BLAS matrix-vector multiplication complete.\n");
    return 0;
}