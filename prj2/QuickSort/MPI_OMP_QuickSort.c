#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include "qc_util.h"


void qs(int *v, int first, int last)
{
    int l, r, pivot, i, temp;

    if (first < last) 
    {
        // Partition
        l = first;
        r = last;
        pivot = v[l];
        while (l <= r)
        {
            while (v[l] < pivot)
                l++;
            while (pivot < v[r])
                r--;
            if (l <= r) 
            {
                temp = v[l];
                v[l] = v[r];
                v[r] = temp;
                l++;
                r--;
            }
        }

        // Recursively
        #pragma omp task
        qs(v, first, l - 1);
        #pragma omp task
        qs(v, l, last);
   }
}


int main(int argc, char *argv[]) {
    if (argc < 4)
    {
        perror("Invalid command.");
        exit(0);
    }

    // Initialize the MPI environment
    MPI_Init(NULL, NULL);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);


    // The total number
    int n;
    n = atoi(argv[1]);

    int threadnums;
    threadnums = atoi(argv[2]);
    omp_set_num_threads(threadnums);

    // Generate an array to sort
    int maxm;
    maxm = atoi(argv[3]);
    int *arr;

    if (!world_rank)
        arr = allocate_array(n, maxm);
    
    int cnt;
    cnt = n / world_size;
    int *recv;
    recv = (int*)malloc(sizeof(int) * cnt);
    double start, end;

    MPI_Barrier(MPI_COMM_WORLD);

    start = MPI_Wtime();
    
    MPI_Scatter(arr, cnt, MPI_INT, recv, MPI_INT, 0, MPI_COMM_WORLD);

    int to_display = 0;

    if (!world_rank)
    {
        if (argc > 4)
        to_display = 1;

        if (to_display )
        {
            printf("======= Before sorting =======\n");
            display(arr, n);
        }
    }
    
    #pragma omp parallel
    {
        #pragma omp single nowait
        {
            qs(recv, 0, cnt);
        }
    }
    
    MPI_Gather(recv, cnt, MPI_INT, arr, MPI_INT, 0, MPI_COMM_WORLD);
    free(recv);

    if (!world_rank)
    {
        merge(arr, n, cnt);
        end = MPI_Wtime();
        if (to_display)
        {
            printf("======= After sorting =======\n");
            display(arr, n);
        }
        printf("time: %f seconds\n", end - start);
        free(arr);
    }

    MPI_Finalize();

    return 0;
} 
