#include "matrix_gen.h"
#include <stdlib.h>
#include <stdio.h>

void allocate_matrix(int ***a, int size)
{
    int i;
    int j;
    int *storage;
    storage = (int*)malloc(N * N * size);
    *a = (int**)malloc(N * sizeof(int*));
    for (i = 0; i < N; i++)
    {
        (*a)[i] = storage + i * N;
    }

    srand(0);

    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {
            (*a)[i][j] = rand() % MAX_NUM;
        }
    }
}


void free_matrix(int ***a)
{
    free((*a)[0]);
    free(*a);
}