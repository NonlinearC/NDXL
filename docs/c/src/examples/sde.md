# Adding Stochasticity {#sde}
\note This page assumes familiarity with the basic layout and API usage described in the main @ref examples page.

The preceeding examples have all looked at vertices whose agents evolve deterministically i.e., repeated runs of the same simulation result in the same outputs and have the same analytical solution.
This is not the case for stochastic differential equations (SDEs), which are explicitly designed to simulate agents that have an element of randomness in their dynamics.
Before we can begin solving SDEs, we need to understand several key concepts used in their definitions.

### Brownian Path
The random fluctuations in an SDE are assumed to follow Brownian motion, and the individual microscopic fluctuations over time are collectively referred to as a Wiener path.
The system can have multiple Wiener paths simultaneously influencing its stochastic motion, and these are called noise channels.
NDXL explicitly defines a single noise channel via the @ref NDXLChannel object i.e., each @ref NDXLChannel is an independent Wiener process.
This makes local @ref NDXLVertexNoise "noise functions" in NDXL only responsible for returning the \f$D^\prime\times M^\prime\f$ diffusion submatrix of the agent they are associated with, where \f$D^\prime\f$ and \f$M^\prime\f$ are respectively the state vector cardinality and number of noise channels of that agent.

### Drift and Diffusion
An SDE for a \f$D\f$-dimensional state vector \f$\vec{X}\f$ is defined most generally as

\f[
	d\vec{X} = f(\vec{X},~t)dt + g(\vec{X},~t)dW
\f]

Here, \f$f\f$ is called the deterministic drift function, \f$g\f$ is the stochastic diffusion matrix, and \f$W\f$ (\f$dW\sim\mathcal{N}(0,~h)\f$) is a Wiener process (increment).
Multiple \f$M\f$ independent noise sources can influence the system simultaneously, so the diffusion matrix has dimensions \f$D\times M\f$, whose entries \f$g_{ij}\f$ are the amplitude of noise channel \f$j\f$ on component \f$i\f$ of the state vector.
This interpretation has several important consequences for agent-based modelling:

- A single noise channel can influence multiple elements of the same state vector
- A single element in a state vector can be influenced by multiple noise channels
- Agents can share noise channels independently of their dynamics or state vectors

### Path Preservation and RSwM
For static stepping algorithms like @ref ndxlNetworkSimulateEuler "Euler-Maruyama", steps are taken to be successful regardless of the local error or behaviour of the Brownian path.
This makes them far more inaccurate than their adaptive counterparts, and should only ever be used as a "preview" of the stochastic output (assuming a very small step size e.g., \f$h\sim10^{-5}\f$) rather than a complete simulation output.

Adaptive algorithms like @ref ndxlNetworkSimulateHeun "Heun-Euler" estimate local error based on user-defined tolerances, and this is where the stochasticity of the system becomes problematic.
If a step is rejected in a deterministic ODE, we know it was because the difference between the two solutions was too large over that step, and the step is simply reduced and retried.
In SDEs, however, the deterministic drift functions could agree nearly perfectly, but the stochastic diffusion components cause the paths to diverge too much, raising the local error.
This means a step can be rejected effectively by random chance.

However, the rejection event itself cannot simply be discarded as a "chance effect"; if we simply reject the step and retry with a fresh Wiener increment, we lose information about why the step was initially rejected, which introduces statistical bias into the Wiener path as a whole.
This is where an algorithm called <i>Rejection Sampling with Memory</i> (RSwM) comes in.
Without going into too much detail, RSwM constructs a <i>Brownian bridge</i> between the rejected step and previously accepted step, which allows for a deterministic interpolation of an intermediate step that should be attempted first, followed by a step that takes us back to the originally rejected time point.
This preserves the statistical integrity and distribution properties of the underlying Wiener process, making adaptive stochastic methods far more suitable for actual simulations involving SDEs.

### It&ocirc; and Stratonovich Interpretations
A further complication with SDE solvers is the calculus behind the solving method, which we eluded to in the @ref meth-stochastic section of the @ref concepts page.
Briefly, the two main schemes are the It&ocirc and Stratonovich interpretations, and the differences between them come down to the mathematical properties preserved:

- It&ocirc; evaluates the integral on the left-hand side of the interval, preserving the martingale property but not the chain rule
- Stratonovich evaluates the integral at the midpoint of the interval, preserving the chain rule but not the martingale property

Fortunately, switching between the schemes is rather simple, requiring only a correction term in the drift function.
This "drift correction term" is defined for component \f$i\f$ of the state vector \f$\vec{X}\f$ as

\f[
    \frac{1}{2}\displaystyle\sum_{j=1}^M{\displaystyle\sum_{j=1}^D{g_{kj}(\vec{X},~t)\frac{\partial g_{ij}(\vec{X},~t)}{\partial x_k}}}
\f]

If solving a Stratonovich equation using an It&ocirc; solver, add the correction to the drift...

