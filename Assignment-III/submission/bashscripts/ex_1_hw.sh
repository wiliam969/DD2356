#!/bin/bash -l

#SBATCH -J A3-ex1
#SBATCH -t 0:10:00
#SBATCH -A edu25.dd2356
#SBATCH -p shared
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=4

gcc -O3 -fopenmp ../hwmp.c -o ../hwmp.x

srun -n 1 ../hwmp.x 
srun hwloc-ls --of svg > hwmp_topo.svg

