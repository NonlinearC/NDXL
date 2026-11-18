//----------------------------------\\
// NDXL source file for edge class	\\
// Copyright Ethan Codron 2026		\\
// GNU GPL-3.0						\\
//----------------------------------\\
// Source file providing all edge	\\
// functions used by the user.		\\
//----------------------------------\\

#include "internal/internal.h"

_NDXLEXP_ NDXLEdge* ndxlCreateEdge(NDXLVertex* _Vert1, NDXLVertex* _Vert2, const double* _State, const size_t _Order, const double _Delay, const char _Directed, NDXLEdgeDynamics _Func, NDXLEdgeDynamics _Noise, NDXLChannel** _Channels, const size_t _Nchannels, NDXLEdgeCallback _Callback, void* _Params)
{
	NDXLEdge* edge = 0;
	if (!_Vert1)
		__setLastError(NDXL_ARG_ERR, "ndxlCreateEdge: null first vertex.");
	else if (!_Vert2)
		__setLastError(NDXL_ARG_ERR, "ndxlCreateEdge: null second vertex.");
	else if (_Vert1->network != _Vert2->network)
		__setLastError(NDXL_VALUE_ERR, "ndxlCreateEdge: mismatched vertex parent networks.");
	else if (_Delay < 0)
		__setLastError(NDXL_VALUE_ERR, "ndxlCreateEdge: negative delay.");
	else if (!_Noise && _Channels)
		__setLastError(NDXL_VALUE_ERR, "ndxlCreateEdge: channels specified with no noise function.");
	else if (!_Noise && _Nchannels)
		__setLastError(NDXL_VALUE_ERR, "ndxlCreateEdge: channel count specified with no noise function.");
	else if (_Channels && !_Nchannels)
		__setLastError(NDXL_VALUE_ERR, "ndxlCreateVertex: channels specified with nonpositive channel count.");
	else
		edge = __ndxlCreateEdge(_Vert1, _Vert2, _State, _Order ? _Order : 1, _Delay, _Directed, _Func, _Noise, _Channels, _Nchannels, _Callback, _Params);
	return edge;
}

_NDXLEXP_ const int ndxlDestroyEdge(NDXLEdge** _Edge)
{
	int _Result = NDXL_ERR;
	if (!_Edge)
		__setLastError(NDXL_ARG_ERR, "ndxlDestroyEdge: null address.");
	else if (!*_Edge)
		__setLastError(NDXL_VALUE_ERR, "ndxlDestroyEdge: null edge.");
	else
	{
		__ndxlDestroyEdge(*_Edge);
		*_Edge = 0;
		_Result = NDXL_OK;
	}
	return _Result;
}

_NDXLEXP_ NDXLEdgeCallback ndxlGetEdgeCallback(NDXLEdge* _Edge)
{
	if (_Edge)
		return __ndxlGetEdgeCallback(_Edge);
	__setLastError(NDXL_ARG_ERR, "ndxlGetEdgeCallback: null edge.");
	return 0;
}

_NDXLEXP_ const size_t ndxlGetEdgeChannelCount(NDXLEdge* _Edge)
{
	if (_Edge)
		return __ndxlGetEdgeChannelCount(_Edge);
	__setLastError(NDXL_ARG_ERR, "ndxlGetEdgeChannelCount: null edge.");
	return NDXL_ERR;
}

_NDXLEXP_ const int ndxlGetEdgeChannels(NDXLEdge* _Edge, NDXLChannel** _Channels)
{
	int _Result = 0;
	if (!_Edge)
		__setLastError(NDXL_ARG_ERR, "ndxlGetEdgeChannels: null edge.");
	else if (!_Channels)
		__setLastError(NDXL_ARG_ERR, "ndxlGetEdgeChannels: null channel output.");
	else
	{
		__ndxlGetEdgeChannels(_Edge, _Channels);
		_Result = NDXL_OK;
	}
	return _Result;
}

_NDXLEXP_ const double ndxlGetEdgeDelay(NDXLEdge* _Edge)
{
	if (_Edge)
		return __ndxlGetEdgeDelay(_Edge);
	__setLastError(NDXL_ARG_ERR, "ndxlGetEdgeDelay: null edge.");
	return (double)(NDXL_ERR);
}

_NDXLEXP_ const char ndxlGetEdgeDirected(NDXLEdge* _Edge)
{
	if (_Edge)
		return __ndxlGetEdgeDirected(_Edge);
	__setLastError(NDXL_ARG_ERR, "ndxlGetEdgeDirected: null edge.");
	return NDXL_ERR;
}

_NDXLEXP_ NDXLEdgeDynamics ndxlGetEdgeDynamics(NDXLEdge* _Edge)
{
	if (_Edge)
		return __ndxlGetEdgeDynamics(_Edge);
	__setLastError(NDXL_ARG_ERR, "ndxlGetEdgeDynamics: null edge.");
	return 0;
}

