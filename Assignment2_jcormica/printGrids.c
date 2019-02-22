
#include <stdio.h>
#include <unistd.h>

#include <mpi.h>
#include <math.h>

#include "printGrids.h"

/*
	printGrids.c -- contains functions for printing out the grid values to terminal.

	Author: John Cormican

	Purpose: To check solutions are sensible and find out results.

	Usage:	Functions called from main.c to give from each processor.
*/

void  print_full_grid(double **x, int nx, int ny)
/* Function to print full grid from any processors. Values outside
the scope of the processor should be printed as -1.*/
{
  int i,j;

  for(j=0; j<ny+2; j++){
    for(i=0; i<nx+2; i++){
      if(fabs(x[j][i]) < 10000.0){
				printf("|%2.6lf| ",x[j][i]);
				} else {
				printf("%9.2lf ",x[j][i]);
      }
    }
    printf("\n");
    fflush(stdout);
    usleep(500);
  }
    fflush(stdout);
    usleep(500);

}

void print_layer(double **x,int myid,int layer, int layer_length, int layers, int sx, int ex, int sy, int ey, MPI_Comm comm)
/* Function used by print_grid_slowly to print out results from all the processors in one layer.*/
{
	int j,p,q;

	MPI_Barrier(comm);
	for (p = sy;p<=ey;p++){
		if (myid/layer_length == layer){
				for (j = 0; j<layer_length;j++){
					usleep(2000);
					if (myid%layer_length == j){
						if (myid%layer_length == 0){
								printf(" |%2.6lf| ", x[p][sx-1]);
						}
						for (q=sx;q<=ex;q++){
							printf("\e[3%dm|%2.6lf|\e[0m ",(myid%6)+1,x[p][q]);
						}
						if (myid%layer_length == layer_length-1){
								printf(" |%2.6lf| \n", x[p][ex+1]);
						}
						fflush(stdout);
					}else{
						usleep(20000);
					}
				}			
		}else{
			usleep(layer_length*20000);
		} 
	}
	MPI_Barrier(comm);
}

void print_grid_slowly(double **x, int layers, int nprocs, int myid, int sx, int ex, int sy, int ey, int nx, int ny, MPI_Comm comm)
/* A function that tries to print out the full matrix sections by section.
 Not reliable for 9+ processors and too slow for large matrix sizes. */
{
	int i;
	int layer_length = nprocs/layers;
	if(myid==0){
		printf("Attempting to print grid together (warning: this doesn't always print reliably):\n\n");
	}	
	MPI_Barrier(comm);
	print_top_line(x, myid, layer_length, sx, ex, comm);
	MPI_Barrier(comm);
	for (i=0;i<layers;i++){
		MPI_Barrier(comm);
		print_layer(x,myid,i,layer_length,layers,sx,ex,sy,ey,comm);
		MPI_Barrier(comm);
	}

	print_bottom_line(x, myid, nprocs, layer_length, sx, ex, ey, comm);

	MPI_Barrier(comm);
	if(myid==0){
		printf("\n\n");
	}	
}		


void print_in_order(double **x, int nx, int ny, MPI_Comm comm)
/* All processors print out grid sequentially. */
{
  int myid, size;
  int i;

  MPI_Comm_rank(comm, &myid);
  MPI_Comm_size(comm, &size);
  MPI_Barrier(comm);
  printf("Attempting to print in order\n");
  MPI_Barrier(comm);

  for(i=0; i<size; i++){
    if( i == myid ){
      printf("proc %d\n",myid);
      fflush(stdout);
      usleep(500);
      print_full_grid(x,nx,ny);
    }
    fflush(stdout);
    usleep(500);
    MPI_Barrier(MPI_COMM_WORLD);
  }
    fflush(stdout);
    usleep(500);
    MPI_Barrier(MPI_COMM_WORLD);
}


void print_top_line(double **x,int myid, int layer_length, int sx, int ex, MPI_Comm comm)
/* Function for use by print_grid_slowly to print top layer which is just the boundary conditions.*/
{
	int j,q;

	MPI_Barrier(comm);
	for (j=0;j<layer_length;j++){
		if (myid == j){
				if (myid == 0){
					printf(" |%2.6lf| ", x[0][sx-1]);
				}
				for (q=sx;q<=ex;q++){
					printf("|%2.6lf| ",x[0][q]);
				}
				if (myid == layer_length-1){
					printf(" |%2.6lf| \n", x[0][ex+1]);
				}
				fflush(stdout);
    }else{
			usleep(500);
		} 
	}
	MPI_Barrier(comm);
}

void print_bottom_line(double **x,int myid,int nprocs, int layer_length, int sx, int ex, int ey, MPI_Comm comm)
/* Function for use by print_grid_slowly to print bottom layer which is just the boundary conditions.*/
{
	int j,q;
	MPI_Barrier(comm);
	for (j=nprocs-layer_length;j<nprocs;j++){
		if (myid == j){
				if (myid == nprocs-layer_length){
					printf(" |%2.6lf| ", x[ey+1][0]);
				}
				for (q=sx;q<=ex;q++){
					printf("|%2.6lf| ",x[ey+1][q]);
				}
				if (myid == nprocs-1){
					printf(" |%2.6lf| \n", x[ey+1][ex+1]);
				}
				fflush(stdout);
    }else{
			usleep(500);
		} 
	}
	MPI_Barrier(comm);
}


