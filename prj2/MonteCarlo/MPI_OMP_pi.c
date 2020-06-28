#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mpi.h"


int Toss(int numTosses)
{
    int          toss, numInCircle = 0;
    double       x, y;
    unsigned int seed = (unsigned)time(NULL);
    srand(seed);
    for (toss = 0; toss < numTosses; toss++) {
        x = rand_r(&seed) / (double)RAND_MAX;
        y = rand_r(&seed) / (double)RAND_MAX;
        if ((x * x + y * y) <= 1.0)
            numInCircle++;
    }
    return numInCircle;
}

int main(int argc, char** argv)
{
    // Invalid case
    if (argc < 3) {
        perror("Invalid command.\n");
        exit(0);
    }

    // Initialize the MPI environment
    MPI_Init(&argc, &argv);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Iteration number
    int niter;
    niter = atoi(argv[1]);
    int numthreads;
    numthreads = atoi(argv[2]);
    int numThreadTosses;

    int    count = 0;  // The number of points in the circle
    int    final_count=0;
    double pi;

    double start, end;

    omp_set_num_threads(numthreads); // Set number of threads
    numThreadTosses = niter / (numthreads * world_size);  // Calculate number of tosses per thread

    MPI_Barrier(MPI_COMM_WORLD);

    start = MPI_Wtime();

    #pragma omp parallel reduction(+:count)
	{
		count = Toss(numThreadTosses);
    }

    MPI_Reduce(&count, &final_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
	end = MPI_Wtime();

    if (!world_rank)
    {
        // Approximate pi
        pi = (double)final_count / niter * 4;
        printf("Pi: %f\n", pi);
        printf("Time: %f seconds\n", end - start);
    }

    MPI_Finalize();
    
    return 0;
}
