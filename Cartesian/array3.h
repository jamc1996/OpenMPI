#ifndef ARRAY3_H
#define ARRAY3_H

#include <stdio.h>
#include <stdlib.h>

typedef struct 
{
	int local_dim[3];
	int proc_dims[3];

	int global_dim[3];
	int part_size[3];
	int procs_extra[3];

	int myid;

	double *** data3d;
	double ** data2d;
	double *	data1d;

} Array3;

Array3 alloc_array3(int dim0, int dim1, int dim2, int procs_dim[], int myid);
void find_blocksizes(Array3* array, int myid);
void free_array3(Array3 *array);
void fill_array3(Array3 *array, int seed);
void print_array3(Array3 *array);

#endif
