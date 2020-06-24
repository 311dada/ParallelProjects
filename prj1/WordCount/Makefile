all: small big

small: small.c
	mpicc -o small small.c hashmap.c

big: big.c
	mpicc -o big big.c 

clean:
	rm -rf small
	rm -rf big