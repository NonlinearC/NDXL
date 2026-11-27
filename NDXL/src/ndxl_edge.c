/**
 * @file ndxl_edge.c
 *
 * @brief API source for NDXL user-level edge functions.
 *
 * Pre-checks as far as possible the input and output to all edge functions.
 *
 * @copyright Copyright (c) 2026, Ethan Codron.
 *
 * Licensed under the GNU General Public License v3.0 only.
 * See the LICENSE file in the repository root.
*/

#include "internal/internal.h"

// Constructor and destructor vvvvv

_NDXLEXP_ NDXLEdge* ndxlCreateEdge(NDXLVertex* _Source, NDXLVertex* _Destination, const double* _State, const size_t _Dimension, const double _Delay, const char _Directed, NDXLEdgeDynamics _Func, NDXLEdgeDynamics _Noise, NDXLChannel** _Channels, const size_t _Nchannels, NDXLEdgeCallback _Callback, const double _Atol, const double _Rtol, void* _Params)
{
	NDXLEdge* edge = 0;
	if (!_Source)
		__ndxlSetLastError(NDXL_ARG_ERR, "ndxlCreateEdge: null source vertex.");
	else if (!_Destination)
		__ndxlSetLastError(NDXL_ARG_ERR, "ndxlCreateEdge: null destination vertex.");
	else if (_Source->network != _Destination->network)
		__ndxlSetLastError(NDXL_VALUE_ERR, "ndxlCreateEdge: mismatched vertex parent networks.");
	else if (_State && !_Dimension)
		__ndxlSetLastError(NDXL_VALUE_ERR, "ndxlCreateEdge: state specified with zero dimension.");
	else if (_Delay < 0)
		__ndxlSetLastError(NDXL_VALUE_ERR, "ndxlCreateEdge: negative delay.");
	else if (!_Noise && _Channels)
		__ndxlSetLastError(NDXL_VALUE_ERR, "ndxlCreateEdge: channels specified with no noise function.");
	else if (!_Noise && _Nchannels)
		__ndxlSetLastError(NDXL_VALUE_ERR, "ndxlCreateEdge: channel count specified with no noise function.");
	else if (_Channels && !_Nchannels)
		__ndxlSetLastError(NDXL_VALUE_ERR, "ndxlCreateVertex: channels specified with nonpositive channel count.");
	else if (_Atol < 0.0)
		__ndxlSetLastError(NDXL_VALUE_ERR, "ndxlCreateEdge: negative absolute tolerance.");
	else if (_Rtol < 0.0)
		__ndxlSetLastError(NDXL_VALUE_ERR, "ndxlCreateEdge: negative relative tolerance.");
	else
		edge = __ndxlCreateEdge(_Source, _Destination, _State, _Dimension ? _Dimension : 1, _Delay, _Directed, _Func, _Noise, _Channels, _Nchannels, _Callback, _Atol, _Rtol, _Params);
	return edge;
}

_NDXLEXP_ const int ndxlDestroyEdge(NDXLEdge* _Edge)
{
	int _Result = NDXL_ERR;
	if (!_Edge)
		__ndxlSetLastError(NDXL_ARG_ERR, "ndxlDestroyEdge: null edge.");
	else
	{
		__ndxlDestroyEdge(_Edge);
		_Result = NDXL_OK;
	}
	return _Result;
}

// ^^^^^ Constructor and destructor | getters vvvvv

_NDXLEXP_ NDXLEdgeCallback ndxlGetEdgeCallback(NDXLEdge* _Edge)
{
	if (_Edge)
		return __ndxlGetEdgeCallback(_Edge);
	__ndxlSetLastError(NDXL_ARG_ERR, "ndxlGetEdgeCallback: null edge.");
	return 0;
}

_NDXLEXP_ const double ndxlGetEdgeDelay(NDXLEdge* _Edge)
{
	if (_Edge)
		return __ndxlGetEdgeDelay(_Edge);
	__ndxlSetLastError(NDXL_ARG_ERR, "ndxlGetEdgeDelay: null edge.");
	return (double)(NDXL_ERR);
}

