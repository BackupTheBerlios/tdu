all: hash.o buildufile.o
	cc hash.o buildufile.o -o buildufile 

buildufile.o: hash.h buildufile.c
	cc -c -Wall buildufile.c

hash.o: hash.h hash.c
	cc -c hash.c
