#!/bin/bash -l

#SBATCH -J A4-b-serial
#SBATCH -t 0:05:00
#SBATCH -A edu25.dd2356
#SBATCH -p shared
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=1
#SBATCH -e A4-b-serial.stderr

. ./perf_env

cc -lm -O3 -march=native ../bonus/serial_game.c -o ../bin/serial_game.out
srun -n 1 perf stat -e ${events} -o mpi_game-stat.stat ../bin/serial_game.out > ../batch_output/serial_game.stdout    