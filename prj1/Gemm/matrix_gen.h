#ifndef MATRIX_GEN_
#define MATRIX_GEN_
#define N 1024
#define MAX_NUM 100

void allocate_matrix(int ***a, int n, int random);

void free_matrix(int ***a);

void save_matrix(char *filename, int **A, int n);

void read_matrix(char *filename, int ***A);

void assign_matrix(int **A, int **B, int n);

void allocate_gen_matrix(int ***a, int h, int w);

#endif