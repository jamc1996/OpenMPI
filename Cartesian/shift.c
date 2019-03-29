#include "shift.h"

void search_back_direction(int pm, int dir, int *back_split, int* proc_dist, Array3* phi, int coord)
{
	int away = 0;
	int temp = pm;
	
	while(temp > 0)
	{
		away --;
		temp -= phi->part_size[dir];
		if( (phi->proc_dims[dir] + coord + away)%phi->proc_dims[dir] < phi->procs_extra[dir])
		{
			temp --;
		} 		
	}
	int final_partsize = phi->part_size[dir];
	if( (phi->proc_dims[dir] + coord + away)%phi->proc_dims[dir] < phi->procs_extra[dir])
	{
		if(temp+final_partsize < phi->local_dim[dir])
		{
			final_partsize++;
		}
	}
	*back_split = final_partsize + temp; 

	*proc_dist = away;
}

void search_forward_direction(int pm, int dir, int *split, int* proc_dist, Array3* phi, int coord)
{
	int away = -1;
	int temp = -pm;
	
	while(temp <= 0)
	{
		away += 1;
		temp += phi->part_size[dir];
		if( (coord + away)%phi->proc_dims[dir] < phi->procs_extra[dir])
		{
			temp ++;
		} 		
	}
	if (temp > phi->local_dim[dir] )
	{
		temp--;
	}
	
	*split = temp;
	*proc_dist = away;
}

void shift(int dir, int pm, MPI_Comm grid, Array3* phi)
{
	int coord[3];
	MPI_Cart_coords(grid, phi->myid, 3, coord);

	while(pm>phi->global_dim[dir])
	{
		pm-=phi->global_dim[dir];
	}
	if (pm==0)
	{
		return;
	}
	
	int rcv_split, rcv_dist;
	int send_split, send_dist;
	if (pm > 0)
	{

		search_back_direction(pm, dir, &rcv_split, &rcv_dist, phi, coord[dir]);
		search_forward_direction(pm, dir, &send_split, &send_dist, phi, coord[dir]);
	}
	else
	{
		search_back_direction(-pm, dir, &send_split, &send_dist, phi, coord[dir]);
		search_forward_direction(-pm, dir, &rcv_split, &rcv_dist, phi, coord[dir]);
	}
	
// 	CHANGE AROUND THE VALUES OF SEND/RECV IF DIFFERENT

	
	MPI_Datatype surface;
	double* temp;
	int proc1_rcvr, proc1_sendr;
	int proc2_rcvr, proc2_sendr;
	int tmp_proc;
	int surf_size;
	if (dir == 0)
	{
		surf_size = rcv_split*phi->local_dim[1]*phi->local_dim[2];
		if (rcv_split != 0)
		{
			temp = malloc(surf_size*sizeof(double));
		}		
		MPI_Type_vector(phi->local_dim[1], phi->local_dim[2], 0, MPI_DOUBLE, &surface);
	}

	else if(dir == 1)
	{
		surf_size = rcv_split*phi->local_dim[1]*phi->local_dim[2];
		if (rcv_split != 0)
		{
			temp = malloc(surf_size*sizeof(double));
		}	
		MPI_Type_vector(phi->local_dim[0], phi->local_dim[2], phi->local_dim[2]*phi->local_dim[1], MPI_DOUBLE, &surface);
	}

	else if (dir == 2)
	{
		surf_size = rcv_split*phi->local_dim[1]*phi->local_dim[2];		
		if (rcv_split != 0)
		{
			temp = malloc(surf_size*sizeof(double));
		}	
		MPI_Type_vector(phi->local_dim[0]*phi->local_dim[1], 1, phi->local_dim[2], MPI_DOUBLE, &surface);
	}

	else
	{
		printf("shift.c: invalid direction for 3d array.\n");
		return;
	}
	MPI_Type_commit(&surface);

	MPI_Cart_shift(grid, dir, rcv_dist, &proc1_rcvr, &tmp_proc);
	MPI_Cart_shift(grid, dir, send_dist+1, &proc1_sendr,&tmp_proc);

	int nsurfaces = phi->local_dim[dir] - send_split;
	int i;

	MPI_Sendrecv(&phi->data3d[send_split+0][0][0], 1 , surface, proc1_rcvr, 0, &temp[0*surf_size], surf_size, MPI_DOUBLE, proc1_sendr, 0, grid, MPI_STATUS_IGNORE);
	

	MPI_Cart_shift(grid, dir, rcv_dist+1, &proc2_rcvr, &tmp_proc);
	MPI_Cart_shift(grid, dir, send_dist, &proc2_sendr,&tmp_proc);


//printf("myid is %d and send_split is %d rec %d and %d is also %d \n",phi->myid,send_split,rcv_split,proc2_rcvr, proc2_sendr);
	
	//MPI_Sendrecv(&phi->data3d[0][0][0], send_split, surface, proc2_rcvr, 0, &phi->data3d[rcv_split][0][0], send_split, surface, proc2_sendr, 0, grid, MPI_STATUS_IGNORE);

	//for (i=0; i<rcv_split; i++)
	{
		//MPI_Sendrecv(&temp[i*surf_size], surf_size, MPI_DOUBLE, phi->myid, 0, &phi->data3d[i][0][0], 1, surface, phi->myid, 0, grid, MPI_STATUS_IGNORE);
	}

	free(temp);
}
