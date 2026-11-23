# Adding Delays {#dde}
\note This page assumes familiarity with the basic layout and API usage described in the main @ref examples page.

### The full code for this example can be found [here](https://github.com/NonlinearC/NDXL/tree/main/examples/ovm-traffic/).

Delay differential equations (DDEs) are characterised by delayed (historical) states influencing the current state of an agent, written as

\f[
    \frac{d\vec{Y}}{dt} = \vec{F}(\vec{Y}(t),~\vec{Y}(t-\vec{\tau}_1),~\vec{Y}(t-\vec{\tau}_2),~\dots~\vec{Y}(t-\vec{\tau}_n),~t),
\f]

where the \f$\tau_i\f$ are the delay terms.
Since ODE systems are only defined for \f$t\ge t_0\f$ (with \f$t_0=0\f$ in NDXL), one inevitably encounters the situation where \f$t-\tau<t_0\f$.
This is solved by use of a history function, which is a special function in DDEs defined for the interval \f$t_0-\tau\f$ that returns the historical states over that time period i.e., "what was the system doing before we started modelling/tracking it?"

In NDXL, delays have a physical interpretation as being the time taken for information to travel from vertex \f$i\f$ to \f$j\f$ along edge \f$ij\f$ i.e., the stage state is only delayed because the edge propagates that information at a finite speed.
This interpretation has several important implications:

- Delays are a property of an edge, with idealised instantaneous information transfer occuring only along edges whose delay is zero
- Information has not yet propagated between vertices \f$i\f$ and \f$j\f$ along edge \f$ij\f$ for \f$t<\tau_{ij}\f$
- Historical states for vertices \f$i\f$ and \f$j\f$ need only be stored up to \f$t-\tau_{ij}\f$

The first inference means that only vertex dynamics, never edge dynamics, can be described by DDEs in NDXL, and vertex \f$i\f$ can only ever depend on the delayed state of vertex \f$j\f$; for vertex \f$i\f$ to depend on its own delayed state, vertex \f$i\f$ and vertex \f$j\f$ must be the same vertex i.e., a self-coupled node.
For vertex \f$i\f$ to be affected by multiple delayed terms from vertex \f$j\f$, mutliple edges \f$ij\f$ must exist, each with its own unique delay.
The second inference implies that the history function for vertices is \f$\vec{x}_0\f$, and for edges is \f$\vec{0}\f$, for \f$t<\tau\f$; this is what the functions @ref ndxlGetVertexNeighbourState() @ref ndxlGetEdgeRelativeState() do, and is why they should almost always be used over retrieving agent states directly.
The third inference is more of a concern for memory management, where an indefinite number of historical states cannot be stored on a finite amount of memory, meaning NDXL only preserves historical states up to \f$t-\tau_{ij}\f$.
Historical states are further used to perform a cubic hermite interpolation when \f$t-\tau_{ij}\f$ does not correspond directly to a discrete stored state (we opted for cubic hermite interpolation due to its curvature preservation properties).

## Optimal Velocity Model {#ovm}
We consider a delayed optimal velocity model (OVM) with \f$20\f$ cars travelling in a circle, where the position \f$x\f$ and velocity \f$v\f$ of car \f$i\f$ are given by the already-deconstructed second-order ODE

\f[
    \frac{dx_i}{dt} = v_i,
\f]
\f[
    \frac{dv_i}{dt} = a_i\left[V\left(\Delta x_i(t-\tau_{ii})\right) - v_i\right].
\f]

The delays here refer to the drivers' reaction times, and the optimal velocity function \f$V\f$ typically takes on a sigmoid curve such as

\f[
    V\left(\Delta x\right) = v_{max}\frac{\tanh{(\Delta x - d)} + \tanh{(d)}}{1 + \tanh{(d)}},
\f]

where \f$\Delta x(t-\tau_{ij})\f$ is the distance between cars \f$i\f$ and \f$j\f$, \f$\tau_{ij}\f$ time ago.

We see that each car (vertex) depends on a delayed state from the car in front of it, and of itself.
Furthermore, each car is not influenced by the car behind it; our network therefore consists of vertices each with a directed edge to the vertex "behind" it, and a directed edge to itself (it is usually a good idea to make self-loops directed to avoid duplicate state storage and wasted memory).
We define the car dynamics function with this structure in mind, remembering also that the order in which edges are added is preserved.
In our dynamics function, we therefore query each edge for its neighbouring vertex, and query the network for common parameters like \f$v_{max}\f$.

```c
// ...

// We again use a custom struct to combine numerical network parameters with a file handle
typedef struct
{
	double vmax;
	double d;
	double tanhd;
	FILE* handle;
} nwkparams;

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
```

Over in `main()`, we will create our cars and the edges between them, baring in mind that `directed` edges allow information to travel from its source vertex to its destination vertex, but not the other way around.
The order in which we specify the vertices in the @ref ndxlCreateEdge() "edge constructor" therefore becomes important, although their states can still be left as `NULL`.

```c
// ...

int main(int argc, char** argv)
{
	ndxlInit();
	
	// Define network-wide parameters and create network
	nwkparams p;
	p.vmax = 60. * (1000. / 3600.);	// Speed limit of 60 kph in m/s
	p.d = 1.5;						// Safe bumber-to-bumber distance in m
	p.tanhd = tanh(p.d);			// Precalculate tanh(d) to save time in function evaluations
	
	// Prepare output file and create network
	p.handle = fopen("cars.csv", "w");
	if (!p.handle)
		return 1;
	fprintf(p.handle, "t");
	NDXLNetwork* nwk = ndxlCreateNetwork(&p);
	
	// Create car vertices, recalling that their `void* params` pointers must persist in memory
	double a[20];	// Reaction sensitivity
	double tau[20];	// Reaction time (will be used in edge creation so we need to populate them now)
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
			cars[i],	// `Source` vertex
			cars[i],	// `Destination` vertex
			NULL,		// State vector
			0,			// Will be forced to a minimum of 1 by default
			tau[i],		// Delay
			1,			// Directed
			NULL,		// No dynamics
			NULL,		// No noise
			NULL,		// No noise channels (must be NULL if noise is NULL)
			0,			// Number of noise channels (must be 0 if noise is NULL)
			NULL,		// No discrete callback
			NULL		// No `void* params`
		);
		
		// Create edge from car ahead
		ndxlCreateEdge(cars[(i + 1) % 20], cars[i], NULL, 0, tau[i], 1, NULL, NULL, NULL, 0, NULL, NULL);
	}
	
	return 0;
}
```

Since the governing equation is fundamentally second-order, we will use the @ref ndxlNetworkSimulateNystrom "static Nystr&ouml;m method" to simulate the network, as it is better suited than the @ref ndxlNetworkSimulateRalston "static Ralston method" to second-order equations.
Outputs are plotted on a circle with circumference \f$100\f$ as that ringlike structure is how we defined our network in the first place.

```c
// ...

int main(int argc, char** argv)
{
	ndxlInit();
	
	// ...
	
	ndxlNetworkSimulateNystrom(nwk, 100.0, 0.1, cb);
	fclose(p.handle);
	return 0;
}
```

![](@ref cars.png)