//----------------------------------\\
// NDXL header for network class	\\
// Ethan Codron 2025				\\
// GNU GPL-3.0						\\
//----------------------------------\\
// Defines a network class to link	\\
// edges and nodes in a graph.		\\
//----------------------------------\\

#ifndef _NDXL_NETWORK_
#define _NDXL_NETWORK_

#include "ndxl_edge.h"

// void NDXLNetworkCallback(NDXLNetwork* _Network, double _Time, void* _Params)
typedef void(*NDXLNetworkCallback)(NDXLNetwork*, double, void*);

#ifdef __cplusplus
extern "C" {
#endif

// Constructors and destructor vvvvv

_NDXLIMP_ NDXLNetwork* _CDECL ndxlCreateNetwork(
	_In_ void* _Params
) _NOEXCEPT;

_NDXLIMP_ const int _CDECL ndxlDestroyNetwork(
	_In_ NDXLNetwork* _Network
) _NOEXCEPT;

// ^^^^^ Constructors and destructor | getters vvvvv

_NDXLIMP_ const int _CDECL ndxlGetNetworkEdges(
	_In_ NDXLNetwork* _Network,
	_Out_ _Notnull_ NDXLEdge** _Edges
) _NOEXCEPT;

_NDXLIMP_ const size_t _CDECL ndxlGetNetworkOrder(
	_In_ NDXLNetwork* _Network
) _NOEXCEPT;

_NDXLIMP_ const size_t _CDECL ndxlGetNetworkSize(
	_In_ NDXLNetwork* _Network
) _NOEXCEPT;

_NDXLIMP_ const int _CDECL ndxlGetNetworkVertices(
	_In_ NDXLNetwork* _Network,
	_Out_ _Notnull_ NDXLVertex** _Vertices
) _NOEXCEPT;

 // ^^^^^ Getters | functions vvvvv

_NDXLIMP_ const int _CDECL ndxlNetworkReset(
	_In_ NDXLNetwork* _Network
) _NOEXCEPT;

_NDXLIMP_ const int _CDECL ndxlNetworkSimulateRalston(
	_In_ NDXLNetwork* _Network,
	_In_ const double _Time,
	_In_ const double _Step,
	_In_ NDXLNetworkCallback _Callback
) _NOEXCEPT;

_NDXLIMP_ const int _CDECL ndxlNetworkSimulateNystrom(
	_In_ NDXLNetwork* _Network,
	_In_ const double _Time,
	_In_ const double _Step,
	_In_ NDXLNetworkCallback _Callback
) _NOEXCEPT;

_NDXLIMP_ const int _CDECL ndxlNetworkSimulateDopri(
	_In_ NDXLNetwork* _Network,
	_In_ const double _Time,
	_In_ const double _Atol,
	_In_ const double _Rtol,
	_In_reads_bytes_(_Tcount * sizeof(double)) const double* _Tstops,
	_In_ const size_t _Tcount,
	_In_ NDXLNetworkCallback _Callback
) _NOEXCEPT;

_NDXLIMP_ const int _CDECL ndxlNetworkSimulateRkck(
	_In_ NDXLNetwork* _Network,
	_In_ const double _Time,
	_In_ const double _Atol,
	_In_ const double _Rtol,
	_In_reads_bytes_(_Tcount * sizeof(double)) const double* _Tstops,
	_In_ const size_t _Tcount,
	_In_ NDXLNetworkCallback _Callback
) _NOEXCEPT;

_NDXLIMP_ const int _CDECL ndxlNetworkSimulateNorsett(
	_In_ NDXLNetwork* _Network,
	_In_ const double _Time,
	_In_ const double _Step,
	_In_ NDXLNetworkCallback _Callback
) _NOEXCEPT;

_NDXLIMP_ const int _CDECL ndxlNetworkSimulateRadau(
	_In_ NDXLNetwork* _Network,
	_In_ const double _Time,
	_In_ const double _Atol,
	_In_ const double _Rtol,
	_In_reads_bytes_(_Tcount * sizeof(double)) const double* _Tstops,
	_In_ const size_t _Tcount,
	_In_ NDXLNetworkCallback _Callback
) _NOEXCEPT;

// ^^^^^ Functions

#ifdef __cplusplus
}
#endif

#endif // _NDXL_NETWORK_