all:
	-gcc -c -O2 -mno-sse -msoft-float -I../config/i386/ -I.. *.c
	ar -crv libsoft-fp.a *.o
	rm -f *.o
