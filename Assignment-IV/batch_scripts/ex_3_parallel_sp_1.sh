#!/bin/bash -l

#SBATCH -J A4-ex3_mpi_ex1
#SBATCH -t 0:15:00
#SBATCH -A edu25.dd2356
#SBATCH -p shared
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=8
#SBATCH --cpus-per-task=1
#SBATCH -e ex3_mpi_ex1.stderr

module load PDC/23.12
module load score-p/8.4-cpeGNU
scorep mpicc cc -lm -O3 -march=native ../ex1/ex_1_mpi.c -o ../bin/ex3_sp_ex1.out

# Run and redirect output
srun -n 8 ../bin/ex3_sp_ex1.out > "../batch_output/ex3_sp_ex1.stdout"