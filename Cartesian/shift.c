#include "shift.h"

/*	shift.cc -- program for moving data around an Array3 struct
 *
 *	Author: John Cormican
 *
 *	Purpouse: Movement of data around processors
 *
 *	Usage: shift function called from main. It uses the other functions as appropriate.
 */

void shift(int dir, int pm, MPI_Comm grid, Array3* phi)
/* Function to move elements of a 3d array phi in direction dir a distance pm. */
{
	// Checking and processing of input
	if (dir > 2 || dir < 0)
	{
		printf("shift.c: invalid direction for 3d array.\n");
		return;
	}

	if (pm > 0)
	{
	  while(pm>phi->global_dim[dir])
	  {
	  	  pm-=phi->global_dim[dir];
	  }
	}
	else
	{
	  while(pm<-phi->global_dim[dir])
	  {
	  	  pm+=phi->global_dim[dir];
	  }
	
	}

	if (pm==0)
	{
		return;
	}
	
	int coord[3];
	MPI_Cart_coords(grid, phi->myid, 3, coord);

	// An info struct created to easily pass
	ShiftInfo info;	
	info.dir = dir;
	info.pm = pm;
	
	//MPI DataType used for moving non-contiguous data:
	MPI_Datatype surface;	
	create_surface_type(&info, phi, &surface);

	// Searching done to find splits where processors and sending to/receiving from
	// Useful values stored in info
	search_back_direction(&info, phi, coord[dir]);
	search_forward_direction(&info, phi, coord[dir]);
	get_send_sizes(&info, phi, coord[dir]);

	double* temp = malloc(info.rcv_size1*info.surf_size*sizeof(double));

	// First entries must be sent to temporary buffer
	move_to_temp(phi, temp, surface, grid, &info);

	// Remainder can be moved directly
	move_directly(phi, surface, grid, &info);

	// Finally data read back from temp into Array3 struct
	move_from_temp(phi, temp, surface, grid, &info);


	free(temp);
}

void create_surface_type(ShiftInfo *info, Array3 *phi, MPI_Datatype* surface)
/* Function to create a new datatype for easier memory transfer. */
{
	if (info->dir == 0)
	{
		info->surf_size = phi->local_dim[1]*phi->local_dim[2];	
		MPI_Type_vector(1, phi->local_dim[1]*phi->local_dim[2], 0, MPI_DOUBLE, surface);
	}
	else if(info->dir == 1)
	{
		info->surf_size = phi->local_dim[0]*phi->local_dim[2];

		MPI_Type_vector(phi->local_dim[0], phi->local_dim[2], phi->local_dim[1]*phi->local_dim[2], MPI_DOUBLE, surface);
	}

	else if (info->dir == 2)
	{
		info->surf_size = phi->local_dim[1]*phi->local_dim[0];			
		MPI_Type_vector(phi->local_dim[0]*phi->local_dim[1], 1, phi->local_dim[2], MPI_DOUBLE, surface);
	}
	MPI_Type_commit(surface);
}

double* find_start_dimension(int dir, int index, Array3* phi)
/* Function to return first element of indexed surface in direction dir.
 * Cuts down on the ammount of copy pasted code. */
{
	if (dir == 0)
	{
		return &phi->data3d[index][0][0];
	}
	else if (dir == 1)
	{
		return &phi->data3d[0][index][0];
	}
	else
	{
		return &phi->data3d[0][0][index];
	}
}



	
void search_back_direction(ShiftInfo *info, Array3* phi, int coord)
/* Function to search in the backwards direction for the receiving/sending processor. */
{
	// away used to track how many processors away we should go
	// temp tracks how many elements from that processor we are sending/receiving
	int away = 0;
	int temp = abs(info->pm);	
	int final_partsize, eflag; 

	// Iterate through processors finding distance away (note changing sizes of processors)
	while(temp > 0)
	{
		eflag = 0;
		away --;
		temp -= phi->part_size[info->dir];
		if( (phi->proc_dims[info->dir] + coord + away)%phi->proc_dims[info->dir] < phi->procs_extra[info->dir])
		{
			eflag = 1;
			temp --;
		} 		
	}
	final_partsize = phi->part_size[info->dir] + eflag;

	// Useful values updated in info struct
	if (info->pm>0)
	{
		info->first_rcvr_size = final_partsize;
		info->rcv_split = final_partsize + temp; 
		info->rcv_dist = away;
	}
	else
	{
		info->first_sendr_size = final_partsize;
		info->send_split = final_partsize + temp; 
		info->send_dist = away;
	}
}

void search_forward_direction(ShiftInfo *info, Array3* phi, int coord)
/* Function to search in the backwards direction for the receiving/sending processor. */
{
	// away used to track how many processors away we should go
	// temp tracks how many elements from that processor we are sending/receiving
	int away = -1;
	int temp = -abs(info->pm);
	int final_partsize, eflag; 
	
	// Iterate through processors finding distance away (note changing sizes of processors)
	while(temp < 0)
	{
		eflag = 0;
		away += 1;
		temp += phi->part_size[info->dir];
		if( (coord + away)%phi->proc_dims[info->dir] < phi->procs_extra[info->dir])
		{
			temp ++;
			eflag = 1;
		} 		
	}
	final_partsize = phi->part_size[info->dir] + eflag;
	
	// Useful values updated in info struct
	if (info->pm > 0)
	{
		info->first_sendr_size = final_partsize;
		info->send_split = temp;
		info->send_dist = away;
	}
	else
	{
		info->first_rcvr_size = final_partsize;
		info->rcv_split = temp;
		info->rcv_dist = away;
	}
}

