/**
 * @file ndxl_vertex.c
 *
 * @brief API source for NDXL user-level vertex functions.
 *
 * Pre-checks as far as possible the input and output to all vertex functions.
 *
 * @copyright Copyright (c) 2026, Ethan Codron.
 *
 * Licensed under the GNU General Public License v3.0 only.
 * See the LICENSE file in the repository root.
*/

#include "internal/internal.h"

// Constructor and destructor vvvvv

_NDXLEXP_ NDXLVertex* ndxlCreateVertex(NDXLNetwork* _Network, const double* _State, const size_t _Dimension, NDXLVertexDynamics _Func, NDXLVertexNoise _Noise, NDXLChannel** _Channels, const size_t _Nchannels, NDXLVertexCallback _Callback, const double _Atol, const double _Rtol, void* _Params)
{
	NDXLVertex* vertex = 0;
	if (!_Network)
		__ndxlSetLastError(NDXL_ARG_ERR, "ndxlCreateVertex: null parent network.");
	else if (_State && !_Dimension)
		__ndxlSetLastError(NDXL_VALUE_ERR, "ndxlCreateVertex: state specified with zero dimension.");
	else if (!_Noise && _Channels)
		__ndxlSetLastError(NDXL_VALUE_ERR, "ndxlCreateVertex: channels specified with no noise function.");
	else if (!_Noise && _Nchannels)
		__ndxlSetLastError(NDXL_VALUE_ERR, "ndxlCreateVertex: channel count specified with no noise function.");
	else if (_Channels && !_Nchannels)
		__ndxlSetLastError(NDXL_VALUE_ERR, "ndxlCreateVertex: channels specified with nonpositive channel count.");
	else if (_Atol < 0.0)
		__ndxlSetLastError(NDXL_VALUE_ERR, "ndxlCreateVertex: negative absolute tolerance.");
	else if (_Rtol < 0.0)
		__ndxlSetLastError(NDXL_VALUE_ERR, "ndxlCreateVertex: negative relative tolerance.");
	else
		vertex = __ndxlCreateVertex(_Network, _State, _Dimension ? _Dimension : 1, _Func, _Noise, _Channels, _Nchannels, _Callback, _Atol, _Rtol, _Params);
	return vertex;
}

_NDXLEXP_ const int ndxlDestroyVertex(NDXLVertex* _Vertex)
{
	int _Result = NDXL_ERR;
	if (!_Vertex)
		__ndxlSetLastError(NDXL_ARG_ERR, "ndxlDestroyVertex: null vertex.");
	else
	{
		__ndxlDestroyVertex(_Vertex);
		_Result = NDXL_OK;
	}
	return _Result;
}

// ^^^^^ Constructor and destructor | getters vvvvv

_NDXLEXP_ NDXLVertexCallback ndxlGetVertexCallback(NDXLVertex* _Vertex)
{
	if (_Vertex)
		return __ndxlGetVertexCallback(_Vertex);
	__ndxlSetLastError(NDXL_ARG_ERR, "ndxlGetVertexCallback: null vertex.");
	return 0;
}

_NDXLEXP_ const size_t ndxlGetVertexDegree(NDXLVertex* _Vertex)
{
	if (_Vertex)
		return __ndxlGetVertexDegree(_Vertex);
	__ndxlSetLastError(NDXL_ARG_ERR, "ndxlGetVertexDegree: null vertex.");
	return NDXL_ERR;
}

_NDXLEXP_ const size_t ndxlGetVertexDegreeIn(NDXLVertex* _Vertex)
{
	if (_Vertex)
		return __ndxlGetVertexDegreeIn(_Vertex);
	__ndxlSetLastError(NDXL_ARG_ERR, "ndxlGetVertexDegree: null vertex.");
	return NDXL_ERR;
}

_NDXLEXP_ const size_t ndxlGetVertexDegreeOut(NDXLVertex* _Vertex)
{
	if (_Vertex)
		return __ndxlGetVertexDegreeOut(_Vertex);
	__ndxlSetLastError(NDXL_ARG_ERR, "ndxlGetVertexDegree: null vertex.");
	return NDXL_ERR;
}

