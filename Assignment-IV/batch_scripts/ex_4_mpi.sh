#!/bin/bash -l
#SBATCH -J A4-ex4-n1
#SBATCH -t 0:20:00
#SBATCH -A edu25.dd2356
#SBATCH -p shared
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=8
#SBATCH --cpus-per-task=1
#SBATCH -e A4-ex4-n1.stderr
process_count=(1 2 4 8)
matrix_sizes=(1000 10000 100000 1000000)

. ./perf_env

# Run and redirect output
for proc in "${process_count[@]}"; do
    for size in "${matrix_sizes[@]}"; do
        cc -lm -O3 -march=native -DN=${size} ../ex4/blas_mpi.c -o ../bin/blas_mpi.out
        srun -n ${proc} perf stat -e ${events} -o mpi_game-stat.stat ../bin/blas_mpi.out > ../batch_output/ex4_mpi_${proc}_${size}_output.stdout
    done
done