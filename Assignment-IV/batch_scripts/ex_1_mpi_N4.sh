#!/bin/bash -l

#SBATCH -J A4-ex1-N4
#SBATCH -t 0:30:00
#SBATCH -A edu25.dd2356
#SBATCH -p shared
#SBATCH --nodes=4
#SBATCH --ntasks-per-node=8
#SBATCH --cpus-per-task=1
#SBATCH -e ex_1_N4.stderr

process_count=(1 2 4 8)

for proc in "${process_count[@]}"; do
    cc -lm -O3 -march=native -DN=10000 -DSTEPS=500 -DIO_ON_OFF=0 ../ex_1/ex_1_mpi.c -o ../bin/ex_1_mpi.out
    srun -n ${proc} perf stat -e L1-dcache-load-misses,power/energy-pkg/,L1-dcache-loads,L1-dcache-prefetches,L1-icache-load-misses,L1-icache-loads,duration_time,cpu-cycles,instructions ../bin/ex_1_mpi.out > ../batch_output/ex1_mpi_N4_${proc}.stdout    
done