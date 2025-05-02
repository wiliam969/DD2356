#!/bin/bash -l

#SBATCH -J A3-ex3-serial
#SBATCH -t 0:10:00
#SBATCH -A edu25.dd2356
#SBATCH -p shared
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=32
#SBATCH -e ex_3_openmp_critical.sdtderr

gcc -O3 -fopenmp ../omp_critical_sum.c -o ../omp_critical_sum.out
threads=(1)

for num_threads in "${threads[@]}"; do
    # Repeat  5 times
    for run in {1..5}; do
        export OMP_NUM_THREADS=${num_threads}

        # Run and redirect output
        srun -n 1 ../omp_critical_sum.out 100 >> "ex_3_openmp_critical_${num_threads}_output.stdout"
    done
done