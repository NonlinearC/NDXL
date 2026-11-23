#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../include/ndxl.h"

double randdbl(double low, double high)
{
	static struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	srand(ts.tv_nsec ^ ts.tv_sec);
	return low + (high - low) * (double)rand() / (double)RAND_MAX;
}

int drift(NDXLVertex* particle, double* dx, const double* x, double t, void* p)
{
	double Gamma = ((double*)p)[0];
	dx[0] = -Gamma * x[0];
	return 0;
}

int diffusion(NDXLVertex* particle, double* dg, const double* x, double t, void* p)
{
	double sigma = ((double*)p)[1];
	dg[0] = sigma;
	return 0;
}

int cb(NDXLNetwork* nwk, double t, void* p)
{
	// Again floating point accumulation error might make t % 2 not exactly 0 at a time stop, so we compare to some arbitrarily small float value
	if (fabs(fmod(t, 2.0)) < 1e-12)
	{
		FILE* handle = (FILE*)p;
		fprintf(handle, "%f", t);
		for (NDXLVertex* body = ndxlGetNetworkFirstVertex(nwk); body; body = ndxlGetNetworkNextVertex(body))
			fprintf(handle, ",%f", ndxlVertexState(body)[0]);
		fprintf(handle, "\n");
	}
	return 0;
}

int main(int argc, char** argv)
{
	ndxlInit();
	
	// Initialise custom params struct
	FILE* fout = fopen("ou.csv", "w");
	if (!fout)
		return 1;
	fprintf(fout, "t");
	
	// Assign params to network
	NDXLNetwork* nwk = ndxlCreateNetwork(fout);
	
	// Define Gamma, sigma, and initial position
	double Gs[2] = { 1.5, 1.0 };
	double x0 = -1.0;
	
	// Create 1000 particles all starting at x0
	for (int i = 0; i < 1000; i++)
	{
		ndxlCreateVertex(
			nwk,
			&x0,
			1,
			drift,		// Dynamics
			diffusion,	// Noise
			NULL,		// We did not create any channels, so we leave this as NULL to specify "all channels must be new and independent"
			1,			// Each particle is driven by only one Wiener process (channel)
			NULL,
			Gs
		);
		fprintf(fout, ",x%d", i);
	}
	fprintf(fout, "\n");
	
	double tstops[6] = { 2.0, 4.0, 6.0, 8.0, 10.0, 12.0 };
	ndxlNetworkSimulateHeun(nwk, 14.0, 1e-3, 1e-3, tstops, 6, cb);
	
	fclose(fout);
	return 0;
}