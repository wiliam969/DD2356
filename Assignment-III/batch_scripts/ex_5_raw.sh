#!/bin/bash -l
# The -l above is required to get the full environment with modules

# Set the allocation (project) to be charged for this job
#SBATCH -A edu25.DD2356

# The name of the job
#SBATCH -J ex_5_sh_wt_raw

# The partition/queue
#SBATCH -p shared

# 10 hours of wall clock time
#SBATCH -t 00:01:00

# Number of nodes
#SBATCH --nodes=1

#SBATCH --ntasks=1 # just one task boyz 

# we will have 16 openmp tasks i guess lets go 
#SBATCH --cpus-per-task=1

cc -O3 -march=native -fopenmp ../shallow_water.c -o ../shallow_water.out

# Use srun to launch the executable using the allocated cores
srun ../shallow_water.out > output.txt