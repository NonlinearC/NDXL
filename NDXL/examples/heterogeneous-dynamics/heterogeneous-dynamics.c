#include <stdio.h>
#include <stdlib.h>

#include "../../NDXL/include/ndxl.h"

int dyn1(NDXLVertex* v1, double* dx1, const double* x1, double t, void* p)
{
    double A = *(double*)p;
    dx1[0] = A * x1[0] * t;
    return 0;
}
 
int dyn2(NDXLVertex* v2, double* dx2, const double* x2, double t, void* p)
{
    double B = ((double*)p)[0];
    double D = ((double*)p)[1];
    dx2[0] = B * sin(x2[0]);
    dx2[1] = -D * x2[1] * cos(t);
    return 0;
}
 
int dyn3(NDXLVertex* v3, double* dx3, const double* x3, double t, void* p)
{
    // Piecewise function
    if (t >= 1.0)
        dx3[0] = t;
    else
        dx3[0] = 0.0;
    return 0;
}

int cb(NDXLNetwork* net, double time, void* params)
{
    FILE* csvfile = (FILE*)params;
    fprintf(csvfile, "%f", time);
    for (NDXLVertex* vertex = ndxlGetNetworkFirstVertex(net); vertex; vertex = ndxlGetNetworkNextVertex(vertex))
		for (size_t i = 0; i < ndxlGetVertexOrder(vertex); i++)
			fprintf(csvfile, ",%f", ndxlVertexState(vertex)[i]);
    fprintf(csvfile, "\n");
    return 0;
}

int main(int argc, char** argv)
{
	ndxlInit();

	FILE* f_output = fopen("hetero.csv", "w");
    if (!f_output)
        return 1;
    fprintf(f_output, "t,x1,x2_0,x2_1,x3\n");
	NDXLNetwork* nwk = ndxlCreateNetwork(f_output);

	// Create extra parameters in a single array which we will offset for the `void* params`
    double ABD[3] = { 1.6, 2.3, 0.7 };
    
    // Create initial conditions for vertex 1
    double x1_0 = 1.0;
    
    // Create initial conditions for vertex 2
    double x2_0[2] = { 0.5, 0.6 };
    
    // Vertex 3 shall have the initial condition x3_0 = 0, so we can simply pass NULL for the state vector
    // Technically, we must then also pass a "state vector size" of 1 to create a scalar
    // However, NDXL enforces a minimum size of 1, so even passing 0 for the length would give the desired result
    
    // Sine ABD is already a double*, we can use pointer arithmetic to get the position in ABD where the `void* params` of vertices 1 and 2 are
    ndxlCreateVertex(nwk, &x1_0, 1, dyn1, NULL, NULL, 0, NULL, ABD);
    ndxlCreateVertex(nwk, x2_0, 2, dyn2, NULL, NULL, 0, NULL, ABD + 1);
    ndxlCreateVertex(nwk, NULL, 1, dyn3, NULL, NULL, 0, NULL, NULL);

	double tstop = 1.0;
    ndxlNetworkSimulateRkck(nwk, 5.0, 1e-6, 1e-3, &tstop, 1, cb);

	return 0;
}