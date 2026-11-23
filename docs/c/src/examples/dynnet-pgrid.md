# Dynamic Power Grids {#dynpow}
\note This page assumes familiarity with the basic layout and API usage described in the main @ref examples page, and familiarity with the dynamic networks semantics described in @ref dynnet.

### The full code for these examples can be found [here](https://github.com/NonlinearC/NDXL/tree/main/examples/cascading-failure/).

In these examples, we will use a model power grid to analyse how a single failure on one edge can propagate through the network, a phenomenon known as a cascading failure.
Power grids are often expressed as networks of generator and load nodes (vertices) coupled by power lines (edges), where the phase angle \f$\Theta\f$ of node \f$i\f$ is given by the swing equation

\f[
    \frac{{\rm d}^2\Theta_i}{{\rm d}t^2} + I_i\frac{{\rm d}\Theta_i}{{\rm d}t} = p_i + \sum_{j=1}^N{A_{ij}\sin{\left(\Theta_j - \Theta_i\right)}}.
\f]

Generator (load) nodes are identified by \f$p\f$ greater (less) than zero, and power flow \f$A\f$ is the edge state vector.
Traditionally, "failure" in these models is modelled by zeroing the power flow \f$A\f$ through an edge \f$ij\f$; in NDXL, we simply remove the edge.
We start as usual by defining our dynamics, though with a twist: while the edges are static, since we are treating their state vector as the power flowing through them, we must update this power flow even though the dynamics are zero.
We will do this by means of an @ref NDXLEdgeCallback "edge callback" - the same callback where we will also test if the edge has failed (and subsequently, should be destroyed).

```c
// ...

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
	
	// Edge state is split into two parts: the static part A[0] is the coupling strength, and A[1] is the power flow
	A[1] = A[0] * sin(ndxlVertexState(ndxlGetEdgeSecondVertex(edge))[0] - ndxlVertexState(ndxlGetEdgeFirstVertex(edge))[0]);
	
	// Check if absolute power flow exceeds limit
	if (fabs(A[1]) > limit)
	{
		printf("Edge %d failed at t = %f\n", idx, t);
		ndxlDestroyEdge(&edge);
	}
	return 0;
}
```

Next, we define the vertices and edges within the network.
Our grid shall have two generators (nodes \f$1\f$ and \f$4\f$; \f$p=1.5\f$) and three loads (nodes \f$0\f$, \f$2\f$, and \f$3\f$; \f$p=-1\f$), with adjacency matrix

\f[
\begin{array}{|ccccc|}
    0 & 1 & 1 & 0 & 1 \\
    1 & 0 & 1 & 1 & 0 \\
    1 & 1 & 0 & 1 & 0 \\
    0 & 1 & 1 & 0 & 1 \\
    1 & 0 & 0 & 1 & 0 \\
\end{array}
\f]