void get_send_sizes(ShiftInfo *info, Array3 *phi, int coord)
/* Extra function to clean up the basic split previously calculated.
 * Necessary to remove bugs from uneven ammount of data on different processors.
 */
{
	info->send_size1 = phi->local_dim[info->dir] - info->send_split;
	info->send_size2 = info->send_split;
	info->rcv_size1 = info->rcv_split;
	info->rcv_size2 = phi->local_dim[info->dir] - info->rcv_split;
	if(info->rcv_size2 == -1)
	{
		info->rcv_size1=0;
		info->rcv_size2=phi->local_dim[info->dir];
		if (info->pm > 0)
		{
			info->rcv_dist--;
		}
		else 
		{
			info->rcv_dist++;
		}
	}
	if(info->send_size1 > phi->part_size[info->dir])
	{
		if((coord + info->send_dist+1)%phi->proc_dims[info->dir] >= phi->procs_extra[info->dir])
		{
			info->send_size1=1;
			info->send_size2=phi->part_size[info->dir];
			if (info->pm > 0)
			{
				info->send_dist++;
			}
			else 
			{
				info->send_dist--;
			}
		}	
	}	
}

void move_to_temp(Array3 *phi, double *temp, MPI_Datatype surface, MPI_Comm grid, ShiftInfo *info)
/* Function for moving the first block of data to a temporary array on the receiving processor. */
{
	// Receiving/Sending processors calculated
	int proc_rcvr, proc_sendr, tmp_proc;
	MPI_Cart_shift(grid, info->dir, info->rcv_dist, &tmp_proc, &proc_sendr);
	MPI_Cart_shift(grid, info->dir, info->send_dist+1, &tmp_proc, &proc_rcvr);
	
	// Sendrecv used as much as possible but may be different number of sends to receives.
	int i;
	int count = info->send_size1;
	int diff = info->send_size1 - info->rcv_size1;
	double* pointer;
	if (diff>0)
	{
		count = info->rcv_size1;
	}

	MPI_Barrier(grid);	
	for (i=0; i<count; i++)
	{
		pointer = find_start_dimension( info->dir, info->send_split+i, phi);
		MPI_Sendrecv( pointer, 1 , surface, proc_rcvr, 0, &temp[i*info->surf_size], info->surf_size, MPI_DOUBLE, proc_sendr, 0, grid, MPI_STATUS_IGNORE);
	}	
	if (diff>0)
	{
		for (i=0;i<(diff);i++)
		{
			pointer = find_start_dimension( info->dir, info->send_split+info->rcv_size1+i, phi);
			MPI_Send(pointer, 1, surface, proc_rcvr, 0, grid);
		}
	}
	else if (diff<0)
	{
		for ( i=0; i<(-diff); i++)
		{
			MPI_Recv(&temp[(count+i)*info->surf_size], info->surf_size, MPI_DOUBLE, proc_sendr, 0, grid, MPI_STATUS_IGNORE);
		}
	}

}

void move_from_temp(Array3* phi, double* temp, MPI_Datatype surface, MPI_Comm grid, ShiftInfo *info)
/* Function to move from temp into Array3 struct */
{
	int i;
	double *pointer;
	// I used MPI_SendRecv even though it was to itself as it was the nicest code and didn't seem to cause slow down.
	for (i=0; i<info->rcv_size1; i++)
	{
		pointer = find_start_dimension( info->dir, i, phi);
		MPI_Sendrecv(&temp[i*info->surf_size], info->surf_size, MPI_DOUBLE, phi->myid, 0, pointer, 1, surface, phi->myid, 0, grid, MPI_STATUS_IGNORE);
	}
}

void move_directly(Array3* phi, MPI_Datatype surface, MPI_Comm grid, ShiftInfo *info)
/* Function to move data from one phi position to another. */
{
	int i;
	
	// Work out what processors we're sending/receiving from
	int proc_rcvr, proc_sendr, tmp_proc;
	MPI_Cart_shift(grid, info->dir, info->rcv_dist+1, &tmp_proc, &proc_sendr);
	MPI_Cart_shift(grid, info->dir, info->send_dist, &tmp_proc, &proc_rcvr);

	// Try to use sendrecv calls as much as possible but if difference between #sends and #receives
	// we to just send or just recv.
	int count = info->send_size2;
	int diff = info->send_size2 - info->rcv_size2;
	double *pointer1, *pointer2;	
	if (diff>0)
	{
		count = info->rcv_size2;
	}

	for (i=1; i<count+1; i++)
	{
		pointer1 = find_start_dimension( info->dir, info->send_size2 - i, phi);
		pointer2 = find_start_dimension( info->dir, phi->local_dim[info->dir]-i, phi);
		MPI_Sendrecv(pointer1, 1, surface, proc_rcvr, 0, pointer2, 1, surface, proc_sendr, 0, grid, MPI_STATUS_IGNORE);
	}
	if (diff>0)
	{
		for (i=1;i<(diff+1);i++)
		{
			pointer1 = find_start_dimension( info->dir, info->send_size2-info->rcv_size2-i, phi);
			MPI_Send(pointer1, 1, surface, proc_rcvr, 0, grid);
		}
	}
	else if (diff<0)
	{
		for ( i=1; i<1-diff; i++)
		{
			pointer2 = find_start_dimension( info->dir, phi->local_dim[info->dir]-count- i, phi);
			MPI_Recv(pointer2,1, surface, proc_sendr, 0, grid, MPI_STATUS_IGNORE);
		}
	}
}



