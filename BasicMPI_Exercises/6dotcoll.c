#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>

#include "mpi.h"

/* 
  6dotcoll.c -- program to dot product of 2 vectors using collective calls. 

  Author: Darach Golden's code edited by John Cormican

  Purpouse: Use multiple processors to calculate dot product.

  Usage: Compile using mpicc and run with mpiexec. 
*/


#define N 1000000

int main(int argc, char *argv[]) {
     double *A, *B;
     double *pA, *pB;
     double sdotprod, pdotprod;
     struct timeval t1, t2;
     int myid, nprocs;
     int partsize;
     int i;

     MPI_Init(&argc, &argv);
     MPI_Comm_rank(MPI_COMM_WORLD, &myid);
     MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

     partsize = N/nprocs;
     srand48(1234+10*myid);

     A = malloc(N * sizeof(double));
     B = malloc(N * sizeof(double));
     for(i = 0; i<N; i++){
	  /* A[i] = 1.0; */
	  /* B[i] = 1.0; */
	  A[i] = drand48();
	  B[i] = drand48();
     }

     gettimeofday(&t1, NULL);
     sdotprod = 0.0;
     for(i=0; i < N; i++){
	  sdotprod = sdotprod + A[i]*B[i];
     }
     gettimeofday(&t2, NULL);
     printf("(myid %d) time take for sequential: %lf (dotprod: %lf)\n", myid,
	    t2.tv_sec - t1.tv_sec +(t2.tv_usec - t1.tv_usec)*1.0E-06, sdotprod);

     /* PARALLEL VERSION */
     MPI_Barrier(MPI_COMM_WORLD);

     pA = malloc(partsize * sizeof(double));
     pB = malloc(partsize * sizeof(double));


     MPI_Scatter(&A[myid*partsize],partsize,MPI_DOUBLE,pA,partsize,MPI_DOUBLE,0,MPI_COMM_WORLD); 
     MPI_Scatter(&B[myid*partsize],partsize,MPI_DOUBLE,pB,partsize,MPI_DOUBLE,0,MPI_COMM_WORLD); 
     

     double localsum = 0.0;
     for(i=0; i < partsize; i++){
	  localsum = localsum + pA[i]*pB[i];
     }
     printf("(myid %d) localsum: %lf\n",myid, localsum);
     
     /* everyone send their part of the answer back to rank 0 */
     MPI_Reduce(&localsum, &pdotprod, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
     printf("(myid: %d)  (dp value: %lf)\n", myid, pdotprod);

     /* if( fabs(pdotprod - sdotprod) > 0.1 ){ */
     /* 	 fprintf(stderr,"Error? parallel dot prod: %lf; serial dot prod: %lf\n", */
     /* 		 pdotprod,sdotprod); */
     /* 	 exit(11); */
     /* } */


     free(A);
     free(B);
     free(pA);
     free(pB);

     MPI_Finalize();

     return 0;
}
