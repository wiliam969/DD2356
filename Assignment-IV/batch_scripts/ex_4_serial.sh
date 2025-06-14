#!/bin/bash -l
#SBATCH -J A4-ex4-ser
#SBATCH -t 0:20:00
#SBATCH -A edu25.dd2356
#SBATCH -p shared
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=1
#SBATCH -e A4-ex4-ser.stderr
#SBATCH --mem=16G
 
matrix_sizes=(500 1000 2000 4000 8000 10000 20000 40000)

. ./perf_env

# Run and redirect output
for size in "${matrix_sizes[@]}"; do
    cc -lm -O3 -march=native -DMATRIX_SIZE=${size} -DIO_ON_OFF=0 ../ex4/blas_serial.c -o ../bin/blas_serial.out
    srun --ntasks-per-node=1 --cpus-per-task=1 perf stat -e ${events} -o ex4_stat_serial_${size}.stat ../bin/blas_serial.out > ../batch_output/ex4_serial_${size}_output.stdout
done
