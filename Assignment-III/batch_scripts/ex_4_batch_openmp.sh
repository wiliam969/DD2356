#!/bin/bash -l
# The -l above is required to get the full environment with modules

# Set the allocation (project) to be charged for this job
#SBATCH -A edu25.DD2356

# The name of the job
#SBATCH -J dftw_homework

# The partition/queue
#SBATCH -p shared

# 10 hours of wall clock time
#SBATCH -t 00:15:00

# Number of nodes
#SBATCH --nodes=1

#SBATCH --ntasks=1 # just one task boyz 

# we will have 16 openmp tasks i guess lets go 
#SBATCH --cpus-per-task=16

#SBATCH --output=DFTW_%j.out

cc -O3 -march=native -fopenmp DFTW_1_openmp.c -o DFTW_1_openmp.out

# Set the number of OpenMP threads
export OMP_NUM_THREADS=32

# Number of runs to perform
num_runs=15

for run in $(seq 1 $num_runs); do
    echo "Starting run $run..."
    # Use srun to launch the executable using the allocated cores
    srun ./DFTW_1_openmp.out
done