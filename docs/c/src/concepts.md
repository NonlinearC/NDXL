# Concepts {#concepts}
This section provides detailed descriptions of the broader terminology used to describe network dynamics, agent-based modelling, and numerical integration.
The @subpage nd section details API-specific terminology and implementation details.

## What is a Network?
A "network" is an abstraction of a (often) real/physical system to a graph, where vertices represent the resident components of the system, and edges represent the coupling/interactions between those components.
Three important network variables arise when discussing its layout:

- <b>Size</b>: the number of edges (directed and undirected) in a network
- <b>Order</b>: the number of vertices (nodes) in a network
- <b>Topology</b>: how the edges couple the vertices (represented by an \f$Order\times Order\f$ adjacency matrix)

Sparse topologies have very few or no edges, while dense topologies usually exhibit multiple edges per vertex.
A common rule of thumb is that a network is "sparse" if less than 50% of its adjacency matrix has nonzero entries, and "dense" otherwise.
The Irish power grid is a very intuitive example of a sparse network: we can represent individual generators and loads as vertices, power transmission lines as edges carrying power to and from the nodes, and the network has a very low edge to vertex ratio.
A solid atomic lattice is then an example of a dense network: the atoms are the vertices, with the edges representing the bonds between neighbouring atoms, and there is a relatively higher edge to vertex ratio.

The term "network dynamics" (see @subpage nd) refers to how the network's agents - vertices and edges - evolve over time, usually expressed as a set of time differential equations.
In a power grid, the dynamics are usually described by the swing equation; in an atomic lattice, by quantum harmonic oscillator models.
Generally speaking, the differential equations governing a network's dynamics can be:

