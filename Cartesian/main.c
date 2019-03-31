#include <stdio.h>
#include <stdlib.h>

#include <mpi.h>
#include "shift.h"
#include "array3.h" 
#include "allocarray.h"
#include "mpisetup.h"

/*		main.c -- program to initialize and manipulate a cartesian grid.
 *
 *		Author: John Cormican
 *
 *		Purpouse:		To practice moving data around a Cartesian grid.
 *
 *		Usage: Compile with mpisetup.c and array3.c and run with mpiexec.
 */

int main(int argc, char *argv[])
/* Function used to test out my shift functions around the matrix. */
{
	//Arbitrary matrix dimensions
	int nx = 100;
	int ny = 100;
	int nz = 100;

	int proc_dims[3];
	int myid;
  int nprocs;
	MPI_Comm cartcomm;


	// Cartesian topology intialized
	init_cart_3d(&argc, argv, &myid, &nprocs, &cartcomm, nx, ny, nz, proc_dims);
	
	// 3d array created on each processor and filled with random values
	Array3 phi;
	alloc_array3(&phi, nx, ny, nz, proc_dims, myid);
	fill_array3(&phi, myid);
	
	// Testing of shift function:	
	if (myid == 0)
	{
		int loc = phi.local_dim[2]-1;
		printf("\nTesting done tracking one value move around the grid and between processors.\n");
		printf("\n\n(myid is %d) and phi[0][0][%d] is %lf\n",myid,loc,phi.data3d[0][0][loc]);
		printf("The grid will now be shifted once in the x_2 direction.\n\n\n");
	}
	shift(2, 1, cartcomm, &phi);
	int val;
	if (myid == 1)
	{
		val = phi.local_dim[1];
		printf("(myid is %d) and phi[0][0][0] is %lf\n",myid,phi.data3d[0][0][0]);
		printf("We now shift the grid by %d in the x_1 direction.\n\n\n",phi.local_dim[1]);
	}
	MPI_Bcast(&val, 1, MPI_INT, 1, cartcomm);
	shift(1, val, cartcomm, &phi);
	if (myid == 1+proc_dims[2])
	{
		printf("(myid is %d) and phi[0][0][0] is %lf\n",myid,phi.data3d[0][0][0]);
		printf("We now shift the grid by -1 in the x_0 direction.\n\n\n");
	}
	shift(0, -1, cartcomm, &phi);
	if (myid == (nprocs+(1+proc_dims[2])-proc_dims[2]*proc_dims[1])%nprocs)
	{
		printf("(myid is %d) and phi[0][%d][0] is %lf\n\n\n",myid,phi.local_dim[0]-1,phi.data3d[phi.local_dim[0]-1][0][0]);
	}

	free_array3(&phi);
	MPI_Finalize();
	return 0;
}



