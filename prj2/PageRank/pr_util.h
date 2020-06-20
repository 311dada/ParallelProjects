#ifndef PR_UTIL_H_
#define PR_UTIL_H_
#include <stdlib.h>
#include <math.h>
#include <time.h>
#define MAX_OUT 10

typedef struct node
{
    int *parents;
    int parent_num;
    int out_degree;
} node;

typedef struct graph
{
    int node_num;
    node *nodes;
} graph;


graph* init_graph(int node_num);

void save_graph(graph* g, char* filename);

void free_graph(graph* g);

graph* read_graph(char* filename);

#endif