#include <stdio.h>
#include <stdlib.h>

#include <mpi.h>
#include "shift.h"
#include "array3.h" 
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
{
	int nx = 10;
	int ny = 10;
	int nz = 10;

	int proc_dims[3];


	int myid;
  int nprocs;


	MPI_Comm cartcomm;


	init_cart_3d(&argc, argv, &myid, &nprocs, &cartcomm, nx, ny, nz, proc_dims);


	Array3 phi = alloc_array3(nx, ny, nz, proc_dims, myid);

	fill_array3(&phi, myid);

	if (myid == 0)
	{
		printf("myid is %d and the phi[%d][%d][%d] is %lf\n",myid,0,0,0,phi.data3d[0][0][0]);
		printf("myid is %d and the phi[%d][%d][%d] is %lf\n",myid,1,0,0,phi.data3d[1][0][0]);
		printf("myid is %d and the phi[%d][%d][%d] is %lf\n",myid,2,0,0,phi.data3d[2][0][0]);
		printf("myid is %d and the phi[%d][%d][%d] is %lf\n",myid,3,0,0,phi.data3d[3][0][0]);
	}
	if (myid == 18)
	{
		printf("myid is %d and the phi[%d][%d][%d] is %lf\n",myid,1,0,0,phi.data3d[1][0][0]);
		printf("myid is %d and the phi[%d][%d][%d] is %lf\n",myid,2,0,0,phi.data3d[2][0][0]);
	}
	shift(0, 2, cartcomm, &phi);




	int me, myself;

	MPI_Cart_shift(cartcomm, 0, 1, &me, &myself);

	//printf("I am %d beautiful %d\n",me, myself);
	//printf("I am processor %d with neighbours up %d and down %d ---> neighbours left and right %d and %d ----> and neighbours forward and backward %d and %d\n",myid,nbr_x1,nbr_x0,nbr_y1, nbr_y0, nbr_z1, nbr_z0);
//printf("I am processor %d and my chunk is %d x %d x %d\n",myid,phi.local_dim[0],phi.local_dim[1], phi.local_dim[2]);
	if (myid == 0)
	{
		printf("myid is %d and the phi[%d][%d][%d] is %lf\n",myid,0,0,0,phi.data3d[0][0][0]);
		printf("myid is %d and the phi[%d][%d][%d] is %lf\n",myid,1,0,0,phi.data3d[1][0][0]);
		printf("myid is %d and the phi[%d][%d][%d] is %lf\n",myid,2,0,0,phi.data3d[2][0][0]);
		printf("myid is %d and the phi[%d][%d][%d] is %lf\n",myid,3,0,0,phi.data3d[3][0][0]);
	}



	free_array3(&phi);
	MPI_Finalize();

	return 0;
}



