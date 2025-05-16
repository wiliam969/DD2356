#!/bin/bash -l
#SBATCH -J A4-ex4-ser
#SBATCH -t 0:20:00
#SBATCH -A edu25.dd2356
#SBATCH -p shared
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=1
#SBATCH -e A4-ex4-ser.stderr
matrix_sizes=(1000 10000 100000 1000000)

. ./perf_env

# Run and redirect output
for proc in "${process_count[@]}"; do
    for size in "${matrix_sizes[@]}"; do
        cc -lm -O3 -march=native -DMATRIX_SIZE=${size} ../ex4/blas_serial.c -o ../bin/blas_serial.out
        srun -n ${proc} perf stat -e ${events} -o ex4_stat_serial_${proc}_${size}.stat ../bin/blas_serial.out > ../batch_output/ex4_serial_${proc}_${size}_output.stdout
    done
done