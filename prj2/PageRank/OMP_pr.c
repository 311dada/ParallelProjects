#include <stdio.h>
#include "pr_util.h"
#include <string.h>
#include <omp.h>
#define ITER 100
#define START 10000


int main(int argc, char** argv)
{
    if (argc < 2)
    {
        perror("Invalid command.");
        exit(0);
    }

    graph *g;
    int threadnums;

    g = read_graph("graph.txt");
    threadnums = atoi(argv[1]);


    // Pagerank
    double *ranks;
    double *pre_ranks;

    pre_ranks = (double*)malloc(sizeof(double) * g->node_num);
    ranks = (double*)malloc(sizeof(double) * g->node_num);

    omp_set_num_threads(threadnums);
    double start, end;
    start = omp_get_wtime();

    #pragma omp parallel for
    for (int i = 0; i < g->node_num; i++)
        pre_ranks[i] = (double)START / g->node_num;
    

    double sum;
    int parent;
    for (int i = 0; i < ITER; i++)
    {
        // For each iteration
        #pragma omp parallel for private(sum, parent)
        for (int j = 0; j < g->node_num; j++)
        {
            sum = 0;
            for (int k = 0; k < g->nodes[j].parent_num; k++)
            {
                parent = g->nodes[j].parents[k];
                sum += pre_ranks[parent] / g->nodes[parent].out_degree;
            }
            ranks[j] = sum;
        }
        

        // In turn
        #pragma omp parallel for
        for (int j = 0; j < g->node_num; j++)
        {
            pre_ranks[j] = ranks[j];
        }
    }

    end = omp_get_wtime();
    // Output
    FILE *out;
    char* s;
    s = (char*)malloc(100);
    sprintf(s, "result_with_%d_threads.txt", threadnums);
    out = fopen(s, "w");
    for (int i = 0; i < g->node_num; i++)
    {
        fprintf(out, "node %d: %.4f\n", i, ranks[i]);
    }
    fclose(out);
    free(ranks);
    free(pre_ranks);
    free(s);

    printf("time: %f seconds\n", end-start);
    return 0;
}
