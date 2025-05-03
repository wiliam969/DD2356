#!/bin/bash -l

#SBATCH -J A3-se_32
#SBATCH -t 0:10:30
#SBATCH -A edu25.dd2356
#SBATCH -p shared
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH -e smp.stderr
#SBATCH --cpus-per-task=32

threads=32

# Compile the program
#ml PDC/23.12
#ml cpeGNU/23.12
gcc -O3 -fopenmp stream.c -o streammp.x
for run in {1..5}; do
    export OMP_NUM_THREADS=${threads}

    output_file="smp_output_${threads}_run${run}.stdout"

    # Run and redirect output
    srun -n 1 ./streammp.x >> "$output_file"
done
srun hwloc-ls --of svg > streammp_topo_${threads}.svg

