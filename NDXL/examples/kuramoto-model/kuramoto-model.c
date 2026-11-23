#include <math.h>
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

int kuramoto(NDXLVertex* oscillator, double* dtheta_i, const double* theta_i, double time, void* params)
{
    // Pointer conversion to extract eigenfrequency from `void* params`
    double omega = *(double*)params;
    
    // Calculate coupling sum by iterating over only the vertices/edges coupled to `oscillator`
    double coupling = 0.0;
    for (NDXLEdge* edge = ndxlGetVertexFirstEdge(oscillator); edge; edge = ndxlGetVertexNextEdge(oscillator, edge))
    {
        double K_ij = ndxlEdgeState(edge)[0];
        double theta_j = ndxlGetVertexNeighbourState(oscillator, edge)[0];
        coupling += K_ij * sin(theta_j - theta_i[0]);
    }
    
    // Write derivative in-place and indicate function success
    dtheta_i[0] = omega + coupling / ndxlGetVertexDegree(oscillator);
    return 0;
}

int perturbation(NDXLVertex* oscillator, double* theta_i, double time, void* params)
{
    if (time == 2.5)
        theta_i[0] = 0.0;
    return 0;
}

int cb(NDXLNetwork* net, double time, void* params)
{
    FILE* csvfile = (FILE*)params;
    fprintf(csvfile, "%f", time);
    for (NDXLVertex* oscillator = ndxlGetNetworkFirstVertex(net); oscillator; oscillator = ndxlGetNetworkNextVertex(oscillator))
    {
        double x = ndxlVertexState(oscillator)[0];
        fprintf(csvfile, ",%f", x);
    }
    fprintf(csvfile, "\n");
    return 0;
}

int main(int argc, char** argv)
{
    ndxlInit();
    FILE* f_output = fopen("kuramoto.csv", "w");
    if (!f_output)
        return 1;
    fprintf(f_output, "t,theta1,theta2,theta3,theta4\n");
    NDXLNetwork* nwk = ndxlCreateNetwork(f_output);
    
    // Eigenfrequencies will be our `void* params`, so they must persist in memory
    double omega[4];
    NDXLVertex* oscillators[4];
    
    // Assign to only the first oscillator
    double theta0 = randdbl(-PI, PI);
    omega[0] = randdbl(0.9, 1.1);
    oscillators[0] = ndxlCreateVertex(
        nwk,
        &theta0,
        1,
        kuramoto,       // Assign Kuramoto dynamics
        NULL,
        NULL,
        0,
        perturbation,   // Assign perturbation
        &omega[0]       // Eigenfrequency
    );
    
    // Do the rest of the oscillators
    for (int i = 1; i < 4; i++)
    {
        theta0 = randdbl(-PI, PI);
        omega[i] = randdbl(0.9, 1.1);
        oscillators[i] = ndxlCreateVertex(nwk, &theta0, 1, kuramoto, NULL, NULL, 0, NULL, &omega[i]);
    }
	
	// Coupling constant K_ij (made identical for all edges)
    double K = 1.0;
    
    // Define topology
    ndxlCreateEdge(
        oscillators[0], // "Source" vertex (interchangeable with "Destination" in undirected edge)
        oscillators[1], // "Destination" vertex (interchangeable with "Source" in undirected edge)
        &K,             // State vector
        1,              // Length of state vector
        0.0,            // No delay (see the "ovm-traffic" example)
        0,              // Not directed (if this value is nonzero, information only flows from "Source" to "Destination" but not the other way around)
        NULL,           // No dynamics (static edge)
        NULL,           // No noise
        NULL,           // No noise channels (must be NULL if noise is NULL)
        0,              // Number of noise channels (must be 0 if noise is NULL)
        NULL,           // No edge callback
        NULL            // No extra `void* params`
    );
    ndxlCreateEdge(oscillators[0], oscillators[2], &K, 1, 0.0, 0, NULL, NULL, NULL, 0, NULL, NULL);
    ndxlCreateEdge(oscillators[1], oscillators[3], &K, 1, 0.0, 0, NULL, NULL, NULL, 0, NULL, NULL);
    ndxlCreateEdge(oscillators[2], oscillators[3], &K, 1, 0.0, 0, NULL, NULL, NULL, 0, NULL, NULL);
	
	// Define the time stops array
    double tstops = 2.5;
    
    // Cash-Karp would also be a suitable choice here
    ndxlNetworkSimulateDopri(
        nwk,
        5.0,        // Simulation duration; the time stops array must be between the current network time (which starts at 0) and that time plus the duration
        1e-6,
        1e-3,
        &tstops,    // Time stops array
        1,          // Length of time stops array
        cb
    );
    
    return 0;
}