#include <stdio.h>
#include <mpi.h>
#include <math.h>

#include "decomp2d.h"

/*
	decomp2d.c -- contains functions for correct layout and carving up grid in 
								2d decomposition between multiple processors.

	Author: John Cormican

	Purpose: To lay out processors properly and divide the grid between processors in a sensible way.

	Usage:	Functions called from main.c by each processor to give procesors layout and update start and end values.
*/

int decomp2d(int nx, int ny, int layers, int nprocs, int myid, int *sxp, int *syp, int *exp, int *eyp)
/*Function takes integers nx,ny as matrix dimensions, along with the number and layout of the processors
and updates the start and end values for the scope of the processor according to it's rank (myid).*/
{
	int layerLength = nprocs/layers;

	//Error checking for sensible number of processors/layers
	if ((nprocs/layers < 1) || (nprocs%layers != 0)){
		printf("Error in processor distribution\n");		
		return 1;
	}

	// A warning for poor choice of processors used:
	if (layers == nprocs && myid == 0)
	{
		printf("Warning: prime number of processors used. Result is 1d decomposition.\n");
	}

  // partsize is the number of grid point in x direction that ALL processors must take.
  // extras is the number of processors that take an extra grid point.

  int partsize = nx/(layerLength);
  int extras = nx%(layerLength);


  if (myid%(layerLength)<extras)
  {
    partsize += 1;
    *sxp = (myid%(layerLength))*partsize + 1; 
    *exp = (myid%(layerLength))*partsize + partsize;
  }
  else
  {
    *sxp = (myid%(layerLength))*partsize + extras+1;
    *exp = (myid%(layerLength))*partsize + extras + partsize;
  }

	// We use the same process in the y direction and reuse partsize/extras variables
	partsize = ny/layers;
	extras = ny%layers;

	if (myid/(nprocs/layers)<extras)
  {
    partsize += 1;
    *syp = (myid/layerLength)*partsize+1; 
    *eyp = (myid/layerLength)*partsize + partsize;
  } 
	else
  {
    *syp = (myid/(nprocs/layers))*partsize + extras + 1;
    *eyp = (myid/(nprocs/layers))*partsize + extras + partsize;
  }

	return 0;
}

int choose_layers(int nprocs, int* layers)
/* Function to set an appropriate number of layers for the decomposition,
if*/
{
	// The processors are arranged into the best available positions 
	int i = nprocs;
	int downlim = (int) ceil(sqrt((double)nprocs));

	/* Try to limit divisions along the x direction as when passing left and right
	we are not passing contiguous blocks of memory. */	
	while(i>=downlim)
	{
		if (nprocs%i == 0)
		{
			*layers = i;
		}
		i--;
	}
	
	return 0;
}




















