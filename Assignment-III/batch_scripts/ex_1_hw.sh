#!/bin/bash -l

#SBATCH -J A3-ex1
#SBATCH -t 0:10:00
#SBATCH -A edu25.dd2356
#SBATCH -p shared
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1

salloc -n 1 -t 0:10:00 -A edu25.DD2356 -p shared
srun -n 1 ./hwmp.x 
srun hwloc-ls --of svg > hwmp_topo.svg

