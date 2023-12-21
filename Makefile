CC = gcc
MPICC = mpicc
CFLAGS = -Wall -Wextra -g

SRC_SERIAL = ./src/serial_qs.c
SRC_OPENMP = ./src/openmp_qs.c
SRC_MPI = ./src/mpi_qs.c
SRC_PTHREADS = ./src/pthreads_qs.c

BIN_SERIAL = ./bin/serial_qs
BIN_OPENMP = ./bin/openmp_qs
BIN_MPI = ./bin/mpi_qs
BIN_PTHREADS = ./bin/pthreads_qs

INPUT = ./in/input.txt

OUTPUT_SERIAL = ./out/serial.out
OUTPUT_OPENMP = ./out/openmp.out
OUTPUT_MPI = ./out/mpi.out
OUTPUT_PTHREADS = ./out/pthreads.out

build:
	$(CC) $(SRC_SERIAL) -fopenmp -o $(BIN_SERIAL) $(CFLAGS)
	$(CC) $(SRC_OPENMP) -fopenmp -o $(BIN_OPENMP) $(CFLAGS)
	$(MPICC) $(SRC_MPI) -o $(BIN_MPI) $(CFLAGS)
	$(CC) $(SRC_PTHREADS) -pthread -o $(BIN_PTHREADS) $(CFLAGS)

run: 
	# $(BIN_SERIAL) $(INPUT) > $(OUTPUT_SERIAL)
	$(BIN_OPENMP) $(INPUT) > $(OUTPUT_OPENMP)
	# mpirun -n 8 --oversubscribe $(BIN_MPI) $(INPUT) > $(OUTPUT_MPI)
	# $(BIN_PTHREADS) $(INPUT) > $(OUTPUT_PTHREADS)

clean:
	rm -rf $(BIN_SERIAL) $(BIN_OPENMP) $(BIN_MPI) $(BIN_PTHREADS)