_NDXLEXP_ NDXLVertex* ndxlGetEdgeDestination(NDXLEdge* _Edge)
{
	if (_Edge)
		return __ndxlGetEdgeDestination(_Edge);
	__ndxlSetLastError(NDXL_ARG_ERR, "ndxlGetEdgeDestination: null edge.");
	return 0;
}

_NDXLEXP_ const size_t ndxlGetEdgeDimension(NDXLEdge* _Edge)
{
	if (_Edge)
		return __ndxlGetEdgeDimension(_Edge);
	__ndxlSetLastError(NDXL_ARG_ERR, "ndxlGetEdgeDimension: null edge.");
	return NDXL_ERR;
}

_NDXLEXP_ const char ndxlGetEdgeDirected(NDXLEdge* _Edge)
{
	if (_Edge)
		return __ndxlGetEdgeDirected(_Edge);
	__ndxlSetLastError(NDXL_ARG_ERR, "ndxlGetEdgeDirected: null edge.");
	return NDXL_ERR;
}

_NDXLEXP_ NDXLEdgeDynamics ndxlGetEdgeDynamics(NDXLEdge* _Edge)
{
	if (_Edge)
		return __ndxlGetEdgeDynamics(_Edge);
	__ndxlSetLastError(NDXL_ARG_ERR, "ndxlGetEdgeDynamics: null edge.");
	return 0;
}

_NDXLEXP_ const double* ndxlGetEdgeInitialState(NDXLEdge* _Edge)
{
	if (_Edge)
		return __ndxlGetEdgeInitialState(_Edge);
	__ndxlSetLastError(NDXL_ARG_ERR, "ndxlGetEdgeInitialState: null edge.");
	return 0;
}

_NDXLEXP_ NDXLNetwork* ndxlGetEdgeNetwork(NDXLEdge* _Edge)
{
	if (_Edge)
		return __ndxlGetEdgeNetwork(_Edge);
	__ndxlSetLastError(NDXL_ARG_ERR, "ndxlGetEdgeNetwork: null edge.");
	return 0;
}

_NDXLEXP_ NDXLEdgeNoise ndxlGetEdgeNoise(NDXLEdge* _Edge)
{
	if (_Edge)
		return __ndxlGetEdgeNoise(_Edge);
	__ndxlSetLastError(NDXL_ARG_ERR, "ndxlGetEdgeNoise: null edge.");
	return 0;
}

_NDXLEXP_ void* ndxlGetEdgeParams(NDXLEdge* _Edge)
{
	if (_Edge)
		return __ndxlGetEdgeParams(_Edge);
	__ndxlSetLastError(NDXL_ARG_ERR, "ndxlGetEdgeParams: null edge.");
	return 0;
}

_NDXLEXP_ const double* ndxlGetEdgeRelativeState(NDXLEdge* _Edge, NDXLVertex* _Vertex)
{
	const double* _Result = 0;
	if (!_Edge)
		__ndxlSetLastError(NDXL_ARG_ERR, "ndxlGetEdgeRelativeState: null edge.");
	else if (!_Vertex)
		__ndxlSetLastError(NDXL_ARG_ERR, "ndxlGetEdgeRelativeState: null vertex.");
	else if (!__ndxlEdgeConnectsVertex(_Edge, _Vertex))
		__ndxlSetLastError(NDXL_VALUE_ERR, "ndxlGetEdgeRelativeState: edge not connected to vertex.");
	else
		_Result = __ndxlGetEdgeRelativeState(_Edge, _Vertex);
	return _Result;
}

_NDXLEXP_ NDXLVertex* ndxlGetEdgeSource(NDXLEdge* _Edge)
{
	if (_Edge)
		return __ndxlGetEdgeSource(_Edge);
	__ndxlSetLastError(NDXL_ARG_ERR, "ndxlGetEdgeSource: null edge.");
	return 0;
}

