# Simulating Higher-Order Dynamics {#highorder}
\note This page assumes familiarity with the basic layout and API usage described in the main @ref examples page.

Suppose we want to simulate a network of interacting objects whose dynamics are defined by second-order differential equations \f$\frac{d^2\vec{x}}{dt^2}\f$.
A famous example of this is the \f$N\f$-body problem, in which the motion of each body is driven by the forces exerted on it by the other \f$N-1\f$ bodies, similar to the Kuramoto model (though the coupling is fundamentally different).
The \f$i^{th}\f$ body's position vector \f$\vec{r}\f$ evolves according to

\f[
    \frac{d^2\vec{r}_i}{dt^2} = -G \sum_{\substack{j=1 \\ j \neq i}}^{N}{m_j \frac{\vec{r}_i - \vec{r}_j}{|\vec{r}_i - \vec{r}_j|^3}},
\f]

where \f$G\f$ is the <b>[gravitational constant](https://physics.nist.gov/cgi-bin/cuu/Value?bg)</b>, and \f$m\f$ is the mass of the body.
To solve this, we transform the second-order equation into a set of two coupled first-order differential equations, by means of a change of variables, where the derivative of the first variable will be fed into the solution of the second.
We define two auxillary variables \f$u\f$ and \f$v\f$ such that

\f[
    \vec{u}=\vec{r},
\f]

and

\f[
    \vec{v}=d\vec{u}=d^2\vec{r}.
\f]

Then the chain rule gives their derivatives

\f[
    d\vec{u}=d\vec{r}=\vec{v}
\f]

and

\f[
   d\vec{v} = -G \sum_{\substack{j=1 \\ j \neq i}}^{N}{m_j \frac{\vec{u}_i - \vec{u}_j}{|\vec{u}_i - \vec{u}_j|^3}},
\f]

which are coupled and first-order.
Because we now have effectively twice the number of differential equations, our state vector also doubles in size; instead of simply describing position, it must describe the position and velocity.

\note This process can be generalised to \f$n^{th}\f$-order equations, where the derivative of the \f$i^{th}\f$ variable is used to solve the derivative of the \f$(i+1)^{th}\f$ variable.

Since \f$G\f$ is constant for all bodies, it does not make sense to assign the `void* params` of each vertex to the same `G` pointer, and we will reserve those for the mass of the bodies.
Instead, we will make \f$G\f$ part of the network's parameters by introducing a custom struct and passing a pointer to it.
Additionally, the scale of the variables involved differ by several orders of magnitude, making numerical stability over even short simulations becomes difficult to achieve, so we shift to a unitised reference frame  where \f$G=m_1=m_2=\dots=m_N=1\f$.

```c
#include <stdio.h>

typedef struct
{
	double G,
	FILE* f_output
} nbody_params;

int main(int argc, char** argv)
{
	ndxlInit();
	
	// Initialise custom params struct
	nbody_params params;
	params.G = 1.0;
	params.f_output = fopen("nbody.csv", "w");
	if (!params.f_output)
		return 1;
	
	// Print "time" header to output file, we will do the rest later in the vertex creation loop
	fprintf(params.f_output, "t");
	
	// Assign params to network
	NDXLNetwork* nwk = ndxlCreateNetwork(&params);
	return 0;
}
```

We will demonstrate here the \f$N\f$-body problem in two dimensions, meaning our state vector has four elements: the first two are the position coordinates, and the second two are the velocities along those axes.
Because each body depends on all other bodies, it would be a waste of memory and time to explicitly define the all-to-all coupling topology; instead, we shall query each body's "parent network" using @ref ndxlGetVertexNetwork(), which we can then use to iterate over all other bodies to compute the interaction sum.

```c
// ...

typedef struct
{
	double G;
	FILE* f_output;
} nbody_params;

int dynamics(NDXLVertex* body, double* dr_i, const double* r_i, double t, void* p)
{
	// Get parent network and unravel state vector components
	NDXLNetwork* parent = ndxlGetVertexNetwork(body);
	double u_xi = r_i[0];
	double u_yi = r_i[1];
	double v_xi = r_i[2];
	double v_yi = r_i[3];
	
	// Iterate over all vertices in the parent network
	double coupling[2] = { 0.0, 0.0 };
	for (NDXLVertex* vert = ndxlGetNetworkFirstVertex(parent); vert; vert = ndxlGetNetworkNextVertex(vert))
	{
		// Ensure `vert` does not refer to the current vertex `body`
		if (body != vert)
		{
			// ndxlVertexState(vert) provides direct access to the Runge-Kutta stage vector, making these operations inexpensive
			double u_xj = ndxlVertexState(vert)[0];
			double u_yj = ndxlVertexState(vert)[1];
			
			// Calculate difference and magnitude of difference
			double diff[2] = { u_xi - u_xj, u_yi - u_yj };
			double mag = sqrt(diff[0] * diff[0] + diff[1] * diff[1]);
			
			// Extract mass of `vert` from its `void* params` pointer
			double m_j = *(double*)ndxlGetVertexParams(vert);
			
			// Update coupling sums
			coupling[0] += m_j * diff[0] / (mag * mag * mag);
			coupling[1] += m_j * diff[1] / (mag * mag * mag);
		}
	}
	
	// Write out the position and velocity derivatives
	dr_i[0] = v_xi;
	dr_i[1] = v_yi;
	dr_i[2] = -(*(nbody_params*)ndxlGetNetworkParams(parent)).G * coupling[0];
	dr_i[3] = -(*(nbody_params*)ndxlGetNetworkParams(parent)).G * coupling[1];
	return 0;
}

int main(int argc, char** argv)
{
	ndxlInit();
	
	// ...
	
	return 0;
}
```

We define our bodies with random uniform initial positions on a circular orbit between $8$ and $10$ distance units away, and initial velocities perpendicular to the initial position vectors from the origin.

```c
#include <math.h>

// ...

int main(int argc, char** argv)
{
	// ...
	
	// m=1 for all agents so we can define it once here
	double mass = 1.0;
	
	for (int i = 0; i < 10; i++)
	{		
		double dist = randdbl(8.0, 10.0);
		double ang = 0.2 * i * PI;
		double r0[4] = { dist * cos(ang), dist * sin(ang), 0.5 * dist * cos(ang + 0.5 * PI), 0.5 * dist * sin(ang + 0.5 * PI) };
		ndxlCreateVertex(nwk, r0, 4, dynamics, NULL, NULL, 0, NULL, &mass);
		
		// Write headers to output file
		fprintf(params.f_output, ",x%d,y%d,vx%d,vy%d", i, i, i, i);
	}
	
	// Remember to append a newline to the output file
	fprintf(params.f_output, "\n");
}
```

We will then simulate the network for a duration of \f$4.5\f$, with time stops at \f$1.5\f$ and \f$3\f$ to get four evenly spaced snapshots of the bodies' orbits.

```c
// ...

int cb(NDXLNetwork* nwk, double t, void* p)
{
	// Check if we are at a time stop before writing out to file
	if ((t == 0.0) || (t == 1.5) || (t == 3.0) || (t == 4.5))
	{
		// Get handle from network's `void* params` and print the current time to it
		FILE* handle = (*(nbody_params*)p).f_output;
		fprintf(handle, "%f", t);
		
		// Iterate over vertices and write output to the file
		for (NDXLVertex* body = ndxlGetNetworkFirstVertex(nwk); body; body = ndxlGetNetworkNextVertex(body))
			fprintf(handle, ",%f,%f,%f,%f", ndxlVertexState(body)[0], ndxlVertexState(body)[1], ndxlVertexState(body)[2], ndxlVertexState(body)[3]);
		
		// Append newline
		fprintf(handle, "\n");
	}
	// Return success indicator
	return 0;
}

int main(int argc, char** argv)
{
	ndxlInit();
	
	// ...
	
	// Define our time stops
	double tstops[2] = { 1.5, 3.0 };
	
	// We will use the Cash-Karp method this time for variety :)
	ndxlNetworkSimulateRkck(nwk, 4.5, 1e-6, 1e-3, tstops, 2, cb);
	
	// Close the file handle before exiting
	fclose((*(nbody_params*)ndxlGetNetworkParams(nwk)).f_output);
	return 0;
}
```

![](@ref nbody.png)