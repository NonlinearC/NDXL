#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../../NDXL/include/ndxl.h"

typedef struct
{
    double G;
    FILE* f_output;
} nbody_params;

double randdbl(double low, double high)
{
	static struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	srand(ts.tv_nsec ^ ts.tv_sec);
	return low + (high - low) * (double)rand() / (double)RAND_MAX;
}

int dynamics(NDXLVertex* body, double* dr_i, const double* r_i, double t, void* p)
{
    // Get parent network and unravel state vector components
    NDXLNetwork* parent = ndxlGetVertexNetwork(body);
    double u_xi = r_i[0];
    double u_yi = r_i[1];
    double v_xi = r_i[2];
    double v_yi = r_i[3];
    
    // Iterate over all vertices in the parent network
    double coupling[2] = { 0.0, 0.0 };
    for (NDXLVertex* vert = ndxlGetNetworkFirstVertex(parent); vert; vert = ndxlGetNetworkNextVertex(vert))
    {
        // Ensure `vert` does not refer to the current vertex `body`
        if (body != vert)
        {
            // ndxlVertexState(vert) provides direct access to the Runge-Kutta stage vector, making these operations inexpensive
            double u_xj = ndxlVertexState(vert)[0];
            double u_yj = ndxlVertexState(vert)[1];
            
            // Calculate difference and magnitude of difference
            double diff[2] = { u_xi - u_xj, u_yi - u_yj };
            double mag = sqrt(diff[0] * diff[0] + diff[1] * diff[1]);
            
            // Extract mass of `vert` from its `void* params` pointer
            double m_j = *(double*)ndxlGetVertexParams(vert);
            
            // Update coupling sums
            coupling[0] += m_j * diff[0] / (mag * mag * mag);
            coupling[1] += m_j * diff[1] / (mag * mag * mag);
        }
    }
    
    // Write out the position and velocity derivatives
    dr_i[0] = v_xi;
    dr_i[1] = v_yi;
    dr_i[2] = -(*(nbody_params*)ndxlGetNetworkParams(parent)).G * coupling[0];
    dr_i[3] = -(*(nbody_params*)ndxlGetNetworkParams(parent)).G * coupling[1];
    return 0;
}

int cb(NDXLNetwork* nwk, double t, void* p)
{
    // Check if we are at a time stop before writing out to file
    if ((t == 0.0) || (t == 1.5) || (t == 3.0) || (t == 4.5))
    {
        // Get handle from network's `void* params` and print the current time to it
        FILE* handle = (*(nbody_params*)p).f_output;
        fprintf(handle, "%f", t);
        
        // Iterate over vertices and write output to the file
        for (NDXLVertex* body = ndxlGetNetworkFirstVertex(nwk); body; body = ndxlGetNetworkNextVertex(body))
            fprintf(handle, ",%f,%f,%f,%f", ndxlVertexState(body)[0], ndxlVertexState(body)[1], ndxlVertexState(body)[2], ndxlVertexState(body)[3]);
        
        // Append newline
        fprintf(handle, "\n");
    }
    // Return success indicator
    return 0;
}

int main(int argc, char** argv)
{
    ndxlInit();
    
    // Initialise custom params struct
    nbody_params params;
    params.G = 1.0;
    params.f_output = fopen("nbody.csv", "w");
    if (!params.f_output)
        return 1;
    
    // Print "time" header to output file, we will do the rest later in the vertex creation loop
    fprintf(params.f_output, "t");
    
    // Assign params to network
    NDXLNetwork* nwk = ndxlCreateNetwork(&params);
	
	// m=1 for all agents so we can define it once here
    double mass = 1.0;
    
    for (int i = 0; i < 10; i++)
    {       
        double dist = randdbl(8.0, 10.0);
        double ang = 0.2 * i * PI;
        double r0[4] = { dist * cos(ang), dist * sin(ang), 0.5 * dist * cos(ang + 0.5 * PI), 0.5 * dist * sin(ang + 0.5 * PI) };
        ndxlCreateVertex(nwk, r0, 4, dynamics, NULL, NULL, 0, NULL, &mass);
        
        // Write headers to output file
        fprintf(params.f_output, ",x%d,y%d,vx%d,vy%d", i, i, i, i);
    }
    
    // Remember to append a newline to the output file
    fprintf(params.f_output, "\n");
	
	// Define our time stops
    double tstops[2] = { 1.5, 3.0 };
    
    // We will use the Cash-Karp method this time for variety :)
    ndxlNetworkSimulateRkck(nwk, 4.5, 1e-6, 1e-3, tstops, 2, cb);
    
    // Close the file handle before exiting
    fclose((*(nbody_params*)ndxlGetNetworkParams(nwk)).f_output);
    return 0;
}