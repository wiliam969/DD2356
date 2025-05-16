#!/bin/bash -l

#SBATCH -J A4-ex3-mpi_ex2
#SBATCH -t 0:15:00
#SBATCH -A edu25.dd2356
#SBATCH -p shared
#SBATCH --nodes=2
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=1
#SBATCH -e ex3_mpi2sp.stderr
export SCOREP_ENABLE_PROFILING=true

scorep cc -lm -O3 -march=native ../ex3/ex_2_parallel_sum.c -o ../bin/ex3_sp_ex2.out

# Run and redirect output
srun perf record -e ../bin/ex3_sp_ex2.out > "../batch_output/ex3_sp_ex2.stdout"