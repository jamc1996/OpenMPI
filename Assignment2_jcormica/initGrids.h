#ifndef INITGRIDS_H
#define INITGRIDS_H



void init_full_grid(double **a, double **b, double **f, int nx, int ny);

void twodinit(double **a, double **b, double **f,
	      int nx, int ny, int sx, int ex, int sy, int ey,
	      double (*lbound)(int , int),
	      double (*dbound)(int , int),
	      double (*rbound)(int , int),
	      double (*ubound)(int , int));

#endif
