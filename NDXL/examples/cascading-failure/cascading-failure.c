#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "../../NDXL/include/ndxl.h"

// Swing equation on vertices
int swing(NDXLVertex* node, double* dTheta, const double* Theta, double t, void* params)
{
	double p = ((double*)params)[0];
	double I = ((double*)params)[1];
	double coupling = 0.0;
	for (NDXLEdge* edge = ndxlGetVertexFirstEdge(node); edge; edge = ndxlGetVertexNextEdge(node, edge))
		coupling += ndxlEdgeState(edge)[0] * sin(ndxlGetVertexNeighbourState(node, edge)[0] - Theta[0]);
	dTheta[0] = Theta[1];
	dTheta[1] = p - I * Theta[1] + coupling;
	return 0;
}

// Discrete callback on edges; we will use the same "indexing" idea as the "Failing Vertices" example
int edge_cb(NDXLEdge* edge, double* A, double t, void* params)
{
	int idx = (int)(((double*)params)[0]);
	double limit = ((double*)params)[1];
	
	// Since all edges use this callback, we can trigger the failure based on `idx`
	// We use `t >= 1` because of accumulated floating point rounding errors (so it will fail somewhere between 1 and 1.003 because h=1e-3)
	if ((t >= 1.0) && (idx == 4))
	{
		// Artificially overload edge so it fails
		A[1] = 2.0;
	}
	else
	{
		// Edge state is split into two parts: the static part A[0] is the coupling strength, and A[1] is the power flow
		A[1] = A[0] * sin(ndxlVertexState(ndxlGetEdgeSecondVertex(edge))[0] - ndxlVertexState(ndxlGetEdgeFirstVertex(edge))[0]);
	}
	
	// Check if absolute power flow exceeds limit
	if (fabs(A[1]) >= limit)
	{
		printf("Edge %d failed at t = %f\n", idx, t);
		ndxlDestroyEdge(&edge);
	}
	return 0;
}

// Output capturing
int nwk_cb(NDXLNetwork* nwk, double t, void* params)
{
	FILE* handle = (FILE*)params;
	fprintf(handle, "%f", t);
	
	// Since some edges will be deleted, we want to make sure we have the edge whose index corresponds to the CSV header
	NDXLEdge* edge = ndxlGetNetworkFirstEdge(nwk);
	int e_idx = (int)(((double*)ndxlGetEdgeParams(edge))[0]);
	for (int idx = 0; idx < 7; idx++)
	{
		if (idx == e_idx)
		{
			fprintf(handle, ",%f", ndxlEdgeState(edge)[1]);
			// Advance edge pointer and check if there are actually edges (and hence indices) left
			edge = ndxlGetNetworkNextEdge(edge);
			if (edge)
				e_idx = (int)(((double*)ndxlGetEdgeParams(edge))[0]);
			else
				e_idx = 8;
		}
		else
		{
			// Artificially insert zeros into output for destroyed edges
			fprintf(handle, ",0");
		}
	}
	fprintf(handle, "\n");
	return 0;
}

int main(int argc, char** argv)
{
	ndxlInit();
	FILE* f_output = fopen("cascfail.csv", "w");
	if (!f_output)
		return 1;
	fprintf(f_output, "t,e1,e2,e3,e4,e5,e6,e7\n");
	NDXLNetwork* nwk = ndxlCreateNetwork(f_output);
	
	// Initial conditions as flat arrays (we will use pointer arithmetic to offset these in vertex creation)
	double Theta0[10] = { 0.8031776440871194, 0.0, 1.076515586764153, 0.0, 0.6863392710509373, 0.0, 0.8031776440871194, 0.0, 1.28131103515625, 0.0 };
	double Params[10] = {  -1.0, 0.1, 1.5, 0.1, -1.0, 0.1, -1.0, 0.1, 1.5, 0.1 };
	
	// Vertex creation (ellipses indicate "similar to above")
	NDXLVertex* load0 = ndxlCreateVertex(nwk, Theta0, 2, swing, NULL, NULL, 0, NULL, Params);
	NDXLVertex* gen0 = ndxlCreateVertex(nwk, Theta0 + 2, 2, swing, NULL, NULL, 0, NULL, Params + 2);
	NDXLVertex* load1 = ndxlCreateVertex(nwk, Theta0 + 4, 2, swing, NULL, NULL, 0, NULL, Params + 4);
	NDXLVertex* load2 = ndxlCreateVertex(nwk, Theta0 + 6, 2, swing, NULL, NULL, 0, NULL, Params + 6);
	NDXLVertex* gen1 = ndxlCreateVertex(nwk, Theta0 + 8, 2, swing, NULL, NULL, 0, NULL, Params + 8);
	
	// Prepare state vector and `void* params` of edges
	double A0[2] = { 1.63, 0.0 };
	double E_params[7][2] = {
		{ 0.0, 1.0 },
		{ 1.0, 1.0 },
		{ 2.0, 1.0 },
		{ 3.0, 1.0 },
		{ 4.0, 1.0 },
		{ 5.0, 1.0 },
		{ 6.0, 1.0 }
	};
	
	// Define topology
	A0[1] = A0[0] * sin(ndxlVertexState(load0)[0] - ndxlVertexState(gen0)[0]);
	ndxlCreateEdge(gen0, load0, A0, 2, 0.0, 0, NULL, NULL, NULL, 0, edge_cb, E_params[0]);
	A0[1] = A0[0] * sin(ndxlVertexState(load0)[0] - ndxlVertexState(load1)[0]);
	ndxlCreateEdge(load1, load0, A0, 2, 0.0, 0, NULL, NULL, NULL, 0, edge_cb, E_params[1]);
	A0[1] = A0[0] * sin(ndxlVertexState(load0)[0] - ndxlVertexState(gen1)[0]);
	ndxlCreateEdge(gen1, load0, A0, 2, 0.0, 0, NULL, NULL, NULL, 0, edge_cb, E_params[2]);
	A0[1] = A0[0] * sin(ndxlVertexState(gen0)[0] - ndxlVertexState(load1)[0]);
	ndxlCreateEdge(load1, gen0, A0, 2, 0.0, 0, NULL, NULL, NULL, 0, edge_cb, E_params[3]);
	A0[1] = A0[0] * sin(ndxlVertexState(gen0)[0] - ndxlVertexState(load2)[0]);
	ndxlCreateEdge(load2, gen0, A0, 2, 0.0, 0, NULL, NULL, NULL, 0, edge_cb, E_params[4]);
	A0[1] = A0[0] * sin(ndxlVertexState(load1)[0] - ndxlVertexState(load2)[0]);
	ndxlCreateEdge(load2, load1, A0, 2, 0.0, 0, NULL, NULL, NULL, 0, edge_cb, E_params[5]);
	A0[1] = A0[0] * sin(ndxlVertexState(load2)[0] - ndxlVertexState(gen1)[0]);
	ndxlCreateEdge(gen1, load2, A0, 2, 0.0, 0, NULL, NULL, NULL, 0, edge_cb, E_params[6]);
	
	ndxlNetworkSimulateNystrom(nwk, 6.0, 1e-3, nwk_cb);
	fclose(f_output);
	return 0;
}