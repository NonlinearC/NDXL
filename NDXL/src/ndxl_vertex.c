//--------------------------------------\\
// NDXL source file for vertex class	\\
// Copyright Ethan Codron 2026			\\
// GNU GPL-3.0							\\
//--------------------------------------\\
// Source file providing all vertex		\\
// functions used by the user.			\\
//--------------------------------------\\

#include "internal/internal.h"

_NDXLEXP_ NDXLVertex* ndxlCreateVertex(NDXLNetwork* _Network, const double* _State, const size_t _Order, NDXLVertexDynamics _Func, NDXLVertexNoise _Noise, NDXLChannel** _Channels, const size_t _Nchannels, NDXLVertexCallback _Callback, void* _Params)
{
	NDXLVertex* vertex = 0;
	if (!_Network)
		__setLastError(NDXL_ARG_ERR, "ndxlCreateVertex: null parent network.");
	else if (!_Noise && _Channels)
		__setLastError(NDXL_VALUE_ERR, "ndxlCreateVertex: channels specified with no noise function.");
	else if (!_Noise && _Nchannels)
		__setLastError(NDXL_VALUE_ERR, "ndxlCreateVertex: channel count specified with no noise function.");
	else if (_Channels && !_Nchannels)
		__setLastError(NDXL_VALUE_ERR, "ndxlCreateVertex: channels specified with nonpositive channel count.");
	else
		vertex = __ndxlCreateVertex(_Network, _State, _Order ? _Order : 1, _Func, _Noise, _Channels, _Nchannels, _Callback, _Params);
	return vertex;
}

_NDXLEXP_ const int ndxlDestroyVertex(NDXLVertex** _Vertex)
{
	int _Result = NDXL_ERR;
	if (!_Vertex)
		__setLastError(NDXL_ARG_ERR, "ndxlDestroyVertex: null adress.");
	else if (!*_Vertex)
		__setLastError(NDXL_VALUE_ERR, "ndxlDestroyVertex: null vertex.");
	else
	{
		__ndxlDestroyVertex(*_Vertex);
		*_Vertex = 0;
		_Result = NDXL_OK;
	}
	return _Result;
}

_NDXLEXP_ NDXLVertexCallback ndxlGetVertexCallback(NDXLVertex* _Vertex)
{
	if (_Vertex)
		return __ndxlGetVertexCallback(_Vertex);
	__setLastError(NDXL_ARG_ERR, "ndxlGetVertexCallback: null vertex.");
	return 0;
}

_NDXLEXP_ const size_t ndxlGetVertexChannelCount(NDXLVertex* _Vertex)
{
	if (_Vertex)
		return __ndxlGetVertexChannelCount(_Vertex);
	__setLastError(NDXL_ARG_ERR, "ndxlGetVertexChannelCount: null vertex.");
	return NDXL_ERR;
}

_NDXLEXP_ const int ndxlGetVertexChannels(NDXLVertex* _Vertex, NDXLChannel** _Channels)
{
	int _Result = NDXL_ERR;
	if (!_Vertex)
		__setLastError(NDXL_ARG_ERR, "ndxlGetVertexChannels: null vertex.");
	else if (!_Channels)
		__setLastError(NDXL_ARG_ERR, "ndxlGetVertexChannels: null channel output.");
	else
	{
		__ndxlGetVertexChannels(_Vertex, _Channels);
		_Result = NDXL_OK;
	}
	return _Result;
}

_NDXLEXP_ const size_t ndxlGetVertexDegree(NDXLVertex* _Vertex)
{
	if (_Vertex)
		return __ndxlGetVertexDegree(_Vertex);
	__setLastError(NDXL_ARG_ERR, "ndxlGetVertexDegree: null vertex.");
	return NDXL_ERR;
}

_NDXLEXP_ const size_t ndxlGetVertexDegreeIn(NDXLVertex* _Vertex)
{
	if (_Vertex)
		return __ndxlGetVertexDegreeIn(_Vertex);
	__setLastError(NDXL_ARG_ERR, "ndxlGetVertexDegree: null vertex.");
	return NDXL_ERR;
}

_NDXLEXP_ const size_t ndxlGetVertexDegreeOut(NDXLVertex* _Vertex)
{
	if (_Vertex)
		return __ndxlGetVertexDegreeOut(_Vertex);
	__setLastError(NDXL_ARG_ERR, "ndxlGetVertexDegree: null vertex.");
	return NDXL_ERR;
}

_NDXLEXP_ NDXLVertexDynamics ndxlGetVertexDynamics(NDXLVertex* _Vertex)
{
	if (_Vertex)
		return __ndxlGetVertexDynamics(_Vertex);
	__setLastError(NDXL_ARG_ERR, "ndxlGetVertexDynamics: null vertex.");
	return 0;
}

_NDXLEXP_ const int ndxlGetVertexEdges(NDXLVertex* _Vertex, NDXLEdge** _Edges)
{
	int _Result = NDXL_ERR;
	if (!_Vertex)
		__setLastError(NDXL_ARG_ERR, "ndxlGetVertexEdges: null vertex.");
	else if (!_Edges)
		__setLastError(NDXL_ARG_ERR, "ndxlGetVertexEdges: null edge output.");
	else
	{
		__ndxlGetVertexEdges(_Vertex, _Edges);
		_Result = NDXL_OK;
	}
	return _Result;
}

