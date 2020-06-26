#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include "matrix_gen.h"


int **conv(int **mat, int row, int col, int **kernel)
{
    int **ans;
    int sum;
    allocate_gen_matrix(&ans, row - 1, col - 1);
    for (int i = 0; i < row - 1; i++)
    {
        for (int j = 0; j < col - 1; j++)
        {
            sum = 0;
            for (int s = i; s < i + 2; s++)
            {
                for (int t = j; t < j + 2; t++)
                    sum += mat[s][t] * kernel[s - i][t - j];
            }
            ans[i][j] = sum;
        }
    }
    return ans;
}


int main(int argc, char **argv)
{
     // Initialize the MPI environment
    MPI_Init(NULL, NULL);

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    int **local_mat;
    int **mat;
    int **kernel;
    int **local_ans;
    int **global_ans;
    int rem;
    int block;
    int cnt;
    int *displs;
    int *recv_cnt;
    double start, end;
    block = (N - 1) / world_size;
    rem = (N - 1) % world_size;

    // Read matrix
    if (!world_rank)
    {
        read_matrix(argv[1], &mat);
        read_matrix(argv[2], &kernel);

        start = MPI_Wtime();
        int row;
        int start = 0;
        int cur = 0;
        displs = (int*)malloc(sizeof(int) * world_size);
        recv_cnt = (int*)malloc(sizeof(int) * world_size);

        // calculate send counts and displacements
        for (int i = 0; i < world_size; i++)
        {
            cnt = i < rem ? block + 1 : block;
            cnt++;
            if (i == 0)
            {
                row = cnt;
                allocate_gen_matrix(&local_mat, cnt, N);
                for (int j = 0; j < cnt * N; j++)
                {
                    local_mat[0][j] = mat[0][j];
                }
            }
            else
            {
                MPI_Send(mat[0] + start, cnt * N, MPI_INT, i, 0, MPI_COMM_WORLD);
            }
            recv_cnt[i] = (cnt - 1) * (N - 1);

            displs[i] = cur;
            cur += recv_cnt[i];
            start += (cnt - 1) * N;
        }
        free_matrix(&mat);
        local_ans = conv(local_mat, row, N, kernel);
        cnt = row;
        allocate_gen_matrix(&global_ans, N - 1, N - 1);
        for (int i = 0; i < (row - 1) * (N - 1); i++)
            global_ans[0][i] = local_ans[0][i];
    }

    // Receive matrix
    else
    {
        allocate_gen_matrix(&kernel, 2, 2);
        cnt = world_rank < rem ? block + 1 : block;
        cnt++;
        allocate_gen_matrix(&local_mat, cnt, N);
        MPI_Recv(local_mat[0], cnt * N, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    MPI_Bcast(kernel[0], 4, MPI_INT, 0, MPI_COMM_WORLD);


    if (world_rank)
    {
        local_ans = conv(local_mat, cnt, N, kernel);
        MPI_Send(local_ans[0], (cnt - 1) * (N - 1), MPI_INT, 0, 1, MPI_COMM_WORLD);
    }

    else
    {
        for (int i = 1; i < world_size; i++)
        {
            MPI_Recv(global_ans[0] + recv_cnt[i - 1], recv_cnt[i], MPI_INT, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        end = MPI_Wtime();
        save_matrix("conv.txt", global_ans, N - 1);
        printf("time: %f seconds\n", end -start);
        free_matrix(&global_ans);
        free(displs);
        free(recv_cnt);
    }
    free_matrix(&local_mat);
    free_matrix(&local_ans);
    free_matrix(&kernel);
    
    MPI_Finalize();
    return 0;
}