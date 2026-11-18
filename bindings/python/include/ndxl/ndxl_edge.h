//------------------------------\\
// NDXL header for edge class	\\
// Ethan Codron 2025			\\
// GNU GPL-3.0					\\
//------------------------------\\
// Defines an edge class which	\\
// connects two vertices along	\\
// a potentially dynamic link.	\\
//------------------------------\\

#ifndef _NDXL_EDGE_
#define _NDXL_EDGE_

#include "ndxl_vertex.h"

// void NDXLEdgeFunction(NDXLEdge* _Edge, double* _OutState, const double* _InState, double _Time, void* _Params)
typedef void(*NDXLEdgeFunction)(NDXLEdge*, double*, const double*, double, void*);

// void NDXLEdgeCallback(NDXLVertex* _Vertex, double* _State, double _Time, void* _Params)
typedef void(*NDXLEdgeCallback)(NDXLEdge*, double*, double, void*);

#ifdef __cplusplus
extern "C" {
#endif

// Constructors and destructor vvvvv

_NDXLIMP_ NDXLEdge* _CDECL ndxlCreateEdge(
	_In_ _Notnull_ NDXLVertex* _Vert1,
	_In_ _Notnull_ NDXLVertex* _Vert2,
	_In_reads_bytes_(_Order * sizeof(double)) const double* _State,
	_In_ const size_t _Order,
	_In_ const double _Delay,
	_In_ const bool _Directed,
	_In_ NDXLEdgeFunction _Func,
	_In_ NDXLEdgeFunction _Noise,
	_In_ NDXLEdgeCallback _Callback,
	_In_ _Maybenull_ void* _Params
) _NOEXCEPT;

_NDXLIMP_ NDXLEdge* _CDECL ndxlCopyEdge(
	_In_ _Notnull_ NDXLEdge* _Edge,
	_In_ _Notnull_ NDXLVertex* _Vert1,
	_In_ _Notnull_ NDXLVertex* _Vert2,
	_In_ const bool _Directed,
	_In_ const bool _CopyState0,
	_In_ _Maybenull_ void* _Params
) _NOEXCEPT;

_NDXLIMP_ const int _CDECL ndxlDestroyEdge(
	_In_ _Notnull_ NDXLEdge* _Edge
) _NOEXCEPT;

// ^^^^^ Constructors and destructor | getters vvvvv

_NDXLIMP_ const double _CDECL ndxlGetEdgeDelay(
	_In_ _Notnull_ NDXLEdge* _Edge
) _NOEXCEPT;

_NDXLIMP_ const size_t _CDECL ndxlGetEdgeOrder(
	_In_ _Notnull_ NDXLEdge* _Edge
) _NOEXCEPT;

_NDXLIMP_ const double* _CDECL ndxlGetEdgeState(
	_In_ _Notnull_ NDXLEdge* _Edge
) _NOEXCEPT;

_NDXLIMP_ const double* _CDECL ndxlGetEdgeState0(
	_In_ _Notnull_ NDXLEdge* _Edge
) _NOEXCEPT;

// ^^^^^ Getters | setters vvvvv

_NDXLIMP_ const int _CDECL ndxlSetEdgeState(
	_In_ _Notnull_ NDXLEdge* _Edge,
	_In_ const double* _State
) _NOEXCEPT;

// ^^^^^ Setters | functions vvvvv

_NDXLIMP_ const int _CDECL ndxlEdgeReset(
	_In_ _Notnull_ NDXLEdge* _Edge
) _NOEXCEPT;

// ^^^^^ Functions

#ifdef __cplusplus
}
#endif

#endif // _NDXL_EDGE_