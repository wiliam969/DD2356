#!/bin/bash -l

#SBATCH -J A4-ex2-serial
#SBATCH -t 0:05:00
#SBATCH -A edu25.dd2356
#SBATCH -p shared
#SBATCH --nodes=2
#SBATCH --ntasks-per-node=2
#SBATCH --cpus-per-task=1
#SBATCH -e ex2_serial_sum.stderr

gcc -O3 -fopenmp ../ex_2/ex2_parallel_sum.c -o ../bin/ex2_parallel_sum.out

# Run and redirect output
srun -N 2 ../bin/ex2_parallel_sum.out > "../batch_output/ex2_parallel_sum_output.stdout"