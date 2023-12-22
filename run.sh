#!/bin/bash

echo "Running serial code..."
./bin/serial_qs ./in/input.txt > ./out/serial.out

echo "Running openmp code..."
./bin/openmp_qs ./in/input.txt > ./out/openmp.out

echo "Running mpi code..."
mpirun -n 4 ./bin/mpi_qs ./in/input.txt > ./out/mpi.out

echo "Running pthreads code..."
./bin/pthreads_qs ./in/input.txt > ./out/pthreads.out

echo "Running openmp + mpi code..."
mpirun -n 4 ./bin/openmp_mpi_qs ./in/input.txt > ./out/openmp_mpi.out

echo "Running pthreads + mpi code..."
mpirun -n 4 ./bin/pthreads_mpi_qs ./in/input.txt > ./out/pthreads_mpi_qs.out

echo "Results:"
head -n 1 ./out/serial.out
head -n 1 ./out/openmp.out
head -n 1 ./out/mpi.out
head -n 1 ./out/pthreads.out
head -n 1 ./out/openmp_mpi.out
head -n 1 ./out/pthreads_mpi_qs.out

# For checking algorithm correctness
# diff ./out/serial.out ./out/openmp.out
# diff ./out/serial.out ./out/mpi.out
# diff ./out/serial.out ./out/pthreads.out
# diff ./out/serial.out ./out/openmp_mpi.out
# diff ./out/serial.out ./out/pthreads_mpi_qs.out