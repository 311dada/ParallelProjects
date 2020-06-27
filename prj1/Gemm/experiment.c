#include "matrix_gen.h"
#include <string.h>
#include <stdio.h>
#include "mpi.h"
#include <math.h>
#include <stdlib.h>
#include "mm.h"

void initial_w_shift(int **local, int s, int p, int rank)
{
    int row;
    int col;
    int to_send;
    int to_recv;
    
    row = rank / p;
    col = rank % p;
    to_send = (col + p - row) % p + row * p; 
    to_recv = (col + row) % p + row * p;
    MPI_Request request;

    if (row == 0)
        return ;

    MPI_Isend(&(local[0][0]), s * s, MPI_INT, to_send, 0, MPI_COMM_WORLD, &request);
    MPI_Recv(&(local[0][0]), s * s, MPI_INT, to_recv, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}

void initial_n_shift(int **local, int s, int p, int rank)
{
    int row;
    int col;
    int to_send;
    int to_recv;
    
    row = rank / p;
    col = rank % p;
    to_send = ((row + p - col) % p) * p + col;
    to_recv = ((row + col) % p) * p + col;
    MPI_Request request;

    if (col == 0)
        return ;
  
    MPI_Isend(&(local[0][0]), s * s, MPI_INT, to_send, 1, MPI_COMM_WORLD, &request);
    MPI_Recv(&(local[0][0]), s * s, MPI_INT, to_recv, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}

void w_shift(int **local, int s, int p, int rank)
{
    int row;
    int col;
    int to_send;
    int to_recv;
    
    row = rank / p;
    col = rank % p;
    to_send = (col + p - 1) % p + row * p; 
    to_recv = (col + 1) % p + row * p;
    MPI_Request request;

    MPI_Isend(&(local[0][0]), s * s, MPI_INT, to_send, 2, MPI_COMM_WORLD, &request);
    MPI_Recv(&(local[0][0]), s * s, MPI_INT, to_recv, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}


void n_shift(int **local, int s, int p, int rank)
{
    int row;
    int col;
    int to_send;
    int to_recv;
    
    row = rank / p;
    col = rank % p;
    to_send = ((row + p - 1) % p) * p + col;
    to_recv = ((row + 1) % p) * p + col;
    MPI_Request request;
  
    MPI_Isend(&(local[0][0]), s * s, MPI_INT, to_send, 3, MPI_COMM_WORLD, &request);
    MPI_Recv(&(local[0][0]), s * s, MPI_INT, to_recv, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}



int main(int argc, char** argv)
{
    int **A, **B, **C;
    double start, end;

    // Initialize the MPI environment
    MPI_Init(NULL, NULL);

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int p;
    p = sqrt(world_size);
    if (p * p != world_size)
    {
        perror("Processor number must be a square number.");
        exit(0);
    }

    if (N % p)
    {
        perror("p cannot divede N.");
        exit(0);
    }

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    if (!world_rank)
    {
        if (argc > 1 && strcmp(argv[1], "random") == 0)
        {
            allocate_matrix(&A, N, 1);
            allocate_matrix(&B, N, 1);
            save_matrix("A.txt", A, N);
            save_matrix("B.txt", B, N);
        }
        else
        {
            read_matrix("A.txt", &A);
            read_matrix("B.txt", &B);
        }
        allocate_matrix(&C, N, 0);
        start = MPI_Wtime();
    }
    
    // MPI_Barrier(MPI_COMM_WORLD);


    // Send tiles
    int **local_A;
    int **local_B;
    int **local_C;
    allocate_matrix(&local_A, N / p, 0);
    allocate_matrix(&local_B, N / p, 0);

    int sizes[2]    = {N, N};         /* global size */
    int subsizes[2] = {N / p, N / p};     /* local size */
    int starts[2]   = {0,0};                        /* where this one starts */
    MPI_Datatype type, subarrtype;
    MPI_Type_create_subarray(2, sizes, subsizes, starts, MPI_ORDER_C, MPI_INT, &type);
    MPI_Type_create_resized(type, 0, N / p * sizeof(int), &subarrtype);
    MPI_Type_commit(&subarrtype);

    int *globalptr_A=NULL;
    int *globalptr_B=NULL;
    if (!world_rank)
    {
        globalptr_A = &(A[0][0]);
        globalptr_B = &(B[0][0]);
    }

    int sendcounts[p * p];
    int displs[p * p];

    if (world_rank == 0) {
        for (int i=0; i<p*p; i++)
            sendcounts[i] = 1;
        int disp = 0;
        for (int i=0; i<p; i++) {
            for (int j=0; j<p; j++) {
                displs[i*p+j] = disp;
                disp += 1;
            }
            disp += ((N / p)-1)*p;
        }
    }

    MPI_Scatterv(globalptr_A, sendcounts, displs, subarrtype, &(local_A[0][0]),
                 N*N/(p*p), MPI_INT,
                 0, MPI_COMM_WORLD);
    MPI_Scatterv(globalptr_B, sendcounts, displs, subarrtype, &(local_B[0][0]),
                 N*N/(p*p), MPI_INT,
                 0, MPI_COMM_WORLD);

    // Initial shift
    initial_w_shift(local_A, N / p, p, world_rank);

    initial_n_shift(local_B, N / p, p, world_rank);


    allocate_matrix(&local_C, N / p, 0);
    // Iterate
    for (int i = 0; i < p; i++)
    {
        // Local matrix multiplication
        matrix_multiplication(local_A, local_B, local_C, N / p);

        // Shift
        if (p > 1)
        {
            w_shift(local_A, N / p, p, world_rank);
            n_shift(local_B, N / p, p, world_rank);
        }
        
    }


    // Gather
    MPI_Request request;
    // MPI_Igatherv(&(local_C[0][0]), N*N/(p*p),  MPI_INT,
    //              &(C[0][0]), sendcounts, displs, subarrtype,
    //              0, MPI_COMM_WORLD, &request);
    
    if (world_rank)
    {
        MPI_Isend(local_C[0], N * N / (p * p), MPI_INT, 0, 4, MPI_COMM_WORLD, &request);
    }

    else
    {
        for (int i = 0; i < N / p; i++)
        {
            for (int j = 0; j < N / p; j++)
            {
                C[i][j] = local_C[i][j];
            }
        }

        int row;
        int col;
        int start_x;
        int start_y;
        for (int k = 1; k < world_size; k++)
        {
            MPI_Recv(local_C[0], N * N / (p * p), MPI_INT, k, 4, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            row = k / p;
            col = k % p;
            start_x = row * N / p;
            start_y = col * N / p;

            for (int i = start_x; i < start_x + N / p; i++)
            {
                for (int j = start_y; j < start_y + N / p; j++)
                {
                    C[i][j] = local_C[i - start_x][j - start_y];
                }
            }

        } 
    }

    // free_matrix(&local_A);
    // free_matrix(&local_B);
    // free_matrix(&local_C);

    if (!world_rank)
    {
        end = MPI_Wtime();

        save_matrix("C.txt", C, N);

        free_matrix(&A);
        free_matrix(&B);
        free_matrix(&C);

        printf("time: %f seconds\n", end - start);
    }

    MPI_Finalize();

    return 0;
}