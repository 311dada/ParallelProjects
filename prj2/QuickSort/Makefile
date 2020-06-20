all: MPI_OMP_QC OMP_QC

MPI_OMP_QC: MPI_OMP_QuickSort.c
	mpicc -fopenmp -o MPI_OMP_QC MPI_OMP_QuickSort.c qc_util.c

OMP_QC: OMP_QuickSort.c
	gcc -fopenmp -o OMP_QC OMP_QuickSort.c qc_util.c

clean:
	rm -f MPI_OMP_QC
	rm -f OMP_QC