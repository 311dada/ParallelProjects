#include <stdio.h>
#include "pr_util.h"
#include <string.h>
#include <omp.h>
#include "mpi.h"
#define ITER 100
#define START 10000


int main(int argc, char** argv)
{
    if (argc < 2)
    {
        perror("Invalid command.");
        exit(0);
    }

    // Initialize the MPI environment
    MPI_Init(NULL, NULL);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    graph *g;
    int threadnums;

    g = read_graph("graph.txt");
    threadnums = atoi(argv[1]);


    // Pagerank
    double *ranks;
    double *pre_ranks;
    double *local_ranks;
    int cnt;
    cnt = g->node_num / world_size;

    if (!world_rank)
    {
        ranks = (double*)malloc(sizeof(double) * g->node_num);
    }
    pre_ranks = (double*)malloc(sizeof(double) * g->node_num);
    local_ranks = (double*)malloc(sizeof(double) * cnt);

    

    omp_set_num_threads(threadnums);
    double start, end;
    MPI_Barrier(MPI_COMM_WORLD);

    start = MPI_Wtime();

    #pragma omp parallel for
    for (int i = 0; i < g->node_num; i++)
        pre_ranks[i] = (double)START / g->node_num;
    

    double sum;
    int parent;
    int index;
    for (int i = 0; i < ITER; i++)
    {
        // For each iteration
        MPI_Bcast(pre_ranks, g->node_num, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        #pragma omp parallel for private(sum, parent, index)
        for (int j = 0; j < cnt; j++)
        {
            sum = 0;
            index = cnt * world_rank + j;
            for (int k = 0; k < g->nodes[index].parent_num; k++)
            {
                parent = g->nodes[index].parents[k];
                sum += pre_ranks[parent] / g->nodes[parent].out_degree;
            }
            local_ranks[j] = sum;
        }

        MPI_Gather(local_ranks, cnt, MPI_DOUBLE, ranks, cnt, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        

        // In turn
        if (!world_rank)
        {
            #pragma omp parallel for
            for (int j = 0; j < g->node_num; j++)
            {
                pre_ranks[j] = ranks[j];
            }
        }
    }

    if (!world_rank)
    {
        end = MPI_Wtime();
        // Output
        FILE *out;
        char* s;
        s = (char*)malloc(100);
        sprintf(s, "result_with_%d_processes_%d_threads.txt", world_size, threadnums);
        out = fopen(s, "w");
        for (int i = 0; i < g->node_num; i++)
        {
            fprintf(out, "node %d: %.4f\n", i, ranks[i]);
        }
        fclose(out);
        free(ranks);
        free(s);
        printf("time: %f seconds\n", end-start);
    }
    
    
    free(pre_ranks);
    free(local_ranks);

    MPI_Finalize();
    
    return 0;
}
