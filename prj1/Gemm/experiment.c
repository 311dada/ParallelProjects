#include "mm.h"
#include "matrix_gen.h"


int main()
{
    int **A, **B, **C;
    allocate_matrix(&A, sizeof(int));
    allocate_matrix(&B, sizeof(int));

    C = matrix_multiplication(A, B);
    matrix_display(C);
    return 0;
}