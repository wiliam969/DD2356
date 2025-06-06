#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cblas.h>
#include <mpi.h>

#ifndef MATRIX_SIZE
#define MATRIX_SIZE 1000  // Global matrix dimension (N × N)
#endif

#ifndef IO_ON_OFF
#define IO_ON_OFF 1       // Toggle result‐writing on rank 0
#endif

void initialize_local(double *local_mat,
                      double *vector,
                      int row_start,
                      int rows_per_proc)
{
    // 1) Initialize the vector exactly as before:
    for (int j = 0; j < MATRIX_SIZE; j++) {
        vector[j] = (double)(j % 50) / 5.0;
    }

    for (int i_local = 0; i_local < rows_per_proc; i_local++) {
        int i_global = row_start + i_local;
        for (int j = 0; j < MATRIX_SIZE; j++) {
            local_mat[i_local * MATRIX_SIZE + j] =
                ((i_global * MATRIX_SIZE + j) % 100) / 10.0;
        }
    }
}

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);
    int rank, nprocs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    // Ensure MATRIX_SIZE is divisible by nprocs
    if (MATRIX_SIZE % nprocs != 0) {
        if (rank == 0) {
            fprintf(stderr,
                    "Error: MATRIX_SIZE (%d) must be divisible by number of ranks (%d)\n",
                    MATRIX_SIZE, nprocs);
        }
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    int rows_per_proc = MATRIX_SIZE / nprocs;
    int row_start     = rank * rows_per_proc;  // global‐index of first row on this rank

    double *local_mat    = malloc(rows_per_proc * MATRIX_SIZE * sizeof(double));
    double *local_result = malloc(rows_per_proc * sizeof(double));
    double *vector       = malloc(MATRIX_SIZE * sizeof(double));

    // Only rank 0 will need the final full result[] array:
    double *result = NULL;
    if (rank == 0) {
        result = malloc(MATRIX_SIZE * sizeof(double));
    }

    initialize_local(local_mat, vector, row_start, rows_per_proc);

    cblas_dgemv(
        CblasRowMajor,    // row‐major storage
        CblasNoTrans,     // A is not transposed
        rows_per_proc,    // number of rows in local_mat
        MATRIX_SIZE,      // number of columns in local_mat
        1.0,              // alpha
        local_mat,        // pointer to A[0][0]
        MATRIX_SIZE,      // leading dimension = global N
        vector,           // x vector
        1,                // stride = 1
        0.0,              // beta = 0
        local_result,     // y output
        1                 // stride = 1
    );

    MPI_Gather(
        local_result,         // sendbuf
        rows_per_proc,        // sendcount
        MPI_DOUBLE,           // sendtype
        result,               // recvbuf (only on root)
        rows_per_proc,        // recvcount
        MPI_DOUBLE,           // recvtype
        0,                    // root
        MPI_COMM_WORLD
    );

    if (rank == 0 && IO_ON_OFF == 1) {
        FILE *f = fopen("blas_mpi_output.txt", "w");
        if (!f) {
            fprintf(stderr, "Error: cannot open blas_mpi_output.txt for writing\n");
        } else {
            for (int i = 0; i < MATRIX_SIZE; i++) {
                fprintf(f, "%f\n", result[i]);
            }
            fclose(f);
        }
    }

    // === Clean up ===
    free(local_mat);
    free(local_result);
    free(vector);
    if (rank == 0) free(result);

    MPI_Finalize();
    return 0;
}
