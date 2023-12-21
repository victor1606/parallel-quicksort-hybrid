CC = gcc
MPICC = mpicc
CFLAGS = -Wall -Wextra -g

SRC_SERIAL = ./src/serial_qs.c
SRC_OPENMP = ./src/openmp_qs.c
SRC_MPI = ./src/mpi_qs.c
SRC_PTHREADS = ./src/pthreads_qs.c
SRC_OPENMP_MPI = ./src/openmp_mpi_qs.c

BIN_SERIAL = ./bin/serial_qs
BIN_OPENMP = ./bin/openmp_qs
BIN_MPI = ./bin/mpi_qs
BIN_PTHREADS = ./bin/pthreads_qs
BIN_OPENMP_MPI = ./bin/openmp_mpi_qs

INPUT = ./in/input.txt

OUTPUT_SERIAL = ./out/serial.out
OUTPUT_OPENMP = ./out/openmp.out
OUTPUT_MPI = ./out/mpi.out
OUTPUT_PTHREADS = ./out/pthreads.out
OUTPUT_OPENMP_MPI = ./out/openmp_mpi.out

build:
	$(CC) $(SRC_SERIAL) -fopenmp -o $(BIN_SERIAL) $(CFLAGS)
	$(CC) $(SRC_OPENMP) -fopenmp -o $(BIN_OPENMP) $(CFLAGS)
	$(MPICC) $(SRC_MPI) -fopenmp -o $(BIN_MPI) $(CFLAGS)
	$(CC) $(SRC_PTHREADS) -fopenmp -pthread -o $(BIN_PTHREADS) $(CFLAGS)
	$(MPICC) $(SRC_OPENMP_MPI) -fopenmp -o $(BIN_OPENMP_MPI) $(CFLAGS)

run: 
	$(BIN_SERIAL) $(INPUT) > $(OUTPUT_SERIAL)
	$(BIN_OPENMP) $(INPUT) > $(OUTPUT_OPENMP)
	mpirun -n 4 --oversubscribe $(BIN_MPI) $(INPUT) > $(OUTPUT_MPI)
	$(BIN_PTHREADS) $(INPUT) > $(OUTPUT_PTHREADS)
	mpirun -n 4 --oversubscribe $(BIN_OPENMP_MPI) $(INPUT) > $(OUTPUT_OPENMP_MPI)

clean:
	rm -rf $(BIN_SERIAL) $(BIN_OPENMP) $(BIN_MPI) $(BIN_PTHREADS) $(BIN_OPENMP_MPI)
