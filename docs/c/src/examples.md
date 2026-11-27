# Examples {#examples}
This page documents the `C` versions of the example simulations used in the NDXL publication, together illustrating the capabilities of NDXL.
For a brief introductory example using the Kuramoto model, see the <b>[NDXL source repository](https://github.com/NonlinearC/NDXL/tree/main/NDXL)</b>.

We recommend reading through the @ref installation guide before writing any code, as this section assumes a similar file structure, familiarity with library linking, and that call source code is placed in `main.c`.
We also strongly recommend first reading through the @ref concepts page, as we will assume you are familiar with the network dynamics-specific terminology.

\note This is NOT a replacement for the <b>[API documentation](topics.html)</b>, which provides detailed explanations on functions, objects, and typedefs used. The sections should be absorbed in tandem.

## Getting Started

### The full code for this example can be found [here](https://github.com/NonlinearC/NDXL/tree/main/NDXL/examples/getting-started/).

All NDXL programs <b>MUST</b> call @ref ndxlInit() before any NDXL functionality.
This function initialises crucial internal variables, and registers garbage collection functions that automatically and safely release any remaining NDXL objects when the program terminates.

\warning The garbage collection in NDXL handles <b>only</b> memory allocated by NDXL. Any memory alloctaed by the user using e.g., `malloc()`, `calloc()`, `realloc()`, must still be manually released by the user using `free()`.

The core object in NDXL is a @ref ndxl_network NDXLNetwork, which is a (initially empty) host object for creating vertices, edges, and noise channels, and which later becomes the target of @ref network_simulation "simulation" functions.
We will start by defining an empty network using @ref ndxlCreateNetwork(), which takes in a single parameter, a `void*` containing any extra data we want the network to use.
We will pass `0` for now, since we do not yet have any data the network should rely on.

```c
#include "../include/ndxl.h"

int main(int argc, char** argv)
{
	ndxlInit();
	NDXLNetwork* nwk = ndxlCreateNetwork(0);
	return 0;
}
```

We now need to populate `nwk` with some vertices, so that we have components to simulate.
Each vertex needs a @ref NDXLVertexDynamics "dynamics function" that describes how its state vector evolves over time \f$t\f$, expressed as a time ordinary differential equations (ODE).
For example, if we want a vertex wtih state vector \f$\vec{x}\f$ to evolve according to

\f[
\frac{d\vec{x}}{dt} = A\vec{x}t
\f]

where \f$A\f$ is some constant, and \f$\vec{x}\f$ has one element (is a scalar), then in NDXL we write:

```c
#include "../include/ndxl.h"

int dynamics(NDXLVertex* vertex, double* dx, const double* x, double t, void* params)
{
	double A = *(double*)params;
	dx[0] = A * x[0] * t;
	return 0;
}

int main(int argc, char** argv)
{
	// ...
}
```

\note The `//...` comments are used to indicate "same as before".

In our function, we exctract \f$A\f$ from the user-defined `void* params` that we will pass to the vertex constructor, and we write the differential equation in-place to the output vector `dx`.
In `main()`, we can now create the vertex, baring in mind that whatever `void* params` points to must persist in memory for the lifetime of the vertex, as NDXL does not copy these data.

```c
#include "../include/ndxl.h"

// ...

int main(int argc, char** argv)
{
	ndxlInit();
	NDXLNetwork* nwk = ndxlCreateNetwork(0);
	
	// Create `void* params` A, and initial conditions x0
	double A = 2.0;
	double x0 = 1.0;
	
	// Create vertex on `nwk`
	NDXLVertex* vert = ndxlCreateVertex(
		nwk,		// Host network
		&x0,		// Initial state vector - must be a pointer to double
		1,			// Length of state vector
		dynamics,	// Dynamics function the vertex state must evolve according to
		NULL,		// No noise function (see the "Adding Stochasticity" page)
		NULL,		// No noise channels (must be NULL if noise function is NULL)
		0,			// Number of noise channels (must be 0 if noise function is NULL)
		NULL,		// No discrete callback
		&A			// User-defined `void* params`
	);
	return 0;
}
```

We could now simulate `nwk`, but we have no way of retrieving the simulated outputs, as NDXL does not automatically store these.
To capture simulation output, we use a @ref network_callbacks "discrete callback" that operates on the network object.
Here, we will write the state vector of `vert` out to a CSV file at each evaluated time step.

```c
#include <stdio.h>

#include "../include/ndxl.h"

int cb(NDXLNetwork* net, double time, void* params)
{
	FILE* csvfile = (FILE*)params;
	NDXLVertex* vertex = ndxlGetNetworkFirstVertex(net);
	double x = ndxlVertexState(vertex)[0];
	fprintf(csvfile, "%f,%f\n", time, x);
	return 0;
}

// ...
```

Because the network only has one vertex, @ref ndxlGetNetworkFirstVertex() will give us the corresponding @ref NDXLVertex pointer.
Since this pointer is shared, we do not need to (and SHOULD NOT) release the associated reosources after we are done using it.
Here, the `void* params` refers to the parameters pointer associated with the network, which, if you'll recall from earlier, we set to `0`!
We therefore need to modify our network creation to have the output file as its `void* params`.

```c
#include <stdio.h>

#include "../include/ndxl.h"

// ...

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
	
	// ...
	
	fclose(f_output);
	return 0;
}
```

We are now ready to simulate our network using one of the @ref network_simulation "simulation" functions.
We will use @ref ndxlNetworkSimulateDopri, which uses the Dormand-Prince Runge-Kutta pair of order 5(4) to integrate the network agents.
Integration starts at the network's current local simulation time, which is set to `0` by default when we created the network, and we simply specify the duration of the simulation.
We can also verify the success of the simulation by checking against @ref NDXL_ERR.

```c
#include <stdio.h>

#include "../include/ndxl.h"

// ...

int main(int argc, char** argv)
{
	ndxlInit();
	
	// ...
	
	int result = ndxlNetworkSimulateDopri(
		nwk,	// Network to simulate
		2.5,	// Simulation duration
		1e-6,	// Absolute tolerance (number of correct decimal places)
		1e-3,	// Relative tolerance (number of correct digits)
		NULL,	// Time-stops array (forced evaluation times)
		0,		// Length of time-stops array (must be 0 if time-stops is NULL)
		cb		// Discrete callback (called once at t = 0, then again after each successful integration step)
	);
	
	// Verify success
	if (result == NDXL_ERR)
		ndxlPrintError();
	
	// ...
	return 0;
}
```

If the network successfully simulates, `states.csv` will contain two columns, namely, the evaluated times, and the state of `vert` at those times.
We can compare the outputs to the analytical solution of our dynamics function, which is

\f[
    x(t) = \exp{\left(\frac{A}{2}t^2\right)}
\f]

given that \f$x(t=0)=1\f$.
We plot the analytical and numerical solutions using a plotting library like <b>[matplotlib](https://matplotlib.org/)</b>.

```python
import matplotlib.pyplot as plt
import numpy as np

# Do not parse header labels
t_num, x_num = np.loadtxt("states.csv", delimiter=',', skiprows=1, unpack=True)

# Analyical solution
t_ana = np.linspace(0., 2.5, 50)
x_ana = np.exp(t_ana**2)

# Visualisation
plt.plot(t_num, x_num, label="Numerical solution")
plt.plot(t_ana, x_ana, ls='--', label="Analytical solution")
plt.xlabel("Time, $t$")
plt.ylabel("$x=f(x,~t)$")
plt.legend(loc="upper left")
plt.savefig("states.png", bbox_inches='tight')
```

![](@ref states.png)

Congratulations! You have successfully set up and simulated your first network in NDXL.
The remainder of the examples cover:
- Multiple agents: @subpage multi
- Injecting perturbations: @subpage callbacks
- Defining topologies: @subpage callbacks-topologies
- Higher-order dynamics: @subpage highorder
- Heterogeneous agent dynamics: @subpage hetero
- Delayed states: @subpage dde
- Stochastic agents: @subpage sde
- Dynamic topologies: @subpage dynnet and @subpage dynpow