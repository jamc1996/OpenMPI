#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include "mpi.h"

/* 
  1-1vec_ed.c -- program to add one to all entries of a vector
                 and write editted vector to a file.

  Author: John Cormican

  Purpouse: Use multiple processors edit a vector.

  Usage: Compile using mpicc and run with mpiexec. By default runs on
         vec16.txt but takes optional argument -f: to read from different 
         file. First line of file should be the length of the vector. 
*/

double *create_vector(char *filename, int *length);
void send_parts(double *vector, int nprocs, int partsize, double *part);
void edit_part(double *part, int partsize);
void RecvWrite_part(double *part, int partsize, int nprocs, MPI_Status stat);

int main(int argc, char *argv[])
/*Reads a vector from a file and writes an editted vector to a new file 
using MPI to perform operation in parallel.*/
{
  //Variables declared for use across all processors.
  int myid,nprocs,partsize;
  char *filename = NULL;
  double *part;
  int c;

  //getopt used to take a different file if required.  
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
  if (filename == NULL)
  {
    filename = "vec16.txt";
  }
  
  //MPI initialized:
  MPI_Status stat;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &myid);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

  if(myid==0)
  { 
    //Processor 0 reads file and distributes to others.
    int length;
    double *vector = create_vector(filename,&length);
    partsize = length/nprocs;
    part = malloc(sizeof(double)*partsize);
    send_parts(vector,nprocs,partsize,part);
    
    //Processor 0 edits its own chunk of the vector.
    part = &vector[0];
    edit_part(part,partsize);

    //Editted parts gathered to processor 0 and written to file.
    RecvWrite_part(part,partsize,nprocs,stat);

    //Free dynamically allocated memory. Part is freed when vector freed.
    free(vector);
  }
  else
  {
    //Other processors receive partsize & part of vector:
    MPI_Recv(&partsize,1,MPI_INT,0,0,MPI_COMM_WORLD,&stat);
    part = malloc(sizeof(double)*partsize);
    MPI_Recv(part,partsize,MPI_DOUBLE,0,1,MPI_COMM_WORLD,&stat);  

    //Part editted and sent back to processor 0.
    edit_part(part,partsize);
    MPI_Send(part,partsize,MPI_DOUBLE, 0,2,MPI_COMM_WORLD);   
    free(part);
  }

  //MPI finalized and function ends.
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

void send_parts(double *vector, int nprocs,int partsize, double *part)
/*Evenly divides up vector among processors. (The layout looked tidier
with this in a function.)*/
{
  int i;

  //Partsize sent first so buffer created on each processor, then part sent.
  for(i=1;i<nprocs;i++)
  {
    MPI_Send(&partsize,1, MPI_INT, i, 0, MPI_COMM_WORLD);
    MPI_Send(&vector[i*partsize],partsize,MPI_DOUBLE, i,1,MPI_COMM_WORLD);
  } 
}

void edit_part(double *part, int partsize)
/*Takes array part of length partsize and adds 1 to each entry.*/
{
  int i;
  for (i=0;i<partsize;i++)
  {
    part[i]++;
  }
}

void RecvWrite_part(double *part, int partsize, int nprocs, MPI_Status stat)
/*Receives editted partial arrays back from*/
{
  //Output file opened & i,j declared:
  int i,j;
  FILE *fpp;
  fpp = fopen("output.txt","w");

  //Route processors own chunk is written to file:
  for (j=0;j<partsize;j++)
  {
    fprintf(fpp,"%lf\n",part[j]);
  }

  //Chunks from other processors received and written to file:
  for(i=1;i<nprocs;i++)
  {
    MPI_Recv(part,partsize, MPI_DOUBLE, i, 2, MPI_COMM_WORLD, &stat);
    for(j=0;j<partsize;j++)
    {
      fprintf(fpp,"%lf\n",part[j]);
    }
  }

  //Output file closed:
  fclose(fpp);
}










