#!/bin/bash -l

#SBATCH -J A3-ex3-serial
#SBATCH -t 0:10:00
#SBATCH -A edu25.dd2356
#SBATCH -p shared
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=128
#SBATCH -e opt_local_sum.stderr

threads=(1 32 64 128)

for num_threads in "${threads[@]}"; do
    gcc -O3 -fopenmp -DMAX_THREADS=${num_threads} ../opt_local_sum.c -o ../opt_local_sum.out
    
    for run in {1..3}; do
        export OMP_NUM_THREADS=${num_threads}

        srun -n 1 ../opt_local_sum.out 10000000 >> "ex_3_opt_local_sum_${num_threads}_output.stdout"
    done
done