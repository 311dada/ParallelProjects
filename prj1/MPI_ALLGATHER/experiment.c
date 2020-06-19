#include "my_mpi_allgather.h"
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
    // Initialize the MPI environment
    MPI_Init(NULL, NULL);

    int data_size = 1;

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    int*   send_data;
    int*   recv_data;
    double time;

    for (int i = 0; i < 8; i++) {
        data_size *= 10;
        send_data = (int*)malloc(sizeof(int) * data_size);
        recv_data = (int*)malloc(sizeof(int) * data_size * world_size);
        MPI_Barrier(MPI_COMM_WORLD);
        time = -MPI_Wtime();
        my_MPI_ALLGATHER(send_data, data_size, MPI_INT, recv_data, data_size,
                      MPI_INT, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);
        time += MPI_Wtime();
        if (world_rank == 0)
            printf("My implementation (data size = %d): %f\n", data_size, time);
        free(send_data);
        free(recv_data);
    }

    data_size = 1;
    for (int i = 0; i < 8; i++) {
        data_size *= 10;
        send_data = (int*)malloc(sizeof(int) * data_size);
        recv_data = (int*)malloc(sizeof(int) * data_size * world_size);
        MPI_Barrier(MPI_COMM_WORLD);
        time = -MPI_Wtime();
        MPI_Allgather(send_data, data_size, MPI_INT, recv_data, data_size,
                      MPI_INT, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);
        time += MPI_Wtime();
        if (world_rank == 0)
            printf("MPI implementation (data size = %d): %f\n", data_size, time);
        free(send_data);
        free(recv_data);
    }


    MPI_Finalize();

    return 0;
}