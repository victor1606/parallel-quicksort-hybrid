#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <omp.h>

#define MAX_THREADS 8 // Limit the number of threads
#define MAX_DEPTH 4   // Limit the recursion depth for new thread creation

typedef struct
{
    int *arr;
    int low;
    int high;
    int depth;
} ThreadArgs;

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
    ThreadArgs *targs = (ThreadArgs *)args;
    int low = targs->low;
    int high = targs->high;
    int depth = targs->depth;
    int *arr = targs->arr;

    if (low < high)
    {
        int pi = partition(arr, low, high);

        ThreadArgs args1 = {arr, low, pi - 1, depth + 1};
        ThreadArgs args2 = {arr, pi + 1, high, depth + 1};

        if (depth < MAX_DEPTH)
        {
            pthread_t thread1, thread2;
            int thread1_created = 0, thread2_created = 0;

            if (pthread_create(&thread1, NULL, quicksort, &args1) == 0)
            {
                thread1_created = 1;
            }
            else
            {
                quicksort(&args1);
            }

            if (pthread_create(&thread2, NULL, quicksort, &args2) == 0)
            {
                thread2_created = 1;
            }
            else
            {
                quicksort(&args2);
            }

            if (thread1_created) {
                pthread_join(thread1, NULL);
            }
            
            if (thread2_created) {
                pthread_join(thread2, NULL);
            }
        }
        else
        {
            quicksort(&args1);
            quicksort(&args2);
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

    ThreadArgs args = {arr, 0, n - 1, 0};
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
