#!/bin/bash -l

#SBATCH -J A4-ex4-serial
#SBATCH -t 0:05:00
#SBATCH -A edu25.dd2356
#SBATCH -p shared
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=1
#SBATCH -e ex4_b_serial_sum.stderr

module load PDC/23.12
module load openblas/0.3.28-cpeGNU-23.12
mpicc -o blas_mvm ../ex4/blas_serial.c -lopenblas -lm

gcc -O3 -fopenmp ../batch_scripts/ex4_b_serial_sum.c -o ../bin/ex4_b_serial_sum.out

# Run and redirect output
srun -n 1 ../bin/ex4_b_serial_sum.out > "../batch_output/ex_4_b_output.stdout"