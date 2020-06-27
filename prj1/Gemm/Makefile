all: mm conv pooling

mm: matrix_gen.c mm.c experiment.c
	mpicc -o mm experiment.c mm.c matrix_gen.c -lm

conv: conv.c matrix_gen.c
	mpicc -o conv matrix_gen.c conv.c

pooling: pooling.c matrix_gen.c
	mpicc -o pooling matrix_gen.c pooling.c

clean:
	rm -f mm
	rm -f pooling
	rm -f conv