#include <stdio.h>
#include <stdlib.h>
#include <cblas.h>
#include <mpi.h>

#ifndef N
#define N 1000  // Matrix size
#endif

void initialize(double *matrix, double *vector) {
    for (int i = 0; i < N * N; i++) {
        matrix[i] = (double)(i % 100) / 10.0;
    }
    for (int i = 0; i < N; i++) {
        vector[i] = (double)(i % 50) / 5.0;
    }
}

int main(int argc, char **argv) {

    MPI_Init(&argc, &argv); 
    int rank, nprocs; 
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); 
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs); 

    /* Ensure divisibility */
    if (N % nprocs != 0) {
        if (rank == 0) fprintf(stderr, "Error: N must be divisible by number of processes\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    int rows_per_proc = N / nprocs; 

    double *matrix = NULL; 
    double *vector = (double*) malloc(N * sizeof(double));
    double *result = NULL; 

    if (rank == 0) {
        matrix = (double*) malloc(N * N * sizeof(double));
        result = (double*) malloc(N * sizeof(double));
        
        initialize(matrix, vector);
    }

    double *local_mat  = malloc(rows_per_proc * N * sizeof(double));
    double *local_result = malloc(rows_per_proc * sizeof(double));

    MPI_Scatter(
        matrix,
        rows_per_proc * N,         /* sendcount */
        MPI_DOUBLE,                /* sendtype  */
        local_mat,                 /* recvbuf   */
        rows_per_proc * N,         /* recvcount */
        MPI_DOUBLE,                /* recvtype  */
        0,                         /* root      */
        MPI_COMM_WORLD
    );

    MPI_Bcast(vector, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Perform matrix-vector multiplication using BLAS
    cblas_dgemv(CblasRowMajor, CblasNoTrans,
        rows_per_proc, N,
        1.0, local_mat, N,
        vector, 1,
        0.0, local_result, 1);
    
    MPI_Gather(
        local_result,                /* sendbuf   */
        rows_per_proc,             /* sendcount */
        MPI_DOUBLE,                /* sendtype  */
        result,                  /* recvbuf   */
        rows_per_proc,             /* recvcount */
        MPI_DOUBLE,                /* recvtype  */
        0,                         /* root      */
        MPI_COMM_WORLD
    );

    // Write output to file
    FILE *f = fopen("blas_mpi_output.txt", "w");
    for (int i = 0; i < N; i++) {
        fprintf(f, "%f\n", result[i]);
    }
    fclose(f);
    
    free(matrix);
    free(vector);
    free(result);
    printf("BLAS matrix-vector multiplication complete.\n");
    return 0;
}