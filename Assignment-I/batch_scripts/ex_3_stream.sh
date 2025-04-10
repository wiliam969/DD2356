#!/bin/bash -l
# The -l above is required to get the full environment with modules

# Set the allocation (project) to be charged for this job
#SBATCH -A edu25.DD2356

# The name of the job
#SBATCH -J stream

# The partition/queue
#SBATCH -p shared

# 10 hours of wall clock time
#SBATCH -t 00:15:00

# Number of nodes
#SBATCH --nodes=1

#SBATCH --mem=20G

# Number of MPI processes per node
#SBATCH --ntasks-per-node=1

# Optional: if your cluster uses cpus-per-task or similar, specify them if needed
# #SBATCH --cpus-per-task=1

# Run the executable named main.x and write the output to my_output_file.txt
srun ./stream.out 10 > stream_file_10.txt
srun ./stream.out 100 > stream_file_100.txt
srun ./stream.out 1000 > stream_file_1000.txt
srun ./stream.out 10000 > stream_file_10000.txt
