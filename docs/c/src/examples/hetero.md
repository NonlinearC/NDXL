# Networks With Heterogeneous Agents {#hetero}
\note This page assumes familiarity with the basic layout and API usage described in the main @ref examples page.

### The full code for this example can be found [here](https://github.com/NonlinearC/NDXL/tree/main/NDXL/examples/heterogeneous-dynamics/).

As explained in the @ref hetero-dynamics section, agents in a network do not necessarily need to share dynamics, noise, or discrete callback functions.
Whenever any of these are distinct for any two agents, the network is said to exhibit heterogeneous dynamics.
Let us modify the basic example shown in the @ref multi section to have the three vertices instead evolve according to

\f[
    \frac{dx_1}{dt} = Ax_1t,
\f]
\f[
    \frac{d\vec{x}_2}{dt} = B\sin{x_2^{[1]}} - Dx_2^{[2]}\cos{t},
\f]
\f[
    \frac{dx_3}{dt} = \begin{cases}
    0 & \text{if}~t < 1 \\
    t & \text{otherwise}
    \end{cases}
\f]

To further emphasise heterogeneity, the second vertex has a two-element state vector, and the third evolves according to a piecewise function.
As before, we simply define the different equations in separate dynamics functions.

```c
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
```

The network callback for writing output to a file has identical structure to what has been shown in previous examples, so we will not repeat it here.
In `main()`, the vertices can either be assigned the dynamics functions as we have been doing up to now...

```c
// ...

int main(int argc, char** argv)
{
	ndxlInit();
	
	// ...
	
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
	
	return 0;
}
```

...or we can set the dynamics after creation using @ref ndxlSetVertexDynamics.
We could take this a step further, and delay initial state and `void* params` assignment until after creation as well, using @ref ndxlVertexState and @ref ndxlSetVertexParams to update them, respectively.
The only caveat is to ensure we pass 2 for the size of vertex 2's state vector, as the size cannot be changed later.

```c
// ...

int main(int argc, char** argv)
{
	ndxlInit();
	
	// ...
	
	// Create three empty, stateless, static vertices
	// For vertices 1 and 3, a state size of 0 or 1 both result in a zero-vector of length 1
	// For vertex 2, we MUST specify the size to be 2
	NDXLVertex* v1 = ndxlCreateVertex(nwk, NULL, 0, NULL, NULL, NULL, 0, NULL, NULL);
	NDXLVertex* v2 = ndxlCreateVertex(nwk, NULL, 2, NULL, NULL, NULL, 0, NULL, NULL);
	NDXLVertex* v3 = ndxlCreateVertex(nwk, NULL, 1, NULL, NULL, NULL, 0, NULL, NULL);
	
	// Create extra parameters and assign them using the parameters setter
	double ABD[3] = { 1.6, 2.3, 0.7 };
	ndxlSetVertexParams(v1, ABD);
	ndxlSetVertexParams(v2, ABD + 1);
	
	// Assign initial conditions for the vertices - since v3 has x3_0 = 0, we don't have to update it at all!
	ndxlVertexState(v1)[0] = 1.0;
	ndxlVertexState(v2)[0] = 0.5;
	ndxlVertexState(v2)[1] = 0.5;
	
	// Assign the dynamics functions
	ndxlSetVertexDynamics(v1, dyn1);
	ndxlSetVertexDynamics(v2, dyn2);
	ndxlSetVertexDynamics(v3, dyn3);
	
	return 0;
}
```

We simulate the network again using the Cash-Karp integrator, with a time stop at \f$t=1\f$ to ensure the piecewise dynamics of `v3` transition at the appropriate time.

```c
// ...

int main(int argc, char** argv)
{
	ndxlInit();
	
	// ...
	
	double tstop = 1.0;
	ndxlNetworkSimulateRkck(nwk, 5.0, 1e-6, 1e-3, &tstop, 1, cb);
	
	return 0;
}
```

![](@ref hetero.png)

## Single-Agent Heterogeneity

### The full code for this example can be found [here](https://github.com/NonlinearC/NDXL/tree/main/NDXL/examples/lotka-volterra/).

The above example is one way to simulate heterogeneous dynamics, but not the only way.
One could just as easily combine the state vectors of all the vertices into a single, contiguous vector, and define the heterogeneous dynamics in the dynamics function.
The code is identical to @ref highorder, and indeed one could consider a coupled system of ODEs as having distinct dynamics equations, because the right-hand side of the equation differs for each variable.
As an example, consider a Lotka-Volterra type interaction between a predator \f$N_{pred}\f$ and prey \f$N_{prey}\f$ population, given by

\f[
    \frac{{\rm d}N_{prey}}{{\rm d}t} = \alpha N_{prey} - \beta N_{prey}N_{pred},
\f]
\f[
    \frac{{\rm d}N_{pred}}{{\rm d}t} = \delta N_{prey}N_{pred} - \gamma N_{pred}.
\f]

Instead of defining separate vertices for the populations, we shall use a single vertex whose state vector contains both populations.
In our dynamics function, we then simply write either equation to its corresponding index in the output vector.

```c
int lv(NDXLVertex* pops, double* dN, const double* N, double t, void* p)
{
	double alpha = ((double*)p)[0];
	double beta = ((double*)p)[1];
	double delta = ((double*)p)[2];
	double gamma = ((double*)p)[3];
	
	// Prey equation
	dN[0] = alpha * N[0] - beta * N[0] * N[1];
	
	// Predator equation
	dN[1] = delta * N[0] * N[1] - gamma * N[1];
	
	return 0;
}
```

Then in `main()` we set up the network as usual, this time having only one vertex.
Output capturing remains semantically unchanged, though we will use a static stepping simulation method here instead to illustrate the parameter differences.

```c
// ...

int main(int argc, char** argv)
{
	ndxlInit();
	
	// ...
	
	double N0[2] = { 40.0, 9.0 };
	double ABDG[4] = { 1.0, 0.1, 0.075, 1.5 };
	ndxlCreateVertex(nwk, N0, 2, lv, NULL, NULL, 0, NULL, ABDG);
	
	// Simulate with the static Ralston method of fourth order
	ndxlNetworkSimulateRalston(
		nwk,
		20.0,
		0.1,	// Step size
		cb
	);
	
	return 0;
}
```

![](@ref lv.png)