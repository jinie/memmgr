OBJECTS=memmgr.o test.o
CFLAGS=-ggdb -march=x86-64 -DWITH_OWN_LIBC
test : $(OBJECTS)
	gcc $(CFLAGS) -o test $(OBJECTS)

memmgr.o : memmgr.c memmgr.h
	gcc $(CFLAGS) -c -o memmgr.o memmgr.c

test.o : test.c
	gcc $(CFLAGS) -c -o test.o test.c

clean :
	rm -f $(OBJECTS) test
