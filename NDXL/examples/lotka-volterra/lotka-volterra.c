#include <stdio.h>
#include <stdlib.h>

#include "../../NDXL/include/ndxl.h"

int lv(NDXLVertex* pops, double* dN, const double* N, double t, void* p)
{
    double alpha = ((double*)p)[0];
    double beta = ((double*)p)[1];
    double delta = ((double*)p)[2];
    double gamma = ((double*)p)[3];
    
    // Prey equation
    dN[0] = alpha * N[0] - beta * N[0] * N[1];
    
    // Predator equation
    dN[1] = delta * N[0] * N[1] - gamma * N[1];
    
    return 0;
}

int cb(NDXLNetwork* net, double time, void* params)
{
    FILE* csvfile = (FILE*)params;
	NDXLVertex* pops = ndxlGetNetworkFirstVertex(net);
    fprintf(csvfile, "%f,%f,%f\n", time, ndxlVertexState(pops)[0], ndxlVertexState(pops)[1]);
    return 0;
}

int main(int argc, char** argv)
{
	ndxlInit();
	
	FILE* f_output = fopen("lv.csv", "w");
    if (!f_output)
        return 1;
    fprintf(f_output, "t,Nprey,Npred\n");
	NDXLNetwork* nwk = ndxlCreateNetwork(f_output);

	double N0[2] = { 40.0, 9.0 };
    double ABDG[4] = { 1.0, 0.1, 0.075, 1.5 };
    ndxlCreateVertex(nwk, N0, 2, lv, NULL, NULL, 0, NULL, ABDG);
    
    // Simulate with the static Ralston method of fourth order
    ndxlNetworkSimulateRalston(
        nwk,
        20.0,
        0.1,    // Step size
        cb
    );

	return 0;
}