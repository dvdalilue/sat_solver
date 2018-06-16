CC=gcc
CFLAGS=-O3
DEPS = proposition.h stack.h
OBJ = main.o proposition.o stack.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o main