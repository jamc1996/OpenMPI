#ifndef SHIFT_H
#define SHIFT_H

#include <mpi.h>

#include "array3.h" 

void shift(int dir, int pm, MPI_Comm grid, Array3* phi);
void search_back_direction(int pm, int dir, int *back_split, int* proc_dist, Array3* phi, int coord);
void search_forward_direction(int pm, int dir, int *split, int* proc_dist, Array3* phi, int coord);

#endif
