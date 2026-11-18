//--------------------------------------------------\\
// NDXL internal header for all function prototypes	\\
// Copyright Ethan Codron 2026						\\
// GNU GPL-3.0										\\
//--------------------------------------------------\\
// Provides definitions for all NDXL structs, and	\\
// prototypes for all internal functions.			\\
//--------------------------------------------------\\

#ifndef _INTERNAL_NDXL_
#define _INTERNAL_NDXL_

#include "corendxl_defs.h"

// For defining structs
#include "../../include/ndxl/ndxl_channel.h"
#include "../../include/ndxl/ndxl_edge.h"
#include "../../include/ndxl/ndxl_network.h"
#include "../../include/ndxl/ndxl_vertex.h"

// Struct forward declarations vvvvv

typedef struct NDXLChannelIncrement NDXLChannelIncrement;
typedef struct NDXLChannelVertex NDXLChannelVertex;
typedef struct NDXLChannelEdge NDXLChannelEdge;
typedef struct NDXLEdgeChannel NDXLEdgeChannel;
typedef struct NDXLVertexChannel NDXLVertexChannel;
typedef struct NDXLVertexState NDXLVertexState;
typedef struct NDXLEdgeNode NDXLEdgeNode;

// ^^^^^ Struct forward declarations | network struct vvvvv

struct NDXLNetwork
{
	void* params;					// Arbitrary pointer to extra data used by this network
	size_t ndv, nde;				// Number of deterministic vertices and edges in network, respectively
	size_t nsv, nse;				// Number of stochastic vertices and edges in network, respectively
	size_t nnc;						// Number of noise channels in network
	double time;					// Local simtime of this network
	double h;						// Current step size
	double s;						// Square root of current step size
	const double* butcher;			// Butcher table C-values being used
	char method_explicit;			// Whether the integration method is explicit or implicit
	size_t jacn;					// Current size of network Jacobian (during/after step)
	size_t jacn_old;				// Previous size of network Jacobian (before step)
	size_t* pivots[2];				// Pivot vectors for PLU decomposition
	double* jac;					// Network Jacobian
	double* erhs;					// RHS of network error estimator
	double* errloc;					// Local network error estimate
	double* mlhs_real;				// Real component of LU-decomposition left hand side
	double* mrhs_real;				// Real component of LU-decomposition right hand side
	double* decoup_real;			// Real component of decoupled increments
	complex_t* mlhs_imag;			// Imaginary component of LU-decomposition left hand side
	complex_t* mrhs_imag;			// Imaginary component of LU-decomposition right hand side
	complex_t* decoup_imag;			// Imaginary component of decoupled increments
	NDXLChannel* channel_head;		// First channel in this network
	NDXLChannel* channel_tail;		// Last channel in this network
	NDXLVertex* vertex_head;		// First vertex in this network
	NDXLVertex* vertex_tail;		// Last vertex in this network
	NDXLEdge* edge_head;			// First edge in this network
	NDXLEdge* edge_tail;			// Last edge in this network
	NDXLNetwork* prev;				// Previous network in network stack
	NDXLNetwork* next;				// Next network in network stack
};

// ^^^^^ Network struct | channel structs vvvvv

struct NDXLChannelVertex
{
	NDXLVertexChannel* vchannel;	// Channel node in vertex stack
	NDXLVertex* vertex;				// Vertex pointed to by this node
	struct NDXLChannelVertex* prev;	// Previous node in stack
	struct NDXLChannelVertex* next;	// Next node in stack
};

struct NDXLChannelEdge
{
	NDXLEdgeChannel* echannel;		// Channel node in edge stack
	NDXLEdge* edge;					// Edge pointed to by this node
	struct NDXLChannelEdge* prev;	// Previous node in stack
	struct NDXLChannelEdge* next;	// Next node in stack
};

struct NDXLChannelIncrement
{
	double dw;						// Rejected Brownian increment
	double h;						// Step size of rejected increment
	NDXLChannel* channel;			// Channel owning this increment
	NDXLChannelIncrement* prev;		// Previous increment in RSwM stack
	NDXLChannelIncrement* next;		// Previous increment in RSwM stack
};

struct NDXLChannel
{
	NDXLNetwork* network;			// Parent network of this channel
	void* params;					// Arbitrary pointer to extra data used by this channel

	char cache;						// Whether this channel has a cached normal draw from Box-Mueller
	double z;						// Cached random normal draw
	double dw;						// Wiener increment
	size_t nv, ne;					// Number of vertices and edges this channel outputs to, respectively
	NDXLChannelVertex* vertex_head;	// Oldest vertex in channel stack
	NDXLChannelVertex* vertex_tail;	// Newest vertex in channel stack
	NDXLChannelEdge* edge_head;		// Oldest edge in channel stack
	NDXLChannelEdge* edge_tail;		// Newest edge in channel stack
	NDXLChannelIncrement* b_head;	// Oldest Brownian increment in RSwM stack
	NDXLChannelIncrement* b_tail;	// Newest Brownian increment in RSwM stack
	NDXLChannel* prev;				// Previous channel in channel stack
	NDXLChannel* next;				// Next channel in channel stack
};

