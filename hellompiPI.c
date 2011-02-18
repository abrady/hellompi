/***************************************************************************
 *     Copyright (c) 2009-2009, Aaron Brady
 *     All Rights Reserved
 *
 * Module Description:
 *  - MPI_Scatter, MPI_Gather
 *
 * Find a given string as a bit pattern in PI.
 * (this is a little cheesy. it just chops off each 32 bit block and converts it into an a letter from a-z, or just 0 for the extras).
 *
 ***************************************************************************/
#include "hellompi.h"
#include "mpi.h"
#include <stdio.h>
#include <string.h>

char pi_main(int id);

int main(int argc, char **argv)
{
	int num_matches;
	char *s;
	
	int dest;
	int i;
	int numtasks, rank, sendcount, source;
	int *sendbuf;
	int recvbuf[2];
	int *gatherbuf;
	int range = 20000; // the highest location in PI to look
	char *match = "abc";
	int slice;
	
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);	

	// if(rank == 0)
	// {
	// 	fprintf(stderr,"debugging...");
	// 	WAIT_FOR_DEBUGGER();
	// 	fprintf(stderr,"attached\n");
	// }
	

	if(argc > 1)
		match = argv[1];

	source = 0;

	// send two pieces of info to each task: start, end
	sendbuf = calloc(sizeof(*sendbuf)*2,numtasks);
	sendcount = 2*numtasks;

	if(!sendbuf)
		return -1;

	slice = (range+numtasks-1)/numtasks; // 
	for(i = 0; i<numtasks; ++i)
	{
		sendbuf[2*i+0] = slice*i;
		sendbuf[2*i+1] = slice*(i+1);
	}

	//----------------------------------------
	// scatter the source data:

	if(rank == source)
		printf("looking for %s in %i digits of PI\n\n",match,range);
	fflush(stdout);

	// task 1 is sending the buffer out to all tasks.  The send 
	// buffer is chopped up and sent to all tasks. All other tasks
	// ignore sendbuf and block until they receive this info.
	MPI_Scatter(sendbuf,2,MPI_INT,recvbuf,2,MPI_INT,source,MPI_COMM_WORLD);
	
	s = match;
	num_matches = 0;
	for(i = recvbuf[0]; i<recvbuf[1] ; ++i)
	{
		char c = pi_main(i);
		
		if(*s == c)
		{	
			s++;
			if(!s[1])
				num_matches++;
		}
		s = match;
	}

	
	// each task now has a slice of the buffer
	// task 0: 1 2 3 4
	// task 1: 4 5 6 7
	printf("rank= %d  Result: %i matches in range %i to %i\n", rank, num_matches, recvbuf[0], recvbuf[1]);

	// this is only for readability, printf can lag behind so the
	// outputs might be intrleaved.
	fflush(stdout);
	
	// ----------------------------------------
	// gather the results back to a different task


	dest = 0;
	if(rank == dest)
		gatherbuf = calloc(sizeof(*gatherbuf),numtasks);
	else
		gatherbuf = NULL;
	
	MPI_Gather(&num_matches,1,MPI_INT,gatherbuf,1,MPI_INT,dest,MPI_COMM_WORLD);

	if(rank == dest)
	{
		int total = 0;
		printf("gathered to %i\n:",rank);
		for(i = 0; i < numtasks; ++i)
		{
			printf("rank= %i: %i results in range %i %i\n",i, gatherbuf[i], sendbuf[i*2+0], sendbuf[i*2+1]);
			total += gatherbuf[i];
		}
		printf("total matches: %i\n",total);
	}

	MPI_Finalize();
}



/*  
    This program implements the BBP algorithm to generate a few hexadecimal
    digits beginning immediately after a given position id, or in other words
    beginning at position id + 1.  On most systems using IEEE 64-bit floating-
    point arithmetic, this code works correctly so long as d is less than
    approximately 1.18 x 10^7.  If 80-bit arithmetic can be employed, this limit
    is significantly higher.  Whatever arithmetic is used, results for a given
    position id can be checked by repeating with id-1 or id+1, and verifying 
    that the hex digits perfectly overlap with an offset of one, except possibly
    for a few trailing digits.  The resulting fractions are typically accurate 
    to at least 11 decimal digits, and to at least 9 hex digits.  
*/

