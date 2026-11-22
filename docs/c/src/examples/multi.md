# Adding Multiple Agents {#multi}
\note This page assumes familiarity with the basic layout and API usage described in the main @ref examples page.

Adding multiple agents to a network allows us to simulate a wider variety of dynamical conditions.
For now, we will only modify the initial conditions and parameters of agents, but later we will see how to assign different dynamics, noise, and discrete callback functions to them and simulate everything together.
The most important point to remember when dealing with multiple agents is that each one's `void* params` must persist in memory as long as the agent is alive.
The same is not true for its initial state vector, as this is copied and stored internally by NDXL.
As a demonstration, let us modify the network we created in the @ref examples section to contain three vertices, each with different values for \f$A\f$ and \f$\vec{x}_0\f$.
Also for demonstration purposes, we will assume there exists a function `randdbl()` which takes in two parameters, `low` and `high`, and returns a random number in the interval \f$\left[low,~high\right]\f$.

```c
#include "../include/ndxl.h"

// ...

int main(int argc, char** argv)
{
	ndxlInit();
	
	// Modify the CSV file to capture outputs for three vertices
	FILE* f_output = fopen("states3.csv", "w");
	if (!f_output)
		return 1;
	fprintf(f_output, "t,x1,x2,x3\n");
	
	// Dedicate space for three vertices and their `void* params` pointers
	NDXLVertex* vertices[3];
	double A[3] = { 2.0, 0.5, 1.75 };
	
	// Create vertices in a loop
	for (int i = 0; i < 3; i++)
	{
		// State is copied so x0 does not need to stay in memory, unlike `A`
		double x0 = randdbl(0.5, 1.5);
		vertices[i] = ndxlCreateVertex(nwk, &x0, 1, dynamics, NULL, NULL, 0, NULL, &A[i]);
	}
	
	return 0;
}
```

Because we now have three vertices, we must also update our network callback to capture the outputs for all three.
We will iterate over the network vertices using the relevant iterators described in the @ref network_traversal section of the API.

```c
#include <stdio.h>

#include "../include/ndxl.h"

int cb(NDXLNetwork* net, double time, void* params)
{
	FILE* csvfile = (FILE*)params;
	fprintf(csvfile, "%f", time);
	
	// Iterate over vertices using network vertex iterators
	for (NDXLVertex* vertex = ndxlGetNetworkFirstVertex(net); vertex; vertex = ndxlGetNetworkNextVertex(vertex))
	{
		// Get state of current vertex and write it out to the file, separated by a comma
		double x = ndxlVertexState(vertex)[0];
		fprintf(csvfile, ",%f", x);
	}
	
	// Remember to append the newline character to the line we just wrote
	fprintf(csvfile, "\n");
	return 0;
}
```

The network can now be simulated as before.
We will also modify our plotting script to read the initial conditions, which will befirst row of the CSV after the headers, and formulate the analytical solutions based on that.

```python
import matplotlib.pyplot as plt
import numpy as np

# Do not parse header labels
t_num, x_num1, x_num2, x_num3 = np.loadtxt("states3.csv", delimiter=',', skiprows=1, unpack=True)
A = np.array([2.0, 0.5, 1.75])

# Extract x0 and compute integration constants
x0 = np.array([x_num1[0], x_num2[0], x_num3[0]])
c = np.log(x0)

# Analyical solutions
t_ana = np.linspace(0., 2.5, 50)
x_ana1 = np.exp(0.5 * A[0] * t_ana**2 + c[0])
x_ana2 = np.exp(0.5 * A[1] * t_ana**2 + c[1])
x_ana3 = np.exp(0.5 * A[2] * t_ana**2 + c[2])

# Visualisation
plt.plot(t_num, x_num1)
plt.plot(t_num, x_num2)
plt.plot(t_num, x_num3)
plt.plot(t_ana, x_ana1, ls='--')
plt.plot(t_ana, x_ana2, ls='--')
plt.plot(t_ana, x_ana3, ls='--')
plt.xlabel("Time, $t$")
plt.ylabel("$x=f(x,~t)$")
plt.savefig("states3.png", bbox_inches='tight')
```

![](@ref states3.png)