#include "allocarray.h"

/*	allocarray.c -- program for memory allocation/freeing/etc. of Array3 struct
 *
 *	Author: John Cormican
 *
 *	Purpouse: Proper memory allocation, management and basic tools for Array3 struct
 *
 *	Usage:	Function called from other scripts to help use of Array3 structs.
 */

void alloc_array3(Array3 *array, int dim0, int dim1, int dim2, int procs_dim[], int myid)
/* Function to allocate memory for an array3 stucture across multiple processors. */
{
	int i, j;

	// Useful values filled in
	array->myid = myid;
	array->global_dim[0] = dim0;
	array->global_dim[1] = dim1;
	array->global_dim[2] = dim2;
	for (i=0; i<3;i++)
	{
		array->proc_dims[i] = procs_dim[i];
	}

	// block sizes calculated.
	find_blocksizes(array, myid);	

	// Memory allocation and pointer positioning
	array->data1d = malloc(sizeof(double)*array->local_dim[0]*array->local_dim[1]*array->local_dim[2]);
	array->data2d = malloc(sizeof(double*)*array->local_dim[0]*array->local_dim[1]);
	array->data3d = malloc(sizeof(double**)*array->local_dim[0]);
	for ( i=0; i<array->local_dim[0]; i++)
	{
		array->data3d[i] = &array->data2d[i*array->local_dim[1]];
		for (j=0; j<array->local_dim[1]; j++)
		{
			array->data2d[(i*array->local_dim[1])+j] = &array->data1d[(i*array->local_dim[1]*array->local_dim[2])+(j*array->local_dim[2])];
		}
	}
}

void find_blocksizes(Array3* array, int myid)
/* Function to find how much of global array is stored on a processor*/
{
	int i,determinant;
	// Size calculated for each dimension of grid
	for (i=0;i<3;i++)
	{
		array->local_dim[i] = array->global_dim[i]/array->proc_dims[i];
		array->part_size[i] = array->local_dim[i];
		array->procs_extra[i] = array->global_dim[i]%array->proc_dims[i];
		if (i == 0)
		{
			determinant = myid/(array->proc_dims[1]*array->proc_dims[2]);
		}
		if (i == 1)
		{
			determinant = (myid/array->proc_dims[2])%array->proc_dims[1];
		}
		if (i == 2)
		{
			determinant = myid%array->proc_dims[2];
		}
		if (determinant < array->procs_extra[i])
		{
			array->local_dim[i]++;
		}
	}
}

void print_array3(Array3 *array)
/* Function used for error checking for small arrays. */
{
	int i,j,k;
	for (i=0;i<4;i++)
	{
		for(j=0;j<3;j++)
		{
			for(k=0;k<4;k++)
			{
				printf(" %lf ",array->data3d[i][j][k]);
			}
			printf("\n");
		}
		printf("\n\n\n");
	}
}

void free_array3(Array3 *array)
/* Function for freeing dynamically allocated memory of array. */
{
	free(array->data1d);
	free(array->data2d);
	free(array->data3d);
}

void fill_array3(Array3 *array, int seed)
/* Function for filling array with random numbers for a given seed. */
{
	srand48(seed);
	int i;
	for (i=0;i<(array->local_dim[0])*(array->local_dim[1])*(array->local_dim[2]);i++)
	{
		array->data1d[i] = (drand48()*4)-2;
	}
}
