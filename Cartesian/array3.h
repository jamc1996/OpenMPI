#ifndef ARRAY3_H
#define ARRAY3_H

#include <stdio.h>
#include <stdlib.h>

typedef struct 
{
	int dim0;
	int dim1;
	int dim2;

	double *** data3d;
	double ** data2d;
	double *	data1d;

} Array3;

Array3 alloc_array3(int dim0, int dim1, int dim2);
void free_array3(Array3 *array);
void fill_array3(Array3 *array, int seed);
void print_array3(Array3 *array);

#endif
