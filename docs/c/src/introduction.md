# Introduction
Welcome to the Network Dynamics eXperimentation Library (NDXL) documentation @NDXL_VERSION@.

Looking for another release? Visit the [documentation version index](https://nonlinearc.github.io/NDXL/).

If this is your first visit, we recommend treating the documentation as a book, going from basic concepts to advanced tutorials and implementation details.

## What is NDXL?
NDXL is a cross-platform and programming language-independent package for simulating complex network dynamics, with an emphasis on dynamic topologies.
What this simply means is:

- NDXL can be used on Windows, Linux, and x86-64 macOS
- It can be integrated into any programming language capable of interfacing with compiled C libraries
- It facilitates simulation of networks whose number and arrangement of vertices and edges change over time
- It supports a wide range of network structures and systems of differential equations, including ordinary, delayed, and stochastic dynamics

## Why use NDXL?
A wide variety of scientific disciplines rely on network-based modelling to simulate complex systems composed of interacting, potentially heterogeneous components.
These include, but are not limited to:

- Physics/chemistry: gas-particle ensembles, solid atomic lattices, many-body problem
- Engineering: power grids, oscillator networks, heat flow, Josephson junctions
- Biology: predator-prey communities, algal clusters, viral infection
- Sociology: follower networks, friend groups
- Information technology: client-server interactions, internet of things

These networks often exhibit behaviour which is difficult/impossible to capture in most other simulation software, for example:

- A power grid with generators and loads constantly being added/removed
- An individual-based ecological model in which organisms reproduce, die, and interact with a changing population
- A computer network with servers going up/down and clients connecting/disconnecting
- An ensemble of gas particles that slowly leak into the environment

NDXL makes simulating these easy, as vertices and edges can be added to/removed from/changed in the network while a simulation is running.
This process is detailed in the @ref dyntop section.

Unfortunately, supporting dynamic topologies introduces additional computational and memory-management overhead compared with implementations designed exclusively for static networks.
NDXL is continuously being optimised to minimise this overhead.

## Where to begin?
Instructions for acquiring, installing, and compiling against the NDXL library and header files can be found in the @ref installation section.
The @ref concepts section then explains the terminology used throughout NDXL and network dynamics, and the @ref ndxl_err section provides details on how and when errors are raised and handled in NDXL.
After that, the @ref examples section will walk you through how to construct and simulate different kinds of networks in NDXL.
Finally, the <b>[API](topics.html)</b> section provides detailed descriptions on the objects and functions available in NDXL.

## FAQ

### How do I use NDXL from other programming languages?
Through language-specific bindings that expose NDXL's compiled C library to the target programming language.
For example, the official Python version is available on PyPI <b>[here](https://pypi.org/project/ndxl/)</b>.
Future work on NDXL will involve creating language-specific bindings for other languages like Julia.

### Can I write my own language-specific wrappers?
Of course! This is one of the intended uses of the API, and why it was written in C using only the C standard library, primitive, and opaque types.
For a detailed guide on how to import NDXL into another programming language (using the Python version as an example), see @subpage crosslang.

\note NDXL is distributed under the <b>[GNU General Public License, version 3 (GPL-3.0)](https://www.gnu.org/licenses/gpl-3.0.en.html)</b>. You are free to develop and distribute your own language bindings and derivative works, provided that you comply with the applicable terms of the licence.

### Should I use NDXL from C?
Situationally.

The API is designed to provide the user with as much control as possible over the underlying mechanics of the network.
Because the C API exposes many configuration parameters directly, constructing and configuring objects can become rather verbose, particularly when many optional parameters are set to `0` or `NULL`.
Languages with features such as default arguments and function overloading can provide more convenient interfaces to the same underlying functionality.
This is why the library is also designed with cross-language interoperability in mind - it is much easier, from a user perspective, to use the [Python version](https://pypi.org/project/ndxl/), for example.
With that said, language-specific wrappers can introduce their own significant amount of overhead, depending on how many inter-language translations need to be done during simulation.

A general rule of thumb would therefore be:
- Use C when performance is important e.g., simulating a 10<sup>7</sup> node algal cluster.
- Use another language wrapper if the simulation is small and convenience matters more than control.

### What kinds of networks can NDXL simulate?
Almost everything.

There are certain classes of differential equation that NDXL does not directly support (yet), for example, differential algebraic equations (DAEs).
NDXL requires the dynamics to be in a standard ODE/DDE/SDE form (see @ref network_simulation "Network Simulation").
So, if the conditions of the differential equation allow the network dynamics can be transformed into an ODE/DDE/SDE, then NDXL can simulate the network.

There are also certain solvers that NDXL does not directly support (yet), such as implicit stochastic solvers.
The available explicit stochastic methods may be inefficient or unsuitable for certain stiff stochastic systems, for which implicit methods would be more appropriate.

So, it is often more a case of "some networks might be more difficult to solve than others", and this is something we intend to tackle in future updates.