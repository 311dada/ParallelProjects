#include "qc_util.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


int* allocate_array(int n, int MAX)
{
    int *num;
    num = (int*)malloc(sizeof(int) * n);

    srand(0);

    for (int i = 0; i < n; i++)
        num[i] = rand() % MAX;
    return num;
}


void display(int* arr, int n)
{
    for (int i = 0; i < n; i++)
    {
        if (i != n - 1)
            printf("%d ", arr[i]);
        else
        {
            printf("%d\n", arr[i]);
        } 
    }
}