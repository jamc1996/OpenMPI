#ifndef SHIFT_H
#define SHIFT_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#include "array3.h" 

/*	shift.h -- header file for shift.c with ShiftInfo struct
 *
 *	Author: John Cormican
 */




/* I created a struct so functions could access all necessary information*/
typedef struct
{
	int send_split;
	int rcv_split;

	int send_dist;
	int rcv_dist;

	int switch_flag;

	int send_size1;
	int rcv_size1;
	int first_sendr_size;
	int first_rcvr_size;
	int rcv_size2;
	int send_size2;
	int surf_size;

	int dir;
	int pm;
	
} ShiftInfo;

void get_send_sizes(ShiftInfo *info, Array3 *phi, int coord);
void create_surface_type(ShiftInfo *info, Array3 *phi, MPI_Datatype* surface);
double* find_start_dimension(int dir, int index, Array3* phi);
void shift(int dir, int pm, MPI_Comm grid, Array3* phi);
void search_back_direction(ShiftInfo *info, Array3* phi, int coord);
void search_forward_direction(ShiftInfo *info, Array3* phi, int coord);
void move_to_temp(Array3 *phi, double *temp, MPI_Datatype surface, MPI_Comm grid, ShiftInfo *info); 
void move_directly(Array3* phi, MPI_Datatype surface, MPI_Comm grid, ShiftInfo *info);
void move_from_temp(Array3* phi, double* temp, MPI_Datatype surface, MPI_Comm grid, ShiftInfo *info);
#endif