\f[
    dX_i = \left[f_i(\vec{X},~t) + \frac{1}{2}\displaystyle\sum_{j=1}^M{\displaystyle\sum_{j=1}^D{g_{kj}(\vec{X},~t)\frac{\partial g_{ij}(\vec{X},~t)}{\partial x_k}}}\right]dt + \displaystyle\sum_{j=1}^M{g_{ij}(\vec{X},~t)dW_j}
\f]

...and subtract it if using a Stratonovich solver to solve an It&ocirc; equation.

## Particle Ensemble
Let us now simulate an ensemble of Ornstein-Uhlenbeck particles, which models the position \f$x\f$ of particle \f$i\f$ as

\f[
    dx_i = -\Gamma x_i{\rm d}t + \sigma dW_i.
\f]

From the equation, we can see that all particles are affected by only one Wiener process, but the process driving each particle is unique.
We start by defining our drift and diffusion functions, which correspond to the dynamics and noise functions in NDXL, respectively.
Where the dynamics function's output was an array with length \f$D\f$, the noise function's output is a flattened matrix (array) with dimensions (length) \f$D\times M\f$

```c
int drift(NDXLVertex* particle, double* dx, const double* x, double t, void* p)
{
	double Gamma = ((double*)p)[0];
	dx[0] = -Gamma * x[0];
	return 0;
}

int diffusion(NDXLVertex* particle, double* dg, const double* x, double t, void* p)
{
	double sigma = ((double*)p)[1];
	dg[0] = sigma;
	return 0;
}
```

Because each particle's Wiener driver is unique, we do not have to create any noise channels.
Instead, we can simply specify the noise function and number of noise channels, leaving the `_Channels` argument itself `NULL`, which NDXL interprets as "create `_Nchannels` new noise channels" (see @ref ndxlCreateVertex()).

```c
// ...

int main(int argc, char** argv)
{
	ndxlInit();
	
	// ...
	
	// Define Gamma, sigma, and initial position
	double Gs[2] = { 1.5, 1.0 };
	double x0 = -1.0;
	
	// Create 1000 particles all starting at x0
	for (int i = 0; i < 1000; i++)
	{
		ndxlCreateVertex(
			nwk,
			&x0,
			1,
			drift,		// Dynamics
			diffusion,	// Noise
			NULL,		// We did not create any channels, so we leave this as NULL to specify "all channels must be new and independent"
			1,			// Each particle is driven by only one Wiener process (channel)
			NULL,
			&Gs
		);
	}

	return 0;
}
```

Because our network has stochastic agents, NDXL will raise a @ref NDXL_SIM_ERR if we try to simulate the network using one of the deterministic methods shown previously (similarly, one cannot use the stochastic methods on a purely deterministic network).
As explained above, we want to use an adaptive method so that the RSwM algorithm's path preservation is in effect.
Similar to the @ref highorder "N-Body example", we will also define a series of time stops at which to take snapshots of the simulated output.

```c
// ...

int main(int argc, char** argv)
{
	ndxlInit();
	
	// ...
	
	double tstops[6] = { 2.0, 4.0, 6.0, 8.0, 10.0, 12.0 };
	ndxlNetworkSimulateHeun(nwk, 14.0, 1e-3, 1e-3, tstops, 6, cb);

	return 0;
}
```

## Specifying noise channels
We will modify our Ornstein-Uhlenbeck ensemble above to have all particles driven by the same Wiener process; what we expect is the initial distribution to simply change position, not shape, as all particles are perturbed by the same random amount each time step.
To achieve this, we will create a @ref NDXLChannel before the vertices and assign it as the only channel to each vertex's `_Channels` array.

```c
// ...

int main(int argc, char** argv)
{
	ndxlInit();
	
	// ...
	
	// Define NDXLChannel object
	NDXLChannel* channel = ndxlCreateChannel(
		nwk,	// Host network; channel can only influence vertices and edges in this network
		NULL	// No extra `void* params` data
	);
	
	// Create 1000 particles using the same channel
	for (int i = 0; i < 1000; i++)
	{
		ndxlCreateVertex(
			nwk,
			&x0,
			1,
			drift,		// Dynamics
			diffusion,	// Noise
			&channel,	// Must be an array of NDXLChannel pointers, hence the use of the addressof operator
			1,			// Each particle is still only driven by one Wiener process (now defined by `channel`)
			NULL,
			&Gs
		);
	}
	
	// ...

	return 0;
}
```

Interestingly, the analytical solution to this model approaches a Gaussian distribution with zero mean, and variance \f$\Sigma^2=\frac{\sigma^2}{2\Gamma}\f$.
We can therefore compare our distributions (blue) to the analytical soltuion (orange) over time, as shown in the images below.

\note The analytical solution is only valid if all particles are driven by independent Wiener processes; for a shared process, the variance is zero, and the distribution collapses into a Dirac-delta distribution that moves randomly about the mean.

<center>
<table>
  <tr>
    <td align="center">
      @image html ou.png "Independent Wiener processes"
    </td>
    <td align="center">
      @image html ou_1chan.png "Shared Wiener process"
    </td>
  </tr>
</table>
</center>