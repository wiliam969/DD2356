#!/bin/bash -l
#SBATCH -J A4-ex4-n1
#SBATCH -t 1:20:00
#SBATCH -A edu25.dd2356
#SBATCH -p shared
#SBATCH --nodes=1
#SBATCH --cpus-per-task=1
#SBATCH -e A4-ex4-n1.stderr
#SBATCH --mem=16G

process_count=(1 2 4 8)
matrix_sizes=(1000 10000 100000)

. ./perf_env

# Run and redirect output
for proc in "${process_count[@]}"; do
    for size in "${matrix_sizes[@]}"; do
        cc -lm -O3 -march=native -DMATRIX_SIZE=${size} ../ex4/blas_mpi_async.c -o ../bin/blas_mpi_async.out
        srun --cpus-per-task=1 --ntasks-per-node=${proc} perf stat -e ${events} -o ex4_mpi__async_stat_${proc}_${size}.stat ../bin/blas_mpi_async.out > ../batch_output/ex4_mpi_async_${proc}_${size}_output.stdout
    done
done
