/* C implementation QuickSort */
#include <stdio.h>
#include <omp.h>
#include <math.h>
#include <stdlib.h>

void swap(int *a, int *b)
{
    int t = *a;
    *a = *b;
    *b = t;
}

/* This function takes last element as pivot, places
the pivot element at its correct position in sorted
    array, and places all smaller (smaller than pivot)
to left of pivot and all greater elements to right
of pivot */
int partition(int arr[], int low, int high)
{
    int pivot = arr[high]; // pivot
    int i = (low - 1);     // Index of smaller element
    for (int j = low; j <= high - 1; j++)
    {
        // If current element is smaller than or
        // equal to pivot
        if (arr[j] <= pivot)
        {
            i++; // increment index of smaller element
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}

void quickSort(int arr[], int low, int high)
{
    if (low < high)
    {
        int pi = partition(arr, low, high);

        #pragma omp task firstprivate(arr, low, pi)
        {
            quickSort(arr, low, pi - 1);
        }

        #pragma omp task firstprivate(arr, high, pi)
        {
            quickSort(arr, pi + 1, high);
        }
    }
}

// Driver program to test above functions
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

    omp_set_num_threads(4);
    start_time = omp_get_wtime();

    #pragma omp parallel
    {
        #pragma omp single
        quickSort(arr, 0, n - 1);
    }

    run_time = omp_get_wtime() - start_time;
    printf("openmp time: %lf\n", run_time);

    // for (int i = 0; i < n; i++)
    // {
    //     printf("%d\n", arr[i]);
    // }
    // printf("\n");

    free(arr);

    return 0;
}