#!/bin/bash
#$ -N Mandelbrot
#$ -q test
#$ -pe one-node-mpi 64
#$ -R y

# Grid Engine Notes:
# -----------------
# 1) Use "-R y" to request job reservation otherwise single 1-core jobs
#    may prevent this multicore MPI job from running.   This is called
#    job starvation.

# Module load boost
module load boost/1.57.0

# Module load OpenMPI
module load openmpi-1.8.3/gcc-4.8.3

# Run the program 
for trial in 1 2; do
echo "trial :${trial} "
mpirun -np 1 ./mandelbrot_mpi_cyclic 1000 1000

done



