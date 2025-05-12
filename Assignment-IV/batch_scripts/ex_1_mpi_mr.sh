#!/bin/bash -l

#SBATCH -J A4-ex1-mr
#SBATCH -t 0:30:00
#SBATCH -A edu25.dd2356
#SBATCH -p shared
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=8
#SBATCH --cpus-per-task=1
#SBATCH -e ex_1_mr.stderr

process_count=(1 2 4 8)

for proc in "${process_count[@]}"; do
    cc -lm -O3 -march=native ../ex_1/ex_1_mpi.c -o ../bin/ex_1_mpi_mr.out
    srun -n ${proc} ../bin/ex_1_mpi_mr.out > "../batch_output/ex1_mpi_mr.stdout"    
done