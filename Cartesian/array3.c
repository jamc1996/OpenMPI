#include "array3.h"

Array3 alloc_array3(int dim0, int dim1, int dim2, int procs_dim[], int myid)
{
	Array3 array;
	int i, j;

	array.myid = myid;

	array.global_dim[0] = dim0;
	array.global_dim[1] = dim1;
	array.global_dim[2] = dim2;


	for (i=0; i<3;i++)
	{
		array.proc_dims[i] = procs_dim[i];
	}

	find_blocksizes(&array, myid);	


	array.data1d = malloc(sizeof(double)*array.local_dim[0]*array.local_dim[1]*array.local_dim[2]);
	array.data2d = malloc(sizeof(double*)*array.local_dim[0]*array.local_dim[1]);
	array.data3d = malloc(sizeof(double**)*array.local_dim[0]);

	for ( i=0; i<array.local_dim[0]; i++)
	{
		array.data3d[i] = &array.data2d[i*array.local_dim[1]];
		for (j=0; j<array.local_dim[1]; j++)
		{
			array.data2d[(i*array.local_dim[1])+j] = &array.data1d[(i*array.local_dim[1]*array.local_dim[2])+(j*array.local_dim[2])];
		}
	}
	return array;
}

void find_blocksizes(Array3* array, int myid)
{
	int i,determinant;
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
			determinant = (myid%(array->proc_dims[0]*array->proc_dims[2]))%array->proc_dims[1];
		}
		if (i == 2)
		{
			determinant = (myid%(array->proc_dims[1]*array->proc_dims[0]))/array->proc_dims[2];
		}
		if (determinant < array->procs_extra[i])
		{
			array->local_dim[i]++;
		}
	}
	
}













void print_array3(Array3 *array)
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
{
	free(array->data1d);
	free(array->data2d);
	free(array->data3d);
}

void fill_array3(Array3 *array, int seed)
{
	srand48(seed);
	int i;
	for (i=0;i<(array->local_dim[0])*(array->local_dim[1])*(array->local_dim[2]);i++)
	{
		array->data1d[i] = (drand48()*4)-2;
	}
}
