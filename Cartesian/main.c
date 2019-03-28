#include <stdio.h>
#include <stdlib.h>

#include <mpi.h>
#include "array3.h" 
#include "mpisetup.h"

void shift(int dir, int pm, MPI_Comm grid, Array3 phi);

int main(int argc, char *argv[])
{
	int nx = 1000;
	int ny = 1000;
	int nz = 1000;

	int proc_dims[3];
	int block_dims[3];

	int myid;
  int nprocs;


	MPI_Comm cartcomm;

	int nbr_x1, nbr_x0, nbr_y1, nbr_y0, nbr_z1, nbr_z0;
	

	init_cart_3d(&argc, argv, &myid, &nprocs, &cartcomm, nx, ny, nz, proc_dims);

	find_blocksz(nx, ny, nz, proc_dims, myid, block_dims);

	MPI_Cart_shift(cartcomm, 0, 1, &nbr_x1, &nbr_x0);
	MPI_Cart_shift(cartcomm, 1, 1, &nbr_y1, &nbr_y0);	
	MPI_Cart_shift(cartcomm, 2, 1, &nbr_z1, &nbr_z0);



	//int me, myself;

//	MPI_Cart_shift(cartcomm, 0, 0, &me, &myself);

	//printf("I am %d beautiful %d\n",me, myself);
	//printf("I am processor %d with neighbours up %d and down %d ---> neighbours left and right %d and %d ----> and neighbours forward and backward %d and %d\n",myid,nbr_x1,nbr_x0,nbr_y1, nbr_y0, nbr_z1, nbr_z0);

	printf("I am processor %d and my chunk is %d x %d x %d\n",myid,block_dims[0],block_dims[1], block_dims[2]);

	MPI_Finalize();

	return 0;
}


void shift(int dir, int pm, MPI_Comm grid, Array3* phi, int nx, int ny, int nz)
{
	MPI_Datatype surface;
	if (dir == 0)
	{
		pm = pm%nx;
		MPI_Type_vector(phi->dim1, phi->dim2, 0, MPI_DOUBLE, &surface);
	}
	else if(dir == 1)
	{
		pm = pm%ny;
		MPI_Type_vector(phi->dim0, phi->dim2, phi->dim2*phi->dim1, MPI_DOUBLE, &surface);
	}
	else if (dir == 2)
	{
		pm = pm%nz;
		MPI_Type_vector(phi->dim0*phi->dim1, 1, phi->dim2, MPI_DOUBLE, &surface);
	}
	else
	{
		printf("shift.c: invalid direction for 3d array.\n");
		return;
	}
	MPI_Type_commit(&surface);
	
	
	
	
}









