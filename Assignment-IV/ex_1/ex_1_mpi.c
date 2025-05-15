#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#ifndef N
#define N 1000 // Grid size
#endif

#ifndef STEPS
#define STEPS 100 // Time steps
#endif

#define C 1.0   // Wave speed
#define DT 0.01 // Time step
#define DX 1.0  // Grid spacing

#ifndef IO_ON_OFF
#define IO_ON_OFF 1 // IO flag
#endif

void initialize(double *u, double *u_prev, int local_n, int global_index_of_local_n) {
    for (int i = 1; i <= local_n; i++) {
        int global_index = global_index_of_local_n + (i - 1);    
        double v = sin(2.0 * M_PI * global_index / N);
        u[i]      = v;
        u_prev[i] = v;
    }
}

void exchange_halos(double *u, int local_n, int left, int right, MPI_Comm comm) {
    MPI_Status status;
    
    
    // sending the "leftist" cell to the left mpi process 
    // and get the first element of the next mpit process
    MPI_Sendrecv(&u[1],          1, MPI_DOUBLE, left,  0,
                 &u[local_n+1], 1, MPI_DOUBLE, right, 0,
                 comm, &status);
    // Now we send the last element to the right
    // and receive / w8 for the first element of the left mpi process 
    MPI_Sendrecv(&u[local_n],   1, MPI_DOUBLE, right, 1,
                 &u[0],         1, MPI_DOUBLE, left,  1,
                 comm, &status);
}

void compute_step(double *u, double *u_prev, double *u_next, int local_n) {
    double coeff = C * C * DT * DT / (DX * DX);
    for (int i = 1; i <= local_n; i++) {
        u_next[i] = 2.0*u[i] - u_prev[i]+ coeff*(u[i+1] - 2.0*u[i] + u[i-1]);
    }
}

void write_output(double *u, int local_n, int step, int rank) {
    if (IO_ON_OFF == 0) return; // IO is off

    char fname[64];
    sprintf(fname, "wave_output_step%03d_rank%02d.txt", step, rank);
    FILE *f = fopen(fname, "w");
    for (int i = 1; i <= local_n; i++) {
        fprintf(f, "%f\n", u[i]);
    }
    fclose(f);
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int rank, nprocs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    
    int base = N / nprocs;
    int rem  = N % nprocs;

    // this will basically our local array which we want to process on each mpi process 
    int local_n = base + (rank < rem ? 1 : 0);
    int global_index_of_local_n = rank * base + (rank < rem ? rank : rem);

    // if rank 0 we have to edges 
    // if rank nprocs = 0 
    // if rank nprocs -1 = 1 
    // because of halo magic  
    int left  = (rank - 1 + nprocs) % nprocs;
    int right = (rank + 1) % nprocs;

    // Allocate with two ghost cells
    double *u      = malloc((local_n + 2) * sizeof(double));
    double *u_prev = malloc((local_n + 2) * sizeof(double));
    double *u_next = malloc((local_n + 2) * sizeof(double));

    initialize(u, u_prev, local_n, global_index_of_local_n);

    for (int step = 0; step < STEPS; step++) {
        exchange_halos(u, local_n, left, right, MPI_COMM_WORLD);
        compute_step(u, u_prev, u_next, local_n);

        // Advance time levels
        for (int i = 1; i <= local_n; i++) {
            u_prev[i] = u[i];
            u[i]      = u_next[i];
        }

        if (step % 10 == 0) {
            write_output(u, local_n, step, rank);
        }
    }

    if (rank == 0 && IO_ON_OFF == 0) {
        printf("Simulation complete across %d ranks.\n", nprocs);
    }

    free(u);
    free(u_prev);
    free(u_next);

    MPI_Finalize();
    return 0;
}