#ifndef MY_MPI_ALLGATHER_
#define MY_MPI_ALLGATHER_
#include <mpi.h>

void my_MPI_ALLGATHER1(void* send_data, int send_count,
    MPI_Datatype send_datatype,
    void* recv_data,
    int recv_count,
    MPI_Datatype recv_datatype,
    MPI_Comm communicator);

void my_MPI_ALLGATHER2(void* send_data, int send_count,
    MPI_Datatype send_datatype,
    void* recv_data,
    int recv_count,
    MPI_Datatype recv_datatype,
    MPI_Comm communicator);

#endif