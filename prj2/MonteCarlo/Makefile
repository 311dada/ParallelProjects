all: MPI_OMP_PI OMP_PI

MPI_OMP_PI: MPI_OMP_pi.c
	mpicc -fopenmp -o MPI_OMP_PI MPI_OMP_pi.c

OMP_PI: OMP_pi.c
	gcc -fopenmp -o OMP_PI OMP_pi.c

clean:
	rm -f MPI_OMP_PI
	rm -f OMP_PI