// ^^^^^ Channel structs | edge structs vvvvv

struct NDXLEdgeChannel
{
	NDXLChannelEdge* cedge;			// Edge node in channel stack
	NDXLChannel* channel;			// Channel pointed to by this node
	struct NDXLEdgeChannel* prev;	// Previous channel in stack
	struct NDXLEdgeChannel* next;	// Next channel in stack
};

struct NDXLVertexState
{
	double* state;					// Stored state
	double* slope;					// Stored function evaluation
	double time;					// Time at which state existed
	struct NDXLVertexState* prev;	// Previous state in state stack
	struct NDXLVertexState* next;	// Next state in state stack
};

struct NDXLEdgeNode
{
	NDXLEdge* edge;					// Edge pointed to by this node
	struct NDXLEdgeNode* prev;		// Previous node in stack
	struct NDXLEdgeNode* next;		// Next node in stack
};

struct NDXLEdge
{
	NDXLVertex* vert1;				// Vertex 1
	NDXLVertex* vert2;				// Vertex 2
	double* state;					// Edge state
	size_t order;					// Order (length) of state vector
	double delay;					// Edge time-lag/delay
	void* params;					// Arbitrary pointer to extra data used by this edge
	char directed;					// Whether edge is directed or not
	NDXLEdgeDynamics func;			// Function according to which the state of this edge evolves
	NDXLEdgeNoise noise;			// Associated noise function on this edge
	size_t m;						// Number of Wiener channels
	NDXLEdgeCallback callback;		// Edge callback

	double* state0;					// Original edge state
	double* stage_state[2];			// Current and next stage equations (used for explicit methods)
	double* null_state;				// When all zeros need to be returned
	char stage;						// Most recent RK stage evaluated (used for neighbour interactions)
	double* rkf[9];					// Maximum states for integration
	double* rkg[2];					// Wiener paths
	double time;					// Local simtime of this edge
	NDXLEdgeChannel* chead;			// Oldest noise channel affecting this vertex	
	NDXLEdgeChannel* ctail;			// Newest noise channel affecting this vertex
	NDXLVertexState* vert1_head;	// Oldest state of vertex 1
	NDXLVertexState* vert1_tail;	// Newest state of vertex 1
	NDXLVertexState* vert2_head;	// Oldest state of vertex 2
	NDXLVertexState* vert2_tail;	// Newest state of vertex 2
	NDXLEdgeNode* node1;			// Node in vertex 1 stack
	NDXLEdgeNode* node2;			// Node in vertex 2 stack
	NDXLEdge* prev;					// Previous edge in network stack
	NDXLEdge* next;					// Next edge in network stack
};

// ^^^^^ Edge structs | vertex struct vvvvv

struct NDXLVertexChannel
{
	NDXLChannelVertex* cvert;		// Vertex node in channel stack
	NDXLChannel* channel;			// Channel pointed to by this node
	struct NDXLVertexChannel* prev;	// Previous channel in stack
	struct NDXLVertexChannel* next;	// Next channel in stack
};

struct NDXLVertex
{
	NDXLNetwork* network;			// Network that owns this vertex
	double* state;					// Current state of this vertex
	size_t order;					// Order (length) of state vector
	void* params;					// Arbitrary pointer to extra data used by this vertex
	NDXLVertexDynamics func;		// Function according to which the state of this vertex evolves
	NDXLVertexNoise noise;			// Associated noise function on this vertex
	size_t m;						// Number of Wiener channels
	NDXLVertexCallback callback;	// State callback

	double* state0;					// Original state of this vertex
	double* stage_state[2];			// Current and next stage equations (used for explicit methods)
	double* null_state;				// When all zeros need to be returned
	double* interp_state;			// Interpolated state between evaluation times (used only for DDEs)
	char stage;						// Most recent RK stage evaluated (used for neighbour interactions)
	double* rkf[9];					// Maximum states for integration
	double* rkg[2];					// Wiener paths
	double time;					// Local simtime of this vertex
	size_t in_degree;				// Number of directed edges leading into this vertex
	size_t out_degree;				// Number of directed edges leading out of this vertex
	size_t degree;					// Number of undirected edges connected to this vertex
	NDXLVertexChannel* chead;		// Oldest noise channel affecting this vertex	
	NDXLVertexChannel* ctail;		// Newest noise channel affecting this vertex	
	NDXLEdgeNode* edge_head;		// Oldest edge connected to this vertex
	NDXLEdgeNode* edge_tail;		// Newest edge connected to this vertex
	NDXLVertex* prev;				// Previous vertex in vertex stack
	NDXLVertex* next;				// Next vertex in vertex stack
};

// ^^^^^ Vertex struct | network functions and stack vvvvv

