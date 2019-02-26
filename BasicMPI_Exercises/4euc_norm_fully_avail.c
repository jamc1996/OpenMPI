#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#include "mpi.h"

/* 
  4euc_norm_avail.c -- program to calculate sum of squares and Euclidean norm of
                       vector from input file and distribute answer across all processors.

  Author: John Cormican

  Purpouse: Use multiple processors to calculate Euclidean Norm and make
            answer available on all processors.

  Usage: Compile using mpicc and run with mpiexec. By default runs on
         vec16.txt but takes optional argument -f: to read from different 
         file. First line of file should be the length of the vector. 
*/

double *create_vector(char *filename, int *lp);
int decompid(int n, int nprocs, int myid, int *sp, int *ep);

int main(int argc, char *argv[])
{
  int myid,nprocs;
  char *filename = NULL;
  int i,length, *starts;
  double *vector;
  double *part;
  int s,e;
  double local_total = 0; 
  double total_total = 0; 
  int root = 0;
  int c;

  while ((c = getopt(argc,argv,"f:")) != -1)
  {
    switch(c)
    {
      case 'f':
	filename = optarg;
        break;
      case '?':
	printf("Invalid Option\n");
	return 1;
    }
  }

  if(filename == NULL)
  {
    filename = "vec16.txt";
  }

  //MPI initialized:
  MPI_Status stat;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &myid);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  
  // Root reads the vector and broadcasts the length
  if(myid==root)
  {
    vector = create_vector(filename,&length);
    starts = malloc(sizeof(int)*nprocs);
  }
  MPI_Bcast(&length,1, MPI_INT,  0, MPI_COMM_WORLD);
  
  // All processors calculate their chunk of vector using decompid
  decompid(length,nprocs,myid,&s,&e);
  part = malloc(sizeof(double)*(1+e-s));
  MPI_Gather(&s,1,MPI_INT,starts,1,MPI_INT,0,MPI_COMM_WORLD); 
  
  // Root distributes chunks of vector based on values gathered.
  if(myid == root)
  {
    for(i=0;i<nprocs-1;i++)
    {
      MPI_Send(&vector[starts[i]],starts[i+1]-starts[i],MPI_DOUBLE,i,1,MPI_COMM_WORLD);
    }
    MPI_Send(&vector[starts[nprocs-1]],length-starts[nprocs-1], MPI_DOUBLE,nprocs-1, 1, MPI_COMM_WORLD);
  }
  
  // All processors receive vector chunk and calculate their local sum.
  MPI_Recv(part,1+e-s, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, &stat);
  for(i=0;i<=(e-s);i++)
  {
    local_total += part[i]*part[i];
  }

  // Sum of local sums brought to Root, square root found and printed.
  MPI_Allreduce(&local_total, &total_total, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
  printf("(My id: %d) Sum of Squares = %lf\n",myid,total_total);
  total_total = sqrt(total_total);
  printf("(My id: %d) Norm = %lf\n",myid,total_total);

  //malloc-ed data is freed:
  if (myid == 0) 
  {
    free(starts);
    free(vector);
  }
  free(part);

  // MPI finalized and program ends:
  MPI_Finalize();
  return 0;
}

double *create_vector(char *filename, int *length)
/* Reads a file containing a vector with the length of the vector at the start. 
Updates length to take length of vector and returns the array itself. */
{
  //Variables declared:
  FILE *fp;
  fp = fopen(filename, "r");
  char *buffer = NULL;
  size_t buffsize = 0;

  //Using pointerm vector length defined within function:
  getline(&buffer, &buffsize,fp);
  *length = atoi(buffer);

  //Vector Created:
  double *array = malloc(sizeof(double)*(*length));
  int i;
  for(i=0;i<(*length);i++)
  {
    buffer = NULL;
    buffsize = 0;
    getline(&buffer,&buffsize,fp);
    array[i] = atof(buffer);
  }

  //File closed and array returned:
  free(buffer);
  fclose(fp);
  return array;
}


int decompid(int n, int nprocs, int myid, int *sp, int *ep)
/*Function take integer n for vector length, integer nprocs for the number
of processors used*/
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
