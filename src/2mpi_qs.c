#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

void swap(int *a, int *b) {
    int t = *a;
    *a = *b;
    *b = t;
}

int partition(int arr[], int low, int high) {
    int pivot = arr[high];
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++) {
        if (arr[j] <= pivot) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}

void quicksort(int arr[], int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quicksort(arr, low, pi - 1);
        quicksort(arr, pi + 1, high);
    }
}

void parallelQuicksort(int *arr, int left, int right, int depth, MPI_Comm comm) {
    int numtasks, rank, pivot, partner, recvsize;
    MPI_Comm_size(comm, &numtasks);
    MPI_Comm_rank(comm, &rank);

    if (numtasks == 1) {
        quicksort(arr, left, right);
    } else if (right > left) {
        int size = right - left + 1;
        pivot = arr[left + size / 2];
        
        int *temparr = (int*)malloc(size * sizeof(int));
        int i, j = 0, k = 0;
        for (i = left; i <= right; i++) {
            if (arr[i] < pivot || (arr[i] == pivot && i < left + size / 2)) {
                temparr[j++] = arr[i];
            } else {
                temparr[size - (++k)] = arr[i];
            }
        }

        for (i = 0; i < j; i++) arr[left + i] = temparr[i];
        for (i = 0; i < k; i++) arr[right - i] = temparr[size - 1 - i];
        free(temparr);

        partner = rank ^ (1 << depth);

        // Ensure the partner rank is within the valid range
        if (partner < numtasks) {
            if (rank < partner) {
                MPI_Send(&j, 1, MPI_INT, partner, 0, comm);
                MPI_Recv(&recvsize, 1, MPI_INT, partner, 0, comm, MPI_STATUS_IGNORE);
                if (j > recvsize) {
                    MPI_Recv(arr + left + j, recvsize, MPI_INT, partner, 0, comm, MPI_STATUS_IGNORE);
                    MPI_Send(arr + left, j, MPI_INT, partner, 0, comm);
                    j = recvsize;
                }
            } else {
                MPI_Recv(&recvsize, 1, MPI_INT, partner, 0, comm, MPI_STATUS_IGNORE);
                MPI_Send(&j, 1, MPI_INT, partner, 0, comm);
                if (recvsize > j) {
                    MPI_Send(arr + left, j, MPI_INT, partner, 0, comm);
                    MPI_Recv(arr + left, recvsize, MPI_INT, partner, 0, comm, MPI_STATUS_IGNORE);
                }
            }
        }

        MPI_Comm newcomm;
        MPI_Comm_split(comm, rank < partner && partner < numtasks, 0, &newcomm);
        if (rank < partner && partner < numtasks) {
            parallelQuicksort(arr, left, left + recvsize - 1, depth + 1, newcomm);
        } else {
            parallelQuicksort(arr, left + recvsize, right, depth + 1, newcomm);
        }
        MPI_Comm_free(&newcomm);
    }
}


int main(int argc, char *argv[]) {
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int *arr;
    int n, i;

    if (rank == 0) {
        if (argc != 2) {
            printf("Usage: %s input_filename\n", argv[0]);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        char *input_filename = argv[1];
        FILE *file = fopen(input_filename, "r");
        if (file == NULL) {
            printf("Failed to open the file.\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        if (fscanf(file, "%d", &n) != 1) {
            printf("Failed to read the array size from the file.\n");
            fclose(file);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        arr = (int *)malloc(n * sizeof(int));
        if (arr == NULL) {
            printf("Memory allocation failed.\n");
            fclose(file);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        for (i = 0; i < n; i++) {
            if (fscanf(file, "%d", &arr[i]) != 1) {
                printf("Failed to read an element from the file.\n");
                fclose(file);
                free(arr);
                MPI_Abort(MPI_COMM_WORLD, 1);
            }
        }
        fclose(file);
    }

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    if (rank != 0) {
        arr = (int*)malloc(n * sizeof(int));
    }

    MPI_Bcast(arr, n, MPI_INT, 0, MPI_COMM_WORLD);

    double start_time = MPI_Wtime();
    parallelQuicksort(arr, 0, n - 1, 0, MPI_COMM_WORLD);
    double end_time = MPI_Wtime();

    if (rank == 0) {
        printf("Parallel time: %lf\n", end_time - start_time);
        // for (i = 0; i < n; i++) {
        //     printf("%d\n", arr[i]);
        // }
    }

    free(arr);
    MPI_Finalize();

    return 0;
}