extern NDXLNetwork* __ndxlNetworkHead;
extern NDXLNetwork* __ndxlNetworkTail;
extern size_t __ndxlNetworkCount;

_NDXLEXP_ NDXLNetwork* __ndxlCreateNetwork(void* _Params);
_NDXLEXP_ void __ndxlDestroyNetwork(NDXLNetwork* _Network);
_NDXLEXP_ void __ndxlCleanupNetworks();
_NDXLEXP_ const size_t __ndxlGetNetworkChannelCount(NDXLNetwork* _Network);
_NDXLEXP_ void __ndxlGetNetworkEdges(NDXLNetwork* _Network, NDXLEdge** _Edges);
_NDXLEXP_ NDXLChannel* __ndxlGetNetworkFirstChannel(NDXLNetwork* _Network);
_NDXLEXP_ NDXLEdge* __ndxlGetNetworkFirstEdge(NDXLNetwork* _Network);
_NDXLEXP_ NDXLVertex* __ndxlGetNetworkFirstVertex(NDXLNetwork* _Network);
_NDXLEXP_ NDXLChannel* __ndxlGetNetworkLastChannel(NDXLNetwork* _Network);
_NDXLEXP_ NDXLEdge* __ndxlGetNetworkLastEdge(NDXLNetwork* _Network);
_NDXLEXP_ NDXLVertex* __ndxlGetNetworkLastVertex(NDXLNetwork* _Network);
_NDXLEXP_ NDXLChannel* __ndxlGetNetworkNextChannel(NDXLChannel* _Channel);
_NDXLEXP_ NDXLEdge* __ndxlGetNetworkNextEdge(NDXLEdge* _Edge);
_NDXLEXP_ NDXLVertex* __ndxlGetNetworkNextVertex(NDXLVertex* _Vertex);
_NDXLEXP_ const size_t __ndxlGetNetworkOrder(NDXLNetwork* _Network);
_NDXLEXP_ void* __ndxlGetNetworkParams(NDXLNetwork* _Network);
_NDXLEXP_ NDXLChannel* __ndxlGetNetworkPreviousChannel(NDXLChannel* _Channel);
_NDXLEXP_ NDXLEdge* __ndxlGetNetworkPreviousEdge(NDXLEdge* _Edge);
_NDXLEXP_ NDXLVertex* __ndxlGetNetworkPreviousVertex(NDXLVertex* _Vertex);
_NDXLEXP_ const size_t __ndxlGetNetworkSize(NDXLNetwork* _Network);
_NDXLEXP_ const double __ndxlGetNetworkTime(NDXLNetwork* _Network);
_NDXLEXP_ void __ndxlGetNetworkVertices(NDXLNetwork* _Network, NDXLVertex** _Vertices);
_NDXLEXP_ void __ndxlSetNetworkInitialStep(NDXLNetwork* _Network, const double _Atol, const double _Rtol, const double _Order);
_NDXLEXP_ void __ndxlSetNetworkInitialStepStochastic(NDXLNetwork* _Network, const double _Atol, const double _Rtol, const double _OrderInv);
_NDXLEXP_ void __ndxlSetNetworkParams(NDXLNetwork* _Network, void* _Params);
_NDXLEXP_ const int __ndxlNetworkSimulateRalston(NDXLNetwork* _Network, const double _Time, const double _Step, NDXLNetworkCallback _Callback);
_NDXLEXP_ const int __ndxlNetworkSimulateNystrom(NDXLNetwork* _Network, const double _Time, const double _Step, NDXLNetworkCallback _Callback);
_NDXLEXP_ const int __ndxlNetworkSimulateDopri(NDXLNetwork* _Network, const double _Time, const double _Atol, const double _Rtol, const double* _Tstops, const size_t _Tcount, NDXLNetworkCallback _Callback);
_NDXLEXP_ const int __ndxlNetworkSimulateRkck(NDXLNetwork* _Network, const double _Time, const double _Atol, const double _Rtol, const double* _Tstops, const size_t _Tcount, NDXLNetworkCallback _Callback);
_NDXLEXP_ const int __ndxlNetworkSimulateEuler(NDXLNetwork* _Network, const double _Time, const double _Step, NDXLNetworkCallback _Callback);
_NDXLEXP_ const int __ndxlNetworkSimulateHeun(NDXLNetwork* _Network, const double _Time, const double _Atol, const double _Rtol, const double* _Tstops, const size_t _Tcount, NDXLNetworkCallback _Callback);
_NDXLEXP_ const int __ndxlNetworkSimulateNorsett(NDXLNetwork* _Network, const double _Time, const double _Step, NDXLNetworkCallback _Callback);
_NDXLEXP_ const int __ndxlNetworkSimulateRadau(NDXLNetwork* _Network, const double _Time, const double _Atol, const double _Rtol, const double* _Tstops, const size_t _Tcount, NDXLNetworkCallback _Callback);

// ^^^^^ Network functions and stack | channel functions vvvvv

