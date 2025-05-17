#!/bin/bash -l

#SBATCH -J A4-ex3-mpi_2t
#SBATCH -t 0:15:00
#SBATCH -A edu25.dd2356
#SBATCH -p shared
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=8
#SBATCH --cpus-per-task=1
#SBATCH -e ex3_mpi2t.stderr


module load PDC/23.12
module load score-p/8.4-cpeGNU
export SCOREP_ENABLE_PROFILING=false
export SCOREP_ENABLE_TRACING=true
scorep mpicc -lm -O3 -march=native ../ex2/ex_2_parallel_sum.c -o ../bin/ex3_sp_ex2.out

# Run and redirect output
srun -n 8 ../bin/ex3_sp_ex2.out > "../batch_output/ex3_sp_ex2.stdout"