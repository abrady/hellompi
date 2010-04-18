/***************************************************************************
 *     Copyright (c) 2009-2009, Aaron Brady
 *     All Rights Reserved
 *
 * Module Description:
 *  just like 01, but async.
 *
 ***************************************************************************/
#include "hellompi.h"
#include "assert.h"
#include "mpi.h"
#include <stdio.h>
#include <string.h>

#define BUFSIZE 128
#define TAG 0


 
int main(int argc, char *argv[])
{
	char idstr[32];
	int numprocs;
	int myid;
	int i;
	MPI_Status stat; 
 
	MPI_Init(&argc,&argv); /* all MPI programs start with MPI_Init; all 'N' processes exist thereafter */
	MPI_Comm_size(MPI_COMM_WORLD,&numprocs); /* find out how big the SPMD world is */
	MPI_Comm_rank(MPI_COMM_WORLD,&myid); /* and this processes' rank is */
 
	/* At this point, all the programs are running equivalently, the rank is used to
	   distinguish the roles of the programs in the SPMD model, with rank 0 often used
	   specially... */
	if(myid == 0)
	{
		char procname[MPI_MAX_PROCESSOR_NAME+1] = "";
		int n;
		MPI_Request *sendreqs = calloc(sizeof(*sendreqs),numprocs);
		MPI_Request *recvreqs = calloc(sizeof(*recvreqs),numprocs);
		char bufsend[32][BUFSIZE] = {0};
		char bufrecv[32][BUFSIZE] = {0};
		
		assert(numprocs < DIMOF(bufsend));

		MPI_Get_processor_name(procname,&n);
		printf("processor name %s\n",procname);
		printf("%d: We have %d processors\n", myid, numprocs);
		for(i=1;i<numprocs;i++)
		{
			sprintf(bufsend[i-1], "Hello %d! ", i);
			// send some data and prep a recv buffer for handling responses
			MPI_Irecv(bufrecv[i-1], BUFSIZE, MPI_CHAR, i, TAG, MPI_COMM_WORLD, recvreqs+i-1);
			MPI_Isend(bufsend[i-1], BUFSIZE, MPI_CHAR, i, TAG, MPI_COMM_WORLD, sendreqs+i-1);
		}

		n = numprocs - 1;
		while(n>0)
		{
			int index = 0;
			int flag = 0;
			MPI_Status status = {0}; 
			int error_code;

			error_code = MPI_Testany(numprocs-1,recvreqs,&index,&flag,&status); 
			if(!flag) // true if any answers returned
				continue;
			if(index == MPI_UNDEFINED)
			{
				fprintf(stderr,"undefined error, probably out of outstanding requests.\n");
				break;
			}
			
			if(!INRANGE0(index,DIMOF(bufrecv)))
			{
				fprintf(stderr,"unusable index %i returned, aborting\n",index);
				break;
			}

			printf("->%d: %s\n", index+1, bufrecv[index]);
			n--;
		}

		free(sendreqs);
		free(recvreqs);
	}
	else
	{
		char buff[BUFSIZE];
		
		/* receive from rank 0: */
		MPI_Recv(buff, BUFSIZE, MPI_CHAR, 0, TAG, MPI_COMM_WORLD, &stat);
		sprintf(idstr, "Processor %d", myid);
		strcat(buff, idstr);
		strcat(buff, "reporting for duty");
		/* send to rank 0: */
		MPI_Send(buff, BUFSIZE, MPI_CHAR, 0, TAG, MPI_COMM_WORLD);
	}
 
	MPI_Finalize(); /* MPI Programs end with MPI Finalize; this is a weak synchronization point */
	return 0;
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
