/***************************************************************************
 *     Copyright (c) 2009-2009, Aaron Brady
 *     All Rights Reserved
 *
 * Module Description:
 *  an example of using MPI_Scan to perform a simple prefix-sum operation.
 * 
 *  Given a list of numbers it returns the sum of each element in the list with its previous
 *  elements. 
 *
 * More about prefix sum:
 * "The prefix sum (also known as the scan, prefix reduction, or partial sum) is an operation on lists in which each element in the result list is obtained from the sum of the elements in the operand list up to its index." [http://en.wikipedia.org/wiki/Prefix_sum]
 *
 ***************************************************************************/
#include "hellompi.h"
#include "mpi.h"
#include <stdio.h>
#include <string.h>
 
int main(int argc, char *argv[])
{
	int send[] = {1,2,3,4,5,6,7,8,9,10};
	int recv[DIMOF(send)];
	int numprocs;
	int myid;
	int i,n,s;
	int sum, answer;
 
	MPI_Init(&argc,&argv); /* all MPI programs start with MPI_Init; all 'N' processes exist thereafter */
	MPI_Comm_size(MPI_COMM_WORLD,&numprocs); /* find out how big the SPMD world is */
	MPI_Comm_rank(MPI_COMM_WORLD,&myid); /* and this processes' rank is */

	if(myid == 0)
	{
		printf("summing: {");
		for(i = 0; i<DIMOF(send); ++i)
			printf("%i,",send[i]);
		printf("}\n");
	}
	

	// figure out which part to work on
	s = (DIMOF(send)+numprocs - 1)/numprocs;
	i = s*myid;
	n = i + s;
	if(n > DIMOF(send))
		n = DIMOF(send);

	// do the work
	sum = 0;
//	printf("processor %2d: summing from %i to %i: {", myid, i, n-1);
	for(; i<n; ++i)
	{
		sum += send[i];
//		printf("%i,",send[i]);
	}
//	printf("}\n");
		
	MPI_Scan(&sum,&answer,1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
	printf("processor %2d: %d", myid, answer);

	MPI_Finalize(); /* MPI Programs end with MPI Finalize; this is a weak synchronization point */
	return 0;
}





















