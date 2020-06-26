#include "matrix_gen.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


void allocate_matrix(int ***a, int n, int random)
{
    int i;
    int j;
    int *storage;
    storage = (int*)malloc(n * n * sizeof(int));
    memset(storage, 0, sizeof(int) * n * n);

    *a = (int**)malloc(n * sizeof(int*));
    for (i = 0; i < n; i++)
    {
        (*a)[i] = storage + i * n;
    }


    if (random)
    {
        for (i = 0; i < n; i++)
        {
            for (j = 0; j < n; j++)
            {
                (*a)[i][j] = rand() % MAX_NUM;
            }
        }
    }
    
}

void save_matrix(char *filename, int **A, int n)
{
    FILE *out;
    out = fopen(filename, "w");

    fprintf(out, "%d\n", n);
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            fprintf(out, "%d ", A[i][j]);
        }
        fprintf(out, "\n");
    }
    fclose(out);
}


void free_matrix(int ***a)
{
    free((*a)[0]);
    free(*a);
}

void read_matrix(char *filename, int ***A)
{
    FILE *in;
    in = fopen(filename, "r");

    int n;
    fscanf(in, "%d\n", &n);
    int *storage;
    storage = (int*)malloc(n * n * sizeof(int));
    *A = (int**)malloc(n * sizeof(int*));
    for (int i = 0; i < n; i++)
    {
        (*A)[i] = storage + i * n;
    }

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            fscanf(in, "%d ", &((*A)[i][j]));
        }
        fscanf(in, "\n");
    }
}


void assign_matrix(int **A, int **B, int n)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            A[i][j] = B[i][j];
        }
    }
}

void allocate_gen_matrix(int ***a, int h, int w)
{
    int i;
    int j;
    int *storage;
    storage = (int*)malloc(h * w * sizeof(int));
    memset(storage, 0, sizeof(int) * h * w);

    *a = (int**)malloc(h * sizeof(int*));
    for (i = 0; i < h; i++)
    {
        (*a)[i] = storage + i * w;
    }
}