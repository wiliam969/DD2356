#!/bin/bash -l

#SBATCH -J A4-ex2-serial
#SBATCH -t 1:45:00
#SBATCH -A edu25.dd2356
#SBATCH -p shared
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=1
#SBATCH -e ex2_serial_sum.stderr
#SBATCH --mem=16G
matrix_sizes=(1000 2000 4000 6000)

. ./perf_env

for size in "${matrix_sizes[@]}"; do
    gcc -O3 -fopenmp -DN=${size} ../ex2/ex2_serial_sum.c -o ../bin/ex2_serial_sum.out
    # Run and redirect output
    srun --cpus-per-task=1 --ntasks-per-node=1 perf stat -e ${events} -o ex2_serial_${size}.data ../bin/ex2_serial_sum.out >> ../batch_output/ex_2_serial_output.stdout
done