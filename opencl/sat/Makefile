CC=gcc
CFLAGS=-O3
DEPS = proposition.h stack.h cnf.h nnf.h anf_lib.h anf.h anf_rpn.h anf_opencl.h array.h
OBJ = main.o proposition.o stack.o cnf.o nnf.o anf_lib.o anf.o anf_rpn.o anf_opencl.o array.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o main
