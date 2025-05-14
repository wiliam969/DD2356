#!/bin/bash -l

#SBATCH -J A4-b-mpi
#SBATCH -t 0:05:00
#SBATCH -A edu25.dd2356
#SBATCH -p shared
#SBATCH --nodes=8
#SBATCH --ntasks-per-node=8
#SBATCH --cpus-per-task=1
#SBATCH -e a4-b-mpi.stderr

. ./perf_env

cc -lm -O3 -march=native ../bonus/mpi_game.c -o ../bin/mpi_game.out
srun -n 8 perf stat -e ${events} -o mpi_game-stat.stat ../bin/mpi_game.out smallspacefiller.rle > ../batch_output/mpi_game.stdout    