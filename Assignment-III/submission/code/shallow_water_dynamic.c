#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define N 500  // Grid size
#define ITER 1000  // Number of iterations
#define DT 0.01  // Time step
#define DX 1.0   // Grid spacing

#ifndef CHUNK_SIZE
#define CHUNK_SIZE 16
#endif

double h[N][N], u[N][N], v[N][N];

void initialize() {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) {
            h[i][j] = 1.0;
            u[i][j] = 0.0;
            v[i][j] = 0.0;
        }
}

void compute() {
    double start_time, end_time;

    start_time = omp_get_wtime();

    #pragma omp parallel for collapse(2) schedule(dynamic,CHUNK_SIZE)
    for (int iter = 0; iter < ITER; iter++) {
        for (int i = 1; i < N - 1; i++) {
            for (int j = 1; j < N - 1; j++) {
                double dudx = (u[i+1][j] - u[i-1][j]) / (2.0 * DX);
                double dvdy = (v[i][j+1] - v[i][j-1]) / (2.0 * DX);
                h[i][j] -= DT * (dudx + dvdy);
            }
        }
    }

    end_time = omp_get_wtime();

    printf("Execution time with scheduling: %f seconds\n", end_time - start_time);
}

void write_output() {
    FILE *f = fopen("output.txt", "w");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            fprintf(f, "%f ", h[i][j]);
        }
        fprintf(f, "\n");
    }
    fclose(f);
}

int main() {
    initialize();
    compute();
    write_output();
    printf("Computation completed.\n");
    return 0;
}