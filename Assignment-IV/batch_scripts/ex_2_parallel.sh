#!/bin/bash -l

#SBATCH -J A4-ex2-mpi
#SBATCH -t 0:15:00
#SBATCH -A edu25.dd2356
#SBATCH -p shared
#SBATCH --nodes=2
#SBATCH --ntasks-per-node=2
#SBATCH --cpus-per-task=1
#SBATCH -e ex2_mpi.stderr

cc -lm -O3 -march=native ../ex2/ex2_parallel_sum.c -o ../bin/ex2_parallel_sum.out

# Run and redirect output
srun perf record -e -N 2 ../bin/ex2_parallel_sum.out > "../batch_output/ex2_parallel_sum_output.stdout"