#!/bin/bash -l
# The -l above is required to get the full environment with modules

# Set the allocation (project) to be charged for this job
#SBATCH -A edu25.DD2356

# The name of the job
#SBATCH -J ex_5_sh_wt

# The partition/queue
#SBATCH -p shared

# 10 hours of wall clock time
#SBATCH -t 00:15:00

# Number of nodes
#SBATCH --nodes=1

#SBATCH --ntasks=1 # just one task boyz 

# we will have 16 openmp tasks i guess lets go 
#SBATCH --cpus-per-task=32

# Define the thread configurations to test
threads=(1 2 4 8 16 32)

cc -O3 -march=native -fopenmp -DCHUNK_SIZE=32 -DN500 ../shallow_water_static.c -o ../shallow_water_static.out

# Loop over each thread configuration
for num_threads in "${threads[@]}";
do
    echo "Running with ${num_threads} threads"

    # Set the number of OpenMP threads
    export OMP_NUM_THREADS=${num_threads}
    # Use srun to launch the executable using the allocated cores
    srun ../shallow_water_static.out > performance_data_static_${num_threads}_500.txt
done

cc -O3 -march=native -fopenmp -DCHUNK_SIZE=32 -DN1000 ../shallow_water_static.c -o ../shallow_water_static.out

# Loop over each thread configuration
for num_threads in "${threads[@]}";
do
    echo "Running with ${num_threads} threads"

    # Set the number of OpenMP threads
    export OMP_NUM_THREADS=${num_threads}
    # Use srun to launch the executable using the allocated cores
    srun ../shallow_water_static.out > performance_data_static_${num_threads}_1000.txt
done

cc -O3 -march=native -fopenmp -DCHUNK_SIZE=32 -DN2000 ../shallow_water_static.c -o ../shallow_water_static.out

# Loop over each thread configuration
for num_threads in "${threads[@]}";
do
    echo "Running with ${num_threads} threads"

    # Set the number of OpenMP threads
    export OMP_NUM_THREADS=${num_threads}
    # Use srun to launch the executable using the allocated cores
    srun ../shallow_water_static.out > performance_data_static_${num_threads}_2000.txt
done

cc -O3 -march=native -fopenmp -DCHUNK_SIZE=32 -DN4000 ../shallow_water_static.c -o ../shallow_water_static.out

# Loop over each thread configuration
for num_threads in "${threads[@]}";
do
    echo "Running with ${num_threads} threads"

    # Set the number of OpenMP threads
    export OMP_NUM_THREADS=${num_threads}
    # Use srun to launch the executable using the allocated cores
    srun ../shallow_water_static.out > performance_data_static_${num_threads}_4000.txt
done