_NDXLEXP_ const size_t ndxlGetVertexDimension(NDXLVertex* _Vertex)
{
	if (_Vertex)
		return __ndxlGetVertexDimension(_Vertex);
	__ndxlSetLastError(NDXL_ARG_ERR, "ndxlGetVertexDimension: null vertex.");
	return NDXL_ERR;
}

_NDXLEXP_ NDXLVertexDynamics ndxlGetVertexDynamics(NDXLVertex* _Vertex)
{
	if (_Vertex)
		return __ndxlGetVertexDynamics(_Vertex);
	__ndxlSetLastError(NDXL_ARG_ERR, "ndxlGetVertexDynamics: null vertex.");
	return 0;
}

_NDXLEXP_ const double* ndxlGetVertexInitialState(NDXLVertex* _Vertex)
{
	if (_Vertex)
		return __ndxlGetVertexInitialState(_Vertex);
	__ndxlSetLastError(NDXL_ARG_ERR, "ndxlGetVertexInitialState: null vertex.");
	return 0;
}

_NDXLEXP_ NDXLVertex* ndxlGetVertexNeighbour(NDXLVertex* _Vertex, NDXLEdge* _Edge)
{
	NDXLVertex* vertex = 0;
	if (!_Vertex)
		__ndxlSetLastError(NDXL_ARG_ERR, "ndxlGetVertexNeighbour: null vertex.");
	else if (!_Edge)
		__ndxlSetLastError(NDXL_ARG_ERR, "ndxlGetVertexNeighbour: null edge.");
	else if (!__ndxlEdgeConnectsVertex(_Edge, _Vertex))
		__ndxlSetLastError(NDXL_VALUE_ERR, "ndxlGetVertexNeighbour: edge not connected to vertex.");
	else
		vertex = __ndxlGetVertexNeighbour(_Vertex, _Edge);
	return vertex;
}

_NDXLEXP_ const size_t ndxlGetVertexNeighbourDimension(NDXLVertex* _Vertex, NDXLEdge* _Edge)
{
	size_t _Result = NDXL_ERR;
	if (!_Vertex)
		__ndxlSetLastError(NDXL_ARG_ERR, "ndxlGetVertexNeighbourDimension: null vertex.");
	else if (!_Edge)
		__ndxlSetLastError(NDXL_ARG_ERR, "ndxlGetVertexNeighbourDimension: null edge.");
	else if (!__ndxlEdgeConnectsVertex(_Edge, _Vertex))
		__ndxlSetLastError(NDXL_VALUE_ERR, "ndxlGetVertexNeighbourDimension: edge not connected to vertex.");
	else
		_Result = __ndxlGetVertexNeighbourDimension(_Vertex, _Edge);
	return _Result;
}

_NDXLEXP_ const double* ndxlGetVertexNeighbourState(NDXLVertex* _Vertex, NDXLEdge* _Edge)
{
	const double* state = 0;
	if (!_Vertex)
		__ndxlSetLastError(NDXL_ARG_ERR, "ndxlGetVertexNeighbourState: null vertex.");
	else if (!_Edge)
		__ndxlSetLastError(NDXL_ARG_ERR, "ndxlGetVertexNeighbourState: null edge.");
	else if (!__ndxlEdgeConnectsVertex(_Edge, _Vertex))
		__ndxlSetLastError(NDXL_VALUE_ERR, "ndxlGetVertexNeighbourState: edge not connected to vertex.");
	else
		state = __ndxlGetVertexNeighbourState(_Vertex, _Edge);
	return state;
}

_NDXLEXP_ NDXLNetwork* ndxlGetVertexNetwork(NDXLVertex* _Vertex)
{
	if (_Vertex)
		return __ndxlGetVertexNetwork(_Vertex);
	__ndxlSetLastError(NDXL_ARG_ERR, "ndxlGetVertexNetwork: null vertex.");
	return 0;
}

_NDXLEXP_ NDXLVertexNoise ndxlGetVertexNoise(NDXLVertex* _Vertex)
{
	if (_Vertex)
		return __ndxlGetVertexNoise(_Vertex);
	__ndxlSetLastError(NDXL_ARG_ERR, "ndxlGetVertexNoise: null vertex.");
	return 0;
}

