#!/bin/bash -l
# The -l above is required to get the full environment with modules

#SBATCH -A edu25.DD2356
#SBATCH -J hw_loc
#SBATCH -p shared
#SBATCH -t 00:10:00
#SBATCH --nodes=1
#SBATCH --mem=128G
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=128

# Run the executable named main.x and write the output to my_output_file.txt
srun hwloc-ls --output-format graphical topology.svg