The initial conditions used are extracted from the <b>[NetworkDynamics.jl cascading failure example](https://juliadynamics.github.io/NetworkDynamics.jl/dev/generated/cascading_failure/)</b>, which correspond to a steady state (no fluctuations in frequency or phase angle).

```c
// ...

int main(int argc, char** argv)
{
	ndxlInit();
	
	// ...
	
	// Initial conditions as flat arrays (we will use pointer arithmetic to offset these in vertex creation)
	double Theta0[10] = { 0.8031776440871194, 0.0, 1.076515586764153, 0.0, 0.6863392710509373, 0.0, 0.8031776440871194, 0.0, 1.28131103515625, 0.0 };
	double Params[10] = {  -1.0, 0.1, 1.5, 0.1, -1.0, 0.1, -1.0, 0.1, 1.5, 0.1 };
	
	// Vertex creation (ellipses indicate "similar to above")
	NDXLVertex* load0 = ndxlCreateVertex(nwk, Theta0, 2, swing, NULL, NULL, 0, NULL, Params);
	NDXLVertex* gen0 = ndxlCreateVertex(nwk, Theta0 + 2, 2, swing, NULL, NULL, 0, NULL, Params + 2);
	NDXLVertex* load1 = ndxlCreateVertex(...);
	NDXLVertex* load2 = ndxlCreateVertex(...);
	NDXLVertex* gen1 = ndxlCreateVertex(...);
	
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
	// ...
	A0[1] = A0[0] * sin(ndxlVertexState(load2)[0] - ndxlVertexState(gen1)[0]);
	ndxlCreateEdge(gen1, load2, A0, 2, 0.0, 0, NULL, NULL, NULL, 0, edge_cb, E_params[6]);
	
	return 0;
}
```

Because the initial conditions represent a steady state of the power grid, running this simulation will only produce flat lines for the power flow of each edge.
We must therefore inject a failure at a time of our choosing to initiate the cascade; we modify `edge_cb` to force edge \f$4\f$ to fail at \f$t=1\f$.

```c
// ...

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
```

One last thing to do before we simulate: we must revisit our output capturing strategy.
Previously, the network callback we used to capture simulation outputs assumed the number and order of agents remained constant, which is no longer true for dynamic networks.
We will therefore again make use of the edge objects' "index" we assigned them, ensuring that the index of the edge we are currently observing matches the current corresponding CSV header.

```c
// ...

int nwk_cb(NDXLNetwork* nwk, double t, void* params)
{
	FILE* handle = (FILE*)params;
	fprintf(handle, "%f", t);
	
	// Since some edges will be deleted, we want to make sure we have the edge whose index corresponds to the CSV header
	int idx = 0;
	for (NDXLEdge* edge = ndxlGetNetworkFirstEdge(nwk); edge; edge = ndxlGetNetworkNextEdge(edge))
	{
		// Artificially insert zeros into output for destroyed edges
		if (idx == (int)(((double*)ndxlGetEdgeParams(edge))[0]))
		{
			fprintf(handle, ",%f", ndxlEdgeState(edge)[1]);
			idx++;
		}
		else
			fprintf(handle, ",0");
	}
	fprintf(handle, "\n");
	return 0;
}

// ...
```

We then use a similar structure for the file output in `main()`.
As witb the @ref failvert example, we simulate this network with a sufficiently small time step so that the reported time of a component failing is sufficiently close to the actual time of failure.
We again use the Nystrom method as the swing equation is second order.

```c
// ...

int main(int argc, char** argv)
{
	ndxlInit();
	
	FILE* f_output = fopen("cascfail.csv", "w");
	if (!f_output)
		return 1;
	fprintf(f_output, "t,e1,e2,e3,e4,e5,e6,e7\n");
	NDXLNetwork* nwk = ndxlCreateNetwork(f_output);
	
	// ...	
	
	ndxlNetworkSimulateNystrom(nwk, 6.0, 1e-3, nwk_cb);
	fclose(f_output);
	return 0;
}
```

![](@ref cascfail.png)

## Cascading Failure in an Unbalanced Grid
Instead of just having the line fail, we will add a miniature generator (\f$p=0.5\f$) assumed to be the cause for the initial line failure, mimicking a line fault caused by an imbalanced grid (\f$\sum{p_i}\neq0\f$).
At the failure time, we couple the generator to the three load nodes, and assume an initial state of the mean phase and frequency across the network, before triggering the initial line failure.
This example serves as a demonstration of adding agents into a network during simulation as opposed to just destroying them.

```c
// ...

// Modify the discrete callback to add the generator and new power lines
int edge_cb(NDXLEdge* edge, double* A, double t, void* params)
{
	NDXLNetwork* grid = ndxlGetEdgeNetwork(edge);
	int idx = (int)(((double*)params)[0]);
	double limit = ((double*)params)[1];
	if ((t >= 1.0) && (idx == 4))
	{
		// Calculate mean phase vector over network (mean frequency is trivially zero before failure)
		double Theta0[2] = { 0.0, 0.0 };
		for (NDXLVertex* node = ndxlGetNetworkFirstVertex(grid); node; node = ndxlGetNetworkNextVertex(node))
			Theta0[0] += ndxlVertexState(node)[0];
		Theta0[0] /= ndxlGetNetworkOrder(grid);
	
		// Add new generator node - notice the use of `malloc()` for the `void* params` meaning we will have to manually `free()` the pointer later
		double* v_params = (double*)malloc(2 * sizeof(double));
		v_params[0] = 0.5;	// Generator, p = 0.5
		v_params[0] = 0.1;	// Inertia = 0.1, same as the rest
		NDXLVertex* gen2 = ndxlCreateVertex(grid, Theta0, 2, swing, NULL, NULL, 0, NULL, v_params);
	
		// Couple new generator to existing load nodes
		double e_idx = 7.0;
		for (NDXLVertex* node = ndxlGetNetworkFirstVertex(grid); node; node = ndxlGetNetworkNextVertex(node))
		{
			// "Load" nodes identified by p < 0
			if (((double*)ndxlGetVertexParams(node))[0] < 0.0)
			{
				double e_state[2] = { 1.63, 1.63 * sin(ndxlVertexState(node)[0] - ndxlVertexState(gen2)[0]) };
				double* e_params = (double*)malloc(2 * sizeof(double));
				e_params[0] = e_idx;
				e_params[1] = 1.0;
				ndxlCreateEdge(gen2, node, e_state, 2, 0.0, 0, NULL, NULL, NULL, 0, edge_cb, e_params);
				e_idx += 1.0;
			}
		}
	
		// Finally, trigger the fail
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

// ...
```

Since we know we are adding three edges, modifying our output CSV headers and callback to include them is trivial.

```c
// ...

int nwk_cb(NDXLNetwork* nwk, double t, void* params)
{
	FILE* handle = (FILE*)params;
	fprintf(handle, "%f", t);
	
	// We iterate from idx 0 to 9 for 10 edges in total
	NDXLEdge* edge = ndxlGetNetworkFirstEdge(nwk);
	for (int idx = 0; idx < 10; idx++)
	{
		if (edge && (idx == (int)(((double*)ndxlGetEdgeParams(edge))[0])))
		{
			fprintf(handle, ",%f", ndxlEdgeState(edge)[1]);
			edge = ndxlGetNetworkNextEdge(edge);
		}
		else
			fprintf(handle, ",0");
	}

	fprintf(handle, "\n");
	return 0;
}

int main(int argc, char** argv)
{
	ndxlInit();
	
	FILE* f_output = fopen("cascfail-addgen.csv", "w");
	if (!f_output)
		return 1;
	fprintf(f_output, "t,e1,e2,e3,e4,e5,e6,e7,e8,e9,e10\n");
	NDXLNetwork* nwk = ndxlCreateNetwork(f_output);
	
	// ...	

	fclose(f_output);
	return 0;
}
```

Recall, however, that we used `malloc()` to create the `void* params` for the new generator and edges in our modified `edge_cb` - we now need to call `free()` on those pointers to avoid memory leaks.
We can use the insertion order to our advantage: the new generator `gen2` will be the last vertex in the network, so we can access it using @ref ndxlGetNetworkLastVertex().
The new edges will be the last three edges, so we can back-iterate over the network using @ref ndxlGetNetworkLastEdge() and @ref ndxlGetNetworkPreviousEdge().

```c
// ...

int main(int argc, char** argv)
{
	ndxlInit();
	
	// ...

	ndxlNetworkSimulateNystrom(nwk, 6.0, 1e-3, nwk_cb);
	fclose(f_output);
	
	// Cleanup `malloc()'d` pointers
	free(ndxlGetVertexParams(ndxlGetNetworkLastVertex(nwk)));
	for (NDXLEdge* edge = ndxlGetNetworkLastEdge(nwk); edge; edge = ndxlGetNetworkPreviousEdge(edge))
		free(ndxlGetEdgeParams(edge));
	
	return 0;
}
```

![](@ref cascfail-addgen.png)