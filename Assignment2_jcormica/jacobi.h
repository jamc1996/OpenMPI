#ifndef JACOBI_H
#define JACOBI_H


void sweep2d(double **a, double **f, int nx, int ny,
	     int sx, int ex, int sy, int ey, double **b);

void exchanglr(double **x, int sx, int ex, int sy, int ey, MPI_Comm comm,
	      int nbrleft, int nbrright, MPI_Datatype mpi_vector);

void exchangud(double **x, int sx, int ex, int sy, int ey, MPI_Comm comm,
	      int nbrup, int nbrdown);

double griddiff(double **a, double **b, int sx, int ex, int sy, int ey);

#endif
