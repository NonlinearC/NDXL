#ifndef _INTERNAL_NDXL_
#define _INTERNAL_NDXL_

#include "../corendxl_defs.h"

// For defining structs
#include "../../include/ndxl/ndxl_edge.h"
#include "../../include/ndxl/ndxl_network.h"
#include "../../include/ndxl/ndxl_vertex.h"

// Internal global variables and functions vvvvv

extern char __ndxlIsInitialised;
extern int __ndxlLastErrorCode;
extern char* __ndxlLastErrorMsg;
extern double __ndxlSqrtEpsilon;
extern NDXLNetwork** __ndxlNetworks;
extern size_t __ndxlNetworkCount;
extern size_t __ndxlNetworkIndex;
extern size_t __ndxlNetworkCap;

_NDXLEXP_ void __ndxlInit();
_NDXLEXP_ const double __ndxlRandomDouble();
_NDXLEXP_ void __ndxlResizeNetworkArray();
_NDXLEXP_ void __ndxlSetLastError(const int _Code, const char* const _Msg);
_NDXLEXP_ const double __ndxlSquare(const double _X);

// ^^^^^ Internal global variables and functions | struct forward declarations vvvvv

typedef struct NDXLChannelIncrement NDXLChannelIncrement;
typedef struct NDXLChannelVertex NDXLChannelVertex;
typedef struct NDXLChannelEdge NDXLChannelEdge;
typedef struct NDXLEdgeChannel NDXLEdgeChannel;
typedef struct NDXLVertexChannel NDXLVertexChannel;
typedef struct NDXLVertexState NDXLVertexState;

// ^^^^^ Struct forward declarations | network struct vvvvv

struct NDXLNetwork
{
	void* params;					// Arbitrary pointer to extra data used by this network

	size_t idx, nidx;				// Absolute index (how manieth network) and relative index (position in network array), respectively
	size_t nv, nsv;					// Number of vertices, and how many are stochastic, respectively
	size_t ne, nse;					// Number of edges, and how many are stochastic, respectively
	size_t nc;						// Number of noise channels
	size_t vidx, vcap;				// Index of next vertex, and maximum number of vertices holdable, respectively
	size_t eidx, ecap;				// Index of next edge, and maximum number of edges holdable, respectively
	size_t cidx, ccap;				// Index of next noise channel, and maximum number of channels holdable, respectively
	size_t aidx;					// Index of next agent (also functions as total number of agents)
	NDXLVertex** vertices;			// Array of NDXLVertex* objects belonging to this network
	NDXLEdge** edges;				// Array of NDXLEdge* objects belonging to this network
	NDXLChannel** channels;			// Array of NDXLChannel* objects belonging to this network

	double time;					// Local simtime of this network
	double h;						// Current step size
	double s;						// Square root of current step size
	double atol, rtol;				// Absolute and relative tolerances, respectively
	const double* butcher;			// Butcher table C-values being used
	char stage;						// Next Runge-Kutta stage to be evaluated
	char method_explicit;			// Whether the integration method is explicit or implicit

	size_t jacn;					// Network-wide state vector dimensionality
	size_t jacn_cap;				// Capacity of state vectors
	double* state;					// Combined state vector across all agents
	double* rkf[9];					// Intermediate Runge-Kutta function evaluations
	double* stage_state[2];			// Intermediate Runge-Kutta stages
};

// ^^^^^ Network struct | channel structs vvvvv

struct NDXLChannelVertex
{
	NDXLVertexChannel* vchannel;	// Channel node in vertex array
	NDXLVertex* vertex;				// Vertex pointed to by this node
	size_t cidx;					// Position in vertex's channel array
};

struct NDXLChannelEdge
{
	NDXLEdgeChannel* echannel;		// Channel node in edge array
	NDXLEdge* edge;					// Edge pointed to by this node
	size_t cidx;					// Position in edge's channel array
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
	size_t idx, cidx;				// Absolute index (how manieth channel) and relative index (position in network array), respectively
	size_t nv, vcap;				// Current and maximum number of vertices affected by channel, respectively
	size_t ne, ecap;				// Current and maximum number of edges affected by channel, respectively
	NDXLChannelVertex** vertices;	// Array of vertices affected by this channel
	NDXLChannelEdge** edges;		// Array of edges affected by this channel
	NDXLChannelIncrement* b_head;	// Oldest Brownian increment in RSwM stack
	NDXLChannelIncrement* b_tail;	// Newest Brownian increment in RSwM stack
};

