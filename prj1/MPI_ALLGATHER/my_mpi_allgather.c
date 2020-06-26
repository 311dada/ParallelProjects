#include "my_mpi_allgather.h"
#include <stdio.h>
#include <string.h>

void my_MPI_ALLGATHER1(void* send_data, int send_count,
    MPI_Datatype send_datatype,
    void* recv_data,
    int recv_count,
    MPI_Datatype recv_datatype,
    MPI_Comm communicator)
{
    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    for (int i = 0; i < world_rank; i++)
    {
        MPI_Recv(recv_data + i * recv_count, recv_count, recv_datatype, i, 0, communicator, MPI_STATUS_IGNORE);
    }
    memcpy(recv_data + world_rank * recv_count, send_data, sizeof(send_data));

    for (int i = 0; i < world_size; i++)
    {
        if (i != world_rank)
            MPI_Send(send_data, send_count, send_datatype, i, 0, communicator);
    }

    for (int i = world_rank + 1; i < world_size; i++)
    {
        MPI_Recv(recv_data + i * recv_count, recv_count, recv_datatype, i, 0, communicator, MPI_STATUS_IGNORE);
    }

}



void my_MPI_ALLGATHER2(void*        send_data,
                      int          send_count,
                      MPI_Datatype send_datatype,
                      void*        recv_data,
                      int          recv_count,
                      MPI_Datatype recv_datatype,
                      MPI_Comm     communicator)
{
    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Send data to process 0
    if (world_rank)
        MPI_Send(send_data, send_count, send_datatype, 0, 0, communicator);
    
    if (!world_rank)
    {
        // Gather
        memcpy(recv_data, send_data, sizeof(send_data));
        for (int to_recv = 1; to_recv < world_size; to_recv++)
            MPI_Recv(recv_data + to_recv * recv_count, recv_count, recv_datatype, to_recv, 0, communicator, MPI_STATUS_IGNORE);
        
        // Broadcast
        for (int to_send = 1; to_send < world_size; to_send++)
            MPI_Send(recv_data, recv_count * world_size, recv_datatype, to_send, 0, communicator);
    }

    if (world_rank)
    {
        // Receive
        MPI_Recv(recv_data, recv_count * world_size, recv_datatype, 0, 0, communicator, MPI_STATUS_IGNORE);
    }
    
}