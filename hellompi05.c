/***************************************************************************
 *     Copyright (c) 2009-2009, Aaron Brady
 *     All Rights Reserved
 *
 * Module Description:
 *   This module demonstrates how to create new groups of processes and do 
 *   processing within that group.
 ***************************************************************************/
#include "hellompi.h"
#include "mpi.h"
#include <stdio.h>
#include <string.h>

#define NPROCS 8

int main(int argc, char **argv)
{
	int        rank, new_rank, sendbuf, recvbuf, numtasks,
		ranks1[4]={0,1,2,3}, ranks2[4]={4,5,6,7};
	MPI_Group  orig_group, new_group;
	MPI_Comm   new_comm;
	
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
	
	if (numtasks != NPROCS) {
		printf("Must specify MP_PROCS= %d. Terminating.\n",NPROCS);
		MPI_Finalize();
		exit(0);
	}
	
	sendbuf = rank;
	
/* Extract the original group handle */
	MPI_Comm_group(MPI_COMM_WORLD, &orig_group);
	
/* Divide tasks into two distinct groups based upon rank */
	if (rank < NPROCS/2) {
		MPI_Group_incl(orig_group, NPROCS/2, ranks1, &new_group);
	}
	else {
		MPI_Group_incl(orig_group, NPROCS/2, ranks2, &new_group);
	}
	
/* Create new new communicator and then perform collective communications */
	MPI_Comm_create(MPI_COMM_WORLD, new_group, &new_comm);
	MPI_Allreduce(&sendbuf, &recvbuf, 1, MPI_INT, MPI_SUM, new_comm);
	
	MPI_Group_rank (new_group, &new_rank);
	printf("rank= %d newrank= %d recvbuf= %d\n",rank,new_rank,recvbuf);
	
	MPI_Finalize();
}



/*
MPI_Errhandler_set(MPI_COMM_WORLD, MPI_ERRORS_RETURN);
error_code = MPI_Send(send_buffer, strlen(send_buffer) + 1, MPI_CHAR,
                      addressee, tag, MPI_COMM_WORLD);
if (error_code != MPI_SUCCESS) {

   char error_string[BUFSIZ];
   int length_of_error_string;

   MPI_Error_string(error_code, error_string, &length_of_error_string);
   fprintf(stderr, "%3d: %s\n", my_rank, error_string);
   send_error = TRUE;
}
*/
