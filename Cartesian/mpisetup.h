#ifndef MPISETUP_H
#define MPISETUP_H

#include <mpi.h>

int init_cart_3d(int *argc, char *argv[], int *myid, int *nprocs, MPI_Comm *cartcomm, int nx, int ny, int nz, int proc_dims[]);
int find_blocksz(int nx, int ny, int nz, int proc_dims[], int myid, int block_dims[]);

#endif
