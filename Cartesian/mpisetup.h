#ifndef MPISETUP_H
#define MPISETUP_H

#include <math.h>

#include <mpi.h>


/*	mpisetup.h -- header file fore mpisetup.c
 *
 *	Author: John Cormican
 */

int init_cart_3d(int *argc, char *argv[], int *myid, int *nprocs, MPI_Comm *cartcomm, int nx, int ny, int nz, int proc_dims[]);
void set_3d_layout(int nprocs, int proc_dims[]);

#endif
