#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define N 500  // Grid size
#define ITER 1000  // Number of iterations
#define DT 0.01  // Time step
#define DX 1.0   // Grid spacing

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
    for (int iter = 0; iter < ITER; iter++) {
        for (int i = 1; i < N - 1; i++) {
            for (int j = 1; j < N - 1; j++) {
                double dudx = (u[i+1][j] - u[i-1][j]) / (2.0 * DX);
                double dvdy = (v[i][j+1] - v[i][j-1]) / (2.0 * DX);
                h[i][j] -= DT * (dudx + dvdy);
            }
        }
    }
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