_NDXLEXP_ NDXLChannelEdge* __ndxlCreateChannelEdge(NDXLChannel* _Channel, NDXLEdge* _Edge);
_NDXLEXP_ NDXLChannelVertex* __ndxlCreateChannelVertex(NDXLChannel* _Channel, NDXLVertex* _Vertex);
_NDXLEXP_ NDXLChannelIncrement* __ndxlCreateChannelIncrement(NDXLChannel* _Channel, const double _Hold, const double _Hnew);
_NDXLEXP_ void __ndxlDestroyChannelEdge(NDXLChannelEdge* _Cedge);
_NDXLEXP_ void __ndxlDestroyChannelVertex(NDXLChannelVertex* _Cvertex);
_NDXLEXP_ void __ndxlDestroyChannelIncrement(NDXLChannelIncrement* _Increment);
_NDXLEXP_ NDXLChannel* __ndxlCreateChannel(NDXLNetwork* _Network, void* _Params);
_NDXLEXP_ void __ndxlDestroyChannel(NDXLChannel* _Channel);
_NDXLEXP_ const size_t __ndxlGetChannelEdgeCount(NDXLChannel* _Channel);
_NDXLEXP_ void __ndxlGetChannelEdges(NDXLChannel* _Channel, NDXLEdge** _Edges);
_NDXLEXP_ NDXLNetwork* __ndxlGetChannelNetwork(NDXLChannel* _Channel);
_NDXLEXP_ void* __ndxlGetChannelParams(NDXLChannel* _Channel);
_NDXLEXP_ const size_t __ndxlGetChannelVertexCount(NDXLChannel* _Channel);
_NDXLEXP_ void __ndxlGetChannelVertices(NDXLChannel* _Channel, NDXLVertex** _Vertices);
_NDXLEXP_ void __ndxlSetChannelParams(NDXLChannel* _Channel, void* _Params);
_NDXLEXP_ const double __ndxlChannelNormal(NDXLChannel* _Channel);
_NDXLEXP_ const double __ndxlChannelRedraw(NDXLChannel* _Channel);

// ^^^^^ Channel functions | vertex functions vvvvv

_NDXLEXP_ NDXLVertexChannel* __ndxlCreateVertexChannel(NDXLVertex* _Vertex, NDXLChannel* _Channel);
_NDXLEXP_ void __ndxlDestroyVertexChannel(NDXLVertexChannel* _Vchannel);
_NDXLEXP_ NDXLVertex* __ndxlCreateVertex(NDXLNetwork* _Network, const double* _State, const size_t _Order, NDXLVertexDynamics _Func, NDXLVertexNoise _Noise, NDXLChannel** _Channels, const size_t _Nchannels, NDXLVertexCallback _Callback, void* _Params);
_NDXLEXP_ void __ndxlDestroyVertex(NDXLVertex* _Vertex);
_NDXLEXP_ const int __ndxlVertexProcessCallback(NDXLVertex* _Vertex);
_NDXLEXP_ const int __ndxlVertexTickRalston(NDXLVertex* _Vertex, const double _Delta);
_NDXLEXP_ const int __ndxlVertexTickNystrom(NDXLVertex* _Vertex, const double _Delta);
_NDXLEXP_ const int __ndxlVertexTickDopri(NDXLVertex* _Vertex, const double _Delta, const double _Atol, const double _Rtol, double* _Errsum);
_NDXLEXP_ const int __ndxlVertexTickRkck(NDXLVertex* _Vertex, const double _Delta, const double _Atol, const double _Rtol, double* _Errsum);
_NDXLEXP_ const int __ndxlVertexTickEuler(NDXLVertex* _Vertex, const double _Delta);
_NDXLEXP_ const int __ndxlVertexTickHeun(NDXLVertex* _Vertex, const double _Delta, const double _Atol, const double _Rtol, double* _Errsum);
_NDXLEXP_ const int __ndxlVertexTickNorsett(NDXLVertex* _Vertex, const double _Delta);
_NDXLEXP_ NDXLVertexCallback __ndxlGetVertexCallback(NDXLVertex* _Vertex);
_NDXLEXP_ const size_t __ndxlGetVertexChannelCount(NDXLVertex* _Vertex);
_NDXLEXP_ void __ndxlGetVertexChannels(NDXLVertex* _Vertex, NDXLChannel** _Channels);
_NDXLEXP_ const size_t __ndxlGetVertexDegree(NDXLVertex* _Vertex);
_NDXLEXP_ const size_t __ndxlGetVertexDegreeIn(NDXLVertex* _Vertex);
_NDXLEXP_ const size_t __ndxlGetVertexDegreeOut(NDXLVertex* _Vertex);
_NDXLEXP_ NDXLVertexDynamics __ndxlGetVertexDynamics(NDXLVertex* _Vertex);
_NDXLEXP_ void __ndxlGetVertexEdges(NDXLVertex* _Vertex, NDXLEdge** _Edges);
_NDXLEXP_ NDXLEdge* __ndxlGetVertexFirstEdge(NDXLVertex* _Vertex);
_NDXLEXP_ NDXLEdge* __ndxlGetVertexLastEdge(NDXLVertex* _Vertex);
_NDXLEXP_ NDXLVertex* __ndxlGetVertexNeighbour(NDXLVertex* _Vertex, NDXLEdge* _Edge);
_NDXLEXP_ const size_t __ndxlGetVertexNeighbourOrder(NDXLVertex* _Vertex, NDXLEdge* _Edge);
_NDXLEXP_ void __ndxlGetVertexNeighbours(NDXLVertex* _Vertex, NDXLVertex** _Neigbours);
_NDXLEXP_ const double* __ndxlGetVertexNeighbourState(NDXLVertex* _Vertex, NDXLEdge* _Edge);
_NDXLEXP_ NDXLNetwork* __ndxlGetVertexNetwork(NDXLVertex* _Vertex);
_NDXLEXP_ NDXLEdge* __ndxlGetVertexNextEdge(NDXLVertex* _Vertex, NDXLEdge* _Edge);
_NDXLEXP_ NDXLVertexNoise __ndxlGetVertexNoise(NDXLVertex* _Vertex);
_NDXLEXP_ const size_t __ndxlGetVertexOrder(NDXLVertex* _Vertex);
_NDXLEXP_ void* __ndxlGetVertexParams(NDXLVertex* _Vertex);
_NDXLEXP_ NDXLEdge* __ndxlGetVertexPreviousEdge(NDXLVertex* _Vertex, NDXLEdge* _Edge);
_NDXLEXP_ const double* __ndxlGetVertexState0(NDXLVertex* _Vertex);
_NDXLEXP_ const double __ndxlGetVertexTime(NDXLVertex* _Vertex);
_NDXLEXP_ void __ndxlSetVertexCallback(NDXLVertex* _Vertex, NDXLVertexCallback _Callback);
_NDXLEXP_ void __ndxlSetVertexDynamics(NDXLVertex* _Vertex, NDXLVertexDynamics _Dynamics);
_NDXLEXP_ void __ndxlSetVertexNoise(NDXLVertex* _Vertex, NDXLVertexNoise _Noise);
_NDXLEXP_ void __ndxlSetVertexParams(NDXLVertex* _Vertex, void* _Params);
_NDXLEXP_ char __ndxlVertexHasNeighbour(NDXLVertex* _Source, NDXLVertex* _Destination);
_NDXLEXP_ double* __ndxlVertexState(NDXLVertex* _Vertex);

