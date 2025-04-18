#!/bin/bash -l


# Set the allocation (project) to be charged for this job
#SBATCH -A edu25.DD2356

# The name of the job
#SBATCH -J topology


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

hwloc-ls --of svg > topology.svg