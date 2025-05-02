#!/bin/bash -l

#SBATCH -J A3-ex3-serial
#SBATCH -t 0:05:00
#SBATCH -A edu25.dd2356
#SBATCH -p shared
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=1

gcc -O3 -fopenmp ../serial_sum.c -o ../serial_sum.out

srun -n 1 ../hwmp.out 1000

