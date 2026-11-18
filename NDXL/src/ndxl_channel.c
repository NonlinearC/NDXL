//------------------------------------------\\
// NDXL source file for noise channel class	\\
// Copyright Ethan Codron 2026				\\
// GNU GPL-3.0								\\
//------------------------------------------\\
// Source file providing all noise channel	\\
// functions used by the user.				\\
//------------------------------------------\\

#include "internal/internal.h"

// Constructor and destructor vvvvv

_NDXLEXP_ NDXLChannel* ndxlCreateChannel(NDXLNetwork* _Network, void* _Params)
{
	NDXLChannel* channel = 0;
	if (!_Network)
		__setLastError(NDXL_ARG_ERR, "ndxlCreateChannel: null parent network.");
	else
		channel = __ndxlCreateChannel(_Network, _Params);
	return channel;
}

_NDXLEXP_ const int ndxlDestroyChannel(NDXLChannel** _Channel)
{
	int _Result = NDXL_ERR;
	if (!_Channel)
		__setLastError(NDXL_ARG_ERR, "ndxlDestroyChannel: null adress.");
	else if (!*_Channel)
		__setLastError(NDXL_VALUE_ERR, "ndxlDestroyChannel: null channel.");
	else
	{
		__ndxlDestroyChannel(*_Channel);
		*_Channel = 0;
		_Result = NDXL_OK;
	}
	return _Result;
}

// ^^^^^ Constructor and destructor | getters vvvvv

_NDXLEXP_ const size_t ndxlGetChannelEdgeCount(NDXLChannel* _Channel)
{
	if (_Channel)
		return __ndxlGetChannelEdgeCount(_Channel);
	__setLastError(NDXL_ARG_ERR, "ndxlGetChannelEdgeCount: null channel.");
	return NDXL_ERR;
}

_NDXLEXP_ const int ndxlGetChannelEdges(NDXLChannel* _Channel, NDXLEdge** _Edges)
{
	int _Result = NDXL_ERR;
	if (!_Channel)
		__setLastError(NDXL_ARG_ERR, "ndxlGetChannelEdges: null channel.");
	else if (!_Edges)
		__setLastError(NDXL_ARG_ERR, "ndxlGetChannelEdges: null edge output.");
	else
	{
		__ndxlGetChannelEdges(_Channel, _Edges);
		_Result = NDXL_OK;
	}
	return _Result;
}

_NDXLEXP_ NDXLNetwork* ndxlGetChannelNetwork(NDXLChannel* _Channel)
{
	if (_Channel)
		return __ndxlGetChannelNetwork(_Channel);
	__setLastError(NDXL_ARG_ERR, "ndxlGetChannelNetwork: null channel.");
	return 0;
}

_NDXLEXP_ void* ndxlGetChannelParams(NDXLChannel* _Channel)
{
	if (_Channel)
		return __ndxlGetChannelParams(_Channel);
	__setLastError(NDXL_ARG_ERR, "ndxlGetChannelParams: null channel.");
	return 0;
}

_NDXLEXP_ const size_t ndxlGetChannelVertexCount(NDXLChannel* _Channel)
{
	if (_Channel)
		return __ndxlGetChannelVertexCount(_Channel);
	__setLastError(NDXL_ARG_ERR, "ndxlGetChannelVertexCount: null channel.");
	return NDXL_ERR;
}

_NDXLEXP_ const int ndxlGetChannelVertices(NDXLChannel* _Channel, NDXLVertex** _Vertices)
{
	int _Result = NDXL_ERR;
	if (!_Channel)
		__setLastError(NDXL_ARG_ERR, "ndxlGetChannelVertices: null channel.");
	else if (!_Vertices)
		__setLastError(NDXL_ARG_ERR, "ndxlGetChannelVertices: null vertex output.");
	else
	{
		__ndxlGetChannelVertices(_Channel, _Vertices);
		_Result = NDXL_OK;
	}
	return _Result;
}

// ^^^^^ Getters | setters vvvvv

_NDXLEXP_ const int ndxlSetChannelParams(NDXLChannel* _Channel, void* _Params)
{
	int _Result = NDXL_ERR;
	if (!_Channel)
		__setLastError(NDXL_ARG_ERR, "ndxlSetChannelParams: null channel.");
	else
	{
		__ndxlSetChannelParams(_Channel, _Params);
		_Result = NDXL_OK;
	}
	return _Result;
}

// ^^^^^ Setters