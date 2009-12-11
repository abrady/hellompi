/***************************************************************************
 *     Copyright (c) 2009-2009, Aaron Brady
 *     All Rights Reserved
 *
 * Module Description:
 *  - MPI_Scatter, MPI_Gather
 *
 * The purpose of these functins is to chop up continguous data amoung
 * a group of tasks and gather the data back into a single buffer
 * after the tasks are done.
 ***************************************************************************/
#include "hellompi.h"
#include "mpi.h"
#include <stdio.h>
#include <string.h>

#define BUFSIZE 128
#define TAG 0
#define SIZE 4

int main(int argc, char **argv)
{
	int dest;
	int i;
	int numtasks, rank, sendcount, recvcount, source;
	float sendbuf[SIZE][SIZE] = {
		{1.0, 2.0, 3.0, 4.0},
		{5.0, 6.0, 7.0, 8.0},
		{9.0, 10.0, 11.0, 12.0},
		{13.0, 14.0, 15.0, 16.0}  };
	float recvbuf[SIZE];
	float gatherbuf[SIZE][SIZE] = {0};
	
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
	
	if (numtasks != SIZE)
	{
		printf("Must specify %d processors. Terminating.\n",SIZE);
		return -1;
	}
	
	source = 1;
	sendcount = SIZE;
	recvcount = SIZE;

	//----------------------------------------
	// scatter the source data:
 
	// task 1 is sending the buffer out to all tasks.  The SIZExSIZE
	// buffer is chpped up and sent to all tasks. All other tasks
	// ignore sendbuf and block until they receive this info.
	MPI_Scatter(sendbuf,sendcount,MPI_FLOAT,recvbuf,recvcount,
				MPI_FLOAT,source,MPI_COMM_WORLD);
	
	// each task now has a slice of the buffer
	// task 0: 1 2 3 4
	// task 1: 4 5 6 7
	printf("rank= %d  Results: %f %f %f %f\n",rank,recvbuf[0],
		   recvbuf[1],recvbuf[2],recvbuf[3]);

	// this is only for readability, printf can lag behind so the
	// outputs might be intrleaved.
	fflush(stdout);
	
	// ----------------------------------------
	// gather the results back to a different task

	// 'recvbuf' is what this task received from task 1, but it is being re-sent
	dest = 0;
	MPI_Gather(recvbuf,SIZE,MPI_FLOAT,gatherbuf,SIZE,MPI_FLOAT,dest,MPI_COMM_WORLD);

	if(rank == dest)
	{
		printf("gathering to %i\n:",rank);
		for(i = 0; i < SIZE; ++i)
			printf("rank= %i results %f %f %f %f\n",i, gatherbuf[i][0],gatherbuf[i][1],gatherbuf[i][2],gatherbuf[i][3]);
	}

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
