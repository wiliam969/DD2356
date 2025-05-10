#!/bin/bash -l

#SBATCH -J A4-ex1-mpi
#SBATCH -t 0:15:00
#SBATCH -A edu25.dd2356
#SBATCH -p shared
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=2
#SBATCH --cpus-per-task=1
#SBATCH -e ex1_mpi.stderr

cc -lm -O3 -march=native ../ex_1/ex_1_mpi.c -o ../bin/ex_1_mpi.out

# Run and redirect output
srun -n 2 ../bin/ex_1_mpi.out > "../batch_output/ex1_mpi.stdout"