/*  David H. Bailey     2006-09-08 */

#include <stdio.h>
#include <math.h>

#pragma warning(disable:4210) // nonstandard function given file scope

char pi_main(int id)
{
  static char lookup[32] = "abcdefghijklmnopqrstuvwxyz:;,_-.";
  double pid, s1, s2, s3, s4;
  double series (int m, int n);
  double y,z,w;
  void ihex (double x, int m, char c[]);
  int tmp;
  
//  id is the digit position.  Digits generated follow immediately after id. 

  s1 = series (1, id);
  s2 = series (4, id);
  s3 = series (5, id);
  s4 = series (6, id);
  pid = 4. * s1 - 2. * s2 - s3 - s4;
  pid = pid - (int) pid + 1.;
  
  z = fabs (pid);
  w = (z - floor(z));
  y = 32. * w;
  tmp = (int)y;
  return lookup[tmp];
}

/*main()
{
  double pid, s1, s2, s3, s4;
  double series (int m, int n);
  void ihex (double x, int m, char c[]);
  int id = 1000000;
#define NHX 16
  char chx[NHX];

//  id is the digit position.  Digits generated follow immediately after id. 

  s1 = series (1, id);
  s2 = series (4, id);
  s3 = series (5, id);
  s4 = series (6, id);
  pid = 4. * s1 - 2. * s2 - s3 - s4;
  pid = pid - (int) pid + 1.;
  ihex (pid, NHX, chx);
  printf (" position = %i\n fraction = %.15f \n hex digits =  %10.10s\n",
  id, pid, chx);
  }*/

void ihex (double x, int nhx, char chx[])

/*  This returns, in chx, the first nhx hex digits of the fraction of x. */

{
  int i;
  double y;
  char hx[] = "0123456789ABCDEF";

  y = fabs (x);

  for (i = 0; i < nhx; i++){
    y = 16. * (y - floor (y));
    chx[i] = hx[(int) y];
  }
}

double series (int m, int id)

/*  This routine evaluates the series  sum_k 16^(id-k)/(8*k+m) 
    using the modular exponentiation technique. */

{
  int k;
  double ak, p, s, t;
  double expm (double x, double y);
#define eps 1e-17

  s = 0.;

/*  Sum the series up to id. */

  for (k = 0; k < id; k++){
    ak = 8 * k + m;
    p = id - k;
    t = expm (p, ak);
    s = s + t / ak;
    s = s - (int) s;
  }

/*  Compute a few terms where k >= id. */

  for (k = id; k <= id + 100; k++){
    ak = 8 * k + m;
    t = pow (16., (double) (id - k)) / ak;
    if (t < eps) break;
    s = s + t;
    s = s - (int) s;
  }
  return s;
}

double expm (double p, double ak)

/*  expm = 16^p mod ak.  This routine uses the left-to-right binary 
    exponentiation scheme. */

{
  int i, j;
  double p1, pt, r;
#define ntp 25
  static double tp[ntp];
  static int tp1 = 0;

/*  If this is the first call to expm, fill the power of two table tp. */

  if (tp1 == 0) {
    tp1 = 1;
    tp[0] = 1.;

    for (i = 1; i < ntp; i++) tp[i] = 2. * tp[i-1];
  }

  if (ak == 1.) return 0.;

/*  Find the greatest power of two less than or equal to p. */

  for (i = 0; i < ntp; i++) if (tp[i] > p) break;

  pt = tp[i-1];
  p1 = p;
  r = 1.;

/*  Perform binary exponentiation algorithm modulo ak. */

  for (j = 1; j <= i; j++){
    if (p1 >= pt){
      r = 16. * r;
      r = r - (int) (r / ak) * ak;
      p1 = p1 - pt;
    }
    pt = 0.5 * pt;
    if (pt >= 1.){
      r = r * r;
      r = r - (int) (r / ak) * ak;
    }
  }

  return r;
}
