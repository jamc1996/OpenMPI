
MPCC = mpicc
CC = gcc
CFLAGS = -Wall

objects = array3.o main.o mpisetup.o

all: arraymaker

arraymaker: $(objects)
	module load cports openmpi;\
	$(MPCC) $(CFLAGS) -o arraymaker $(objects) -lm

main.o: main.c
	module load cports openmpi;\
	$(MPCC) $(CFLAGS) -c $<

array3.o: array3.c array3.h
	$(CC) $(CFLAGS) -c $<

mpisetup.o: mpisetup.c mpisetup.h
	module load cports openmpi;\
	$(MPCC) $(CFLAGS) -c $<

.PHONY: clean
clean:
	rm -f $(objects) arraymaker


test: arraymaker
	module load cports openmpi;\
	mpiexec -n 12 ./arraymaker
