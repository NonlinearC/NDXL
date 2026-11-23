#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../../NDXL/include/ndxl.h"

double randdbl(double low, double high)
{
	static struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	srand(ts.tv_nsec ^ ts.tv_sec);
	return low + (high - low) * (double)rand() / (double)RAND_MAX;
}

int dynamics(NDXLVertex* vert, double* dx, const double* x, double t, void* p)
{
	dx[0] = x[0] * t;
	return 0;
}

int callback(NDXLVertex* vert, double* x, double t, void* p)
{
	// Extract limit and ID from `void* params`
	double limit = ((double*)p)[0];
	int id = (int)(((double*)p)[1]);
	
	
	if (x[0] >= limit)
	{
		printf("Vertex %d destroyed at t = %f\n", id, t);
		ndxlDestroyVertex(&vert);
	}
	
	// Importantly, we do not operate on `vert` after it has been destroyed
	return 0;
}

int main(int argc, char** argv)
{
	ndxlInit();
	NDXLNetwork* nwk = ndxlCreateNetwork(NULL);
	
	// Prepare initial state and space for each vertex's `void* params`
	double x0 = 1.0;
	double ulim[20];
	
	// Create vertices
	for (int i = 0; i < 10; i++)
	{
		ulim[2 * i] = randdbl(0.5, 7.0);
		ulim[2 * i + 1] = i;
		ndxlCreateVertex(nwk, &x0, 1, dynamics, NULL, NULL, 0, callback, ulim + 2 * i);
	}
	
	ndxlNetworkSimulateRalston(nwk, 2.0, 1e-3, NULL);
	printf("After simulation, network has %d vertices left\n", ndxlGetNetworkOrder(nwk));

	return 0;
}