// ^^^^^ Channel structs | vertex structs vvvvv

struct NDXLVertexChannel
{
	NDXLChannelVertex* cvert;		// Vertex node in channel array
	NDXLChannel* channel;			// Channel pointed to by this node
	size_t vidx;					// Position in channel's vertex array
};

struct NDXLVertex
{
	NDXLNetwork* network;			// Network in which this vertex resides
	double* state0;					// Initial state vector of this vertex
	double* interp_state;			// For when an interpolated or zero-vector needs to be returned
	size_t dimension, datsz;		// Size of vector in elements and bytes, respectively
	NDXLVertexDynamics dynamics;	// Dynamics function this vertex's state evolves according to
	NDXLVertexNoise noise;			// Noise function on this vertex
	NDXLVertexCallback callback;	// Discrete callback function for this vertex
	double atol, rtol;				// Absolute and relative tolerances, respectively
	void* params;					// Arbitrary pointer to extra data used by this vertex

	size_t idx, vidx, aidx, nidx;	// Absolute index (how manieth vertex), relative index (position in network array), agent index (used to judge half of network array), and state vector index, respectively
	size_t ne, ecap;				// Current and maximum number of edges connected to this vertex, respectively
	size_t nc, ccap;				// Current and maximum number of noise channels affecting this vertex, respectively
	size_t indeg, outdeg;			// In-degree and out-degree, respectively
	double time;					// Local simulation time of this vertex
	double* rkg[2];					// Local diffusion matrices for stochastic methods
	NDXLEdge** edges;				// Array of edges connected to this vertex
	NDXLVertexChannel** channels;	// Array of noise channels affecting this vertex
};

// ^^^^^ Vertex structs | edge structs vvvvv

struct NDXLEdgeChannel
{
	NDXLChannelEdge* cedge;			// Edge node in channel array
	NDXLChannel* channel;			// Channel pointed to by this node
	size_t eidx;					// Position in channel's edge array
};

struct NDXLVertexState
{
	double* state;					// Stored state
	double* slope;					// Stored slope
	double time;					// Time at which state existed
	NDXLVertexState* prev;			// Previous state in state stack
	NDXLVertexState* next;			// Next state in state stack
};

struct NDXLEdge
{
	NDXLVertex* src, *dest;			// Source and destination vertices, respectively
	double* state0;					// Initial state vector of this edge
	double* null_state;				// For when the zero-vector needs to be returned
	size_t dimension, datsz;		// Size of vector in elements and bytes, respectively
	double delay;					// Time taken for information to travel between vertices
	char directed;					// If true, information only flows from src to dest, and both ways otherwise
	NDXLEdgeDynamics dynamics;		// Dynamics function this edge's state evolves according to
	NDXLEdgeNoise noise;			// Noise function on this edge
	NDXLEdgeCallback callback;		// Discrete callback function for this edge
	double atol, rtol;				// Absolute and relative tolerances, respectively
	void* params;					// Arbitrary pointer to extra data used by this edge

	size_t idx, eidx, aidx, nidx;	// Absolute index (how manieth edge), relative index (position in network array), agent index (used to judge half of network array), and state vector index, respectively
	size_t sidx, didx;				// Index in source and destination vertex edge arrays, respectively
	size_t nc, ccap;				// Current and maximum number of noise channels affecting this edge, respectively
	double time;					// Local simulation time of this edge
	double* rkg[2];					// Local diffusion matrices for stochastic methods
	NDXLVertexState* src_head;		// Oldest historical state of source vertex
	NDXLVertexState* src_tail;		// Newest historical state of source vertex
	NDXLVertexState* dest_head;		// Oldest historical state of destination vertex
	NDXLVertexState* dest_tail;		// Newest historical state of destination vertex
	NDXLEdgeChannel** channels;		// Array of noise channels affecting this edge
};

// ^^^^^ Edge structs | network functions vvvvv

