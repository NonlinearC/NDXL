# NDXL
![Network Dynamics eXperimentation Library](NDXL-logo.png)

Network Dynamics eXperimentation Library (NDXL) is a cross-platform, language-independent C library for simulating complex networks and their dynamics.
NDXL places particular emphasis on networks whose topologies — the number of agents and their connectivity — evolve over time.
The library is written in C using only primitive and opaque types, making it straightforward to interface with from other programming languages (see, for example, the [Python implementation](https://pypi.org/project/ndxl/)).

## Features
The core object in NDXL, `NDXLNetwork`, serves as the basis for building a network from vertices (`NDXLVertex`; resident components) and edges (`NDXLEdge`; coupling between components) representing a complex system, which can then be numerically simulated.
Vertex and edge dynamics are defined independently, allowing heterogeneous components and combinations of different dynamical behaviours.
Supported dynamics include:

- Ordinary differential equations (ODEs)
- Stochastic differential equations (SDEs)
- Static agents (no dynamics)

Vertices can additionally depend on delayed states of their nearest neighbours and the edges that join them (delay differential equations; DDEs).
These behaviours are not mutually exclusive: for example, a vertex can have stochastic dynamics while depending on delayed states of its neighbours, whereas another vertex may use only delayed states and the connecting edges remain static.

## NDXL Use-Cases
A wide variety of scientific disciplines rely on network-based modelling to simulate complex systems composed of interacting, potentially heterogeneous components.

- Physics/chemistry: gas-particle ensembles, solid atomic lattices, many-body problem
- Engineering: power grids, oscillator networks, heat flow, Josephson junctions
- Biology: predator-prey communities, algal communities, viral infection
- Sociology: follower networks, friend groups
- Information technology: client-server interactions, internet of things

In addition to simulating these networks under standard conditions, NDXL shines when simulating these networks subject to evolving topologies:

- A power grid with generators and loads constantly being added/removed
- An individual-based ecological model in which organisms reproduce, die, and interact with a changing population
- A computer network with servers going up/down and clients connecting/disconnecting
- An ensemble of gas particles that slowly leak into the environment

## Getting Started
The precompiled libraries and header files are found in [NDXL/lib](./NDXL/lib/) and [NDXL/include](./NDXL/include/), respectively.
For getting started with the C API, see the [C API](./NDXL/).

Language-specific bindings and their installation guides can be found in [bindings](./bindings/).
For getting started with the Python API, see the [Python bindings](./bindings/python/).

The NDXL [documentation](https://github.com/NonlinearC/NDXL) provides detailed explanations of the library's architecture, concepts, API functions and usage, together with tutorials covering its functionality.

## Contributing
Questions and bug reports on this page are welcome; for details, see [how to contribute](CONTRIBUTING.md).

## Citing NDXL
pls add me to paper

## License
NDXL is released under the GNU GPL-3.0 license, allowing users to create their own derivative works and distribute them under the same licensing conditions.
Official language bindings are distributed under the same licence.
Users are also welcome to develop their own external wrappers for NDXL (see the [documentation](https://github.com/NonlinearC/NDXL) for details).