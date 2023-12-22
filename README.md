# Quicksort - OpenMP, MPI, Pthreads, OpenMP + MPI hybrid

Calugaritoiu Ion-Victor 342C1

- "run.sh" runs all implementations and prints the recorded durations.
- Used "omp_get_wtime" to monitor performance for all 4 versions.

## 1. OpenMP
I used the "#pragma omp single" directive to designate the quickSort function
as a single task that should be executed only once by one of the threads.

"quickSort(arr, 0, n - 1)" initiates the QuickSort algorithm on the entire
array. This task will be executed concurrently by multiple threads, where
each thread works on a different portion of the array. This parallel approach
significantly speeds up the sorting process for large arrays.

"#pragma omp task firstprivate(arr, low, pi)" creates a task for parallel
execution and specifies that the variables arr, low, and pi should be made
private to each task - each task has independent copies of the variables in
order to work on the array concurrently.

## 2. MPI
The array is divided into chunks, each processed by a different MPI process,
and then merged to form the sorted array.

Rank 0 process calculates the chunk size, divides the array and distributes it
to all processes using MPI_Scatter.
After every process sorts its chunk using quicksort, the array is reassembled
using the merge function. In each step, processes are paired, and one process
in each pair sends its sorted chunk to the other, which merges the two chunks.
This step is repeated, doubling the distance between paired processes each time
(step = 2 * step), until all chunks are merged into a single sorted array in
the master process.

## 3. Pthreads
After partitioning, the array is divided into two parts: from low to pi - 1 and
from pi + 1 to high. If the current recursion depth is less than MAX_DEPTH, two
threads (thread1 and thread2) are created for sorting these two partitions 
concurrently. Had to use a MAX_DEPTH variable in order to limit the amount
of created threads. Not imposing a limit resulted in SEGFAULT errors for my 
test array (6 million elements).

Thread 1: Sorts the left partition of the array (from low to pi - 1).
Thread 2: Sorts the right partition (from pi + 1 to high).

pthread_join is used to wait for each thread to finish its execution. This
ensures that the sorting of both partitions is completed before moving up the
recursion stack.

## 4. OpenMP & MPI
I added the OpenMP directives to the MPI implementation.

## 5. Pthreads & MPI
Added the Pthreads quicksort function to the MPI implementation.

## 6. Results: 4 threads/procs
- serial time: 1.044696  
- openmp time: 0.375250  
- mpi time: 0.727631  
- pthreads time: 0.413353  
- openmp + mpi time: 0.766816  
- pthreads + mpi time: 0.706754  

## 7. Sources:
https://www.geeksforgeeks.org/implementation-of-quick-sort-using-mpi-omp-and-posix-thread/  
https://iq.opengenus.org/parallel-quicksort/
