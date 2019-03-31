#include "mpisetup.h"

/*	mpisetup.c -- program to set up 3d cartesian grid of processors with processors
 *
 *	Author: John Cormican
 *
 *	Purpouse:  to set up a 3d cartesian grid of processors.
 *
 * 	Usage: init_cart_3d called from main function to set up grid.
 */


int init_cart_3d(int *argc, char *argv[], int *myid, int *nprocs, MPI_Comm *cartcomm, int nx, int ny, int nz, int proc_dims[])
/* Function to initialize a 3d grid of processors with periodic boundary conditions. */
{
	// Basic MPI Initialisation
	MPI_Init(argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, myid);
	MPI_Comm_size(MPI_COMM_WORLD, nprocs);


	int ndims = 3;
	int periods[3] = {1,1,1};
	int reorder = 0;

	// Processors laid out as evenly as possible
	set_3d_layout(*nprocs, proc_dims);

	// Cartesian layout created
	MPI_Cart_create(MPI_COMM_WORLD, ndims, proc_dims, periods, reorder, cartcomm);

	return 0;
}

void set_3d_layout(int nprocs, int proc_dims[])
/* Functiion to set the processor dimensions. Attempts to split as evenly as possible. */
{
	// The processors are arranged into the best available positions 
	int i = nprocs;
	int downlim = (int) floor(cbrt((double)nprocs));

	/* Set dim0 as close as possible to cube root. */	
	while(i>=downlim)
	{
		if (nprocs%i == 0)
		{
			proc_dims[0] = i;
		}
		i--;
	}

	/* Set dim0 as close as possible to square root. */	
	i = nprocs/proc_dims[0];
	downlim = (int) floor(sqrt((double)i));
	while(i>=downlim)
	{
		if (nprocs%i == 0)
		{
			proc_dims[1] = i;
		}
		i--;
	}
	
	proc_dims[2] = nprocs/(proc_dims[0]*proc_dims[1]);
}