_NDXLEXP_ NDXLNetwork* __ndxlCreateNetwork(void* _Params);
_NDXLEXP_ void __ndxlDestroyNetwork(NDXLNetwork* _Network);
_NDXLEXP_ const size_t __ndxlGetNetworkNoiseDimension(NDXLNetwork* _Network);
_NDXLEXP_ const size_t __ndxlGetNetworkOrder(NDXLNetwork* _Network);
_NDXLEXP_ void* __ndxlGetNetworkParams(NDXLNetwork* _Network);
_NDXLEXP_ const size_t __ndxlGetNetworkSize(NDXLNetwork* _Network);
_NDXLEXP_ const double __ndxlGetNetworkTime(NDXLNetwork* _Network);
_NDXLEXP_ void __ndxlSetNetworkParams(NDXLNetwork* _Network, void* _Params);
_NDXLEXP_ NDXLChannel* __ndxlNetworkChannel(NDXLNetwork* _Network, const size_t _Idx);
_NDXLEXP_ NDXLChannel* __ndxlNetworkChannelByIndex(NDXLNetwork* _Network, const size_t _Idx);
_NDXLEXP_ NDXLEdge* __ndxlNetworkEdge(NDXLNetwork* _Network, const size_t _Idx);
_NDXLEXP_ NDXLEdge* __ndxlNetworkEdgeByIndex(NDXLNetwork* _Network, const size_t _Idx);
_NDXLEXP_ NDXLVertex* __ndxlNetworkVertex(NDXLNetwork* _Network, const size_t _Idx);
_NDXLEXP_ NDXLVertex* __ndxlNetworkVertexByIndex(NDXLNetwork* _Network, const size_t _Idx);
_NDXLEXP_ void __ndxlNetworkResizeArrays(NDXLNetwork* _Network);
_NDXLEXP_ void __ndxlNetworkResizeVectors(NDXLNetwork* _Network);
_NDXLEXP_ const int __ndxlNetworkSimulateRalston(NDXLNetwork* _Network, const double _Time, const double _Step, NDXLNetworkCallback _Callback);
_NDXLEXP_ const int __ndxlNetworkSimulateNystrom(NDXLNetwork* _Network, const double _Time, const double _Step, NDXLNetworkCallback _Callback);
_NDXLEXP_ const int __ndxlNetworkSimulateDopri(NDXLNetwork* _Network, const double _Time, const double _Atol, const double _Rtol, const double* _Tstops, const size_t _Tcount, NDXLNetworkCallback _Callback);
_NDXLEXP_ const int __ndxlNetworkSimulateRkck(NDXLNetwork* _Network, const double _Time, const double _Atol, const double _Rtol, const double* _Tstops, const size_t _Tcount, NDXLNetworkCallback _Callback);
_NDXLEXP_ const int __ndxlNetworkSimulateEuler(NDXLNetwork* _Network, const double _Time, const double _Step, NDXLNetworkCallback _Callback);
_NDXLEXP_ const int __ndxlNetworkSimulateHeun(NDXLNetwork* _Network, const double _Time, const double _Atol, const double _Rtol, const double* _Tstops, const size_t _Tcount, NDXLNetworkCallback _Callback);
_NDXLEXP_ const int __ndxlNetworkSimulateRadau(NDXLNetwork* _Network, const double _Time, const double _Atol, const double _Rtol, const double* _Tstops, const size_t _Tcount, NDXLNetworkCallback _Callback);


// ^^^^^ Network functions | channel functions vvvvv

_NDXLEXP_ NDXLChannelEdge* __ndxlCreateChannelEdge(NDXLChannel* _Channel, NDXLEdge* _Edge);
_NDXLEXP_ NDXLChannelVertex* __ndxlCreateChannelVertex(NDXLChannel* _Channel, NDXLVertex* _Vertex);
_NDXLEXP_ NDXLChannelIncrement* __ndxlCreateChannelIncrement(NDXLChannel* _Channel, const double _Hold, const double _Hnew);
_NDXLEXP_ void __ndxlDestroyChannelEdge(NDXLChannelEdge* _Cedge);
_NDXLEXP_ void __ndxlDestroyChannelVertex(NDXLChannelVertex* _Cvertex);
_NDXLEXP_ void __ndxlDestroyChannelIncrement(NDXLChannelIncrement* _Increment);
_NDXLEXP_ NDXLChannel* __ndxlCreateChannel(NDXLNetwork* _Network, void* _Params);
_NDXLEXP_ void __ndxlDestroyChannel(NDXLChannel* _Channel);
_NDXLEXP_ const size_t __ndxlGetChannelEdgeDimension(NDXLChannel* _Channel);
_NDXLEXP_ NDXLNetwork* __ndxlGetChannelNetwork(NDXLChannel* _Channel);
_NDXLEXP_ void* __ndxlGetChannelParams(NDXLChannel* _Channel);
_NDXLEXP_ const size_t __ndxlGetChannelVertexDimension(NDXLChannel* _Channel);
_NDXLEXP_ void __ndxlSetChannelParams(NDXLChannel* _Channel, void* _Params);
_NDXLEXP_ NDXLEdge* __ndxlChannelEdge(NDXLChannel* _Channel, const size_t _Idx);
_NDXLEXP_ const double __ndxlChannelNormal(NDXLChannel* _Channel);
_NDXLEXP_ const double __ndxlChannelRedraw(NDXLChannel* _Channel);
_NDXLEXP_ void __ndxlChannelResizeEdgeArray(NDXLChannel* _Channel);
_NDXLEXP_ void __ndxlChannelResizeVertexArray(NDXLChannel* _Channel);
_NDXLEXP_ NDXLVertex* __ndxlChannelVertex(NDXLChannel* _Channel, const size_t _Idx);

