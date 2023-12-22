#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <omp.h>

// Limit the recursion depth for new thread creation
#define MAX_DEPTH 4

typedef struct
{
    int *arr;
    int low;
    int high;
    int depth;
} thread_args;

void swap(int *a, int *b)
{
    int t = *a;
    *a = *b;
    *b = t;
}

int partition(int arr[], int low, int high)
{
    int pivot = arr[high];
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++)
    {
        if (arr[j] <= pivot)
        {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}

void *quicksort(void *args)
{
    thread_args *t_args = (thread_args *)args;
    int low = t_args->low;
    int high = t_args->high;
    int depth = t_args->depth;
    int *arr = t_args->arr;

    if (low < high)
    {
        int pi = partition(arr, low, high);

        thread_args left_args = {arr, low, pi - 1, depth + 1};
        thread_args right_args = {arr, pi + 1, high, depth + 1};

        if (depth < MAX_DEPTH)
        {
            pthread_t left_thread, right_thread;
            pthread_create(&left_thread, NULL, quicksort, &left_args);
            pthread_create(&right_thread, NULL, quicksort, &right_args);

            pthread_join(left_thread, NULL);
            pthread_join(right_thread, NULL);
        }
        else
        {
            quicksort(&left_args);
            quicksort(&right_args);
        }
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s input_filename\n", argv[0]);
        return 1;
    }

    char *input_filename = argv[1];
    FILE *file = fopen(input_filename, "r");

    if (file == NULL)
    {
        printf("Failed to open the file.\n");
        return 1;
    }

    int n;
    if (fscanf(file, "%d", &n) != 1)
    {
        printf("Failed to read the array size from the file.\n");
        fclose(file);
        return 1;
    }

    int *arr = (int *)malloc(n * sizeof(int));
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

    double start_time, run_time;
    start_time = omp_get_wtime();

    thread_args args = {arr, 0, n - 1, 0};
    quicksort(&args);

    run_time = omp_get_wtime() - start_time;
    printf("pthreads time: %lf\n", run_time);

    // for (int i = 0; i < n; i++)
    // {
    //     printf("%d\n", arr[i]);
    // }
    // printf("\n");

    free(arr);

    return 0;
}
