
MPCC = mpicc
CC = gcc
CFLAGS = -Wall

objects = array3.o main.o mpisetup.o shift.o

all: arraymaker

arraymaker: $(objects)
	module load cports openmpi;\
	$(MPCC) $(CFLAGS) -o arraymaker $(objects) -lm

main.o: main.c
	module load cports openmpi;\
	$(MPCC) $(CFLAGS) -c $<

array3.o: array3.c array3.h
	module load cports openmpi;\
	$(MPCC) $(CFLAGS) -c $<

mpisetup.o: mpisetup.c mpisetup.h
	module load cports openmpi;\
	$(MPCC) $(CFLAGS) -c $<

shift.o: shift.c shift.h
	module load cports openmpi;\
	$(MPCC) $(CFLAGS) -c $<

.PHONY: clean
clean:
	rm -f $(objects) arraymaker


test: arraymaker
	module load cports openmpi;\
	mpiexec -n 27 ./arraymaker
