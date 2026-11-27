#ifndef _CORENDXL_DEFS_
#define _CORENDXL_DEFS_

#include <float.h>	// DBL_EPSILON
#include <stdio.h>	// printf(), stdout, stderr
#include <stdlib.h>	// calloc(), free(), malloc(), rand(), realloc(), srand()
#include <string.h>	// memcpy(), memset()
#include <time.h>	// struct timespec, clock_gettime()

#include "../include/unicpp/unicpp.h"	// complex_t, TWO_PI

#ifdef NDXL_STATIC
	#define _NDXLEXP_
#else
	#ifdef _MSC_VER
		#define _NDXLEXP_ __declspec(dllexport)
	#else
		#define _NDXLEXP_ __attribute__((visibility("default")))
	#endif // MSVC
#endif // NDXL_STATIC

// Internal macros vvvvv

#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#define EPS		__ndxlSqrtEpsilon
#define ITERMAX	5

#endif // _CORENDXL_DEFS_