_NDXLEXP_ void* ndxlGetVertexParams(NDXLVertex* _Vertex)
{
	if (_Vertex)
		return __ndxlGetVertexParams(_Vertex);
	__ndxlSetLastError(NDXL_ARG_ERR, "ndxlGetVertexParams: null vertex.");
	return 0;
}

_NDXLEXP_ const double ndxlGetVertexTime(NDXLVertex* _Vertex)
{
	if (_Vertex)
		return __ndxlGetVertexTime(_Vertex);
	__ndxlSetLastError(NDXL_ARG_ERR, "ndxlGetVertexTime: null vertex.");
	return (double)(NDXL_ERR);
}

// ^^^^^ Getters | setters vvvvv

_NDXLEXP_ const int ndxlSetVertexCallback(NDXLVertex* _Vertex, NDXLVertexCallback _Callback)
{
	int _Result = NDXL_ERR;
	if (!_Vertex)
		__ndxlSetLastError(NDXL_ARG_ERR, "ndxlSetVertexCallback: null vertex.");
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
		__ndxlSetLastError(NDXL_ARG_ERR, "ndxlSetVertexDynamics: null vertex.");
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
		__ndxlSetLastError(NDXL_ARG_ERR, "ndxlSetVertexNoise: null vertex.");
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
		__ndxlSetLastError(NDXL_ARG_ERR, "ndxlSetVertexParams: null vertex.");
	else
	{
		__ndxlSetVertexParams(_Vertex, _Params);
		_Result = NDXL_OK;
	}
	return _Result;
}

// ^^^^^ Setters | functions vvvvv

_NDXLEXP_ NDXLChannel* ndxlVertexChannel(NDXLVertex* _Vertex, const size_t _Idx)
{
	NDXLChannel* channel = 0;
	if (!_Vertex)
		__ndxlSetLastError(NDXL_ARG_ERR, "ndxlVertexChannel: null vertex.");
	else if (_Idx < 0)
		__ndxlSetLastError(NDXL_VALUE_ERR, "ndxlVertexChannel: negative array index.");
	else if (_Idx >= _Vertex->nc)
		__ndxlSetLastError(NDXL_VALUE_ERR, "ndxlVertexChannel: array index out of bounds.");
	else
		channel = __ndxlVertexChannel(_Vertex, _Idx);
	return channel;
}

_NDXLEXP_ NDXLEdge* ndxlVertexEdge(NDXLVertex* _Vertex, const size_t _Idx)
{
	NDXLEdge* edge = 0;
	if (!_Vertex)
		__ndxlSetLastError(NDXL_ARG_ERR, "ndxlVertexEdge: null vertex.");
	else if (_Idx < 0)
		__ndxlSetLastError(NDXL_VALUE_ERR, "ndxlVertexEdge: negative array index.");
	else if (_Idx >= _Vertex->ne)
		__ndxlSetLastError(NDXL_VALUE_ERR, "ndxlVertexEdge: array index out of bounds.");
	else
		edge = __ndxlVertexEdge(_Vertex, _Idx);
	return edge;
}

_NDXLEXP_ const char ndxlVertexHasNeighbour(NDXLVertex* _Source, NDXLVertex* _Destination)
{
	char _Result = NDXL_ERR;
	if (!_Source)
		__ndxlSetLastError(NDXL_ARG_ERR, "ndxlVertexHasNeighbour: null source vertex.");
	else if (!_Destination)
		__ndxlSetLastError(NDXL_ARG_ERR, "ndxlVertexHasNeighbour: null destination vertex.");
	else
		_Result = __ndxlVertexHasNeighbour(_Source, _Destination);
	return _Result;
}

_NDXLEXP_ double* ndxlVertexState(NDXLVertex* _Vertex)
{
	if (_Vertex)
		return __ndxlVertexState(_Vertex);
	__ndxlSetLastError(NDXL_ARG_ERR, "ndxlVertexState: null vertex.");
	return 0;
}

// ^^^^^ Functions