#!/bin/bash -l

#SBATCH -J A4-ex1-serial
#SBATCH -t 0:15:00
#SBATCH -A edu25.dd2356
#SBATCH -p shared
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=1
#SBATCH -e ex1_serial.stderr

 cc -lm -O3 -march=native -DN=10000 -DSTEPS=500 -DIO_ON_OFF=0 ../ex_1/ex_1_serial.c -o ../bin/ex_1_serial.out
    srun -n 1 ../bin/ex_1_mpi_mr.out > "../batch_output/ex1_serial.stdout"    