_NDXLEXP_ NDXLEdge* ndxlGetVertexFirstEdge(NDXLVertex* _Vertex)
{
	if (_Vertex)
		return __ndxlGetVertexFirstEdge(_Vertex);
	__setLastError(NDXL_ARG_ERR, "ndxlGetVertexFirstEdge: null vertex.");
	return 0;
}

_NDXLEXP_ const double* ndxlGetVertexInitialState(NDXLVertex* _Vertex)
{
	if (_Vertex)
		return __ndxlGetVertexState0(_Vertex);
	__setLastError(NDXL_ARG_ERR, "ndxlGetVertexInitialState: null vertex.");
	return 0;
}

_NDXLEXP_ NDXLEdge* ndxlGetVertexLastEdge(NDXLVertex* _Vertex)
{
	if (_Vertex)
		return __ndxlGetVertexLastEdge(_Vertex);
	__setLastError(NDXL_ARG_ERR, "ndxlGetVertexLastEdge: null vertex.");
	return 0;
}

_NDXLEXP_ NDXLVertex* ndxlGetVertexNeighbour(NDXLVertex* _Vertex, NDXLEdge* _Edge)
{
	NDXLVertex* vertex = 0;
	if (!_Vertex)
		__setLastError(NDXL_ARG_ERR, "ndxlGetVertexNeighbour: null vertex.");
	else if (!_Edge)
		__setLastError(NDXL_ARG_ERR, "ndxlGetVertexNeighbour: null edge.");
	else if (!__ndxlEdgeConnectsVertex(_Edge, _Vertex))
		__setLastError(NDXL_VALUE_ERR, "ndxlGetVertexNeighbour: edge not connected to vertex.");
	else
		vertex = __ndxlGetVertexNeighbour(_Vertex, _Edge);
	return vertex;
}

_NDXLEXP_ const size_t ndxlGetVertexNeighbourOrder(NDXLVertex* _Vertex, NDXLEdge* _Edge)
{
	size_t _Result = NDXL_ERR;
	if (!_Vertex)
		__setLastError(NDXL_ARG_ERR, "ndxlGetVertexNeighbourOrder: null vertex.");
	else if (!_Edge)
		__setLastError(NDXL_ARG_ERR, "ndxlGetVertexNeighbourOrder: null edge.");
	else if (!__ndxlEdgeConnectsVertex(_Edge, _Vertex))
		__setLastError(NDXL_VALUE_ERR, "ndxlGetVertexNeighbourOrder: edge not connected to vertex.");
	else
		_Result = __ndxlGetVertexNeighbourOrder(_Vertex, _Edge);
	return _Result;
}

_NDXLEXP_ const int ndxlGetVertexNeighbours(NDXLVertex* _Vertex, NDXLVertex** _Neighbours)
{
	int _Result = NDXL_ERR;
	if (!_Vertex)
		__setLastError(NDXL_ARG_ERR, "ndxlGetVertexNeighbours: null vertex.");
	else if (!_Neighbours)
		__setLastError(NDXL_ARG_ERR, "ndxlGetVertexNeighbours: null vertex output.");
	else
	{
		__ndxlGetVertexNeighbours(_Vertex, _Neighbours);
		_Result = NDXL_OK;
	}
	return _Result;
}

_NDXLEXP_ const double* ndxlGetVertexNeighbourState(NDXLVertex* _Vertex, NDXLEdge* _Edge)
{
	const double* state = 0;
	if (!_Vertex)
		__setLastError(NDXL_ARG_ERR, "ndxlGetVertexNeighbourState: null vertex.");
	else if (!_Edge)
		__setLastError(NDXL_ARG_ERR, "ndxlGetVertexNeighbourState: null edge.");
	else if (!__ndxlEdgeConnectsVertex(_Edge, _Vertex))
		__setLastError(NDXL_VALUE_ERR, "ndxlGetVertexNeighbourState: edge not connected to vertex.");
	else
		state = __ndxlGetVertexNeighbourState(_Vertex, _Edge);
	return state;
}

_NDXLEXP_ NDXLNetwork* ndxlGetVertexNetwork(NDXLVertex* _Vertex)
{
	if (_Vertex)
		return __ndxlGetVertexNetwork(_Vertex);
	__setLastError(NDXL_ARG_ERR, "ndxlGetVertexNetwork: null vertex.");
	return 0;
}

_NDXLEXP_ NDXLEdge* ndxlGetVertexNextEdge(NDXLVertex* _Vertex, NDXLEdge* _Edge)
{
	NDXLEdge* edge = 0;
	if (!_Vertex)
		__setLastError(NDXL_ARG_ERR, "ndxlGetVertexNextEdge: null vertex.");
	else if (!_Edge)
		__setLastError(NDXL_ARG_ERR, "ndxlGetVertexNextEdge: null edge.");
	else if (!__ndxlEdgeConnectsVertex(_Edge, _Vertex))
		__setLastError(NDXL_VALUE_ERR, "ndxlGetVertexNextEdge: edge not connected to vertex.");
	else
		edge = __ndxlGetVertexNextEdge(_Vertex, _Edge);
	return edge;
}

