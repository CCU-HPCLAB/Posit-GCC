all:
	-gcc -c -O2 -msoft-float -m32 -I../config/i386/ -I.. *.c
	ar -crv libsoft-fp.a *.o
	rm -f *.o
	cp -f libsoft-fp.a /home/hubertka/gcc/linuxgcc1102/bin