// ^^^^^ Vertex functions | edge functions vvvvv

_NDXLEXP_ NDXLEdgeChannel* __ndxlCreateEdgeChannel(NDXLEdge* _Edge, NDXLChannel* _Channel);
_NDXLEXP_ void __ndxlDestroyEdgeChannel(NDXLEdgeChannel* _Echannel);
_NDXLEXP_ NDXLVertexState* __ndxlCreateVertexState(NDXLVertex* _Vertex, const double _Time);
_NDXLEXP_ void __ndxlDestroyVertexState(NDXLVertexState* _State);
_NDXLEXP_ NDXLEdgeNode* __ndxlCreateEdgeNode(NDXLEdge* _Edge, NDXLVertex* _Vertex);
_NDXLEXP_ void __ndxlDestroyEdgeNode(NDXLEdgeNode* _Node);
_NDXLEXP_ NDXLEdge* __ndxlCreateEdge(NDXLVertex* _Vert1, NDXLVertex* _Vert2, const double* _State, const size_t _Order, const double _Delay, const char _Directed, NDXLEdgeDynamics _Func, NDXLEdgeNoise _Noise, NDXLChannel** _Channels, const size_t _Nchannels, NDXLEdgeCallback _Callback, void* _Params);
_NDXLEXP_ void __ndxlDestroyEdge(NDXLEdge* _Edge);
_NDXLEXP_ const int __ndxlEdgeProcessCallback(NDXLEdge* _Edge);
_NDXLEXP_ void __ndxlEdgeAccumulateEffects(NDXLEdge* _Edge, const double _Delta);
_NDXLEXP_ const int __ndxlEdgeTickRalston(NDXLEdge* _Edge, const double _Delta);
_NDXLEXP_ const int __ndxlEdgeTickNystrom(NDXLEdge* _Edge, const double _Delta);
_NDXLEXP_ const int __ndxlEdgeTickDopri(NDXLEdge* _Edge, const double _Delta, const double _Atol, const double _Rtol, double* _Errsum);
_NDXLEXP_ const int __ndxlEdgeTickRkck(NDXLEdge* _Edge, const double _Delta, const double _Atol, const double _Rtol, double* _Errsum);
_NDXLEXP_ const int __ndxlEdgeTickEuler(NDXLEdge* _Edge, const double _Delta);
_NDXLEXP_ const int __ndxlEdgeTickHeun(NDXLEdge* _Edge, const double _Delta, const double _Atol, const double _Rtol, double* _Errsum);
_NDXLEXP_ const int __ndxlEdgeTickNorsett(NDXLEdge* _Edge, const double _Delta);
_NDXLEXP_ NDXLEdgeCallback __ndxlGetEdgeCallback(NDXLEdge* _Edge);
_NDXLEXP_ const size_t __ndxlGetEdgeChannelCount(NDXLEdge* _Edge);
_NDXLEXP_ void __ndxlGetEdgeChannels(NDXLEdge* _Edge, NDXLChannel** _Channels);
_NDXLEXP_ const double __ndxlGetEdgeDelay(NDXLEdge* _Edge);
_NDXLEXP_ const char __ndxlGetEdgeDirected(NDXLEdge* _Edge);
_NDXLEXP_ NDXLEdgeDynamics __ndxlGetEdgeDynamics(NDXLEdge* _Edge);
_NDXLEXP_ NDXLVertex* __ndxlGetEdgeFirstVertex(NDXLEdge* _Edge);
_NDXLEXP_ NDXLEdgeNoise __ndxlGetEdgeNoise(NDXLEdge* _Edge);
_NDXLEXP_ NDXLNetwork* __ndxlGetEdgeNetwork(NDXLEdge* _Edge);
_NDXLEXP_ const size_t __ndxlGetEdgeOrder(NDXLEdge* _Edge);
_NDXLEXP_ void* __ndxlGetEdgeParams(NDXLEdge* _Edge);
_NDXLEXP_ const double* __ndxlGetEdgeRelativeState(NDXLEdge* _Edge, NDXLVertex* _Vertex);
_NDXLEXP_ NDXLVertex* __ndxlGetEdgeSecondVertex(NDXLEdge* _Edge);
_NDXLEXP_ const double* __ndxlGetEdgeState0(NDXLEdge* _Edge);
_NDXLEXP_ const double __ndxlGetEdgeTime(NDXLEdge* _Edge);
_NDXLEXP_ void __ndxlSetEdgeCallback(NDXLEdge* _Edge, NDXLEdgeCallback _Callback);
_NDXLEXP_ void __ndxlSetEdgeDynamics(NDXLEdge* _Edge, NDXLEdgeDynamics _Func);
_NDXLEXP_ void __ndxlSetEdgeNoise(NDXLEdge* _Edge, NDXLEdgeNoise _Noise);
_NDXLEXP_ void __ndxlSetEdgeParams(NDXLEdge* _Edge, void* _Params);
_NDXLEXP_ const char __ndxlEdgeConnectsVertex(NDXLEdge* _Edge, NDXLVertex* _Vertex);
_NDXLEXP_ double* __ndxlEdgeState(NDXLEdge* _Edge);

