#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define N       20      // Global grid size
#define STEPS   300     // Number of simulation steps

// Count neighbors around cell (i,j) in a (local_rows+2) x N grid with periodic boundaries
int count_neighbors(int (*g)[N], int i, int j, int local_rows) {
    int sum = 0;
    int up    = (i - 1 + local_rows + 2) % (local_rows + 2);
    int down  = (i + 1) % (local_rows + 2);
    int left  = (j - 1 + N) % N;
    int right = (j + 1) % N;

    sum += g[up][left]; sum += g[up][j]; 
    sum += g[up][right];
    sum += g[i][left];
    sum += g[i][right];
    sum += g[down][left]; 
    sum += g[down][j]; 
    sum += g[down][right];
    return sum;
}

// Write the full N x N grid to a file (only called on rank 0)
void write_output_full(int *full, int step) {
    char fname[64];
    sprintf(fname, "gol_output_%d.txt", step);
    FILE *f = fopen(fname, "w");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            fprintf(f, "%d ", full[i * N + j]);
        }
        fprintf(f, "\n");
    }
    fclose(f);
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int base = N / size;
    int rem  = N % size;
    int local_rows = base + (rank < rem ? 1 : 0);

    // Allocate local grid with ghost rows
    int (*grid)[N]     = malloc((local_rows + 2) * sizeof *grid);
    int (*new_grid)[N] = malloc((local_rows + 2) * sizeof *new_grid);

    // Prepare scatter/gather metadata on rank 0
    int *recvcounts = NULL, *displs = NULL, *full_grid = NULL;
    if (rank == 0) {
        recvcounts = malloc(size * sizeof(int));
        displs     = malloc(size * sizeof(int));
        full_grid  = malloc(N * N * sizeof(int));
        int offset = 0;
        for (int r = 0; r < size; r++) {
            int rows_r = base + (r < rem ? 1 : 0);
            recvcounts[r] = rows_r * N;
            displs[r]     = offset;
            offset       += rows_r * N;
        }
        // Generate initial full grid with fixed seed
        srand(1);
        for (int i = 0; i < N * N; i++) {
            full_grid[i] = rand() % 2;
        }
    }

    // Distribute initial grid
    MPI_Scatterv(full_grid, recvcounts, displs, MPI_INT,
                 &grid[1][0], local_rows * N, MPI_INT,
                 0, MPI_COMM_WORLD);

    int up   = (rank - 1 + size) % size;
    int down = (rank + 1) % size;
    MPI_Request reqs[4];
    MPI_Status  stats[4];

    // Main simulation loop
    for (int step = 0; step < STEPS; step++) {
        MPI_Irecv(grid[0],             N, MPI_INT, up,   0, MPI_COMM_WORLD, &reqs[0]);
        MPI_Irecv(grid[local_rows+1],  N, MPI_INT, down, 1, MPI_COMM_WORLD, &reqs[1]);
        MPI_Isend(grid[1],             N, MPI_INT, up,   1, MPI_COMM_WORLD, &reqs[2]);
        MPI_Isend(grid[local_rows],    N, MPI_INT, down, 0, MPI_COMM_WORLD, &reqs[3]);

        // Compute interior rows
        for (int i = 2; i <= local_rows - 1; i++) {
            for (int j = 0; j < N; j++) {
                int n = count_neighbors(grid, i, j, local_rows);
                new_grid[i][j] = (grid[i][j] == 1)
                    ? ((n < 2 || n > 3) ? 0 : 1)
                    : (n == 3 ? 1 : 0);
            }
        }

        MPI_Waitall(4, reqs, stats);

        // Compute border rows
        for (int t = 0; t < 2; t++) {
            int i = (t == 0 ? 1 : local_rows);
            for (int j = 0; j < N; j++) {
                int n = count_neighbors(grid, i, j, local_rows);
                new_grid[i][j] = (grid[i][j] == 1)
                    ? ((n < 2 || n > 3) ? 0 : 1)
                    : (n == 3 ? 1 : 0);
            }
        }

        // Swap pointers
        int (*tmp)[N] = grid; grid = new_grid; new_grid = tmp;

        // Gather and output every 10 steps on rank 0
        if (step % 10 == 0) {
            MPI_Gatherv(&grid[1][0],      local_rows * N, MPI_INT,
                        full_grid, recvcounts, displs, MPI_INT,
                        0, MPI_COMM_WORLD);
            if (rank == 0) {
                write_output_full(full_grid, step);
            }
        }
    }

    // Cleanup
    if (rank == 0) {
        free(full_grid);
        free(recvcounts);
        free(displs);
    }
    free(grid);
    free(new_grid);
    MPI_Finalize();
    return 0;
}
