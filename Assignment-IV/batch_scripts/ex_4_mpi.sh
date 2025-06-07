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
matrix_sizes=(500 1000 2000 4000 8000 10000)

. ./perf_env

# Run and redirect output
for proc in "${process_count[@]}"; do
    for size in "${matrix_sizes[@]}"; do
        cc -lm -O3 -march=native -DMATRIX_SIZE=${size} -DIO_ON_OFF=0 ../ex4/blas_mpi.c -o ../bin/blas_mpi.out
        srun --cpus-per-task=1 --ntasks-per-node=${proc} perf stat -e ${events} -o ex4_mpi_stat_${proc}_${size}.stat ../bin/blas_mpi.out > ../batch_output/ex4_mpi_${proc}_${size}_output.stdout
    done
done
