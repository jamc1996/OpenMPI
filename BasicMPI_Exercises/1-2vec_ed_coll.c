#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include "mpi.h"

/* 
  1-2vec_ed_coll.c -- program to add one to all entries of a vector
                      and write edited vector to a file.

  Author: John Cormican

  Purpouse: Use multiple processors edit a vector using collective calls.

  Usage: Compile using mpicc and run with mpiexec. By default runs on
         vec16.txt but takes optional argument -f: to read from different 
         file. First line of file should be the length of the vector. 
*/


double *create_vector(char *filename, int *length);
void edit_part(double *part, int partsize);
void write_gathered(double *vector, int length);

int main(int argc, char *argv[])
/*Reads a vector from a file and writes an editted vector to a new file 
using MPI to perform operation in parallel.*/
{
  //Variables declared for use across all processors
  int myid,nprocs,partsize,length;
  char *filename = NULL;
  double *vector, *part;
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
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &myid);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);


  if(myid==0)
  { 
    //Processor 0 reads file.
    vector = create_vector(filename,&length);
    partsize = length/nprocs;
  }
  
  //Information sent using collective calls:
  MPI_Bcast(&partsize,1, MPI_INT,  0, MPI_COMM_WORLD);
  part = malloc(sizeof(double)*partsize);
  MPI_Scatter(&vector[myid*partsize],partsize,MPI_DOUBLE,part,partsize,MPI_DOUBLE,0,MPI_COMM_WORLD); 
  
  //Every node including root edits their chunk:
  edit_part(part,partsize);

  //Information gathered back into vector:
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Gather(part,partsize,MPI_DOUBLE,vector,partsize,MPI_DOUBLE,0,MPI_COMM_WORLD);   

  //Root writes vector to output file and frees vector:
  if (myid==0)
  {
    write_gathered(vector,length);
    free(vector);
  }
  free(part);
  //MPI finalized and function ends:
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


void edit_part(double *part, int partsize)
/*Takes array part of length partsize and adds 1 to each entry.*/
{
  int i;
  for (i=0;i<partsize;i++)
  {
    part[i]++;
  }
}

void write_gathered(double *vector, int length)
/*Receives edited partial arrays back from*/
{
  //Output file opened and i declared:
  int i;
  FILE *fpp;
  fpp = fopen("collective_output.txt","w");

  for(i=0;i<length;i++)
  {
    fprintf(fpp,"%lf\n",vector[i]);
  }

  //Output file closed:
  fclose(fpp);
}