// ^^^^^ Channel functions | vertex functions vvvvv

_NDXLEXP_ NDXLVertexChannel* __ndxlCreateVertexChannel(NDXLVertex* _Vertex, NDXLChannel* _Channel);
_NDXLEXP_ void __ndxlDestroyVertexChannel(NDXLVertexChannel* _Vchannel);
_NDXLEXP_ NDXLVertex* __ndxlCreateVertex(NDXLNetwork* _Network, const double* _State, const size_t _Dimension, NDXLVertexDynamics _Func, NDXLVertexNoise _Noise, NDXLChannel** _Channels, const size_t _Nchannels, NDXLVertexCallback _Callback, const double _Atol, const double _Rtol, void* _Params);
_NDXLEXP_ void __ndxlDestroyVertex(NDXLVertex* _Vertex);
_NDXLEXP_ NDXLVertexCallback __ndxlGetVertexCallback(NDXLVertex* _Vertex);
_NDXLEXP_ const size_t __ndxlGetVertexDegree(NDXLVertex* _Vertex);
_NDXLEXP_ const size_t __ndxlGetVertexDegreeIn(NDXLVertex* _Vertex);
_NDXLEXP_ const size_t __ndxlGetVertexDegreeOut(NDXLVertex* _Vertex);
_NDXLEXP_ const size_t __ndxlGetVertexDimension(NDXLVertex* _Vertex);
_NDXLEXP_ NDXLVertexDynamics __ndxlGetVertexDynamics(NDXLVertex* _Vertex);
_NDXLEXP_ const double* __ndxlGetVertexInitialState(NDXLVertex* _Vertex);
_NDXLEXP_ NDXLVertex* __ndxlGetVertexNeighbour(NDXLVertex* _Vertex, NDXLEdge* _Edge);
_NDXLEXP_ const size_t __ndxlGetVertexNeighbourDimension(NDXLVertex* _Vertex, NDXLEdge* _Edge);
_NDXLEXP_ const size_t __ndxlGetVertexNeighbourNoiseDimension(NDXLVertex* _Vertex, NDXLEdge* _Edge);
_NDXLEXP_ const double* __ndxlGetVertexNeighbourState(NDXLVertex* _Vertex, NDXLEdge* _Edge);
_NDXLEXP_ NDXLNetwork* __ndxlGetVertexNetwork(NDXLVertex* _Vertex);
_NDXLEXP_ NDXLVertexNoise __ndxlGetVertexNoise(NDXLVertex* _Vertex);
_NDXLEXP_ const size_t __ndxlGetVertexNoiseDimension(NDXLVertex* _Vertex);
_NDXLEXP_ void* __ndxlGetVertexParams(NDXLVertex* _Vertex);
_NDXLEXP_ const double __ndxlGetVertexTime(NDXLVertex* _Vertex);
_NDXLEXP_ void __ndxlSetVertexCallback(NDXLVertex* _Vertex, NDXLVertexCallback _Callback);
_NDXLEXP_ void __ndxlSetVertexDynamics(NDXLVertex* _Vertex, NDXLVertexDynamics _Dynamics);
_NDXLEXP_ void __ndxlSetVertexNoise(NDXLVertex* _Vertex, NDXLVertexNoise _Noise);
_NDXLEXP_ void __ndxlSetVertexParams(NDXLVertex* _Vertex, void* _Params);
_NDXLEXP_ NDXLChannel* __ndxlVertexChannel(NDXLVertex* _Vertex, const size_t _Idx);
_NDXLEXP_ NDXLEdge* __ndxlVertexEdge(NDXLVertex* _Vertex, const size_t _Idx);
_NDXLEXP_ const char __ndxlVertexHasNeighbour(NDXLVertex* _Source, NDXLVertex* _Destination);
_NDXLEXP_ void __ndxlVertexResizeChannelArray(NDXLVertex* _Vertex);
_NDXLEXP_ void __ndxlVertexResizeEdgeArray(NDXLVertex* _Vertex);
_NDXLEXP_ double* __ndxlVertexState(NDXLVertex* _Vertex);