_NDXLEXP_ NDXLVertex* ndxlGetEdgeFirstVertex(NDXLEdge* _Edge)
{
	if (_Edge)
		return __ndxlGetEdgeFirstVertex(_Edge);
	__setLastError(NDXL_ARG_ERR, "ndxlGetEdgeFirstVertex: null edge.");
	return 0;
}

_NDXLEXP_ const double* ndxlGetEdgeInitialState(NDXLEdge* _Edge)
{
	if (_Edge)
		return __ndxlGetEdgeState0(_Edge);
	__setLastError(NDXL_ARG_ERR, "ndxlGetEdgeInitialState: null edge.");
	return 0;
}

_NDXLEXP_ NDXLNetwork* ndxlGetEdgeNetwork(NDXLEdge* _Edge)
{
	if (_Edge)
		return __ndxlGetEdgeNetwork(_Edge);
	__setLastError(NDXL_ARG_ERR, "ndxlGetEdgeNetwork: null edge.");
	return 0;
}

_NDXLEXP_ NDXLEdgeNoise ndxlGetEdgeNoise(NDXLEdge* _Edge)
{
	if (_Edge)
		return __ndxlGetEdgeNoise(_Edge);
	__setLastError(NDXL_ARG_ERR, "ndxlGetEdgeNoise: null edge.");
	return 0;
}

_NDXLEXP_ const size_t ndxlGetEdgeOrder(NDXLEdge* _Edge)
{
	if (_Edge)
		return __ndxlGetEdgeOrder(_Edge);
	__setLastError(NDXL_ARG_ERR, "ndxlGetEdgeOrder: null edge.");
	return NDXL_ERR;
}

_NDXLEXP_ void* ndxlGetEdgeParams(NDXLEdge* _Edge)
{
	if (_Edge)
		return __ndxlGetEdgeParams(_Edge);
	__setLastError(NDXL_ARG_ERR, "ndxlGetEdgeParams: null edge.");
	return 0;
}

_NDXLEXP_ const double* ndxlGetEdgeRelativeState(NDXLEdge* _Edge, NDXLVertex* _Vertex)
{
	const double* _Result = 0;
	if (!_Edge)
		__setLastError(NDXL_ARG_ERR, "ndxlGetEdgeRelativeState: null edge.");
	else if (!_Vertex)
		__setLastError(NDXL_ARG_ERR, "ndxlGetEdgeRelativeState: null vertex.");
	else if (!__ndxlEdgeConnectsVertex(_Edge, _Vertex))
		__setLastError(NDXL_VALUE_ERR, "ndxlGetEdgeRelativeState: edge not connected to vertex.");
	else
		_Result = __ndxlGetEdgeRelativeState(_Edge, _Vertex);
	return _Result;
}

_NDXLEXP_ NDXLVertex* ndxlGetEdgeSecondVertex(NDXLEdge* _Edge)
{
	if (_Edge)
		return __ndxlGetEdgeSecondVertex(_Edge);
	__setLastError(NDXL_ARG_ERR, "ndxlGetEdgeSecondVertex: null edge.");
	return 0;
}

_NDXLEXP_ const double ndxlGetEdgeTime(NDXLEdge* _Edge)
{
	if (_Edge)
		return __ndxlGetEdgeTime(_Edge);
	__setLastError(NDXL_ARG_ERR, "ndxlGetEdgeTime: null edge.");
	return (double)(NDXL_ERR);
}

_NDXLEXP_ const int ndxlSetEdgeCallback(NDXLEdge* _Edge, NDXLEdgeCallback _Callback)
{
	int _Result = NDXL_ERR;
	if (!_Edge)
		__setLastError(NDXL_ARG_ERR, "ndxlSetEdgeCallback: null edge.");
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
		__setLastError(NDXL_ARG_ERR, "ndxlSetEdgeDynamics: null edge.");
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
		__setLastError(NDXL_ARG_ERR, "ndxlSetEdgeNoise: null edge.");
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
		__setLastError(NDXL_ARG_ERR, "ndxlSetEdgeParams: null edge.");
	else
	{
		__ndxlSetEdgeParams(_Edge, _Params);
		_Result = NDXL_OK;
	}
	return _Result;
}

_NDXLEXP_ const char ndxlEdgeConnectsVertex(NDXLEdge* _Edge, NDXLVertex* _Vertex)
{
	char _Result = NDXL_ERR;
	if (!_Edge)
		__setLastError(NDXL_ARG_ERR, "ndxlEdgeConnectsVertex: null edge.");
	else if (!_Vertex)
		__setLastError(NDXL_ARG_ERR, "ndxlEdgeConnectsVertex: null vertex.");
	else
		_Result = __ndxlEdgeConnectsVertex(_Edge, _Vertex);
	return _Result;
}

_NDXLEXP_ double* _CDECL ndxlEdgeState(NDXLEdge* _Edge)
{
	if (_Edge)
		return __ndxlEdgeState(_Edge);
	__setLastError(NDXL_ARG_ERR, "ndxlEdgeState: null edge.");
	return 0;
}