_NDXLEXP_ NDXLVertexNoise ndxlGetVertexNoise(NDXLVertex* _Vertex)
{
	if (_Vertex)
		return __ndxlGetVertexNoise(_Vertex);
	__setLastError(NDXL_ARG_ERR, "ndxlGetVertexNoise: null vertex.");
	return 0;
}

_NDXLEXP_ const size_t ndxlGetVertexOrder(NDXLVertex* _Vertex)
{
	if (_Vertex)
		return __ndxlGetVertexOrder(_Vertex);
	__setLastError(NDXL_ARG_ERR, "ndxlGetVertexOrder: null vertex.");
	return NDXL_ERR;
}

_NDXLEXP_ void* ndxlGetVertexParams(NDXLVertex* _Vertex)
{
	if (_Vertex)
		return __ndxlGetVertexParams(_Vertex);
	__setLastError(NDXL_ARG_ERR, "ndxlGetVertexParams: null vertex.");
	return 0;
}

_NDXLEXP_ NDXLEdge* ndxlGetVertexPreviousEdge(NDXLVertex* _Vertex, NDXLEdge* _Edge)
{
	NDXLEdge* edge = 0;
	if (!_Vertex)
		__setLastError(NDXL_ARG_ERR, "ndxlGetVertexPreviousEdge: null vertex.");
	else if (!_Edge)
		__setLastError(NDXL_ARG_ERR, "ndxlGetVertexPreviousEdge: null edge.");
	else if (!__ndxlEdgeConnectsVertex(_Edge, _Vertex))
		__setLastError(NDXL_VALUE_ERR, "ndxlGetVertexPreviousEdge: edge not connected to vertex.");
	else
		edge = __ndxlGetVertexPreviousEdge(_Vertex, _Edge);
	return edge;
}

_NDXLEXP_ const double ndxlGetVertexTime(NDXLVertex* _Vertex)
{
	if (_Vertex)
		return __ndxlGetVertexTime(_Vertex);
	__setLastError(NDXL_ARG_ERR, "ndxlGetVertexTime: null vertex.");
	return (double)(NDXL_ERR);
}

_NDXLEXP_ const int ndxlSetVertexCallback(NDXLVertex* _Vertex, NDXLVertexCallback _Callback)
{
	int _Result = NDXL_ERR;
	if (!_Vertex)
		__setLastError(NDXL_ARG_ERR, "ndxlSetVertexCallback: null vertex.");
	else
	{
		__ndxlSetVertexCallback(_Vertex, _Callback);
		_Result = NDXL_OK;
	}
	return _Result;
}

_NDXLEXP_ const int ndxlSetVertexDynamics(NDXLVertex* _Vertex, NDXLVertexDynamics _Dynamics)
{
	int _Result = NDXL_ERR;
	if (!_Vertex)
		__setLastError(NDXL_ARG_ERR, "ndxlSetVertexDynamics: null vertex.");
	else
	{
		__ndxlSetVertexDynamics(_Vertex, _Dynamics);
		_Result = NDXL_OK;
	}
	return _Result;
}

_NDXLEXP_ const int ndxlSetVertexNoise(NDXLVertex* _Vertex, NDXLVertexNoise _Noise)
{
	int _Result = NDXL_ERR;
	if (!_Vertex)
		__setLastError(NDXL_ARG_ERR, "ndxlSetVertexNoise: null vertex.");
	else
	{
		__ndxlSetVertexNoise(_Vertex, _Noise);
		_Result = NDXL_OK;
	}
	return _Result;
}

_NDXLEXP_ const int ndxlSetVertexParams(NDXLVertex* _Vertex, void* _Params)
{
	int _Result = NDXL_ERR;
	if (!_Vertex)
		__setLastError(NDXL_ARG_ERR, "ndxlSetVertexParams: null vertex.");
	else
	{
		__ndxlSetVertexParams(_Vertex, _Params);
		_Result = NDXL_OK;
	}
	return _Result;
}

_NDXLEXP_ const char ndxlVertexHasNeighbour(NDXLVertex* _Source, NDXLVertex* _Destination)
{
	char _Result = NDXL_ERR;
	if (!_Source)
		__setLastError(NDXL_ARG_ERR, "ndxlVertexHasNeighbour: null source vertex.");
	else if (!_Destination)
		__setLastError(NDXL_ARG_ERR, "ndxlVertexHasNeighbour: null destination vertex.");
	else
		_Result = __ndxlVertexHasNeighbour(_Source, _Destination);
	return _Result;
}

_NDXLEXP_ double* ndxlVertexState(NDXLVertex* _Vertex)
{
	if (_Vertex)
		return __ndxlVertexState(_Vertex);
	__setLastError(NDXL_ARG_ERR, "ndxlVertexState: null vertex.");
	return 0;
}