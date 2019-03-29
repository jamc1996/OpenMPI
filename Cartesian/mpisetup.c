#include "mpisetup.h"

int init_cart_3d(int *argc, char *argv[], int *myid, int *nprocs, MPI_Comm *cartcomm, int nx, int ny, int nz, int proc_dims[])
{
	MPI_Init(argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, myid);
	MPI_Comm_size(MPI_COMM_WORLD, nprocs);

	int i;
	int ndims = 3;

	int periods[3] = {1,1,1};
	int reorder = 0;
	proc_dims[0] = 3;
	proc_dims[1] = 3;
	proc_dims[2] = 3;	


	for	(i = 0; i<ndims; i++)
	{
		periods[i] = 1;
	}

//	organize_dimensions(dims, nprocs, nx, ny, nz);

	MPI_Cart_create(MPI_COMM_WORLD, ndims, proc_dims, periods, reorder, cartcomm);

	return 0;
}



/*
int decomp3d(int nx, int ny, int nz, int xprocs, int yprocs, int zprocs, int myid, int *sx, int *sy, int *sz, int *ex, int *ey, int *ez)
Function takes integers nx,ny as matrix dimensions, along with the number and layout of the processors
and updates the start and end values for the scope of the processor according to it's rank (myid).
{
	

  // partsize is the number of grid point in x direction that ALL processors must take.
  // extras is the number of processors that take an extra grid point.

  int z_partsize = nz/zprocs;
  int z_extras = nz%zprocs;
	int z_det = myid%(yprocs*xprocs);
	
  if (z_det < extras)
  {
    partsize += 1;
    *sz = (z_det)*z_partsize; 
    *ez = (z_det)*z_partsize + z_partsize;
  }
  else
  {
    *sz = (z_det*partsize + z_extras+1;
    *ez = (z_det*partsize + z_extras + partsize;
  }

	// We use the same process in the y direction and reuse partsize/extras variables
	int y_partsize = ny/yprocs;
	int y_extras = ny%yprocs;
	int y_det = (myid/zprocs)%xprocs;

	if (y_det<y_extras)
  {
    partsize += 1;
    *syp = (y_det*partsize); 
    *eyp = (y_det*partsize) + partsize;
  } 
	else
  {
    *syp = (y_det*partsize) + extras;
    *eyp = (y_det*partsize) + extras + partsize;
  }

	int x_partsize = nx/xprocs;
	int x_extras = nx%xprocs;
	int x_det = myid/(zprocs*yprocs);

	if (x_det<extras)
  {
    partsize += 1;
    *syp = x_det*partsize; 
    *eyp = (myid/layerLength)*partsize + partsize;
  } 
	else
  {
    *syp = (x_det*partsize) + extras;
    *eyp = (x_det*partsize) + extras + partsize;
  }


	return 0;
}*/
