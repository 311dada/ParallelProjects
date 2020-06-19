#include "mpi.h"
#include <stdlib.h>
#include <stdio.h>
#include "time.h"

typedef int dtype;
#define MPI_TYPE MPI_INT

int MY_MPI_Allgather(
    void *send_data,
    int send_count,
    MPI_Datatype send_datatype,
    void *recv_data,
    int recv_count,
    MPI_Datatype recv_datatype,
    MPI_Comm communicator);
int *generate_random_data(int size);
void set_buffer(int *res, int *buffer, int size, int disp);
int equal(int *a, int *b, int size);
void print_arr(int *arr, int size);

int main(int argc, char **argv)
{
    int id, p;
    double elapsed_time;
    double my_elasped_time;
    if (argc != 2)
    {
        printf("must set number of data to send\n");
        return 0;
    }
    MPI_Init(&argc, &argv);
    /* start the timer */
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);

    /* create random data */
    srand((unsigned)clock());
    int n = atoi(argv[1]);
    dtype *data = generate_random_data(n * p);
    MPI_Bcast(data, n * p, MPI_TYPE, 0, MPI_COMM_WORLD);
    dtype *data_to_send = (dtype *)malloc(n * sizeof(dtype));
    for (int i = 0; i < n; i++)
        data_to_send[i] = data[n * id + i];
    dtype *receive_buffer = (dtype *)malloc(n * p * sizeof(dtype));
    dtype *receive_buffer2 = (dtype *)malloc(n * p * sizeof(dtype));


    /* test my all gather */
    MPI_Barrier(MPI_COMM_WORLD);
    if (!id)
        my_elasped_time = MPI_Wtime();
    MY_MPI_Allgather(data_to_send, n, MPI_TYPE, receive_buffer, n, MPI_TYPE, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    if (!id)
        my_elasped_time = MPI_Wtime() - my_elasped_time;

    /* test all gather */
    MPI_Barrier(MPI_COMM_WORLD);
    if (!id)
        elapsed_time = MPI_Wtime();
    MPI_Allgather(data_to_send, n, MPI_TYPE, receive_buffer2, n, MPI_TYPE, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    if (!id)
        elapsed_time = MPI_Wtime() - elapsed_time;

    /* check result */
    if (!id)
    {
        if (equal(receive_buffer, receive_buffer2, n * p))
        {
            // printf("my all gather works fine\n");
        }
        else
        {
            printf("my all gather fails\n");
            // print_arr(receive_buffer, n * p);
            // print_arr(receive_buffer2, n * p);
        }
        printf("my all gather use: %f\n", my_elasped_time);
        printf("all gather use %f\n", elapsed_time);
    }
    free(data);
    free(data_to_send);
    free(receive_buffer2);
    free(receive_buffer);
    MPI_Finalize();
    return 0;
}

void print_arr(int *arr, int size)
{
    for (int i = 0; i < size; i++)
        printf("%d ", arr[i]);
    printf("\n");
}

int equal(int *a, int *b, int size)
{
    for (int i = 0; i < size; i++)
    {
        if (a[i] != b[i])
            return 0;
    }
    return 1;
}

int *generate_random_data(int size)
{
    dtype *arr = (dtype *)malloc(size * sizeof(dtype));
    for (int i = 0; i < size; i++)
        arr[i] = (dtype)rand() % 1000;
    return arr;
}

int MY_MPI_Allgather(
    void *send_data,
    int send_count,
    MPI_Datatype send_datatype,
    void *recv_data,
    int recv_count,
    MPI_Datatype recv_datatype,
    MPI_Comm communicator)
{
    int id, p;
    dtype *buffer = (dtype *)malloc(recv_count * sizeof(dtype));
    MPI_Comm_rank(communicator, &id);
    MPI_Comm_size(communicator, &p);
    MPI_Status status;
    // process i send data to j
    for (int i = 0; i < p; i++)
        for (int j = 0; j < p; j++)
        {
            if (id == i && i == j)
            {
                set_buffer(recv_data, send_data, recv_count, i * recv_count);
            }
            else if (id == i)
            {
                MPI_Send(send_data, send_count, MPI_INT, j, 0, communicator);
            }
            else if (id == j)
            {
                MPI_Recv(buffer, recv_count, recv_datatype, i, 0, communicator, &status);
                set_buffer(recv_data, buffer, recv_count, i * recv_count);
            }
        }
    free(buffer);
}

void set_buffer(int *res, int *buffer, int size, int disp)
{
    for (int i = 0; i < size; i++)
        res[i + disp] = buffer[i];
}