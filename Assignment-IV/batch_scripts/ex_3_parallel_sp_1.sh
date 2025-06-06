#!/bin/bash -l

#SBATCH -J A4-ex3_mpi_ex1
#SBATCH -t 0:15:00
#SBATCH -A edu25.dd2356
#SBATCH -p shared
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=8
#SBATCH --cpus-per-task=1
#SBATCH -e ex3_mpi_ex1.stderr
process_count=(8)

module load PDCOLD/23.12
module load score-p/8.4-cpeGNU

# Run and redirect output
scorep mpicc -lm -O3 -march=native -DN=1000000 -DSTEPS=500 -DIO_ON_OFF=0 ../ex_1/ex_1_mpi.c -o ../bin/ex_1_mpi.out

srun -n ${process_count} ../bin/ex_1_mpi.out 