// ^^^^^ Edge functions | integrator functions vvvvv

static inline const int __ndxlNetworkJacobian(NDXLNetwork* _Network)
{
	// Algorithm variables
	size_t _Jidx = 0;
	NDXLVertex* vert, *jvert;
	NDXLEdge* edge, *jedge;

	// Iteration over vertices
	for (vert = _Network->vertex_head; vert; _Jidx += vert->order, vert = vert->next)
	{
		for (size_t i = 0; i < vert->order; i++)
		{
			// Scale perturbation
			const double _Eps = EPS * fmax(1.0, vert->state[i]);

			// Compute forward perturbation, store in rkf[6]
			vert->stage_state[0][i] += _Eps;
			for (jvert = _Network->vertex_head; jvert; jvert = jvert->next)
			{
				if (jvert->func)
				{
					if (jvert->func(jvert, jvert->rkf[6], jvert->stage_state[0], jvert->time, jvert->params))
						return NDXL_SIM_ERR;
				}
				else
					memset(jvert->rkf[6], 0, jvert->order * sizeof(double));
			}
			for (jedge = _Network->edge_head; jedge; jedge = jedge->next)
			{
				if (jedge->func)
				{
					if (jedge->func(jedge, jedge->rkf[6], jedge->stage_state[0], jedge->time, jedge->params))
						return NDXL_SIM_ERR;
				}
				else
					memset(jedge->rkf[6], 0, jedge->order * sizeof(double));
			}

			// Compute backward perturbation, store in rkf[7]
			vert->stage_state[0][i] -= (2. * _Eps);
			for (jvert = _Network->vertex_head; jvert; jvert = jvert->next)
			{
				if (jvert->func)
				{
					if (jvert->func(jvert, jvert->rkf[7], jvert->stage_state[0], jvert->time, jvert->params))
						return NDXL_SIM_ERR;
				}
				else
					memset(jvert->rkf[7], 0, jvert->order * sizeof(double));
			}
			for (jedge = _Network->edge_head; jedge; jedge = jedge->next)
			{
				if (jedge->func)
				{
					if (jedge->func(jedge, jedge->rkf[7], jedge->stage_state[0], jedge->time, jedge->params))
						return NDXL_SIM_ERR;
				}
				else
					memset(jedge->rkf[7], 0, jedge->order * sizeof(double));
			}

			// Restore stage_state[0] state and compute finite difference approximation
			vert->stage_state[0][i] += _Eps;
			size_t _Jrow = 0;
			for (jvert = _Network->vertex_head; jvert; _Jrow += jvert->order, jvert = jvert->next)
			{
				for (size_t j = 0; j < jvert->order; j++)
					_Network->jac[(_Jrow + j) * _Network->jacn + _Jidx + i] = (jvert->rkf[6][j] - jvert->rkf[7][j]) / (2.0 * _Eps);
			}
			for (jedge = _Network->edge_head; jedge; _Jrow += jedge->order, jedge = jedge->next)
			{
				for (size_t j = 0; j < jedge->order; j++)
					_Network->jac[(_Jrow + j) * _Network->jacn + _Jidx + i] = (jedge->rkf[6][j] - jedge->rkf[7][j]) / (2.0 * _Eps);
			}
		}
	}

	// Iteration over edges
	for (edge = _Network->edge_head; edge; _Jidx += edge->order, edge = edge->next)
	{
		for (size_t i = 0; i < edge->order; i++)
		{
			// Scale perturbation
			const double _Eps = EPS * fmax(1.0, edge->state[i]);

			// Compute forward perturbation, store in rkf[6]
			edge->stage_state[0][i] += _Eps;
			for (jvert = _Network->vertex_head; jvert; jvert = jvert->next)
			{
				if (jvert->func)
				{
					if (jvert->func(jvert, jvert->rkf[6], jvert->stage_state[0], jvert->time, jvert->params))
						return NDXL_SIM_ERR;
				}
				else
					memset(jvert->rkf[6], 0, jvert->order * sizeof(double));
			}
			for (jedge = _Network->edge_head; jedge; jedge = jedge->next)
			{
				if (jedge->func)
				{
					if (jedge->func(jedge, jedge->rkf[6], jedge->stage_state[0], jedge->time, jedge->params))
						return NDXL_SIM_ERR;
				}
				else
					memset(jedge->rkf[6], 0, jedge->order * sizeof(double));
			}

			// Compute backward perturbation, store in rkf[7]
			edge->stage_state[0][i] -= (2. * _Eps);
			for (jvert = _Network->vertex_head; jvert; jvert = jvert->next)
			{
				if (jvert->func)
				{
					if (jvert->func(jvert, jvert->rkf[7], jvert->stage_state[0], jvert->time, jvert->params))
						return NDXL_SIM_ERR;
				}
				else
					memset(jvert->rkf[7], 0, jvert->order * sizeof(double));
			}
			for (jedge = _Network->edge_head; jedge; jedge = jedge->next)
			{
				if (jedge->func)
				{
					if (jedge->func(jedge, jedge->rkf[7], jedge->stage_state[0], jedge->time, jedge->params))
						return NDXL_SIM_ERR;
				}
				else
					memset(jedge->rkf[7], 0, jedge->order * sizeof(double));
			}

			// Restore stage_state[0] state and compute finite difference approximation
			edge->stage_state[0][i] += _Eps;
			size_t _Jrow = 0;
			for (jvert = _Network->vertex_head; jvert; _Jrow += jvert->order, jvert = jvert->next)
			{
				for (size_t j = 0; j < jvert->order; j++)
					_Network->jac[(_Jrow + j) * _Network->jacn + _Jidx + i] = (jvert->rkf[6][j] - jvert->rkf[7][j]) / (2.0 * _Eps);
			}
			for (jedge = _Network->edge_head; jedge; _Jrow += jedge->order, jedge = jedge->next)
			{
				for (size_t j = 0; j < jedge->order; j++)
					_Network->jac[(_Jrow + j) * _Network->jacn + _Jidx + i] = (jedge->rkf[6][j] - jedge->rkf[7][j]) / (2.0 * _Eps);
			}
		}
	}

	// Success
	return 0;
}

