#ifndef ALLOCARRAY_H
#define ALLOCARRAY_H

#include "array3.h"

#include <stdio.h>
#include <stdlib.h>

/*	allocarray.h -- header file for allocarray.c
 *
 * 	Author: John Cormican
 */

void alloc_array3(Array3 *array, int dim0, int dim1, int dim2, int procs_dim[], int myid);
void find_blocksizes(Array3* array, int myid);
void free_array3(Array3 *array);
void fill_array3(Array3 *array, int seed);
void print_array3(Array3 *array);

#endif
