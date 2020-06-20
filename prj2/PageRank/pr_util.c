#include "pr_util.h"
#include <stdio.h>
#define min(a,b) ((a)>(b)?(b):(a))


int is_in(int id, int* arr, int num)
{
    for (int i = 0; i < num; i++)
    {
        if (arr[i] == id)
            return 1;
    }
    return 0;
}



graph* init_graph(int node_num)
{
    graph* g;
    g = (graph*)malloc(sizeof(graph));
    g->node_num = node_num;
    g->nodes = (node*)malloc(sizeof(node) * node_num);

    for (int i = 0; i < node_num; i++)
    {
        g->nodes[i].parent_num = 0;
        g->nodes[i].parents = NULL;
        g->nodes[i].out_degree = 0;
    }

    int max_out;
    max_out = min(MAX_OUT, node_num);
    // Randomly generate out edges for each node
    int out_degree;
    int child;
    for (int i = 0; i < node_num; i++)
    {
        out_degree = rand() % max_out + 1;
        g->nodes[i].out_degree = out_degree;
        for (int j = 0; j < out_degree; j++)
        {
            child = rand() % node_num;
            while (is_in(i, g->nodes[child].parents, g->nodes[child].parent_num))
            {
                child = rand() % node_num;
            }
            g->nodes[child].parent_num++;
            g->nodes[child].parents = realloc(g->nodes[child].parents, sizeof(int) * g->nodes[child].parent_num);
            g->nodes[child].parents[g->nodes[child].parent_num - 1] = i;
        }
    }

    return g;
}

void save_graph(graph* g, char* filename)
{
    FILE *out;
    out = fopen(filename, "w");

    fprintf(out, "%d\n", g->node_num);

    for (int i = 0; i < g->node_num; i++)
    {
        fprintf(out, "%d %d ", g->nodes[i].out_degree, g->nodes[i].parent_num);

        for (int j = 0; j < g->nodes[i].parent_num; j++)
        {
            fprintf(out, "%d ", g->nodes[i].parents[j]);
        }
        fprintf(out, "\n");
    }
    fclose(out);
}

void free_graph(graph* g)
{
    for (int i = 0; i < g->node_num; i++)
    {
        free(g->nodes[i].parents);
    }
    free(g->nodes);
    free(g);
}

graph* read_graph(char *filename)
{
    FILE *in;
    in = fopen(filename, "r");

    graph *g;
    g = (graph*)malloc(sizeof(graph));
    fscanf(in, "%d\n", &g->node_num);

    g->nodes = (node*)malloc(sizeof(node) * g->node_num);

    for (int i = 0; i < g->node_num; i++)
    {
        fscanf(in, "%d %d ", &g->nodes[i].out_degree, &g->nodes[i].parent_num);

        g->nodes[i].parents = (int*)malloc(sizeof(int) * g->nodes[i].parent_num);

        for (int j = 0; j < g->nodes[i].parent_num; j++)
        {
            fscanf(in, "%d ", &g->nodes[i].parents[j]);
        }
        fscanf(in, "\n");
    }
    fclose(in);
    return g;
}