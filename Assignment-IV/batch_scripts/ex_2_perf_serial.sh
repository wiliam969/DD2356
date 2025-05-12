#!/bin/bash -l

#SBATCH -J A4-ex2-serial
#SBATCH -t 0:05:00
#SBATCH -A edu25.dd2356
#SBATCH -p shared
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=1
#SBATCH -e ex2_serial_sum.stderr

gcc -O3 -fopenmp ../ex2/ex2_serial_sum.c -o ../bin/ex2_serial_sum.out

# Run and redirect output
srun -n 1 perf record -e 'sched:*' -o serial.data ../bin/ex2_serial_sum.out > ../batch_output/ex_2_serial_output.stdout
