#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Function to parse the RLE file and initialize the full grid on rank 0
int parse_rle_file(const char *filename, int N, int **grid) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        fprintf(stderr, "Error: Cannot open RLE file '%s'\n", filename);
        return -1;
    }
    // Allocate full NxN grid and initialize all cells to dead (0)
    int total_cells = N * N;
    *grid = (int*) malloc(total_cells * sizeof(int));
    if (*grid == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for grid\n");
        fclose(f);
        return -1;
    }
    for (int i = 0; i < total_cells; ++i) {
        (*grid)[i] = 0;
    }

    // Read past any comment lines (starting with '#')
    char line[256];
    int width = 0, height = 0;
    // First non-comment line should contain "x = <width>, y = <height>"
    while (fgets(line, sizeof(line), f)) {
        if (line[0] == '#') continue;         // skip comments
        if (strlen(line) == 0) continue;      // skip empty lines
        // Check if this line contains the x,y header
        if (sscanf(line, "x = %d , y = %d", &width, &height) == 2) {
            break;
        }
    }
    if (width <= 0 || height <= 0) {
        fprintf(stderr, "Error: RLE header not found or invalid in '%s'\n", filename);
        fclose(f);
        free(*grid);
        return -1;
    }
    // Determine placement offset to center the pattern in the N x N grid (if smaller)
    int offset_row = 0;
    int offset_col = 0;
    if (height < N) offset_row = (N - height) / 2;
    if (width  < N) offset_col = (N - width) / 2;

    // Read the RLE pattern lines and decode them
    int row = 0, col = 0;
    // We may need to concatenate multiple lines of RLE, as the pattern can span lines
    char rle_data[16384];
    rle_data[0] = '\0';
    // Concatenate all remaining lines into rle_data until we hit '!'
    while (fgets(line, sizeof(line), f)) {
        if (line[0] == '#') continue;  // skip comment lines in pattern, just in case
        // Remove end-of-line characters
        size_t len = strlen(line);
        while (len > 0 && (line[len-1] == '\n' || line[len-1] == '\r')) {
            line[len-1] = '\0';
            len--;
        }
        if (len == 0) continue;
        // Append line to rle_data
        strcat(rle_data, line);
        // If line contains '!' (end of pattern), stop reading further
        if (strchr(line, '!') != NULL) {
            break;
        }
    }
    fclose(f);

    // Now parse the rle_data string
    // According to RLE spec: digits denote repeat count for next symbol; symbols are 'b', 'o', '$', '!'
    int repeat = 0;
    for (char *p = rle_data; *p != '\0'; ++p) {
        if (isdigit((unsigned char)*p)) {
            // build up the repeat count (could be multiple digits)
            repeat = repeat * 10 + (*p - '0');
        } else if (*p == 'b' || *p == 'o') {
            // If no repeat count was specified, default is 1
            int count = (repeat == 0 ? 1 : repeat);
            // Determine the value to set (o = 1, b = 0)
            int val = (*p == 'o') ? 1 : 0;
            for (int k = 0; k < count; ++k) {
                if (row + offset_row < N && col + offset_col < N) {
                    (*grid)[(row + offset_row) * N + (col + offset_col)] = val;
                }
                col++;
            }
            repeat = 0;  // reset repeat
        } else if (*p == '$') {
            // End of a row in the pattern. If a number precedes '$', it indicates how many rows to skip.
            int count = (repeat == 0 ? 1 : repeat);
            repeat = 0;
            row += count;
            col = 0;
        } else if (*p == '!') {
            // End of pattern
            break;
        }
        // (Any other characters like spaces are ignored)
    }
    return 0;  // success
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Default parameters (can be overridden by command line arguments):
    int N = 300;               // grid size N x N
    int steps = 500;         // number of generations to simulate
    char *input_file = NULL;
    if (rank == 0) {
        // Parse command line arguments for input file, N, and steps
        if (argc < 2) {
            fprintf(stderr, "Usage: %s <pattern.rle> [N] [steps]\n", argv[0]);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        input_file = argv[1];
        if (argc >= 3) {
            N = atoi(argv[2]);
        }
        if (argc >= 4) {
            steps = atoi(argv[3]);
        }
    }

    // Broadcast N and steps from rank 0 to everyone (and possibly input file name length)
    // First, if N was not provided, rank 0 should determine N from the pattern size
    int *full_grid = NULL;
    if (rank == 0) {
        // If N was not specified by user, we might use the pattern's own width/height as N.
        // parse_rle_file will fill the grid; we need N before calling it if it wasn't given.
        // We handle that by doing a pre-scan of the file to get width,height if needed.
        if (N == 0) {
            // Open file just to read width and height from header
            FILE *f = fopen(input_file, "r");
            if (!f) {
                fprintf(stderr, "Error: Cannot open file '%s'\n", input_file);
                MPI_Abort(MPI_COMM_WORLD, 1);
            }
            char header[128];
            int w=0, h=0;
            while (fgets(header, sizeof(header), f)) {
                if (header[0] == '#') continue;
                if (sscanf(header, "x = %d , y = %d", &w, &h) == 2) {
                    break;
                }
            }
            fclose(f);
            if (w <= 0 || h <= 0) {
                fprintf(stderr, "Error: Invalid RLE header, cannot determine grid size\n");
                MPI_Abort(MPI_COMM_WORLD, 1);
            }
            // Choose N as max of width and height to accommodate pattern, or use width if expecting non-rectangular:
            N = (w > h ? w : h);
        }
    }
    // Broadcast N and steps to all processes
    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&steps, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Rank 0 reads and initializes the full grid
    if (rank == 0) {
        if (parse_rle_file(input_file, N, &full_grid) != 0) {
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }

    // Now distribute the grid to all processes
    // Determine how many rows each process gets
    int base_rows = N / size;        // integer division
    int extra = N % size;           // remainder for uneven division
    int local_rows;
    if (rank < extra) {
        local_rows = base_rows + 1;
    } else {
        local_rows = base_rows;
    }

    // Calculate displacements and counts for scatter/gather
    int *sendcounts = NULL;
    int *displs = NULL;
    if (rank == 0) {
        sendcounts = (int*) malloc(size * sizeof(int));
        displs     = (int*) malloc(size * sizeof(int));
        int offset = 0;
        for (int r = 0; r < size; ++r) {
            int rows = (r < extra ? base_rows + 1 : base_rows);
            sendcounts[r] = rows * N;
            displs[r] = offset;
            offset += rows * N;
        }
    }

    // Allocate local arrays (including space for ghost rows)
    // We allocate local_rows for the actual data, and 2 extra rows for ghosts.
    int local_size = local_rows * N;
    int *local_current = (int*) malloc(local_size * sizeof(int));
    int *local_next    = (int*) malloc(local_size * sizeof(int));
    int *ghost_top     = (int*) malloc(N * sizeof(int));
    int *ghost_bottom  = (int*) malloc(N * sizeof(int));
    if (!local_current || !local_next || !ghost_top || !ghost_bottom) {
        fprintf(stderr, "Rank %d: Memory allocation failed for local grids\n", rank);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    // Scatter the initial grid rows from rank 0 to all ranks
    MPI_Scatterv(full_grid, sendcounts, displs, MPI_INT,
                 local_current, local_size, MPI_INT,
                 0, MPI_COMM_WORLD);
    // If full_grid was allocated on rank 0, we can free it now (to save memory)
    if (rank == 0) {
        free(full_grid);
        free(sendcounts);
        free(displs);
    }

    // Initialize ghost halos for top and bottom.
    // For interior processes, we'll fill these after exchanges; for boundary processes, they stay 0.
    for (int j = 0; j < N; ++j) {
        ghost_top[j] = 0;
        ghost_bottom[j] = 0;
    }

    // Convenience: determine neighbor ranks for up and down
    int neighbor_up   = (rank == 0 ? MPI_PROC_NULL : rank - 1);
    int neighbor_down = (rank == size - 1 ? MPI_PROC_NULL : rank + 1);

    // Simulation loop
    for (int step = 1; step <= steps; step++) {
        // ** Halo exchange: share boundary rows with neighbors **
        // Send bottom row to neighbor_down, receive from neighbor_up into ghost_top
        MPI_Sendrecv(
            /* send */ (local_current + (local_rows - 1) * N), N, MPI_INT, neighbor_down, 0,
            /* recv */ ghost_top, N, MPI_INT, neighbor_up, 0,
            MPI_COMM_WORLD, MPI_STATUS_IGNORE
        );
        // Send top row to neighbor_up, receive from neighbor_down into ghost_bottom
        MPI_Sendrecv(
            /* send */ local_current, N, MPI_INT, neighbor_up, 1,
            /* recv */ ghost_bottom, N, MPI_INT, neighbor_down, 1,
            MPI_COMM_WORLD, MPI_STATUS_IGNORE
        );
        // Note: We used different tags (0 and 1) for the two directions to avoid any confusion.

        // ** Compute next generation for local cells **
        for (int i = 0; i < local_rows; ++i) {
            for (int j = 0; j < N; ++j) {
                int global_i; 
                // Determine the global row index of the cell (needed for boundary logic)
                // We can compute global offset by how many rows were before this rank.
                // Actually, easier: use displs from scatter. But we freed displs on non-root.
                // Instead, compute global_i manually:
                // global_i = (rank's preceding rows) + i, where preceding rows = rank*base_rows + min(rank, extra).
                int rows_before = rank * base_rows + (rank < extra ? rank : extra);
                global_i = rows_before + i;
                // Count live neighbors
                int live_neighbors = 0;
                // Neighbor coordinates relative to current subgrid
                // We will check each of the 8 neighbors with careful boundary checks
                for (int di = -1; di <= 1; ++di) {
                    for (int dj = -1; dj <= 1; ++dj) {
                        if (di == 0 && dj == 0) continue;  // skip the cell itself
                        int ni = i + di;
                        int nj = j + dj;
                        int neighbor_val;
                        if (ni < 0) {
                            // Above the local subgrid: use ghost_top
                            // Only if global neighbor exists (global_i = 0 means out of global grid)
                            neighbor_val = (neighbor_up == MPI_PROC_NULL) ? 0 : 
                                           ((nj >= 0 && nj < N) ? ghost_top[nj] : 0);
                        } else if (ni >= local_rows) {
                            // Below the local subgrid: use ghost_bottom
                            neighbor_val = (neighbor_down == MPI_PROC_NULL) ? 0 :
                                           ((nj >= 0 && nj < N) ? ghost_bottom[nj] : 0);
                        } else {
                            // Neighbor in the local subgrid (same process)
                            if (nj < 0 || nj >= N) {
                                // Out of bounds horizontally (outside global grid)
                                neighbor_val = 0;
                            } else {
                                neighbor_val = local_current[ni * N + nj];
                            }
                        }
                        live_neighbors += neighbor_val;
                    }
                }
                // Apply Game of Life rules to determine this cell's fate
                int current_state = local_current[i * N + j];
                int next_state;
                if (current_state == 1) {
                    // Cell is alive
                    if (live_neighbors < 2 || live_neighbors > 3) {
                        next_state = 0; // dies
                    } else {
                        next_state = 1; // lives on
                    }
                } else {
                    // Cell is dead
                    if (live_neighbors == 3) {
                        next_state = 1; // becomes alive
                    } else {
                        next_state = 0; // stays dead
                    }
                }
                local_next[i * N + j] = next_state;
            }
        }

        // Swap current and next generation buffers
        int *temp = local_current;
        local_current = local_next;
        local_next = temp;

        // ** Gather and output every 10 steps **
        if (step % 10 == 0) {
            // Allocate receive buffer only on root for the full grid
            int *gather_buf = NULL;
            int *recvcounts = NULL;
            int *recvdispls = NULL;
            if (rank == 0) {
                gather_buf = (int*) malloc(N * N * sizeof(int));
                recvcounts = (int*) malloc(size * sizeof(int));
                recvdispls = (int*) malloc(size * sizeof(int));
                // Recompute counts and displacements (since we freed after scatter)
                int offset = 0;
                for (int r = 0; r < size; ++r) {
                    int rows = (r < extra ? base_rows + 1 : base_rows);
                    recvcounts[r] = rows * N;
                    recvdispls[r] = offset;
                    offset += rows * N;
                }
            }
            // Use MPI_Gatherv to collect all subgrids at root
            MPI_Gatherv(local_current, local_rows * N, MPI_INT,
                        gather_buf, recvcounts, recvdispls, MPI_INT,
                        0, MPI_COMM_WORLD);
            if (rank == 0) {
                // Write the gathered grid to file
                char filename[64];
                sprintf(filename, "mpi_gol_output_%04d.txt", step);
                FILE *out = fopen(filename, "w");
                if (!out) {
                    fprintf(stderr, "Error: Cannot open output file '%s'\n", filename);
                } else {
                    for (int r = 0; r < N; ++r) {
                        for (int c = 0; c < N; ++c) {
                            int cell = gather_buf[r * N + c];
                            fputc(cell ? '1' : '0', out);
                            if (c < N - 1) fputc(' ', out);  // space separate (optional)
                        }
                        fputc('\n', out);
                    }
                    fclose(out);
                }
                free(gather_buf);
                free(recvcounts);
                free(recvdispls);
            }
        }
    }

    // Free local buffers
    free(local_current);
    free(local_next);
    free(ghost_top);
    free(ghost_bottom);

    MPI_Finalize();
    return 0;
}
