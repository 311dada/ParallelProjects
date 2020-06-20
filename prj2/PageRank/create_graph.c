#include "pr_util.h"

int main(int argc, char** argv)
{
    graph *g;
    int node_num;
    node_num = atoi(argv[1]);
    g = init_graph(node_num);
    save_graph(g, "graph.txt");

    return 0;
}