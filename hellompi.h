/***************************************************************************
 *     Copyright (c) 2009-2009, Aaron Brady
 *     All Rights Reserved
 *
 * Module Description:
 *
 *
 ***************************************************************************/
#ifndef HELLOMPI_H
#define HELLOMPI_H


#define WIN32_LEAN_AND_MEAN 
#define _WIN32_WINNT 	0x0501
#include <windows.h>


#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <time.h>

#pragma warning(disable:4996)

#define DIMOF(A) (sizeof(A)/sizeof(*(A)))
#define INRANGE(N,S,E) (((N)>=(S)) && ((N)<E))
#define INRANGE0(N,E) INRANGE(N,0,E)


#endif //HELLOMPI_H
