#!/bin/bash -l
# The -l above is required to get the full environment with modules

# Set the allocation (project) to be charged for this job
#SBATCH -A edu25.DD2356

# The name of the job
#SBATCH -J myjob

# The partition/queue
#SBATCH -p shared

# 10 hours of wall clock time
#SBATCH -t 00:10:00

# Number of nodes
#SBATCH --nodes=1

# Number of MPI processes per node
#SBATCH --ntasks-per-node=1

# Optional: if your cluster uses cpus-per-task or similar, specify them if needed
# #SBATCH --cpus-per-task=1

# Run the executable named main.x and write the output to my_output_file.txt
srun ./spmv.out $SLURM_ARRAY_TASK_ID > my_output_file_$SLURM_ARRAY_TASK_ID.txt
