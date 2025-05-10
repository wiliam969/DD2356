#!/bin/bash -l

#SBATCH -J A4-ex1-mpi
#SBATCH -t 0:15:00
#SBATCH -A edu25.dd2356
#SBATCH -p shared
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=1
#SBATCH -e ex1_serial.stderr

cc -lm -O3 -march=native ../ex_1/ex1_haloexchange.c -o ../bin/ex1_haloexchange.out

# Run and redirect output
srun -n 1 ../bin/ex1_haloexchange.out > "../batch_output/ex1_serial.stdout"
