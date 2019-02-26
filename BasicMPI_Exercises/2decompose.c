#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "mpi.h"


int n = 100000;

void get_length(FILE *fp, int *length);
int decompid(int n, int nprocs, int myid, int *s, int *e);

int main(int argc, char *argv[])
/*Basic MPI Code to demonstrate use of the decompid funtion.*/
{
  //Getopt used to change size of array, default size is 25.
  int c;
  while ((c = getopt (argc,argv,"s:")) != -1)
  {
    switch(c)
    {
      case 's':
	n = atoi(optarg);
        break;
      case '?':
	printf("Invalid Option\n");
	return 1;
    }
  }

  //Variables declared
  int nprocs,myid,i,s,e;
  int local_sum = 0;
  int array[n];

  //Array initialized
  for(i=0;i<n;i++)
  {
    array[i] = 1;
  }

  //MPI Initialized.
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &myid);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);


  //The decompid function is used:
  decompid(n,nprocs,myid,&s,&e);
  printf("(On %d) we start at %d and end at %d\n",myid,s,e);
  for (i=s;i<=e;i++)
  {
    local_sum += array[i];
  }
  printf("(On %d) we take a chunk of size %d\n",myid,local_sum);

  //MPI finalized and main ends.
  MPI_Finalize();
  return 0;
}


int decompid(int n, int nprocs, int myid, int *sp, int *ep)
/*Function take integer n for vector length, integer nprocs for the number
of processors used*/
/*This was used slightly dfferently here to in q5. In q5 decompid was used to
read directly from file start and end are greater by 1.*/
{
  // partsize is the number ALL processors must take.
  // extras is the number of processors that take an extra piece.
  int partsize = n/nprocs;
  int extras = n%nprocs;
   
  //These processors take an 1 extra piece.
  if (myid<extras)
  {
    partsize += 1;
    *sp = myid*partsize; 
    *ep = myid*partsize + partsize - 1;
    return 0;
  }
  //These processes do not need to take extra piece.
  else
  {
    *sp = myid*partsize + extras;
    *ep = myid*partsize + extras + partsize - 1;
    return 0;
  }
}