- Multidimensional (the state of an agent is described by multiple evolving parameters): see @ref state-vector "State Vector"
- Heterogeneous (not every agent shares the same dynamics): see @ref hetero-dynamics
- Stochastic (noise signals induce random fluctuations in an agent's dynamics): see @ref stochasticity
- Delayed (the current time derivative of one agent depends on a past state of another agent): see @ref delays
- Higher order (second, third, fourth, etc time derivatives): see @ref higherorder

NDXL is built to simulate networks whose dynamics are any combination of the above, as shown in the @ref examples section.
Simulation is done by means of numerical integration, which is detailed in the @ref integration section below.

### How many agents can a network have?
Theoretically, an infinite amount.

In practice, software is limited by hardware, particularly RAM.
Larger networks also take longer to simulate because the effective size of the network state vector grows proportional to the number of agents therein.
In NDXL, the theoretical limit is around 10<sup>8</sup> nodes, though this will require a substantial amount of RAM (on the order of terrabytes).

### Can the number of agents in a network change?
Theoretically, yes.

In practice, this is often impossible (or at least, exceedingly difficult), so networks are treated as having static topologies.
The reason for this is that agent state vectors are combined into one large, contiguous state vector, and the network dynamics are written as a set of differential equations over that one vector.
This means that once the network is defined, its layout in memory is fixed, and the whole network state vector is passed to a dedicated differential equation solver (see @ref integration).

NDXL targets this gap in numerical simulation specifically, by implementing custom integrator logic that operates over individual agents' satte vectors, rather than the whole network at once.
This allows vertices and edges to be added, removed, and reconfigured during simulation, resulting in a @ref dyntop "Dynamic Topology".

### Can agent dynamics change during simulation?
Hypothetically yes, though this comes with the caveat of changing the numerical guarantees of the underlying integration method (see @ref integration).

As with the number of agents, changing agent dynamics during simulation in practice is often difficult, if not impossible.
NDXL's @ref dyntop approach makes this process seamless; one simply updates the dynamics according to which an agent evolves in a discrete callback event (see @ref network_callbacks "Network Callbacks").

## Numerical Integration {#integration}
Complex network simulation is achieved by numerically solving (integrating) the differential equations governing the dynamics of a network's agents.
One of the most widely used strategies for this - and the one that NDXL adopts - is the Runge-Kutta family of differential equation solvers.
These methods approximate the solution to the initial value problem \f$\frac{d\vec{Y}}{dt} = \vec{f}(\vec{Y},~t);~\vec{Y}(t=t_0)=\vec{Y}_0\f$ as

\f[
    \vec{Y}_{n+1} = \vec{Y}_n + h\sum_{i=1}^s{B_i\vec{K}_i},
\f]

where the \f$\vec{K}_i\f$ are called the intermediate stages (see e.g., @ref ndxlVertexState(), @ref ndxlEdgeState()), and are given by

\f[
    \vec{K}_i = \vec{F}\left(\vec{Y}_n + h\sum_{j=1}^s{A_{ij}\vec{K}_j},~t_n + hC_i\right).
\f]

Here, \f$h\f$ is the step size (how far forward in time to integrate), \f$s\f$ is the number of stages, and the \f$A\f$, \f$B\f$, and \f$C\f$ coefficients are given by the method's Butcher tableau of the form

\f[
\begin{array}{c|cccc}
    C_1 & A_{11} & A_{12} & \dots & A_{1s} \\
    C_2 & A_{21} & A_{22} & \dots & A_{2s} \\
    \vdots & \vdots & \vdots & \ddots & \vdots \\
    C_s & A_{s1} & A_{s2} & \dots & A_{ss} \\
    \hline
    & B_1 & B_2 & \dots & B_s
\end{array}
\f]

Methods that use the same \f$h\f$ for each step are called static stepping methods, and generally trade computational efficiency (higher \f$h\f$, fewer steps) for precision (lower \f$h\f$, more steps).
They are best suited for simulations where the time scale matters for simulating discrete events, or if you want a quick, rough idea of the output that is (mostly) guaranteed to reach the final simulation time in a set number of steps.

Methods that alter \f$h\f$ each step are called adaptive stepping methods, and increase (decrease) \f$h\f$ if the error is low (high).
Accuracy of a high-order solution is assumed, while a lower-order solution estimates the local error based on user-defined absolute (number of correct decimal places) and relative (number of correct digits) tolerances.
They are best suited for most simulations, but do not guarantee any particular time point will be evaluated, making discrete callback based events difficult to incorporate.
In NDXL, this problem is tackled by passing an array of "time stops" to an adaptive solver, which is a list of time points the solver must evaluate.

### Explicit methods {#meth-explicit}
If \f$A\f$ is strictly lower triangular, the method is said to be explicit, as each $\vec{K}_i$ can be calculated before moving on to $\vec{K}_{i+1}$.
Each agent's \f$\vec{K}_i\f$ must be evaluated before computing any agent's \f$\vec{K}_{i+1}\f$, which in NDXL, is handled by storing the most recently computed intermediate stage for each agent separately, then updating the state that is exposed to other agents after all agents have been iterated over (see e.g., @ref ndxlGetVertexNeighbourState(), @ref ndxlGetEdgeRelativeState()).
Explicit methods are computationally efficient, and are suitable for low to moderately stiff dynamics.
The explicit methods currently implemented in NDXL are:

- Ralston's static fourth-order method @ref ndxlNetworkSimulateRalston
- Nystr&ouml;m's static fifth-order method @ref ndxlNetworkSimulateNystrom
- Dormand and Prince's adaptive fifth-order method with fourth-order error estimate @ref ndxlNetworkSimulateDopri
- Cash and Karp's adaptive fifth-order method with fourth-order error estimate @ref ndxlNetworkSimulateRkck

### Implicit methods {#meth-implicit}
Runge-Kutta methods where \f$A\f$ is not lower triangular are called implicit methods, and in these cases each \f$\vec{K}\f$ cannot be solved analytically, because they depend on each other simultaneously.
Instead, the \f$\vec{K}_i\f$ are approximated over the entire network simultaneously using root-finding algorithms like Newton-Raphson iteration; then the intermediate states exposed by agents in this case correspond to the intermediate stage equations after each Newton-Raphson step.
Implicit methods are favourable when the system dynamics exhibit highly stiff behaviour, but they are more computationally expensive than their explicit counterparts.
NDXL currently only implements one implicit method:

- Radau IIA adaptive fifth-order method with third-order error estimate @ref ndxlNetworkSimulateRadau

### Stochastic methods {#meth-stochastic}
Because Butcher tableaus are constructed to provide a solution of desired order to a deterministic ODE, applying the same tableau directly to a stochastic differential equation (SDE) results in order reduction to an often unknown degree.
Dedicated stochastic solvers are constructed around their own Butcher tableau's with a strong and weak order.
Here, the strong order measures how fast an individual simulated path approaches the exact path, while the weak order measures how fast the probability distribution of simulated paths approach the true average.

Numerically solving SDEs is further categorised into two classifications, namely, the Stratonovich and It&ocirc; interpretations.
The former preserves the chain rule of ordinary calculus, and is hence preferred in physics and engineering applications, while the latter preserves the martingale property, making it more suited to applications like financial analytics.
The stochastic methods currently implemented in NDXL are:

- Euler and Maruyama's static It&ocirc; method of strong order 0.5 @ref ndxlNetworkSimulateEuler
- Heun and Euler's adaptive Stratonovich method of strong order 1 with strong order 0.5 error estimate @ref ndxlNetworkSimulateHeun

Switching between the Stratonovich and It&ocirc; schemes is possible via means of a drift correction term, further detailed in the @ref sde examples.