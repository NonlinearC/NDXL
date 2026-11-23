#include <stdio.h>
#include <stdlib.h>

#include "../../NDXL/include/ndxl.h"

int dynamics(NDXLVertex* vertex, double* dx, const double* x, double t, void* params)
{
    double A = *(double*)params;
    dx[0] = A * x[0] * t;
    return 0;
}

int cb(NDXLNetwork* net, double time, void* params)
{
    FILE* csvfile = (FILE*)params;
    NDXLVertex* vertex = ndxlGetNetworkFirstVertex(net);
    double x = ndxlVertexState(vertex)[0];
    fprintf(csvfile, "%f,%f\n", time, x);
    return 0;
}

int main(int argc, char** argv)
{
	ndxlInit();

	// Create file handle, and write column headers if successful
    FILE* f_output = fopen("states.csv", "w");
    if (!f_output)
        return 1;
    fprintf(f_output, "t,x\n");
    
    // Assign file handle to network's `void* params`
    NDXLNetwork* nwk = ndxlCreateNetwork(f_output);

	// Create `void* params` A, and initial conditions x0
    double A = 2.0;
    double x0 = 1.0;
    
    // Create vertex on `nwk`
    NDXLVertex* vert = ndxlCreateVertex(
        nwk,        // Host network
        &x0,        // Initial state vector - must be a pointer to double
        1,          // Length of state vector
        dynamics,   // Dynamics function the vertex state must evolve according to
        NULL,       // No noise function (see the "ornstein-uhlenbeck" examples)
        NULL,       // No noise channels (must be NULL if noise function is NULL)
        0,          // Number of noise channels (must be 0 if noise function is NULL)
        NULL,       // No discrete callback
        &A          // User-defined `void* params`
    );
	
	int result = ndxlNetworkSimulateDopri(
        nwk,    // Network to simulate
        2.5,    // Simulation duration
        1e-6,   // Absolute tolerance (number of correct decimal places)
        1e-3,   // Relative tolerance (number of correct digits)
        NULL,   // Time-stops array (forced evaluation times)
        0,      // Length of time-stops array (must be 0 if time-stops is NULL)
        cb      // Discrete callback (called once at t = 0, then again after each successful integration step)
    );
    
    // Verify success
    if (result == NDXL_ERR)
        ndxlPrintError();
	
	fclose(f_output);
	return 0;
}