#include <stdio.h>
#include <stdlib.h>
#include <cblas.h>
#include <mpi.h>

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

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);
    int rank, nprocs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    /* Ensure divisibility */
    if (MATRIX_SIZE % nprocs != 0) {
        if (rank == 0) {
            fprintf(stderr, "Error: MATRIX_SIZE must be divisible by number of processes\n");
        }
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    int rows_per_proc = MATRIX_SIZE / nprocs;

    double *matrix = NULL;
    double *vector = (double *) malloc(MATRIX_SIZE * sizeof(double));
    double *result = NULL;

    if (rank == 0) {
        matrix = (double *) malloc(MATRIX_SIZE * MATRIX_SIZE * sizeof(double));
        result = (double *) malloc(MATRIX_SIZE * sizeof(double));
        initialize(matrix, vector);
    }

    // Each rank allocates space for its submatrix and partial result
    double *local_mat = malloc(rows_per_proc * MATRIX_SIZE * sizeof(double));
    double *local_result = malloc(rows_per_proc * sizeof(double));

    MPI_Request reqs[2];
    MPI_Status  stats[2];

    MPI_Iscatter(
        matrix,                            // sendbuf (root only)
        rows_per_proc * MATRIX_SIZE,       // sendcount
        MPI_DOUBLE,                        // sendtype
        local_mat,                         // recvbuf
        rows_per_proc * MATRIX_SIZE,       // recvcount
        MPI_DOUBLE,                        // recvtype
        0,                                  // root
        MPI_COMM_WORLD,
        &reqs[0]                           // MPI_Request handle
    );

    MPI_Ibcast(
        vector,             // buffer
        MATRIX_SIZE,        // count
        MPI_DOUBLE,         // datatype
        0,                  // root
        MPI_COMM_WORLD,
        &reqs[1]            // MPI_Request handle
    );

    MPI_Waitall(2, reqs, stats);

    cblas_dgemv(
        CblasRowMajor, CblasNoTrans,
        rows_per_proc,          // m
        MATRIX_SIZE,            // n
        1.0,                    // alpha
        local_mat,              // A
        MATRIX_SIZE,            // lda
        vector,                 // x
        1,                      // incx
        0.0,                    // beta
        local_result,           // y
        1                       // incy
    );

    MPI_Request req_gather;
    MPI_Igather(
        local_result,        // sendbuf
        rows_per_proc,       // sendcount
        MPI_DOUBLE,          // sendtype
        result,              // recvbuf (root only)
        rows_per_proc,       // recvcount
        MPI_DOUBLE,          // recvtype
        0,                   // root
        MPI_COMM_WORLD,
        &req_gather          // MPI_Request handle
    );

    MPI_Wait(&req_gather, MPI_STATUS_IGNORE);

    if (rank == 0 && IO_ON_OFF == 1) {
        FILE *f = fopen("blas_mpi_output.txt", "w");
        if (f == NULL) {
            fprintf(stderr, "Error: could not open blas_mpi_output.txt for writing\n");
        } else {
            for (int i = 0; i < MATRIX_SIZE; i++) {
                fprintf(f, "%f\n", result[i]);
            }
            fclose(f);
        }
    }

    free(local_mat);
    free(local_result);
    if (rank == 0) {
        free(matrix);
        free(result);
    }
    free(vector);

    MPI_Finalize();
    return 0;
}
