#!/bin/bash -l

#SBATCH -J A3-SDG128
#SBATCH -t 0:00:30
#SBATCH -A edu25.dd2356
#SBATCH -p shared
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH -e smp.stderr

# Compile the program with OpenMP
gcc -fopenmp stream.c -o streammp.x

# Set number of threads
export OMP_NUM_THREADS=128

# Define OpenMP scheduling types
schedules=("static" "dynamic" "guided")

# Loop over each schedule type
for schedule in "${schedules[@]}"; do
    # Set the OpenMP schedule
    export OMP_SCHEDULE="${schedule}"

    # Run 5 times for each schedule
    for run in {1..5}; do
        output_file="smp_output_${schedule}_run${run}.stdout"
        srun -n 1 ./streammp.x >> "$output_file"
    done
done
