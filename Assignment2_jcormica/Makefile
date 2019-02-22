
CC = mpicc
CFLAGS = -Wall

objects = decomp2d.o boundConds.o initGrids.o printGrids.o jacobi.o main.o

all: poisson

poisson: $(objects)
	module load cports openmpi; \
	$(CC) $(CFLAGS) -o poisson $(objects) -lm

decomp2d.o: decomp2d.c decomp2d.h
	module load cports openmpi; \
	$(CC) $(CFLAGS) -c $<

boundConds.o: boundConds.c boundConds.h
	module load cports openmpi; \
	$(CC) $(CFLAGS) -c $<

initGrids.o: initGrids.c initGrids.h
	module load cports openmpi; \
	$(CC) $(CFLAGS) -c $<

jacobi.o: jacobi.c jacobi.h
	module load cports openmpi; \
	$(CC) $(CFLAGS) -c $<

printGrids.o: printGrids.c printGrids.h
	module load cports openmpi; \
	$(CC) $(CFLAGS) -c $<

main.o: main.c
	module load cports openmpi; \
	$(CC) $(CFLAGS) -c $<



.PHONY: clean
clean:
	rm -f $(objects) poisson

test: poisson
	module load cports openmpi; \
	mpiexec -n 2 ./poisson
	module load cports openmpi; \
	mpiexec -n 4 ./poisson
	module load cports openmpi; \
	mpiexec -n 8 ./poisson
	module load cports openmpi; \
	mpiexec -n 16 ./poisson
	module load cports openmpi; \
	mpiexec -n 32 ./poisson