_NDXLEXP_ const double ndxlGetEdgeTime(NDXLEdge* _Edge)
{
	if (_Edge)
		return __ndxlGetEdgeTime(_Edge);
	__ndxlSetLastError(NDXL_ARG_ERR, "ndxlGetEdgeTime: null edge.");
	return (double)(NDXL_ERR);
}

// ^^^^^ Getters | setters vvvvv

_NDXLEXP_ const int ndxlSetEdgeCallback(NDXLEdge* _Edge, NDXLEdgeCallback _Callback)
{
	int _Result = NDXL_ERR;
	if (!_Edge)
		__ndxlSetLastError(NDXL_ARG_ERR, "ndxlSetEdgeCallback: null edge.");
	else
	{
		__ndxlSetEdgeCallback(_Edge, _Callback);
		_Result = NDXL_OK;
	}
	return _Result;
}

_NDXLEXP_ const int ndxlSetEdgeDynamics(NDXLEdge* _Edge, NDXLEdgeDynamics _Func)
{
	int _Result = NDXL_ERR;
	if (!_Edge)
		__ndxlSetLastError(NDXL_ARG_ERR, "ndxlSetEdgeDynamics: null edge.");
	else
	{
		__ndxlSetEdgeDynamics(_Edge, _Func);
		_Result = NDXL_OK;
	}
	return _Result;
}

_NDXLEXP_ const int ndxlSetEdgeNoise(NDXLEdge* _Edge, NDXLEdgeNoise _Noise)
{
	int _Result = NDXL_ERR;
	if (!_Edge)
		__ndxlSetLastError(NDXL_ARG_ERR, "ndxlSetEdgeNoise: null edge.");
	else
	{
		__ndxlSetEdgeNoise(_Edge, _Noise);
		_Result = NDXL_OK;
	}
	return _Result;
}

_NDXLEXP_ const int ndxlSetEdgeParams(NDXLEdge* _Edge, void* _Params)
{
	int _Result = NDXL_ERR;
	if (!_Edge)
		__ndxlSetLastError(NDXL_ARG_ERR, "ndxlSetEdgeParams: null edge.");
	else
	{
		__ndxlSetEdgeParams(_Edge, _Params);
		_Result = NDXL_OK;
	}
	return _Result;
}

// ^^^^^ Setters | functions vvvvv

_NDXLEXP_ NDXLChannel* ndxlEdgeChannel(NDXLEdge* _Edge, const size_t _Idx)
{
	NDXLChannel* channel = 0;
	if (!_Edge)
		__ndxlSetLastError(NDXL_ARG_ERR, "ndxlEdgeChannel: null edge.");
	else if (_Idx < 0)
		__ndxlSetLastError(NDXL_VALUE_ERR, "ndxlEdgeChannel: negative array index.");
	else if (_Idx >= _Edge->nc)
		__ndxlSetLastError(NDXL_VALUE_ERR, "ndxlEdgeChannel: array index out of bounds.");
	else
		channel = __ndxlEdgeChannel(_Edge, _Idx);
	return channel;
}

_NDXLEXP_ const char ndxlEdgeConnectsVertex(NDXLEdge* _Edge, NDXLVertex* _Vertex)
{
	char _Result = NDXL_ERR;
	if (!_Edge)
		__ndxlSetLastError(NDXL_ARG_ERR, "ndxlEdgeConnectsVertex: null edge.");
	else if (!_Vertex)
		__ndxlSetLastError(NDXL_ARG_ERR, "ndxlEdgeConnectsVertex: null vertex.");
	else
		_Result = __ndxlEdgeConnectsVertex(_Edge, _Vertex);
	return _Result;
}

_NDXLEXP_ double* _CDECL ndxlEdgeState(NDXLEdge* _Edge)
{
	if (_Edge)
		return __ndxlEdgeState(_Edge);
	__ndxlSetLastError(NDXL_ARG_ERR, "ndxlEdgeState: null edge.");
	return 0;
}

// ^^^^^ Functions