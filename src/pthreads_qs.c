#ifndef _REENTRANT
#define _REENTRANT
#endif
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#include <sys/time.h>

double read_timer()
{
    static bool initialized = false;
    static struct timeval start;
    struct timeval end;
    if (!initialized)
    {
        gettimeofday(&start, NULL);
        initialized = true;
    }
    gettimeofday(&end, NULL);
    return (end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec);
}

double start_time, end_time; /* start and end times */

typedef struct qsort_starter
{
    int *arr;
    int low;
    int high;
} quickSort_parameters;

void swap(int *a, int *b)
{
    int t = *a;
    *a = *b;
    *b = t;
}

int partition(int *arr, int low, int high, int pivot)
{
    int pivotValue = arr[pivot];
    swap(&arr[pivot], &arr[high]); // pivot
    int s = low;                   // Index of smaller element

    for (int i = low; i < high; i++)
    {
        // If current element is smaller than or
        // equal to pivot
        if (arr[i] <= pivotValue)
        {
            swap(&arr[i], &arr[s]);
            s++; // increment index of smaller element
        }
    }
    swap(&arr[s], &arr[high]);
    return s;
}

/*Sequencial QuickSort version*/
void quickSort(int *arr, int low, int high)
{
    if (low < high)
    {
        int pivotPosition = low + (high - low) / 2;
        pivotPosition = partition(arr, low, high, pivotPosition);
        quickSort(arr, low, pivotPosition - 1);
        quickSort(arr, pivotPosition + 1, high);
    }
}

void concurrent_quickSort(int *arr, int low, int high);

void *worker_quickSort(void *initialValues)
{
    quickSort_parameters *parameters = initialValues;
    concurrent_quickSort(parameters->arr, parameters->low, parameters->high);
    return NULL;
}

#define THRESHOLD 100000  // You can adjust this value based on experimentation

void concurrent_quickSort(int *arr, int low, int high) {
    if (low < high) {
        // Check the size of the subarray
        if (high - low < THRESHOLD) {
            // If the size is below the threshold, use sequential quicksort
            quickSort(arr, low, high);
        } else {
            // Existing code for creating threads
            int pivotPos = low + (high - low) / 2;
            pivotPos = partition(arr, low, high, pivotPos);
            pthread_t thread;

            quickSort_parameters thread_param = {arr, low, pivotPos - 1};
            pthread_create(&thread, NULL, worker_quickSort, &thread_param);

            concurrent_quickSort(arr, pivotPos + 1, high);

            pthread_join(thread, NULL);
        }
    }
}

int main(int argc, char **argv)
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


    start_time = read_timer();
    concurrent_quickSort(arr, 0, n - 1);
    end_time = read_timer();

    printf("pthreads time: %lf\n", end_time - start_time);

    // for (int i = 0; i < n - 1; i++)
    // {
    //     printf("%d\n", arr[i]);
    // }
    // printf("\n");
   

    free(arr);
    return 0;
}