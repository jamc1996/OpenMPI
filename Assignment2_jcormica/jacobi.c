#include <stdio.h>

#include <mpi.h>

#include "jacobi.h"

/*
	jacobi.c -- contains functions for Jacobi iteration in parallel to solve the poisson equation on a grid.

	Author: John Cormican

	Purpose: To implement Jacobi iteration and empirically solve an equation across the grid.

	Usage:	Functions called from loop in main.c to implement iteration and to check error between successive iterations.
*/


void sweep2d(double **a, double **f, int nx, int ny,
	     int sx, int ex, int sy, int ey, double **b)
/* Using finite difference method, this updates values of the second matrix give (within 
scope of the processor) using the values of the first matrix given.*/
{
  double hx, hy;
  int i,j;
	
  hx = 1.0/(double)(nx+1);
	hy = 1.0/(double)(ny+1);

	
  for(i=sy; i<=ey; i++){
    for(j=sx; j<=ex; j++){
      b[i][j] = 0.25 * ( a[i-1][j] + a[i+1][j] + a[i][j+1] + a[i][j-1]  - hx*hy*f[i][j] );
    }
  }
}

void exchanglr(double **x, int sx, int ex, int sy, int ey, MPI_Comm comm,
	      int nbrleft, int nbrright, MPI_Datatype mpi_vector)
/* Function trades halo values of processor with it's neighbours on the right and left.*/
{
	//Pass left and receive from right:
  MPI_Sendrecv(&x[sy][ex], 1, mpi_vector, nbrright, 0, &x[sy][sx-1], 1, mpi_vector, nbrleft, 0, comm, MPI_STATUS_IGNORE);

	//Pass right and receive from left:
  MPI_Sendrecv(&x[sy][sx], 1, mpi_vector, nbrleft, 1, &x[sy][ex+1], 1, mpi_vector, nbrright, 1, comm, MPI_STATUS_IGNORE);

}

void exchangud(double **x, int sx, int ex, int sy, int ey, MPI_Comm comm,
	      int nbrup, int nbrdown)
/* Function trades halo values of processor with it's neighbours above and below.*/
{
	//Pass down and receive from above:
  MPI_Sendrecv(&x[ey][sx], 1+ex-sx, MPI_DOUBLE, nbrdown, 0, &x[sy-1][sx], 1+ex-sx, MPI_DOUBLE, nbrup, 0, comm, MPI_STATUS_IGNORE);

	//Pass up and receive from below:
  MPI_Sendrecv(&x[sy][sx], 1+ex-sx, MPI_DOUBLE, nbrup, 1, &x[ey+1][sx], 1+ex-sx, MPI_DOUBLE, nbrdown, 1, comm, MPI_STATUS_IGNORE);

}

double griddiff(double **a, double **b, int sx, int ex, int sy, int ey)
/* Function for finding the sum of the squares of the difference at each point in the 
scope of the processor - giving local difference/error. */
{
  double sum;
  double tmp;
  int i, j;

  sum = 0.0;

  for(i=sx; i<=ex; i++){
    for(j=sy;j<=ey;j++){
      tmp = (a[i][j] - b[i][j]);
      sum = sum + tmp*tmp;
    }
  }

  return sum;

}
