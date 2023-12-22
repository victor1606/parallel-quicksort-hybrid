#include <mpi.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <omp.h>

// Limit the recursion depth for new thread creation
#define MAX_DEPTH 4

typedef struct {
    int *arr;
    int start;
    int end;
    int depth;
} thread_args;


void swap(int *arr, int i, int j)
{
    int t = arr[i];
    arr[i] = arr[j];
    arr[j] = t;
}

void quicksort(int *arr, int start, int end) {
    if (end <= 1) {
        return;
    }

    int pivot = arr[start + end / 2];
    swap(arr, start, start + end / 2);

    int index = start;
    for (int i = start + 1; i < start + end; i++) {
        if (arr[i] < pivot) {
            index++;
            swap(arr, i, index);
        }
    }

    swap(arr, start, index);

    quicksort(arr, start, index - start);
    quicksort(arr, index + 1, start + end - index - 1);
}

void *quicksort_thread(void *args)
{
    thread_args *t_args = (thread_args *)args;
    int start = t_args->start;
    int end = t_args->end;
    int depth = t_args->depth;
    int *arr = t_args->arr;

    if (end <= 1) {
        return NULL;
    }

    int pivot = arr[start + end / 2];
    swap(arr, start, start + end / 2);

    int index = start;
    for (int i = start + 1; i < start + end; i++) {
        if (arr[i] < pivot) {
            index++;
            swap(arr, i, index);
        }
    }

    swap(arr, start, index);

    if (depth < MAX_DEPTH) {
        thread_args left_args = {arr, start, index - start, depth + 1};
        thread_args right_args = {arr, index + 1, start + end - index - 1, depth + 1};

        pthread_t left_thread, right_thread;
        pthread_create(&left_thread, NULL, quicksort_thread, &left_args);
        pthread_create(&right_thread, NULL, quicksort_thread, &right_args);

        pthread_join(left_thread, NULL);
        pthread_join(right_thread, NULL);
    } else {
        quicksort(arr, start, index - start);
        quicksort(arr, index + 1, start + end - index - 1);
    }

    return NULL;
}

// Used to merge 2 arrays
int *merge(int *arr1, int n1, int *arr2, int n2)
{
    int *result = (int *)malloc((n1 + n2) * sizeof(int));
    int i = 0;
    int j = 0;
    int k;

    for (k = 0; k < n1 + n2; k++)
    {
        if (i >= n1)
        {
            result[k] = arr2[j];
            j++;
        }
        else if (j >= n2)
        {
            result[k] = arr1[i];
            i++;
        }
        else if (arr1[i] < arr2[j])
        {
            result[k] = arr1[i];
            i++;
        }
        else
        {
            result[k] = arr2[j];
            j++;
        }
    }
    return result;
}

int main(int argc, char *argv[])
{
    int n;
    int *arr = NULL;
    int chunk_size, own_chunk_size;
    int *chunk;
    MPI_Status status;

    if (argc != 2)
    {
        printf("Usage: %s input_filename\n", argv[0]);
        return 1;
    }

    int size, rank;
    
    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0)
    {
        char *input_filename = argv[1];
        FILE *file = fopen(input_filename, "r");
        if (file == NULL)
        {
            printf("Error in opening file\n");
            exit(-1);
        }

        if (fscanf(file, "%d", &n) != 1)
        {
            printf("Failed to read the array size from the file.\n");
            fclose(file);
            return 1;
        }

        // Computing chunk size
        chunk_size = (n % size == 0) ? (n / size) : (n / size - 1);

        arr = (int *)malloc(size * chunk_size * sizeof(int));
        if (arr == NULL)
        {
            printf("Memory allocation failed.\n");
            fclose(file);
            return 1;
        }

        for (int i = 0; i < n; i++)
        {
            if (fscanf(file, "%d", &arr[i]) != 1)
            {
                printf("Failed to read an element from the file.\n");
                fclose(file);
                free(arr);
                return 1;
            }
        }

        fclose(file);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    double start_time, run_time;
    start_time = omp_get_wtime();

    // Broadcast array size
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Computing chunk size
    chunk_size = (n % size == 0) ? (n / size) : n / (size - 1);

    chunk = (int *)malloc(chunk_size * sizeof(int));
    if (chunk == NULL)
    {
        printf("Memory allocation failed.\n");
        return 1;
    }

    // Broadcast chunk size
    MPI_Scatter(arr, chunk_size, MPI_INT, chunk, chunk_size, MPI_INT, 0,
        MPI_COMM_WORLD);
    
    free(arr);

    own_chunk_size = (n >= chunk_size * (rank + 1))
        ? chunk_size
        : (n - chunk_size * rank);

    thread_args args = {chunk, 0, own_chunk_size, 0}; // Depth starts at 0
    pthread_t sorting_thread;
    pthread_create(&sorting_thread, NULL, quicksort_thread, &args);
    pthread_join(sorting_thread, NULL);

    // quicksort(chunk, 0, own_chunk_size);

    for (int step = 1; step < size; step = 2 * step)
    {
        if (rank % (2 * step) != 0)
        {
            MPI_Send(chunk, own_chunk_size, MPI_INT, rank - step, 0,
                MPI_COMM_WORLD);
            break;
        }

        if (rank + step < size)
        {
            int received_chunk_size = (n >= chunk_size * (rank + 2 * step))
                    ? (chunk_size * step)
                    : (n - chunk_size * (rank + step));
                    
            int *chunk_received = (int *)malloc(received_chunk_size *
                sizeof(int));
            if (chunk_received == NULL)
            {
                printf("Memory allocation failed.\n");
                return 1;
            }

            MPI_Recv(chunk_received, received_chunk_size, MPI_INT, rank + step,
                0, MPI_COMM_WORLD, &status);

            arr = merge(chunk, own_chunk_size, chunk_received,
                received_chunk_size);

            free(chunk);
            free(chunk_received);
            chunk = arr;
            own_chunk_size = own_chunk_size + received_chunk_size;
        }
    }

    if (rank == 0)
    {
        run_time = omp_get_wtime() - start_time;
        printf("pthreads + mpi time: %lf\n", run_time);

        // for (int i = 0; i < n; i++)
        // {
        //     printf("%d\n", chunk[i]);
        // }
        // printf("\n");
    }

    MPI_Finalize();
    return 0;
}
