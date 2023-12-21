#!/bin/bash

./bin/serial_qs ./in/input.txt
./bin/openmp_qs ./in/input.txt
mpirun -n 4 ./bin/mpi_qs ./in/input.txt
./bin/pthreads_qs ./in/input.txt
mpirun -n 4 ./bin/openmp_mpi_qs ./in/input.txt

# ./bin/serial_qs ./in/input.txt > ./out/serial.out
# ./bin/openmp_qs ./in/input.txt > ./out/openmp.out
# mpirun -n 4 ./bin/mpi_qs ./in/input.txt > ./out/mpi.out
# ./bin/pthreads_qs ./in/input.txt > ./out/pthreads.out
# mpirun -n 4 ./bin/openmp_mpi_qs ./in/input.txt > ./out/openmp_mpi.out

# diff ./out/serial.out ./out/openmp.out
# diff ./out/serial.out ./out/mpi.out
# diff ./out/serial.out ./out/pthreads.out
# diff ./out/serial.out ./out/openmp_mpi.out

# head -n 1 ./out/serial.out
# head -n 1 ./out/openmp.out
