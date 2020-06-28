all: small big small_freq big_freq

small: small.c hashmap.c
	mpicc -o small small.c hashmap.c

big: big.c hashmap.c
	mpicc -o big big.c hashmap.c

small_freq: small_freq.c hashmap.c
	mpicc -o small_freq small_freq.c hashmap.c

big_freq: big_freq.c hashmap.c 
	mpicc -o big_freq big_freq.c hashmap.c

clean:
	rm -rf small
	rm -rf big
	rm -rf small_freq
	rm -rf big_freq