// ^^^^^ Vertex functions | edge functions vvvvv

_NDXLEXP_ NDXLEdgeChannel* __ndxlCreateEdgeChannel(NDXLEdge* _Edge, NDXLChannel* _Channel);
_NDXLEXP_ void __ndxlDestroyEdgeChannel(NDXLEdgeChannel* _Echannel);
_NDXLEXP_ NDXLVertexState* __ndxlCreateVertexState(NDXLEdge* _Edge, const char _Which);
_NDXLEXP_ void __ndxlDestroyVertexState(NDXLVertexState* _State);
_NDXLEXP_ NDXLEdge* __ndxlCreateEdge(NDXLVertex* _Vert1, NDXLVertex* _Vert2, const double* _State, const size_t _Dimension, const double _Delay, const char _Directed, NDXLEdgeDynamics _Func, NDXLEdgeNoise _Noise, NDXLChannel** _Channels, const size_t _Nchannels, NDXLEdgeCallback _Callback, const double _Atol, const double _Rtol, void* _Params);
_NDXLEXP_ void __ndxlDestroyEdge(NDXLEdge* _Edge);
_NDXLEXP_ void __ndxlEdgeAccumulateEffects(NDXLEdge* _Edge, const double _Delta);
_NDXLEXP_ NDXLEdgeCallback __ndxlGetEdgeCallback(NDXLEdge* _Edge);
_NDXLEXP_ const double __ndxlGetEdgeDelay(NDXLEdge* _Edge);
_NDXLEXP_ NDXLVertex* __ndxlGetEdgeDestination(NDXLEdge* _Edge);
_NDXLEXP_ const size_t __ndxlGetEdgeDimension(NDXLEdge* _Edge);
_NDXLEXP_ const char __ndxlGetEdgeDirected(NDXLEdge* _Edge);
_NDXLEXP_ NDXLEdgeDynamics __ndxlGetEdgeDynamics(NDXLEdge* _Edge);
_NDXLEXP_ const double* __ndxlGetEdgeInitialState(NDXLEdge* _Edge);
_NDXLEXP_ NDXLNetwork* __ndxlGetEdgeNetwork(NDXLEdge* _Edge);
_NDXLEXP_ NDXLEdgeNoise __ndxlGetEdgeNoise(NDXLEdge* _Edge);
_NDXLEXP_ const size_t __ndxlGetEdgeNoiseDimension(NDXLEdge* _Edge);
_NDXLEXP_ void* __ndxlGetEdgeParams(NDXLEdge* _Edge);
_NDXLEXP_ const double* __ndxlGetEdgeRelativeState(NDXLEdge* _Edge, NDXLVertex* _Vertex);
_NDXLEXP_ NDXLVertex* __ndxlGetEdgeSource(NDXLEdge* _Edge);
_NDXLEXP_ const double __ndxlGetEdgeTime(NDXLEdge* _Edge);
_NDXLEXP_ void __ndxlSetEdgeCallback(NDXLEdge* _Edge, NDXLEdgeCallback _Callback);
_NDXLEXP_ void __ndxlSetEdgeDynamics(NDXLEdge* _Edge, NDXLEdgeDynamics _Func);
_NDXLEXP_ void __ndxlSetEdgeNoise(NDXLEdge* _Edge, NDXLEdgeNoise _Noise);
_NDXLEXP_ void __ndxlSetEdgeParams(NDXLEdge* _Edge, void* _Params);
_NDXLEXP_ NDXLChannel* __ndxlEdgeChannel(NDXLEdge* _Edge, const size_t _Idx);
_NDXLEXP_ const char __ndxlEdgeConnectsVertex(NDXLEdge* _Edge, NDXLVertex* _Vertex);
_NDXLEXP_ void __ndxlEdgeResizeChannelArray(NDXLEdge* _Edge);
_NDXLEXP_ double* __ndxlEdgeState(NDXLEdge* _Edge);

// ^^^^^ Edge funtions

#endif // _INTERNAL_NDXL_
