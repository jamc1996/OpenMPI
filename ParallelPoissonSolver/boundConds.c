#include <mpi.h>

#include "boundConds.h"

#define FD_LX  0.0
#define FD_UX  1.0
#define FD_LY  0.0
#define FD_UY  1.0

/*
	boundConds.c -- contains functions needed to calculate boundary conditions
									for a case of the poisson equation.

	Author: John Cormican

	Purpose: Calculate the state at any grid point along the boundary.

	Usage:	Called from initGrids.c to fill in the boundary of the grid.
*/


double fxzero(int yind, int ny)
/* Function for the boundary conditions along x=0. */
{
	double y;
	getycoord(yind,ny,&y);
	return y/(1+y*y);
}

double fxone(int yind, int ny)
/* Function for the boundary conditions along x=1. */
{
	double y;
	getycoord(yind,ny,&y);
	return y/(4+y*y);
}

double fyzero(int xind, int nx)
/* Function for the boundary conditions along y=0. */
{
  return 0.0;
}


double fyone(int xind, int nx)
/* Function for the boundary conditions along y=1. */
{
	double x;
	getxcoord(xind,nx,&x);
	return (double)1 /(double)((1+x)*(1+x) + 1);
}

void getycoord(int ind, int n, double *y)
/* Function takes a grid index and updates y coordinate according the y-limits. */
{
	double h = (double) (FD_UY - FD_LY) / (double)(n+1);
	*y = h*(double)ind;
}

void getxcoord(int ind, int n, double *x)
/* Function takes a grid index and updates x coordinate according the x-limits. */
{
	double h = (double) (FD_UX - FD_LX) / (double)(n+1);
	*x = h*(double)ind;
}
