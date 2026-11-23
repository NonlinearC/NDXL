## Injecting Perturbations {#callbacks}
\note This page assumes familiarity with the basic layout and API usage described in the main @ref examples page.

### The full code for this example can be found [here](https://github.com/NonlinearC/NDXL/tree/main/NDXL/examples/kuramoto-model/).

One of the most intuitive introductory examples to simulating network dynamics is the Kuramoto model, which we briefly illustrated on the [C API homepage](https://github.com/NonlinearC/NDXL/tree/main/NDXL).
The example shown here includes a perturbation of one of the oscillators to hint at the discrete callback functionality of NDXL.
The Kuramoto model describes the phase \f$\theta\f$ evolution of an oscillator with eigenfrequency \f$\omega\f$ as

\f[
    \frac{d\theta_i}{dt} = \omega_i + \frac{1}{N_i}\displaystyle\sum_{j=1}^N{K_{ij}\sin{\left(\theta_j - \theta_i\right)}}.
\f]

Here, \f$N_i\f$ is the degree of oscillator \f$i\f$ (how many nearest neighbours it has), and \f$K_{ij}\f$ is the coupling strength between oscillators \f$i\f$ and \f$j\f$.
As usual, we start by defining the dynamics of the oscillators (vertices).
Similar to @ref network_traversal functions, the edges and neighbouring vertices of a vertex can be iterated over in NDXL using its @ref vertex_traversal functions.
We will also use @ref ndxlGetVertexDegree() to query the degree of each oscillator.

```c
#include <math.h>

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
```

Next, let us define the perturbation callback shown in the paper, which set the phase of the target oscillator to \f$0\f$ at \f$t=2.5\f$.
Because discrete callbacks are called after each successful integration step, we cannot simply alter the state vector; we must ensure the called time is correct first.

```c
int perturbation(NDXLVertex* oscillator, double* theta_i, double time, void* params)
{
	if (time == 2.5)
		theta_i[0] = 0.0;
	return 0;
}
```

We create the oscillators next, applying the callback only to the first oscillator.
Assigning the callback can either be done during vertex creation:

```c
// ...

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
		kuramoto,		// Assign Kuramoto dynamics
		NULL,
		NULL,
		0,
		perturbation,	// Assign perturbation
		&omega[0]		// Eigenfrequency
	);
	
	// Do the rest of the oscillators
	for (int i = 1; i < 4; i++)
	{
		theta0 = randdbl(-PI, PI);
		omega[i] = randdbl(0.9, 1.1);
		oscillators[i] = ndxlCreateVertex(nwk, &theta0, 1, kuramoto, NULL, NULL, 0, NULL, &omega[i]);
	}
	
	return 0;
}
```

Or after the fact using @ref ndxlSetVertexCallback() :

```c
// ...

int main(int argc, char** argv)
{
	// ...
	for (int i = 0; i < 4; i++)
	{
		double theta0 = randdbl(-PI, PI);
		omega[i] = randdbl(0.9, 1.1);
		oscillators[i] = ndxlCreateVertex(nwk, &theta0, 1, kuramoto, NULL, NULL, 0, NULL, &omega[i]);
	}
	ndxlSetVertexCallback(oscillators[0], perturbation);
	
	return 0;
}
```

## Defining the Topology {#callbacks-topologies}
Before we can simulate our Kuramoto network, we need the edges that define the topology.
Failing this, the oscillators will have no neighbours/edges to iterate over, and even worse, @ref ndxlGetVertexDegree() will return \f$0\f$, resulting in division by \f$0\f$!
We will create a ring network with adjacency matrix

\f[
\begin{array}{|cccc|}
    0 & 1 & 1 & 0 \\
    1 & 0 & 0 & 1 \\
    1 & 0 & 0 & 1 \\
    0 & 1 & 1 & 0 \\
\end{array}
\f]

Although there are eight entries, we will only have four edges, each connecting a unique pair of vertices.
The "duplicate" entries are a result of the edges being undirected i.e., if oscillator \f$i\f$ is connected to \f$j\f$, then oscillator \f$j\f$ is also connected to oscillator \f$i\f$ via the same edge.

```c
// ...

int main(int argc, char** argv)
{
	// ...
	
	// Coupling constant K_ij (made identical for all edges)
	double K = 1.0;
	
	// Define topology
	ndxlCreateEdge(
		oscillators[0],	// "Source" vertex (interchangeable with "Destination" in undirected edge)
		oscillators[1],	// "Destination" vertex (interchangeable with "Source" in undirected edge)
		&K,				// State vector
		1,				// Length of state vector
		0.0,			// No delay (see the "Adding Delays" page)
		0,				// Not directed (if this value is nonzero, information only flows from "Source" to "Destination" but not the other way around)
		NULL,			// No dynamics (static edge)
		NULL,			// No noise
		NULL,			// No noise channels (must be NULL if noise is NULL)
		0,				// Number of noise channels (must be 0 if noise is NULL)
		NULL,			// No edge callback
		NULL			// No extra `void* params`
	);
	ndxlCreateEdge(oscillators[0], oscillators[2], &K, 1, 0.0, 0, NULL, NULL, NULL, 0, NULL, NULL);
	ndxlCreateEdge(oscillators[1], oscillators[3], &K, 1, 0.0, 0, NULL, NULL, NULL, 0, NULL, NULL);
	ndxlCreateEdge(oscillators[2], oscillators[3], &K, 1, 0.0, 0, NULL, NULL, NULL, 0, NULL, NULL);
}
```

We can now simulate our network as before; however, as mentioned in the @integration section, adaptive methods like @ref ndxlNetworkSimulateDopri() do not guarantee that our target time in our `perturbation` callback will be evaluated.
We therefore have two options for ensuring the condition `time == 2.5` eventually evaluates to true:

- Use a static method with a step size \f$h\f$ such that \f$2.5\mod h=0\f$
- Use the time stops parameter provided by adaptive methods

The first option works in theory, however, there is a computational issue, namely, floating point rounding.
Without convoluting the details:

- Computers have limited precision with which to represent decimal numbers, meaning repeated floating point addition accumulates rounding errors
- This means that, for a hypothetical \f$h=0.1\f$, `time` after the first integration step would be \f$0+0.1=0.1\f$, however, after the third step it would be \f$0.30000000000000004\f$
- So our theory of using \f$2.5\mod h=0\f$ is not wrong, but in programming \f$0.3\neq0.30000000000000004\f$ - the same might hold true for testing `time == 2.5`

\note In NDXL, networks and agents accumulate time differently. In a static method, agents use repeated addition, which magnifies their floating point error, while networks calculate their current time as their start time plus the number of steps multiplied by the step size. This makes network time "technically" more accurate, but still not fully reliable!

The second approach therefore appears to be our only option.
The `_Tstops` parameter, like a state vector, is an array containing a list of times the solver must evaluate; the array must be sorted and lie within the interval \[@ref ndxlGetNetworkTime(), @ref ndxlGetNetworkTime() + _Tspan\], where `_Tspan` is the simulation duration (second argument to the simulate function).
We must also provide the length of the time stops vector, which in this case is \f$1\f$.

```c
// ...

int main(int argc, char** argv)
{
	// ...
	
	// Define the time stops array
	double tstops = 2.5;
	
	// Cash-Karp would also be a suitable choice here
	ndxlNetworkSimulateDopri(
		nwk,
		5.0,		// Simulation duration; the time stops array must be between the current network time (which starts at 0) and that time plus the duration
		1e-6,
		1e-3,
		&tstops,	// Time stops array
		1,			// Length of time stops array
		cb
	);
	
	return 0;
}
```

Our output verifies the perturbation at `time == 2.5`:

![](@ref kuramoto.png)