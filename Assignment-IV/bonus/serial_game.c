#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define N 300          // Grid size at least as big as RLE pattern dimensions
#define STEPS 510     // Number of simulation steps

int grid[N][N];
int new_grid[N][N];

// Load RLE pattern into grid, placing origin at top-left
void load_rle(const char *fname) {
    FILE *f = fopen(fname, "r");
    if (!f) { perror(fname); exit(EXIT_FAILURE); }
    char line[256];
    // Skip comments and parse header
    while (fgets(line, sizeof(line), f)) {
        if (line[0] == '#') continue;
        int x, y;
        if (sscanf(line, "x = %d, y = %d", &x, &y) >= 2) break;
    }
    // Initialize grid to dead
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            grid[i][j] = 0;

    // Read body into buffer
    char *body = malloc(100000);
    size_t pos = 0;
    while (fgets(line, sizeof(line), f)) {
        size_t len = strlen(line);
        memcpy(body + pos, line, len);
        pos += len;
    }
    body[pos] = '\0';
    fclose(f);

    int x = 0, y = 0, count = 0;
    for (char *p = body; *p && *p != '!'; p++) {
        if (isdigit(*p)) {
            count = count * 10 + (*p - '0');
        } else if (*p == 'b' || *p == 'o') {
            if (count == 0) count = 1;
            for (int k = 0; k < count; k++) {
                if (y < N && x < N) {
                    grid[y][x] = (*p == 'o');
                }
                x++;
            }
            count = 0;
        } else if (*p == '$') {
            if (count == 0) count = 1;
            y += count;
            x = 0;
            count = 0;
        }
        // ignore other chars (spaces, line breaks)
    }
    free(body);
}

void load_rle_centered(const char *fname) {
    FILE *f = fopen(fname, "r");
    if (!f) { perror(fname); exit(1); }

    char line[256];
    int pat_w = 0, pat_h = 0;
    // 1) Skip comments until we read x,y
    while (fgets(line, sizeof(line), f)) {
        if (line[0] == '#') continue;
        if (sscanf(line, "x = %d, y = %d", &pat_w, &pat_h) == 2) {
            break;
        }
    }

    // 2) Compute centering offsets
    int offset_row = (N - pat_h) / 2;
    int offset_col = (N - pat_w) / 2;

    // 3) Zero the grid
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            grid[i][j] = 0;

    // 4) Read the body into a big buffer
    char *body = malloc(100000);
    size_t pos = 0;
    while (fgets(line, sizeof(line), f)) {
        size_t len = strlen(line);
        memcpy(body + pos, line, len);
        pos += len;
    }
    body[pos] = '\0';
    fclose(f);

    // 5) Decode runs, placing at (offset_row + y, offset_col + x)
    int x = 0, y = 0, count = 0;
    for (char *p = body; *p && *p != '!'; p++) {
        if (isdigit(*p)) {
            count = count * 10 + (*p - '0');
        }
        else if (*p == 'b' || *p == 'o') {
            if (count == 0) count = 1;
            for (int k = 0; k < count; k++) {
                int gi = offset_row + y;
                int gj = offset_col + x;
                if (gi >= 0 && gi < N && gj >= 0 && gj < N)
                    grid[gi][gj] = (*p == 'o');
                x++;
            }
            count = 0;
        }
        else if (*p == '$') {
            if (count == 0) count = 1;
            y += count;
            x  = 0;
            count = 0;
        }
        // ignore spaces, line breaks, etc.
    }
    free(body);
}


// Count live neighbors with wrap-around
int count_neighbors(int i, int j) {
    int sum = 0;
    for (int di = -1; di <= 1; di++) {
        for (int dj = -1; dj <= 1; dj++) {
            if (di == 0 && dj == 0) continue;
            int ni = (i + di + N) % N;
            int nj = (j + dj + N) % N;
            sum += grid[ni][nj];
        }
    }
    return sum;
}

// Apply Game of Life rules
void update() {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            int n = count_neighbors(i, j);
            if (grid[i][j] == 1) {
                new_grid[i][j] = (n < 2 || n > 3) ? 0 : 1;
            } else {
                new_grid[i][j] = (n == 3) ? 1 : 0;
            }
        }
    }
    // Copy back
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            grid[i][j] = new_grid[i][j];
}

// Write grid to file
void write_output(int step) {
    char fname[64];
    sprintf(fname, "serial_gol_output_%d.txt", step);
    FILE *f = fopen(fname, "w");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            fprintf(f, "%d ", grid[i][j]);
        }
        fprintf(f, "\n");
    }
    fclose(f);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s pattern.rle\n", argv[0]);
        return EXIT_FAILURE;
    }

    load_rle_centered(argv[1]);
    write_output(0);

    for (int step = 1; step <= STEPS; step++) {
        update();
        if (step % 10 == 0) write_output(step);
    }

    return EXIT_SUCCESS;
}
