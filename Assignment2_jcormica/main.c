#include <stdio.h>
#include <stdlib.h>

#include <mpi.h>

#include "printGrids.h"
#include "jacobi.h"
#include "decomp2d.h"
#include "boundConds.h"
#include "initGrids.h"

#define maxit 20000

/*
	main.c -- implements jacobi iteration to solve the poisson equation for set boundary conditions across multiple processors.

	Author: John Cormican

	Purpose: To demonstrate parallel computation and 2d decomposition using poisson equation.

	Usage:	Compiled with companion programs with mpicc and runs with mpiexec.
*/

int main(int argc, char *argv[]){

	//Variables Declared:
  double **a,**b,**f;												// I decided to malloc the grids.
	double *avals,*bvals,*fvals;
	int nx, ny, layers;												// Grid size and layers of processors
	int myid, nprocs;
	int nbrleft, nbrright, nbrdown, nbrup;
	int sx, sy, ex, ey;												// Start points in the x & y directions
	int it,i;																	
	MPI_Comm cartcomm;												// Variables for the Cartesian topology
	int ndims, dims[2], periods[2], reorder;
	double glob_diff, ldiff;									// Variables for error between successive iterations
	double tol = 1.0E-07;		


	// Mpi Initialized:
	MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &myid);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

	
	choose_layers(nprocs,&layers);	// layout the decomposition of the matrix among processors
	nx = 10;					// number of points of interest along x direction
	ny = 10;					// number of points of interest along y direction


	//Memory is allocated according to nx and ny. Data stored contiguously:
	avals = malloc((nx+2)*(ny+2)*sizeof(double)); 
	bvals = malloc((nx+2)*(ny+2)*sizeof(double));	
	fvals = malloc((nx+2)*(ny+2)*sizeof(double));
	a = malloc((ny+2)*sizeof(double*));
	b = malloc((ny+2)*sizeof(double*));
	f = malloc((ny+2)*sizeof(double*));
	for (i = 0; i<ny+2;i++){
		a[i] = &avals[i*(nx+2)];
		b[i] = &bvals[i*(nx+2)];
		f[i] = &fvals[i*(nx+2)];
	}

	// Values set for 2d Cartesian topology:
	ndims      = 2;
  dims[0]    = layers;
	dims[1] 	 = nprocs/layers;
  periods[0] = 0;
	periods[1] = 0;
  reorder    = 0;

	// cartcomm created:
  MPI_Cart_create(MPI_COMM_WORLD, ndims, dims, periods, reorder, &cartcomm);
  MPI_Cart_shift(cartcomm, 1, 1, &nbrleft, &nbrright);
  MPI_Cart_shift(cartcomm, 0, 1, &nbrup, &nbrdown);

	// Grid initiated with junk values -1 at each point.
	init_full_grid(a,b,f,nx,ny);

	// Carve up grid between processors:
  decomp2d(nx, ny, layers, nprocs, myid, &sx, &sy, &ex, &ey);

  //printf("(myid: %d) nx: %d sx,ex: %d,%d; sy,ey: %d,%d; nbrleft: %d; nbrright: %d; nbrup: %d, nbrdown: %d\n\n",
	// myid, nx , sx, ex, sy, ey, nbrleft, nbrright,nbrup,nbrdown);


	// column type created for tranferring columns between processors:
	MPI_Datatype column;
	MPI_Type_vector(1+ey-sy, 1, nx+2, MPI_DOUBLE, &column);
	MPI_Type_commit(&column);


	// Proper initialisation, boundary values set, elsewhere in processor chunk 0.
	twodinit(a, b, f, nx, ny, sx, ex, sy, ey, fxzero, fyone, fxone, fyzero);


	// Now use Jacobi iteration to solve system:
	for (it =0;it<maxit;it++){
		
		// sweep2d updates the values of grid in b using a
		// exchange functions then give each processor the updated values it needs:
		sweep2d(a, f, nx, ny, sx, ex, sy, ey, b);
		exchanglr(b, sx, ex, sy, ey, cartcomm, nbrleft, nbrright, column);
		exchangud(b, sx, ex, sy, ey, cartcomm, nbrup, nbrdown);

		// same is repeated as a is updated using b and values traded.
		sweep2d(b, f, nx, ny, sx, ex, sy, ey, a);		
		exchanglr(a, sx, ex, sy, ey, cartcomm, nbrleft, nbrright, column);
		exchangud(a, sx, ex, sy, ey, cartcomm, nbrup, nbrdown);

		// A measure of the error is calculated, option to print every second iteration:		
		ldiff = griddiff(a,b,sx,ex,sy,ey);
		MPI_Allreduce(&ldiff, &glob_diff, 1, MPI_DOUBLE, MPI_SUM, cartcomm);
    if(myid==0 && it%2==0){
     	//printf("(myid %d) locdiff: %14.10lf; glob_diff: %14.10lf\n",myid, ldiff,
			//			glob_diff);
	  }

		// If the error goes below our set tolerance the loop breaks:
   	if( glob_diff < tol ){
      if(myid==0){
				printf("\n===> Iterative solution converged after %d iterations (processors: %d, tol: %E) <===\n\n",it,nprocs,tol);
      }
      break;
    }	
	}
	if (it == maxit){
		printf("Max iterations reached, no convergence!\n");
	}

	// This prints the grid but is slow and less reliable for larger numbers of processors.
	if (nprocs<9 && nx <= 18 && ny <= 18){
		print_grid_slowly(a, layers, nprocs, myid, sx, ex, sy, ey, nx, ny, MPI_COMM_WORLD);
	}

	// Dynamically allocated memory freed.
	free(avals);
	free(a);
	free(bvals);
	free(b);
	free(fvals);
	free(f);

	MPI_Finalize();
	return 0;
}
