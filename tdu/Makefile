all: hash.o buildufile.o
	cc hash.o buildufile.o -o buildufile 

buildufile.o: hash.h buildufile.c
	#cc -c --enable-checking -g -O0 -v -da -Q buildufile.c
	#cc -c -g -O0 -v -da -Q buildufile.c
	cc -c buildufile.c

hash.o: hash.h hash.c
	cc -c hash.c
