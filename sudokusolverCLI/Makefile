CC=gcc
CFLAGS=-I.
DEPS = main.h fileops.h validitycheck.h dumbsolve.h
OBJ = main.o fileops.o validitycheck.o dumbsolve.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

sudokusolver: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o core

