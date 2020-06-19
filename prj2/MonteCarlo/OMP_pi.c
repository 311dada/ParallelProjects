#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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

    // Iteration number
    int niter;
    niter = atoi(argv[1]);
    int numthreads;
    numthreads = atoi(argv[2]);
    int numThreadTosses;

    int    count = 0;  // The number of points in the circle
    double pi;

    double start, end;

    omp_set_num_threads(numthreads); // Set number of threads
    numThreadTosses = niter / numthreads;  // Calculate number of tosses per thread

    start = omp_get_wtime();

    #pragma omp parallel reduction(+:count)
	{
		count = Toss(numThreadTosses);
    }

	end = omp_get_wtime();

    // Approximate pi
    pi = (double)count / niter * 4;
    printf("Pi: %f\n", pi);
    printf("Time: %f seconds\n", end - start);

    return 0;
}
