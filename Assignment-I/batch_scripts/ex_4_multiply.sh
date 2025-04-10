#!/bin/bash -l
# The -l above is required to get the full environment with modules

# Set the allocation (project) to be charged for this job
#SBATCH -A edu25.DD2356

# The name of the job
#SBATCH -J matrix_multiply

# The partition/queue
#SBATCH -p shared

# 10 hours of wall clock time
#SBATCH -t 00:15:00

# Number of nodes
#SBATCH --nodes=1

#SBATCH --mem=20G

# Number of MPI processes per node
#SBATCH --ntasks-per-node=1

# Optional: if your cluster uses cpus-per-task or similar, specify them if needed
# #SBATCH --cpus-per-task=1

# Run the executable named main.x and write the output to my_output_file.txt
srun  perf stat -e L1-dcache-load-misses,L1-dcache-loads,L1-dcache-prefetches,L1-icache-load-misses,L1-icache-loads,duration_time,cpu-cycles,instructions ./matrix_multiply.out 64 > matrix_multiply_64.txt
srun  perf stat -e L1-dcache-load-misses,L1-dcache-loads,L1-dcache-prefetches,L1-icache-load-misses,L1-icache-loads,duration_time,cpu-cycles,instructions ./matrix_multiply_opt.out 64 > matrix_multiply_opt_64.txt
srun  perf stat -e L1-dcache-load-misses,L1-dcache-loads,L1-dcache-prefetches,L1-icache-load-misses,L1-icache-loads,duration_time,cpu-cycles,instructions ./matrix_multiply.out 1000 > matrix_multiply_1000.txt
srun  perf stat -e L1-dcache-load-misses,L1-dcache-loads,L1-dcache-prefetches,L1-icache-load-misses,L1-icache-loads,duration_time,cpu-cycles,instructions ./matrix_multiply_opt.out 1000 > matrix_multiply_opt_1000.txt
