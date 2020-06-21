#include <mpi.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

int count_words(char* filename, uint64_t offset_start, uint64_t sz);
uint64_t find_next_boundary(char* filename, uint64_t offset_start, uint64_t sz);

void manager(char* filename, int numprocs) {
    int cnt = 0;
    int local_cnt = 0;
    FILE* ifp = fopen(filename, "r");
    fseek(ifp, 0, SEEK_END);
    uint64_t sz_total = ftell(ifp);
    uint64_t sz_per_proc = sz_total / (numprocs - 1);

    uint64_t offset_start = 0;
    fclose(ifp);

    for (int i = 1; i < numprocs; ++i) {
        uint64_t sz = sz_per_proc;
        if (i == numprocs - 1) {
            sz = sz_total - offset_start;
            MPI_Send(&offset_start, 1, MPI_UINT64_T, i, 1, MPI_COMM_WORLD);
            MPI_Send(&sz, 1, MPI_UINT64_T, i, 1, MPI_COMM_WORLD);
        } else {
            MPI_Send(&offset_start, 1, MPI_UINT64_T, i, 1, MPI_COMM_WORLD);
            MPI_Send(&sz, 1, MPI_UINT64_T, i, 1, MPI_COMM_WORLD);
            offset_start += sz;
        }
    }
    for (int i = 1; i < numprocs; ++i) {
        MPI_Recv(&local_cnt, 1, MPI_INT, i, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        cnt += local_cnt;
        MPI_Send(&offset_start, 1, MPI_UINT64_T, i, 0, MPI_COMM_WORLD);
        MPI_Send(&offset_start, 1, MPI_UINT64_T, i, 0, MPI_COMM_WORLD);
    }
    printf("words: %d\n", cnt);
}


void worker(char* filename) {
    MPI_Status status;
    uint64_t offset_start, sz;

    MPI_Recv(&offset_start, 1, MPI_UINT64_T, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    MPI_Recv(&sz, 1, MPI_UINT64_T, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    while ( status.MPI_TAG > 0 ) {
        int count = count_words(filename, offset_start, sz);
        MPI_Send(&count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Recv(&offset_start, 1, MPI_UINT64_T, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        MPI_Recv(&sz, 1, MPI_UINT64_T, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    }
}

int count_words(char* filename, uint64_t offset_start, uint64_t sz) {
    FILE* ifp = fopen(filename, "r");
    fseek(ifp, offset_start, SEEK_SET);
    char ch;
    char ch_prev = ' ';
    int cnt = 0;
    while (sz-- > 0)
    {
        ch = fgetc(ifp);
        if (feof(ifp)) break;
        if (isspace(ch) && !isspace(ch_prev)) ++cnt;
        ch_prev = ch;
    }
    if (!isspace(ch_prev)) ++cnt;
    fclose(ifp);
    return cnt;
}

uint64_t find_next_boundary(char* filename, uint64_t offset_start, uint64_t sz) {
    FILE* ifp = fopen(filename, "r");
    char ch;
    fseek(ifp, offset_start + sz, SEEK_SET);
    while (1)
    {
        ch = fgetc(ifp);
        if (feof(ifp)) break;
        if (isspace(ch)) break;
        ++sz;
    }
    fclose(ifp);
    return sz;
}

int main(int argc, char** argv) {
    // Initialize the MPI environment. The two arguments to MPI Init are not // currently used by MPI implementations, but are there in case future // implementations might need the arguments.
    MPI_Init(NULL, NULL);

    // Get the number of processes
    int world_size; MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    if (world_size == 1) {
        fprintf(stderr, "at least 2 processors are requried");
        MPI_Finalize(); 
        exit(1);
    }

    // Get the rank of the process
    int world_rank; MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);



    char* filename = argv[1];
    if (world_rank == 0)
        manager(filename, world_size);
    else
        worker(filename);

    // Finalize the MPI environment. No more MPI calls can be made after this
    MPI_Finalize(); 
}
