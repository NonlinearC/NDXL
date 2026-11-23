# Simulating Networks With Dynamic Topologies {#dynnet}
\note This page assumes familiarity with the basic layout and API usage described in the main @ref examples page.

### The full code for these examples can be found [here](https://github.com/NonlinearC/NDXL/tree/main/examples/failing-vertices/).

The focal point of NDXL's architecture is its ability to simulate dynamic networks, which we @ref dyntop "outlined earlier".
This is facilitated by storing the state vector, intermediate stages, slope evaluations, noise matrices, and much more information locally for each agent, rather than in contiguous array over the entire network.
Unfortunately, this approach makes NDXL slower and more memory intensive than existing software solutions when simulating large static networks; its advantage remains specifically in dynamic networks.

The most important rule to keep in mind when simulating dynamic networks is that of when agents and noise channels in a network can be added, removed, or modified.
To that end, we distinguish between <b>calculation</b> (something that evolves an agent's state vector) and <b>modification</b> (something that alters the dynamics, noise, `void* params`, or callback of an agent).

### Agent dynamics (@ref NDXLVertexDynamics "NDXLVertexDynamics"; @ref NDXLEdgeDynamics "NDXLEdgeDynamics") and noise functions (@ref NDXLVertexNoise "NDXLVertexNoise"; @ref NDXLEdgeNoise "NDXLEdgeNoise") must ONLY be used for calculation
These functions are called multiple times per step, because all integration methods have multiple stages, and steps in an adaptive stepping algorithm may fail, triggering a retry with a smaller step.
Furthermore, intermediate stages are evaluated at fractional increments of the time step between the previous and next step, meaning any modifications in these functions would happen at different times, on different scales, multiple times per step.
Such modifications could therefore cause, among other issues: unexpected arithmetic errors, numerical instability, and incorrect error calculations.

<b>Your program will not necessarily crash, your simulation will just be very, very wrong!</b>

### Agent callbacks (@ref NDXLVertexCallback "NDXLVertexCallback"; @ref NDXLEdgeCallback "NDXLEdgeCallback") can be used for calculation and modification
Callbacks are only evaluated once at the end of a successful time step, so modifications made will take effect on the next step.
You could even use the callback of one agent to modify the properties of another agent, or destroy the calling agent (though this should be the last action taken in the callback to avoid dereferencing `NULL`).

### Network callbacks (@ref NDXLNetworkCallback "NDXLNetworkCallback") can be used for calculation and modification
A network callback is like Annie Oakley: anything the agent callback can do, the network callback can do better.

Network callbacks, as we have seen before, are primarily used to capture simulation output over the whole network as once.
In the context of dynamic networks, they are also typically used to

- Update the states of agents based on network wide rules
- Evaluate agents and add or remove agents based on certain criteria
- Reconfigure the topology by changing edge properties

## Failing Vertices {#failvert}
As an introductory example, let us consider a network with \f$10\f$ homogeneous vertices whose state vectors evolve according to

\f[
    \frac{dx}{dt} = xt
\f]
\f[
    x_0 = 1
\f]

Each vertex is then given a unique "upper limit" to their state, uniformly random between \f$0.5\f$ and \f$7\f$.

```c
// ...

int dynamics(NDXLVertex* vert, double* dx, const double* x, double t, void* p)
{
	dx[0] = x[0] * t;
	return 0;
}

int main(int argc, char** argv)
{
	ndxlInit();
	NDXLNetwork* nwk = ndxlCreateNetwork(NULL);
	
	// Prepare initial state and space for each vertex's `void* params`
	double x0 = 1.0;
	double ulim[20];
	
	// Create vertices
	for (int i = 0; i < 10; i++)
	{
		// Define the upper limit
		ulim[2 * i] = randdbl(0.5, 7.0);
		
		// Give each vertex an ID for callback output (see below)
		ulim[2 * i + 1] = i;
		
		ndxlCreateVertex(nwk, &x0, 1, dynamics, NULL, NULL, 0, NULL, ulim + 2 * i);
	}
}
```

What makes this network dynamic is the upper limit: if the state of a vertex reaches or exceeds this limit, the vertex "fails" and is removed from the network.
To do this, we'll define a vertex callback which destroys its called if this condition is met.

```c
// ...

int callback(NDXLVertex* vert, double* x, double t, void* p)
{
	// Extract limit and ID from `void* params`
	double limit = ((double*)p)[0];
	int id = (int)(((double*)p)[1]);
	
	if (x[0] >= limit)
	{
		printf("Vertex %d destroyed at t = %f\n", id, t);
		ndxlDestroyVertex(&vert);
	}
	
	// Importantly, we do not operate on `vert` after it has been destroyed
	return 0;
}

// ...
```

We can then set the callback of each vertex either by modifying the creation loop...

```c
// ...

int main(int argc, char** argv)
{
	ndxlInit();
	
	// ...
	
	// Create vertices
	for (int i = 0; i < 10; i++)
	{
		ulim[2 * i] = randdbl(0.5, 7.0);
		ulim[2 * i + 1] = i;
		ndxlCreateVertex(nwk, &x0, 1, dynamics, NULL, NULL, 0, callback, ulim + 2 * i);
	}
}
```

...or post-creation using @ref ndxlSetVertexCallback().

```c
// ...

int main(int argc, char** argv)
{
	ndxlInit();
	
	// ...
	
	// Create vertices
	for (int i = 0; i < 10; i++)
	{
		ulim[2 * i] = randdbl(0.5, 7.0);
		ulim[2 * i + 1] = i;
		ndxlCreateVertex(nwk, &x0, 1, dynamics, NULL, NULL, 0, NULL, ulim + 2 * i);
	}
	
	// Assign callbacks
	for (NDXLVertex* vert = ndxlGetNetworkFirstVertex(nwk); vert; vert = ndxlGetNetworkNextVertex(vert))
		ndxlSetVertexCallback(vert, callback);
}
```

Before simulating the network, there is one thing we need to decide, namely, time scale.
Because `callback` is evaluated after each successful time step, a vertex could be destroyed much later than it is supposed to be; consider, for example:

- Vertex \f$1\f$ has state \f$3\f$ at time \f$2\f$ with a `limit` of \f$5\f$
- The next time step size is 1.7, taking us to time \f$3.7\f$
- At \f$t=3.7\f$, vertex \f$1\f$ has a state of \f$6\f$, meaning it will be destroyed
- But \f$6\f$ is so much larger than \f$5\f$ that it should have been destroyed quite some time before \f$3.7\f$

Ordinarily this is solved with a continuous condition checker e.g., continuous callbacks in Julia's <b>[`DifferentialEquations.jl`](https://docs.sciml.ai/DiffEqDocs/stable/)</b>, or events in Python's <b>[`SciPy`](https://scipy.org/)</b>.
However, NDXL does not (yet) have a mechanism for this, so we must instead poll the callback on such a small time scale that the time of each vertex's destruction is sufficiently accurate.
We could do this in one of two ways:

- Use an adaptive method with time stops at regular intervals
- Use a static method with a very small step size

Efficiency points us to the second option, so we will use @ref ndxlNetworkSimulateRalston() with \f$h=10^{-3}\f$.
We will also print the number of vertices in `nwk` afterwards to verify the agents have all been destroyed.

```c
// ...

int main(int argc, char** argv)
{
	ndxlInit();
	
	// ...
	
	ndxlNetworkSimulateRalston(nwk, 2.0, 1e-3, NULL);
	printf("After simulation, network has %d vertices left\n", ndxlGetNetworkOrder(nwk));

	return 0;
}
```

The simulation outputs to `stdout`:

```
Vertex 5 destroyed at t = 0.001000
Vertex 4 destroyed at t = 0.544000
Vertex 3 destroyed at t = 0.967000
Vertex 0 destroyed at t = 1.098000
Vertex 8 destroyed at t = 1.724000
Vertex 7 destroyed at t = 1.750000
Vertex 6 destroyed at t = 1.821000
Vertex 2 destroyed at t = 1.919000
Vertex 9 destroyed at t = 1.929000
Vertex 1 destroyed at t = 1.960000
After simulation, network has 0 vertices left
```

To see how one failure can induce another, and how agents can be re-added to a network, see @ref dynpow.