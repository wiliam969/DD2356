#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define N       20    // global grid size
#define STEPS   300   // total timesteps

// local grids will be of size (local_rows+2) x N
static int *grid, *new_grid;

// index accessor for 2D flattened arrays
#define IDX(i,j)   ((i)*(N) + (j))

// count neighbors in your local grid (with ghost rows at 0 and local_rows+1)
int count_neighbors(int *g, int i, int j, int local_rows) {
    int sum = 0,
        up    = (i-1 + local_rows+2) % (local_rows+2),
        down  = (i+1)   % (local_rows+2);

    int left  = (j-1 + N) % N,
        right = (j+1)       % N;

    sum += g[IDX(up,   left)];
    sum += g[IDX(up,   j)];
    sum += g[IDX(up,   right)];
    sum += g[IDX(i,    left)];
    sum += g[IDX(i,    right)];
    sum += g[IDX(down, left)];
    sum += g[IDX(down, j)];
    sum += g[IDX(down, right)];
    return sum;
}

void write_output_full(int *full, int step) {
    char fname[64];
    sprintf(fname, "mpi_gol_output_%d.txt", step);
    FILE *f = fopen(fname, "w");
    for(int i=0;i<N;i++){
      for(int j=0;j<N;j++){
        fprintf(f, "%d ", full[i*N + j]);
      }
      fprintf(f, "\n");
    }
    fclose(f);
}

int main(int argc, char **argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Figure out how many rows each rank gets (block distribution)
    int base = N / size;
    int rem  = N % size;
    int local_rows = base + (rank < rem ? 1 : 0);

    // Allocate with two extra ghost rows
    grid     = malloc((local_rows+2) * N * sizeof(int));
    new_grid = malloc((local_rows+2) * N * sizeof(int));

    // Seed and initialize only inner rows [1..local_rows]
    srand(time(NULL) + rank*123);
    for(int i = 1; i <= local_rows; i++) {
      for(int j = 0; j < N; j++) {
        grid[IDX(i,j)] = rand() % 2;
      }
    }

    // Pre‐compute neighbor ranks (periodic)
    int up   = (rank - 1 + size) % size;
    int down = (rank + 1     ) % size;

    MPI_Request reqs[4];
    MPI_Status  stats[4];

    // Buffer for gather on rank 0
    int *full_grid = NULL;
    int *recvcounts = NULL, *displs = NULL;
    if (rank == 0) {
      full_grid  = malloc(N*N * sizeof(int));
      recvcounts = malloc(size * sizeof(int));
      displs     = malloc(size * sizeof(int));
      // set up displacements for MPI_Gatherv
      int offset = 0;
      for(int r=0;r<size;r++){
        int rows_r = base + (r<rem?1:0);
        recvcounts[r] = rows_r * N;
        displs[r]     = offset;
        offset       += rows_r * N;
      }
    }

    for(int step=0; step<STEPS; step++) {
      // 1) Post non-blocking receives into ghost rows
      MPI_Irecv(&grid[IDX(0,0)],        N, MPI_INT, up,   0, MPI_COMM_WORLD, &reqs[0]);
      MPI_Irecv(&grid[IDX(local_rows+1,0)], N, MPI_INT, down, 1, MPI_COMM_WORLD, &reqs[1]);
      // 2) Post non-blocking sends from real border rows
      MPI_Isend(&grid[IDX(1,0)],        N, MPI_INT, up,   1, MPI_COMM_WORLD, &reqs[2]);
      MPI_Isend(&grid[IDX(local_rows,0)],   N, MPI_INT, down, 0, MPI_COMM_WORLD, &reqs[3]);

      // 3) Update interior rows [2..local_rows-1] while halos are in flight
      for(int i=2; i<=local_rows-1; i++) {
        for(int j=0; j<N; j++) {
          int n = count_neighbors(grid, i, j, local_rows);
          int idx = IDX(i,j);
          if      (grid[idx]==1 && (n<2 || n>3)) new_grid[idx] = 0;
          else if (grid[idx]==0 && n==3)         new_grid[idx] = 1;
          else                                   new_grid[idx] = grid[idx];
        }
      }

      // 4) Wait for all halo exchanges to complete
      MPI_Waitall(4, reqs, stats);

      // 5) Update the two border rows [1] and [local_rows]
      for(int ii = 1; ii <= local_rows; ii += (local_rows-1)) {
        for(int j=0; j<N; j++) {
          int n = count_neighbors(grid, ii, j, local_rows);
          int idx = IDX(ii,j);
          if      (grid[idx]==1 && (n<2 || n>3)) new_grid[idx] = 0;
          else if (grid[idx]==0 && n==3)         new_grid[idx] = 1;
          else                                   new_grid[idx] = grid[idx];
        }
      }

      // 6) Swap grids
      int *tmp = grid; grid = new_grid; new_grid = tmp;

      // 7) Every 10 steps: gather and write on rank 0
      if (step % 10 == 0) {
        MPI_Gatherv(&grid[IDX(1,0)], local_rows*N, MPI_INT,
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
