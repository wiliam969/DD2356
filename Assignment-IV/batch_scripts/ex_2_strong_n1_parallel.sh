#!/bin/bash -l

#SBATCH -J A4-ex2-N1-ss1
#SBATCH -t 1:45:00
#SBATCH -A edu25.dd2356
#SBATCH -p shared
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=8
#SBATCH --cpus-per-task=1
#SBATCH -e ex2_p_N1_ss1.stderr
#SBATCH --mem=16G

process_count=(1 2 4 8)
matrix_sizes=(1000 2000 4000 8000 10000)

. ./perf_env

# Run and redirect output
for proc in "${process_count[@]}"; do
    for size in "${matrix_sizes[@]}"; do
        cc -lm -O3 -march=native -DN=${size} -DIO_ON_OFF=0 ../ex2/ex2_parallel_sum.c -o ../bin/ex2_parallel_strong_n1_sum.out
        srun --cpus-per-task=1 --ntasks-per-node=${proc} perf stat -e ${events} -o ex2_parallel_strong_sum_n1_${proc}_${size}.data ../bin/ex2_parallel_strong_n1_sum.out > ../batch_output/ex2_parallel_strong_sum_n1_${proc}_${size}_output.stdout
    done
done
