#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define N 1000  // Matrix dimension

/* Initialize the full N×N matrix on root */
void initialize_matrix(double *matrix) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            matrix[i * N + j] = i + j * 0.01;
        }
    }
}

/* Compute row sums for a local block of rows */
void compute_local_row_sums(double *local_mat, double *local_sums, int rows_per_proc) {
    for (int i = 0; i < rows_per_proc; i++) {
        double sum = 0.0;
        for (int j = 0; j < N; j++) {
            sum += local_mat[i * N + j];
        }
        local_sums[i] = sum;
    }
}

/* Write the gathered row sums (root only) */
void write_output(double *row_sums) {
    FILE *f = fopen("row_sums_output.txt", "w");
    for (int i = 0; i < N; i++) {
        fprintf(f, "%f\n", row_sums[i]);
    }
    fclose(f);
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

    double *matrix   = NULL;
    double *row_sums = NULL;

    // define init array on rank 0 where we ultimately also gather the data
    /// once more  data again
    if (rank == 0) {
        matrix   = malloc(N * N * sizeof(double));
        row_sums = malloc(N * sizeof(double));
        initialize_matrix(matrix);
    }

    double *local_mat  = malloc(rows_per_proc * N * sizeof(double));
    double *local_sums = malloc(rows_per_proc * sizeof(double));

    /* We aim that the scatter will distribute the array to the different ranks 
    /  This is based on the matrix and the rows_per_proc * N   
    */
    MPI_Scatter(
        matrix,                    /* sendbuf   */
        rows_per_proc * N,         /* sendcount */
        MPI_DOUBLE,                /* sendtype  */
        local_mat,                 /* recvbuf   */
        rows_per_proc * N,         /* recvcount */
        MPI_DOUBLE,                /* recvtype  */
        0,                         /* root      */
        MPI_COMM_WORLD
    );

    /* 2) Local computation: row sums */
    compute_local_row_sums(local_mat, local_sums, rows_per_proc);

    /*
      Consequently after the computation is done the rank should send the result back to rank 0 
    */
    MPI_Gather(
        local_sums,                /* sendbuf   */
        rows_per_proc,             /* sendcount */
        MPI_DOUBLE,                /* sendtype  */
        row_sums,                  /* recvbuf   */
        rows_per_proc,             /* recvcount */
        MPI_DOUBLE,                /* recvtype  */
        0,                         /* root      */
        MPI_COMM_WORLD
    );

    /* 
    / This is not necessarily needed as mpi_gather already sent everything to rank 0 
    / however it gives us an performance boost as we dont need to loop
    / over the rows again to add them up (saving up on O(n))
    */
    double local_total = 0.0;
    for (int i = 0; i < rows_per_proc; i++) local_total += local_sums[i];
    double global_total = 0.0;
    MPI_Reduce(
        &local_total,              /* sendbuf */
        &global_total,             /* recvbuf */
        1,                         /* count   */
        MPI_DOUBLE,                /* datatype*/
        MPI_SUM,                   /* op      */
        0,                         /* root    */
        MPI_COMM_WORLD
    );

    /* Root writes output and prints total */
    if (rank == 0) {
        write_output(row_sums);
        printf("Total sum of matrix elements = %f\n", global_total);
        free(matrix);
        free(row_sums);
    }

    free(local_mat);
    free(local_sums);

    MPI_Finalize();
    return 0;
}
