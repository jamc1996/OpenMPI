#include "array3.h"

Array3 alloc_array3(int dim0, int dim1, int dim2)
{
	Array3 array;
	int i, j;

	array.dim0 = dim0;
	array.dim1 = dim1;
	array.dim2 = dim2;

	array.data1d = malloc(sizeof(double)*dim0*dim1*dim2);
	array.data2d = malloc(sizeof(double*)*dim0*dim1);
	array.data3d = malloc(sizeof(double**)*dim0);

	for ( i=0; i<dim0; i++)
	{
		array.data3d[i] = &array.data2d[i*dim1];
		for (j=0; j<dim1; j++)
		{
			array.data2d[(i*dim1)+j] = &array.data1d[(i*dim1*dim2)+(j*dim2)];
		}
	}
	return array;
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
	for (i=0;i<(array->dim0)*(array->dim1)*(array->dim2);i++)
	{
		array->data1d[i] = (drand48()*4)-2;
	}
}
