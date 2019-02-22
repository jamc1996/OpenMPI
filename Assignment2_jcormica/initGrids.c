#include <mpi.h>

#include <stdio.h>

#include "initGrids.h"


/*
	initGrids.c -- contains functions needed to initialize a grid in parallel
								 with given boundary conditions.

	Author: John Cormican

	Purpose: Initialize a grid for computation.

	Usage:	Functions called from.c to initialize grids for computation.
*/

void init_full_grid(double **a, double **b, double **f, int nx, int ny)
/* Initialize all values in a,b, and f grids to junk value of -1.*/
{
  int i,j;
  const double junkval = -1;

  for(i=0; i < ny+2; i++){
    for(j=0; j< nx+2; j++){
      a[i][j] = junkval;
      b[i][j] = junkval;
      f[i][j] = junkval;
    }
  }

}

void twodinit(double **a, double **b, double **f,
	      int nx, int ny, int sx, int ex, int sy, int ey,
	      double (*lbound)(int , int),
	      double (*dbound)(int , int),
	      double (*rbound)(int , int),
	      double (*ubound)(int , int))
/* Function where each processor sets the values within it's scope. Boundary points
are set to correct value and points within are set to 0. */
{
  int i,j;

  //Everything set to zero:

  for(i=sy-1; i<=ey+1; i++){
    for(j=sx-1; j <= ex+1; j++){
      a[i][j] = 0.0;
      b[i][j] = 0.0;
      f[i][j] = 0.0;	
		}
  }
	// We are taking f==0 everywhere (Laplace Equation), so we do not update this.

	// Now all boundary values are set:

 	if( sx == 1){
    for(j=sy-1; j<=ey+1; j++){
      a[j][0] = lbound(j, ny);
      b[j][0] = lbound(j, ny);
    }
  }
 
 	if( ex == nx ){
    for(j=sy-1; j<=ey+1; j++){
      a[j][nx+1] = rbound(j, ny);
      b[j][nx+1] = rbound(j, ny);
    }

  }

	if( sy == 0){
		for(j=sx-1;j<=ex+1;j++){
			a[0][j] = ubound(j,nx);
			b[0][j] = ubound(j,nx);
			}
	}
	
	if( ey == ny){
		for(j=sx-1;j<=ex+1;j++){
			a[ny+1][j] = dbound(j,nx);
			b[ny+1][j] = dbound(j,nx);
		}
	}


	
}
