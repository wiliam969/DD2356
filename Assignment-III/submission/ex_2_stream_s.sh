#!/bin/bash -l

#SBATCH -J A2-s_serial
#SBATCH -t 0:00:30
#SBATCH -A edu25.dd2356
#SBATCH -p shared
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=128
#SBATCH -e smp.stderr

# Compile the program
#ml PDC/23.12
#ml cpeGNU/23.12

gcc -O3 -fopenmp stream_guided.c -o streammp_g.x
schedule="static" 
num_threads = 128
export OMP_SCHEDULE="${schedule}"
# Loop over each thread configuration
    # Repeat  5 times
    for run in {1..5}; do
        export OMP_NUM_THREADS=${num_threads}
        output_file="smp_output_${num_threads}_run${run}.stdout"
        # Run and redirect output
        srun -n 1 ./streammp.x >> "$output_file"
    done
srun hwloc-ls --of svg > streammp_topo_${num_threads}.svg

