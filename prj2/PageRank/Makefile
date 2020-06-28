all: OMP_PR MPI_OMP_PR GRAPH

GRAPH: create_graph.c pr_util.c
	gcc -o GRAPH create_graph.c pr_util.c

OMP_PR: OMP_pr.c pr_util.c
	gcc -o OMP_PR -fopenmp OMP_pr.c pr_util.c

MPI_OMP_PR: MPI_OMP_pr.c pr_util.c
	mpicc -o MPI_OMP_PR -fopenmp MPI_OMP_pr.c pr_util.c

clean:
	rm -rf OMP_PR
	rm -rf MPI_OMP_PR
	rm -rf GRAPH