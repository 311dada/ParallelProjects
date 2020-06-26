#include "matrix_gen.h"
#include "mm.h"
#include <stdio.h>
#include <math.h>

void matrix_multiplication(int **A, int **B, int **C, int s)
{

    for (int i = 0; i < s; i++)
    {
        for (int j = 0; j < s; j++)
        {
            for (int k = 0; k < s; k++)
                C[i][j] += A[i][k] * B[k][j];
        }
    }
}

void matrix_display(int **A)
{
    int i, j;
    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {
            printf("%d ", A[i][j]);
        }
        printf("\n");
    }
}