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

int dynamics(NDXLVertex* vertex, double* dx, const double* x, double t, void* params)
{
    double A = *(double*)params;
    dx[0] = A * x[0] * t;
    return 0;
}

int cb(NDXLNetwork* net, double time, void* params)
{
    FILE* csvfile = (FILE*)params;
    fprintf(csvfile, "%f", time);
    
    // Iterate over vertices using network vertex iterators
    for (NDXLVertex* vertex = ndxlGetNetworkFirstVertex(net); vertex; vertex = ndxlGetNetworkNextVertex(vertex))
    {
        // Get state of current vertex and write it out to the file, separated by a comma
        double x = ndxlVertexState(vertex)[0];
        fprintf(csvfile, ",%f", x);
    }
    
    // Remember to append the newline character to the line we just wrote
    fprintf(csvfile, "\n");
    return 0;
}

int main(int argc, char** argv)
{
	ndxlInit();

	// Modify the CSV file to capture outputs for three vertices
    FILE* f_output = fopen("states3.csv", "w");
    if (!f_output)
        return 1;
    fprintf(f_output, "t,x1,x2,x3\n");
	NDXLNetwork* nwk = ndxlCreateNetwork(f_output);
    
    // Dedicate space for three vertices and their `void* params` pointers
    NDXLVertex* vertices[3];
    double A[3] = { 2.0, 0.5, 1.75 };
    
    // Create vertices in a loop
    for (int i = 0; i < 3; i++)
    {
        // State is copied so x0 does not need to stay in memory, unlike `A`
        double x0 = randdbl(0.5, 1.5);
        vertices[i] = ndxlCreateVertex(nwk, &x0, 1, dynamics, NULL, NULL, 0, NULL, &A[i]);
    }
	
	int result = ndxlNetworkSimulateDopri(nwk, 2.5, 1e-6, 1e-3, NULL, 0, cb);
    if (result == NDXL_ERR)
        ndxlPrintError();

	fclose(f_output);
	return 0;
}