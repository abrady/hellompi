/***************************************************************************
 *     Copyright (c) 2009-2009, Aaron Brady
 *     All Rights Reserved
 *
 * Module Description:
 *  very basic hello world MPI program. sends and recvs are 'synchronous'
 *
 ***************************************************************************/
#include "hellompi.h"
#include "mpi.h"
#include <stdio.h>
#include <string.h>

#define BUFSIZE 128
#define TAG 0
 
int main(int argc, char *argv[])
{
	char idstr[32];
	char buff[BUFSIZE];
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
		MPI_Get_processor_name(procname,&n);
		printf("processor name %s\n",procname);
		printf("%d: We have %d processors\n", myid, numprocs);
		for(i=1;i<numprocs;i++)
		{
			sprintf(buff, "Hello %d! ", i);
			MPI_Send(buff, BUFSIZE, MPI_CHAR, i, TAG, MPI_COMM_WORLD);
		}
		for(i=1;i<numprocs;i++)
		{
			MPI_Recv(buff, BUFSIZE, MPI_CHAR, i, TAG, MPI_COMM_WORLD, &stat);
			printf("%d: %s\n", myid, buff);
		}
	}
	else
	{
		/* receive from rank 0: */
		MPI_Recv(buff, BUFSIZE, MPI_CHAR, 0, TAG, MPI_COMM_WORLD, &stat);
		sprintf(idstr, "Processor %d ", myid);
		strcat(buff, idstr);
		strcat(buff, "reporting for duty");
		/* send to rank 0: */
		MPI_Send(buff, BUFSIZE, MPI_CHAR, 0, TAG, MPI_COMM_WORLD);
	}
 
	MPI_Finalize(); /* MPI Programs end with MPI Finalize; this is a weak synchronization point */
	return 0;
}

