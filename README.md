# quicksort

Calugaritoiu Ion-Victor 342C1

1. OpenMP

I used the "#pragma omp single" directive to designate the quickSort function
as a single task that should be executed only once by one of the threads.

"quickSort(arr, 0, n - 1)" initiates the QuickSort algorithm on the entire
array. This task will be executed concurrently by multiple threads, where each
thread works on a different portion of the array. This parallel approach
significantly speeds up the sorting process for large arrays.

"#pragma omp task firstprivate(arr, low, pi)" creates a task for parallel
execution and specifies that the variables arr, low, and pi should be made
private to each task - each task has independent copies of the variables in
order to work on the array concurrently.

2. MPI


3. Pthreads


4. OpenMP & MPI