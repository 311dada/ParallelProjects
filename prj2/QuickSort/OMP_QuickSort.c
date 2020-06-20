#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
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
    arr = allocate_array(n, maxm);

    int to_display = 0;
    if (argc > 4)
        to_display = 1;

    if (to_display)
    {
        printf("======= Before sorting =======\n");
        display(arr, n);
    }
    

    double start, end;
    start = omp_get_wtime();
    #pragma omp parallel
    {
        #pragma omp single nowait
        {
            qs(arr, 0, n);
        }
    }
    end = omp_get_wtime();

    if (to_display)
    {
        printf("======= After sorting =======\n");
        display(arr, n);
    }
    
    printf("time: %f seconds\n", end - start);
    free(arr);

    return 0;
} 
