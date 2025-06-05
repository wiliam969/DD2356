#!/bin/bash -l

#SBATCH -J A4-ex2-N4-ws4
#SBATCH -t 0:45:00
#SBATCH -A edu25.dd2356
#SBATCH -p shared
#SBATCH --nodes=4
#SBATCH --ntasks-per-node=8
#SBATCH --cpus-per-task=1
#SBATCH -e ex2_N2_ws4.stderr
#SBATCH --mem=16G

process_count=(1 2 4 8)
matrix_sizes=(1000 2000 4000 8000)

. ./perf_env

# Run and redirect output
for proc in "${process_count[@]}"; do
    for size in "${matrix_sizes[@]}"; do
        cc -lm -O3 -march=native -DN="$((proc * size))" ../ex2/ex2_parallel_sum.c -o ../bin/ex2_parallel_weak_n4_sum.out
        srun --cpus-per-task=1 --ntasks-per-node=${proc} perf stat -e ${events} -o ex2_parallel_weak_sum_n4_${proc}_${size}.data ../bin/ex2_parallel_weak_n4_sum.out > ../batch_output/ex2_parallel_weak_sum_n4_${proc}_${size}_output.stdout
    done
done
