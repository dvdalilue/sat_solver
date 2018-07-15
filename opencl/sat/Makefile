CC=gcc
CFLAGS=-O3 -l OpenCL
DEPS = proposition.h stack.h cnf.h nnf.h anf_op_lib.h anf_op.h anf_gpu.h
OBJ = main.o proposition.o stack.o cnf.o nnf.o anf_op_lib.o anf_op.o anf_gpu.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o main
