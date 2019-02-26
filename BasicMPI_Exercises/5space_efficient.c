#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#include "mpi.h"

/* 
  5space_efficient.c -- program to calculate sum of squares and 
                        Euclidean norm of vector from input file.

  Author: John Cormican

  Purpouse: Use multiple processors to calculate Euclidean Norm in more efficient way.

  Usage: Compile using mpicc and run with mpiexec. First line of file 
         should be the length of the vector. 
*/

void get_length(FILE *fp, int *length);
double *create_pvector(FILE *fp, int length, int myid, int nprocs, int* s, int* e);
int decompid(int n, int nprocs, int myid, int *sp, int *ep);

int main(int argc, char *argv[])
{
  int myid,nprocs;
  char *filename = "vec16.txt";
  int i,length;
  double *p_vector;
  int s,e;
  double local_total = 0; 
  double total_total = 0; 
  int root = 0;
  FILE *fp;
  fp = fopen(filename, "r");

  //MPI initialized:
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &myid);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  
  // Root reads the vector and broadcasts the length
  if(myid==root)
  {
    get_length(fp, &length);
  }
  MPI_Bcast(&length,1, MPI_INT,  0, MPI_COMM_WORLD);
 
  p_vector = create_pvector(fp,length,myid,nprocs,&s,&e);
  for(i=0;i<=(e-s);i++)
  {
    local_total += p_vector[i]*p_vector[i];
  }

  // Sum of local sums brought to Root, square root found and printed.
  MPI_Reduce(&local_total, &total_total, 1, MPI_DOUBLE, MPI_SUM, root, MPI_COMM_WORLD);
  if(myid == root)
  {
    printf("Sum of Squares = %lf\n",total_total);
    total_total = sqrt(total_total);
    printf("Norm = %lf\n",total_total);
  }

  // malloc-ed variables freed:
  free(p_vector);

  // MPI finalized and program ends:
  MPI_Finalize();
  return 0;
}

void get_length(FILE *fp, int *length)
/* Reads a file containing a vector with the length of the vector at the start. 
Updates length to take length of vector and returns the array itself. */
{
  //Variables declared:
  char *buffer = NULL;
  size_t buffsize = 0;
  
  
  getline(&buffer, &buffsize,fp);
  *length = atoi(buffer);
}

double *create_pvector(FILE *fp, int length, int myid, int nprocs, int *s, int *e)
/* Reads a file containing a vector with the length of the vector at the start. 
Updates length to take length of vector and returns the array itself. */
{
  //Variables declared:
  char *buffer = NULL;
  size_t buffsize = 0;
  
  //As other processors have been reading it seemed safest to start at top of file
  rewind(fp);

  //This decompid gives
  decompid(length, nprocs, myid, s, e);
  
  //Vector Created and filled from file:
  double *array = malloc(sizeof(double)*(1+(*e)-(*s)));
  int i,j=0;
  for(i=0;i<(length+1);i++)
  {
    buffer = NULL;
    buffsize = 0;
    getline(&buffer,&buffsize,fp);
    if(i >= *s && i <= *e)
    { 
      array[j] = atof(buffer);
      j++;
    }
  }

  //Array returned:
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
    *sp = myid*partsize +1; 
    *ep = myid*partsize + partsize;
    return 0;
  }
  //These processes do not need to take extra piece.
  else
  {
    *sp = myid*partsize + extras +1;
    *ep = myid*partsize + extras + partsize;
    return 0;
  }
}
