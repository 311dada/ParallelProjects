#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include "matrix_gen.h"
#define max(a,b) ((a) > (b) ? (a) : (b))


int **pooling(int **mat, int row, int col)
{
    int **ans;
    int sum;
    allocate_gen_matrix(&ans, row - 1, col - 1);
    for (int i = 0; i < row - 1; i++)
    {
        for (int j = 0; j < col - 1; j++)
        {
            sum = -100000000;
            for (int s = i; s < i + 2; s++)
            {
                for (int t = j; t < j + 2; t++)
                    sum = max(sum, mat[s][t]);
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
        local_ans = pooling(local_mat, row, N);
        cnt = row;
        allocate_gen_matrix(&global_ans, N - 1, N - 1);
        for (int i = 0; i < (row - 1) * (N - 1); i++)
            global_ans[0][i] = local_ans[0][i];
    }

    // Receive matrix
    else
    {
        cnt = world_rank < rem ? block + 1 : block;
        cnt++;
        allocate_gen_matrix(&local_mat, cnt, N);
        MPI_Recv(local_mat[0], cnt * N, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }


    if (world_rank)
    {
        local_ans = pooling(local_mat, cnt, N);
        MPI_Send(local_ans[0], (cnt - 1) * (N - 1), MPI_INT, 0, 1, MPI_COMM_WORLD);
    }

    else
    {
        for (int i = 1; i < world_size; i++)
        {
            MPI_Recv(global_ans[0] + recv_cnt[i - 1], recv_cnt[i], MPI_INT, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        end = MPI_Wtime();
        save_matrix("pooling.txt", global_ans, N - 1);
        printf("time: %f seconds\n", end -start);
        free_matrix(&global_ans);
        free(displs);
        free(recv_cnt);
    }
    free_matrix(&local_mat);
    free_matrix(&local_ans);
    
    MPI_Finalize();
    return 0;
}