#include <mpi.h>

void print_in_order(double **x, int nx, int ny, MPI_Comm comm);

void print_grid_slowly(double **x, int layers, int nprocs, int myid, int sx, int ex, int sy, int ey, int nx, int ny, MPI_Comm comm);
void  print_full_grid(double **x, int nx, int ny);

void print_layer(double **x,int myid,int layer, int layer_length, int layers, int sx, int ex, int sy, int ey, MPI_Comm comm);

void print_top_line(double **x,int myid, int layer_length, int sx, int ex, MPI_Comm comm);
void print_bottom_line(double **x,int myid,int nprocs, int layer_length, int sx, int ex, int ey, MPI_Comm comm);
