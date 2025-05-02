#!/bin/bash -l

#SBATCH -J A3-ex3-serial
#SBATCH -t 0:05:00
#SBATCH -A edu25.dd2356
#SBATCH -p shared
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=1
#SBATCH -e ex_3_serial.stderr

gcc -O3 -fopenmp ../serial_sum.c -o ../serial_sum.out


# Repeat  5 times
for run in {1..5}; do
    export OMP_NUM_THREADS=${num_threads}

    # Run and redirect output
    srun -n 1 ../serial_sum.out 10000000 >> "ex_3_serial_output.stdout"
done