static inline void __pluDecomposeZ(complex_t* _Mat, size_t* _P, const size_t _Dim)
{
	// Algorithm variables
	size_t i, j, k, _Imax;
	double _Amax, _Aabs;
	complex_t z;

	// Unit permutation matrix
	for (i = 0; i <= _Dim; i++)
		_P[i] = i;

	// Decompose
	for (i = 0; i < _Dim; i++)
	{
		_Amax = 0.0;
		_Imax = 0;

		// For complex values, pivoting condition is checked against magnitude of entry
		for (k = i; k < _Dim; k++)
			if ((_Aabs = uniMag(_Mat[k * _Dim + i])) > _Amax)
			{
				_Amax = _Aabs;
				_Imax = k;
			}

		// Pivot rows and permutation matrix
		if (_Imax != i)
		{
			for (k = 0; k < _Dim; k++)
			{
				z = _Mat[i * _Dim + k];
				_Mat[i * _Dim + k] = _Mat[_Imax * _Dim + k];
				_Mat[_Imax * _Dim + k] = z;
			}
			j = _P[i];
			_P[i] = _P[_Imax];
			_P[_Imax] = j;
			_P[_Dim]++;
		}

		// Factorisation
		for (j = i + 1; j < _Dim; j++)
		{
			_Mat[j * _Dim + i] = uniDivComplexComplex(_Mat[j * _Dim + i], _Mat[i * _Dim + i]);
			for (k = i + 1; k < _Dim; k++)
				_Mat[j * _Dim + k] = uniSubComplexComplex(_Mat[j * _Dim + k], uniMulComplexComplex(_Mat[j * _Dim + i], _Mat[i * _Dim + k]));
		}
	}
}

