# Network Dynamics {#nd}
This page is dedicated to terminology pertaining to "network dynamics" in general, as well as how certain concepts are implemented/interpreted in NDXL.

## What is Meant by "State Vector"? {#state-vector}
An agent's state vector is the vector of time-dependent properties that fully describe the agent.
Because a scalar can be represented as a unit-length array, NDXL treats all state vectors as arrays, allowing for multidimensional dynamics to be evaluated by default: the time derivative for each element is just written in-place to the output array similar to the @ref examples "Getting Started" example.
In a power grid modelled by the swing equation, for example, each node has two states, namely, its phase angle \f$\theta\f$ and frequency \f$\omega\f$, while each transmission line is typically only described by its coupling constant \f$A\f$.
We can explicitly write the state vector of node \f$i\f$ as:

\f[
	\vec{x}_i = \left\langle\theta_i,~\omega_i\right\rangle
\f]

And similarly the state vector of edge \f$ij\f$ is:

\f[
	\vec{y}_{ij} = \left\langle A_{ij}\right\rangle
\f]

### Do all elements of a state vector change over time?
They can, but they do not necessarily have to.
An element constant with respect to time is called a <b>static element</b>, and if all elements of an agent's state vector are static, then the agent itself is said to be a <b>static agent</b>.
Static elements in NDXL are characterised by a return value of \f$0\f$ for that element in its dynamics function; static agents can be simply assinged `NULL` for their dynamics.

### What if agent dynamics depend on constants in the equation? {#dynamicsexample}
Suppose the dynamics of a vertex are given by

\f[
	\frac{d\vec{x}}{dt} = Ct + D\vec{x}
\f]

where \f$C\f$ and \f$D\f$ are constants.
In the agent's constructor, these numbers are passed using the `void* params` argument, which is always the last argument in a constructor.
During dynamics evaluation, they can be extracted by converting the `void* params` to whatever datatype the constants initially were defined as, as shown in the @ref examples "Getting Started" example.

## Heterogeneous Dynamics {#hetero-dynamics}
It is common to express the dynamics of network agents as a single function according to which each agent's state vector evolves, and have their state vectors and `void* params` refer to parameters.
In the Kuramoto model, for example, each vertex has a unit state vector containing its phase \f$\theta\f$, with its `void* params` containing its eigenfrequency, and evolves according to the agentic Kuramoto model.
In the swing equation, each vertex has a two-component state vector containing its phase \f$\theta\f$ and frequency \f$\omega\f$, and evolves according to the swing equation.
There is, however, no strict requirement saying agents must:

- Have identically cardinal state vectors
- Depend on the same time-variant parameters as others
- Evolve according to the same dynamics, noise, or discrete callback functions
- Be influenced by the same noise channels and/or to the same degree
- Share user-defined `void* params` (constants)
- Have the same initial conditions

Whenever one or more of the first four conditions is false, the agents are said to be heterogeneous (initial conditions and constants are usually unique by default).
This gives rise to the term "heterogeneous dynamics", simply meaning that two or more agents in the network do not exhibit common dynamics.
This can be, for example:

- One node evolving according to the Kuramoto model, its neighbour to the swing equation
- One node being static, its neighbour dynamic
- One node being @ref stochasticity "stochastic", its neighbour purely deterministic
- One node having a ten-element state vector, its neighbour a scalar state
- One node being influenced by noise channels \f$1\f$, \f$3\f$, and \f$6\f$, its neighbour by channels \f$2\f$ and \f$5\f$

NDXL makes simulating heterogeneous dynamics easy by assigning agent properties directly to the agents themselves, as outlined in the @ref hetero "Heterogeneous Dynamics" example.

## Stochasticity {#stochasticity}
The default network dynamics equations are written as purely deterministic ODEs, however, some systems exhibit random fluctuations in the differential equation signal.
These fluctuations are called noise, and add an element of stochasticity to the system, transforming the governing dynamics from an ODE to an SDE (stochastic differential equation).
The stochastic contributions are assumed to follow Brownian motion, and the random path taken from start to finish is called a Wiener process.
This process is further documented in the @ref sde "Stochasticity" examples.

## Delays {#delays}
A delay is a concept in information theory that pertains to the time \f$\tau\f$ taken for information to travel from point A to point B.
In the context of network dynamics, and specifically under the physical interpretation of a network, "delays" refer to the time taken for a signal to propagate from one vertex to its neighbour along a given edge.
NDXL therefore treats delays as explicit edge properties, and each edge is responsible for mediating delayed interactions between the vertices it connects.
The memory and implications are documented in @ref edge_creation "edge creation", while the mathematical details are further explained in the @ref dde "Delays" examples.

## Higher-Order Dynamics {#higherorder}
The descriptions of differential equations governing agent dynamics have thus far (@ref dynamicsexample "Dynamics" and @ref stochasticity) been of first-order: \f$\frac{d\vec{x}}{dt}\f$.
Of course, many systems exhibit higher-order dynamics, written as \f$\frac{d^n\vec{x}}{dt^n}\f$.
To solve these systems using the first-order solvers, we must rewrite the \f$n^{th}\f$-order equation as a set of \f$n\f$ coupled first-order equations by using a substitution of variables.
The state vector of the agent then goes from having one element to \f$n\f$ elements, where the derivative of the \f$i^{th}\f$ component is used to compute the derivative of the \f$(i+1)^{th}\f$ component.
This process is further detailed in the @ref highorder "Higher-Order Dynamics" examples.

## Dynamic Topologies {#dyntop}
The size, order, and topology are usually fixed for a given network i.e., once the network is created, these variables cannot - and do not - change.
For some applications and models, this approach is sufficient.
However, there also exist networks for which these variables are not constant (see the examples listed in the @ref introduction); we call these networks <b><i>Dynamic Networks</i></b>, and is the specific class of network NDXL was built around simulating.

While other packages can theoretically simulate dynamic networks, their approach often wastes computational resources, and/or imposes a hard limit on the amount of new agents that can be brought into the network.
The reason for this is "dead" agents are usually simulated by zeroing their contributions to the overall dynamics, and "new" agents are simulated by reserving space before the simulation for their eventual existence.
By allowing agents to be created and destroyed during simulation, only ever those that are still part of the network are actually simulated, making NDXL the optimal choice for simulating dynamic networks.
See @ref dynnet for details how to simulate dynamic topology networks, what the rules are for when and how to add/remove/rearrange agents, and what the limitations, mathematical implications, and performance impacts are.