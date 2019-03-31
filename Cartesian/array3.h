#ifndef ARRAY3_H
#define ARRAY3_H

/*	array3.h -- header file containing the array3 struct.
 *
 * 	Author: John Cormican
 */

typedef struct 
{
	// Array information.
	int local_dim[3];
	int proc_dims[3];

	double *** data3d;
	double ** data2d;
	double * data1d;

	// These values could have been recalculated but was neater to store here.
	int global_dim[3];
	int part_size[3];
	int procs_extra[3];
	int myid;

} Array3;

#endif