static inline void __pluSolveZ(complex_t* _Ans, complex_t* _Mat, complex_t* _Rhs, size_t* _P, const size_t _Dim)
{
	// Algorithm variables
	size_t i, j;

	// Forward substitution Ly = Pb
	for (i = 0; i < _Dim; i++)
	{
		_Ans[i] = _Rhs[_P[i]];
		for (j = 0; j < i; j++)
			_Ans[i] = uniSubComplexComplex(_Ans[i], uniMulComplexComplex(_Mat[i * _Dim + j], _Ans[j]));
	}

	// Backward substitution Ux = y
	for (i = _Dim - 1; i >= 0; i--)
	{
		for (j = i + 1; j < _Dim; j++)
			_Ans[i] = uniSubComplexComplex(_Ans[i], uniMulComplexComplex(_Mat[i * _Dim + j], _Ans[j]));
		_Ans[i] = uniDivComplexComplex(_Ans[i], _Mat[i * _Dim + i]);
		if (i == 0)
			break;
	}
}

static inline void __pluDecompose(double* _Mat, size_t* _P, const size_t _Dim)
{
	// Algorithm variables
	size_t i, j, k, _Imax;
	double _Amax, _Aabs;
	double d;

	// Unit permutation matrix
	for (i = 0; i <= _Dim; i++)
		_P[i] = i;

	// Decompose
	for (i = 0; i < _Dim; i++)
	{
		_Amax = 0.0;
		_Imax = 0;

		// For complex values, pivoting condition is checked against magnitude of entry
		for (k = i; k < _Dim; k++)
			if ((_Aabs = fabs(_Mat[k * _Dim + i])) > _Amax)
			{
				_Amax = _Aabs;
				_Imax = k;
			}

		// Pivot rows and permutation matrix
		if (_Imax != i)
		{
			for (k = 0; k < _Dim; k++)
			{
				d = _Mat[i * _Dim + k];
				_Mat[i * _Dim + k] = _Mat[_Imax * _Dim + k];
				_Mat[_Imax * _Dim + k] = d;
			}
			j = _P[i];
			_P[i] = _P[_Imax];
			_P[_Imax] = j;
			_P[_Dim]++;
		}

		// Factorisation
		for (j = i + 1; j < _Dim; j++)
		{
			_Mat[j * _Dim + i] /= _Mat[i * _Dim + i];
			for (k = i + 1; k < _Dim; k++)
				_Mat[j * _Dim + k] -= (_Mat[j * _Dim + i] * _Mat[i * _Dim + k]);
		}
	}
}

static inline void __pluSolve(double* _Ans, double* _Mat, double* _Rhs, size_t* _P, const size_t _Dim)
{
	// Algorithm variables
	size_t i, j;

	// Forward substitution Ly = Pb
	for (i = 0; i < _Dim; i++)
	{
		_Ans[i] = _Rhs[_P[i]];
		for (j = 0; j < i; j++)
			_Ans[i] -= (_Mat[i * _Dim + j] * _Ans[j]);
	}

	// Backward substitution Ux = y
	for (i = _Dim - 1; i >= 0; i--)
	{
		for (j = i + 1; j < _Dim; j++)
			_Ans[i] -= (_Mat[i * _Dim + j] * _Ans[j]);
		_Ans[i] /= _Mat[i * _Dim + i];
		if (i == 0)
			break;
	}
}

INTEGRATOR_RALSTON(Vertex)
INTEGRATOR_NYSTROM(Vertex)
INTEGRATOR_DOPRI(Vertex)
INTEGRATOR_RKCK(Vertex)
INTEGRATOR_EULER(Vertex)
INTEGRATOR_HEUN(Vertex)

INTEGRATOR_RALSTON(Edge)
INTEGRATOR_NYSTROM(Edge)
INTEGRATOR_DOPRI(Edge)
INTEGRATOR_RKCK(Edge)
INTEGRATOR_EULER(Edge)
INTEGRATOR_HEUN(Edge)

// ^^^^^ Integrator functions

#endif // _INTERNAL_NDXL_