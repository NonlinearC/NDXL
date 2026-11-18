//------------------------------\\
// NDXL header for vertex class	\\
// Ethan Codron 2025			\\
// GNU GPL-3.0					\\
//------------------------------\\
// Defines a vertex class with	\\
// properties pertaining to the	\\
// evolution dynamics of a net-	\\
// work vertex/node.			\\
//------------------------------\\

#ifndef _NDXL_VERTEX_
#define _NDXL_VERTEX_

#include "../ndxlcore.h"

// void NDXLVertexFunction(NDXLVertex* _Vertex, double* _OutState, const double* _InState, double _Time, void* _Params)
typedef void(*NDXLVertexFunction)(NDXLVertex*, double*, const double*, double, void*);

// void NDXLVertexCallback(NDXLVertex* _Vertex, double* _State, double _Time, void* _Params)
typedef void(*NDXLVertexCallback)(NDXLVertex*, double*, double, void*);

#ifdef __cplusplus
extern "C" {
#endif

// Constructors and destructor vvvvv

_NDXLIMP_ NDXLVertex* _CDECL ndxlCreateVertex(
	_In_ _Notnull_ NDXLNetwork* _Network,
	_In_reads_bytes_(_Order * sizeof(double)) _Notnull_ const double* _State,
	_In_ const size_t _Order,
	_In_ _Maybenull_ NDXLVertexFunction _Func,
	_In_ _Maybenull_ NDXLVertexFunction _Noise,
	_In_ _Maybenull_ NDXLVertexCallback _Callback,
	_In_ _Maybenull_ void* _Params
) _NOEXCEPT;

_NDXLIMP_ NDXLVertex* _CDECL ndxlCopyVertex(
	_In_ _Notnull_ NDXLVertex* _Vertex,
	_In_ _Notnull_ NDXLNetwork* _Network,
	_In_ const bool _CopyState0,
	_In_ _Maybenull_ void* _Params
) _NOEXCEPT;

_NDXLIMP_ const int _CDECL ndxlDestroyVertex(
	_In_ _Notnull_ NDXLVertex* _Vertex
) _NOEXCEPT;

// ^^^^^ Constructors and destructor | getters vvvvv

_NDXLIMP_ const size_t _CDECL ndxlGetVertexDegree(
	_In_ _Notnull_ NDXLVertex* _Vertex
) _NOEXCEPT;

_NDXLIMP_ const size_t _CDECL ndxlGetVertexInDegree(
	_In_ _Notnull_ NDXLVertex* _Vertex
) _NOEXCEPT;

_NDXLIMP_ const size_t _CDECL ndxlGetVertexOutDegree(
	_In_ _Notnull_ NDXLVertex* _Vertex
) _NOEXCEPT;

_NDXLIMP_ const int _CDECL ndxlGetVertexEdges(
	_In_ _Notnull_ NDXLVertex* _Vertex,
	_Out_ _Notnull_ NDXLEdge** _Edges
) _NOEXCEPT;

_NDXLIMP_ NDXLEdge* _CDECL ndxlGetVertexFirstEdge(
	_In_ _Notnull_ NDXLVertex* _Vertex
) _NOEXCEPT;

_NDXLIMP_ NDXLEdge* _CDECL ndxlGetVertexLastEdge(
	_In_ _Notnull_ NDXLVertex* _Vertex
) _NOEXCEPT;

_NDXLIMP_ NDXLVertex* _CDECL ndxlGetVertexNeighbour(
	_In_ _Notnull_ NDXLVertex* _Vertex,
	_In_ _Notnull_ NDXLEdge* _Edge
) _NOEXCEPT;

_NDXLIMP_ const int _CDECL ndxlGetVertexNeighbours(
	_In_ _Notnull_ NDXLVertex* _Vertex,
	_Out_ _Notnull_ NDXLVertex** _Neigbours
) _NOEXCEPT;

_NDXLIMP_ const double* _CDECL ndxlGetVertexNeighbourState(
	_In_ _Notnull_ NDXLVertex* _Vertex,
	_In_ _Notnull_ NDXLEdge* _Edge
) _NOEXCEPT;

_NDXLIMP_ NDXLNetwork* _CDECL ndxlGetVertexNetwork(
	_In_ _Notnull_ NDXLVertex* _Vertex
) _NOEXCEPT;

_NDXLIMP_ NDXLEdge* _CDECL ndxlGetVertexNextEdge(
	_In_ _Notnull_ NDXLVertex* _Vertex,
	_In_ _Notnull_ NDXLEdge* _Edge
) _NOEXCEPT;

_NDXLIMP_ const size_t _CDECL ndxlGetVertexOrder(
	_In_ _Notnull_ NDXLVertex* _Vertex
) _NOEXCEPT;

_NDXLIMP_ const double* _CDECL ndxlGetVertexState(
	_In_ _Notnull_ NDXLVertex* _Vertex
) _NOEXCEPT;

_NDXLIMP_ const double* _CDECL ndxlGetVertexState0(
	_In_ _Notnull_ NDXLVertex* _Vertex
) _NOEXCEPT;

// ^^^^^ Getters | setters vvvvv

_NDXLIMP_ const int _CDECL ndxlSetVertexState(
	_In_ _Notnull_ NDXLVertex* _Vertex,
	_In_ _Notnull_ const double* _State
) _NOEXCEPT;

// ^^^^^ Setters | functions vvvvv

_NDXLIMP_ const int _CDECL ndxlVertexReset(
	_In_ _Notnull_ NDXLVertex* _Vertex
) _NOEXCEPT;

// ^^^^^ Functions

#ifdef __cplusplus
}
#endif

#endif // _NDXL_VERTEX_