/***************************************************************************
 *     Copyright (c) 2009-2009, Aaron Brady
 *     All Rights Reserved
 *
 * Module Description:
 *   This module demonstrates how to serialize a structure composed of the
 *   basic MPI types using the MPI_Type_ functions
 ***************************************************************************/
#include "hellompi.h"
#include "mpi.h"
#include <stdio.h>
#include <string.h>

#define NELEM 32 // send 4 structs to each task

int main(int argc, char **argv)
{
	int nsets;
	int numtasks, rank, i;
	
	typedef struct {
		float x, y, z;
		float velocity;
		int  n, type;
	}          Particle;
	Particle     p[NELEM], particles[NELEM];
	MPI_Datatype particletype, oldtypes[2]; 
	int          blockcounts[2];
	
/* MPI_Aint type used to be consistent with syntax of */
/* MPI_Type_extent routine */
	MPI_Aint    offsets[2], extent;
	
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
	
/* Setup description of the 4 MPI_FLOAT fields x, y, z, velocity */
	offsets[0] = 0;
	oldtypes[0] = MPI_FLOAT;
	blockcounts[0] = 4;
	
/* Setup description of the 2 MPI_INT fields n, type */
/* Need to first figure offset by getting size of MPI_FLOAT */
	MPI_Type_extent(MPI_FLOAT, &extent);
	offsets[1] = 4 * extent;
	oldtypes[1] = MPI_INT;
	blockcounts[1] = 2;
	
/* Now define structured type and commit it */
	MPI_Type_struct(2, blockcounts, offsets, oldtypes, &particletype);
	MPI_Type_commit(&particletype);
	
/* Initialize the particle array and then send it to each task */
	if (rank == 0) 
	{
		for (i=0; i<NELEM; i++) {
			particles[i].x = i * 1.0f;
			particles[i].y = i * -1.0f;
			particles[i].z = i * 1.0f; 
			particles[i].velocity = 0.25f;
			particles[i].n = i;
			particles[i].type = i % 2; 
		}
	}

	nsets = (NELEM+numtasks-1)/numtasks;
	MPI_Scatter(particles,nsets,particletype,p,nsets,
				particletype,0,MPI_COMM_WORLD);
	printf("rank= %d   %6.2f %6.2f %6.2f %6.2f %2d %2d\n", rank,p[3].x,
			   p[3].y,p[3].z,p[3].velocity,p[3].n,p[3].type);
	
	MPI_Type_free(&particletype);
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
