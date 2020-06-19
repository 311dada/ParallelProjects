#include "matrix_gen.h"
#include "mm.h"
#include <mpi.h>
#include <stdio.h>
#include <math.h>

int **matrix_multiplication(int **A, int **B, int p = 64)
{
    int **C;
    allocate_matrix(&C, sizeof(int));

    // TODO: implementation
    // Initialize the MPI environment
    MPI_Init(NULL, NULL);

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    

    return C;
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