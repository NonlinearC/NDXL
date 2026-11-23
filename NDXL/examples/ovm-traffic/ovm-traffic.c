#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../../NDXL/include/ndxl.h"

// We again use a custom struct to combine numerical network parameters with a file handle
typedef struct
{
    double vmax;
    double d;
    double tanhd;
    FILE* handle;
} nwkparams;

double randdbl(double low, double high)
{
	static struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	srand(ts.tv_nsec ^ ts.tv_sec);
	return low + (high - low) * (double)rand() / (double)RAND_MAX;
}

int acc(NDXLVertex* car, double* dxv_i, const double* xv_i, double t, void* p)
{
    double vmax = (*(nwkparams*)ndxlGetNetworkParams(ndxlGetVertexNetwork(car))).vmax;
    double d = (*(nwkparams*)ndxlGetNetworkParams(ndxlGetVertexNetwork(car))).d;
    double tanhd = (*(nwkparams*)ndxlGetNetworkParams(ndxlGetVertexNetwork(car))).tanhd;
    double a = *(double*)p;
    double x_ahead, x_delayed;
    
    // Since each vertex has three edges (one incoming, one outgoing, and one self-loop), we iterate over all three to appropriately populate x_ahead and x_delayed
    for (NDXLEdge* edge = ndxlGetVertexFirstEdge(car); edge; edge = ndxlGetVertexNextEdge(car, edge))
    {
        // Check for self-loop
        if (ndxlGetVertexNeighbour(car, edge) == car)
        {
            // Because the equations are delayed, we must use ndxlGetVertexNeighbourState() to factor in those delays
            // This is even true for the self-loop, as the historical state information for the "neighbour" is what we want to query (just that the "neighbour" happens to be the vertex itself)
            x_delayed = ndxlGetVertexNeighbourState(car, edge)[0];
        }
        // Check for `car` being the "destination" vertex along `edge` i.e., information flows "to" `car`
        else if (car == ndxlGetEdgeSecondVertex(edge))
        {
            x_ahead = ndxlGetVertexNeighbourState(car, edge)[0];
        }
    }
    
    // Calculate optimal velocity and write out derivatives
    double V = vmax * (tanh(x_ahead - x_delayed - d) + tanhd) / (1.0 + tanhd);
    dxv_i[0] = xv_i[1];
    dxv_i[1] = a * (V - xv_i[1]);
    return 0;
}

int cb(NDXLNetwork* net, double time, void* params)
{
	// Check that calling time is a capture time
	if (fabs(fmod(time, 20.0)) < 1e-12)
	{
		FILE* csvfile = ((nwkparams*)params)->handle;
		fprintf(csvfile, "%f", time);
		for (NDXLVertex* car = ndxlGetNetworkFirstVertex(net); car; car = ndxlGetNetworkNextVertex(car))
		{
			double* xv = ndxlVertexState(car);
			fprintf(csvfile, ",%f", xv[0]);
		}
		fprintf(csvfile, "\n");
	}
    return 0;
}

int main(int argc, char** argv)
{
	ndxlInit();

	// Define network-wide parameters and create network
    nwkparams p;
    p.vmax = 60. * (1000. / 3600.); // Speed limit of 60 kph in m/s
    p.d = 1.5;                      // Safe bumber-to-bumber distance in m
    p.tanhd = tanh(p.d);            // Precalculate tanh(d) to save time in function evaluations
    
    // Prepare output file and create network
    p.handle = fopen("cars.csv", "w");
    if (!p.handle)
        return 1;
    fprintf(p.handle, "t");
    NDXLNetwork* nwk = ndxlCreateNetwork(&p);
    
    // Create car vertices, recalling that their `void* params` pointers must persist in memory
    double a[20];   // Reaction sensitivity
    double tau[20]; // Reaction time (will be used in edge creation so we need to populate them now)
    NDXLVertex* cars[20];
    for (int i = 0; i < 20; i++)
    {
        // Cars start uniformly spaced around a circle of circumference 100
        double xv0[2] = { 5.0 * i, randdbl(10.0, 15.0) };
        a[i] = randdbl(0.1, 0.3);
        tau[i] = randdbl(0.5, 1.5);
        cars[i] = ndxlCreateVertex(nwk, xv0, 2, acc, NULL, NULL, 0, NULL, &a[i]);
        
        // Write header to csv
        fprintf(p.handle, ",x%d", i);
    }
    fprintf(p.handle, "\n");
    
    // Create edges according to the topology described above
    for (int i = 0; i < 20; i++)
    {
        // Create self-loop
        ndxlCreateEdge(
            cars[i],    // `Source` vertex
            cars[i],    // `Destination` vertex
            NULL,       // State vector
            0,          // Will be forced to a minimum of 1 by default
            tau[i],     // Delay
            1,          // Directed
            NULL,       // No dynamics
            NULL,       // No noise
            NULL,       // No noise channels (must be NULL if noise is NULL)
            0,          // Number of noise channels (must be 0 if noise is NULL)
            NULL,       // No discrete callback
            NULL        // No `void* params`
        );
        
        // Create edge from car ahead
        ndxlCreateEdge(cars[(i + 1) % 20], cars[i], NULL, 0, tau[i], 1, NULL, NULL, NULL, 0, NULL, NULL);
    }

	ndxlNetworkSimulateNystrom(nwk, 100.0, 0.1, cb);
    fclose(p.handle);
	return 0;
}