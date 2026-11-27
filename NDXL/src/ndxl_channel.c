/**
 * @file ndxl_channel.c
 *
 * @brief API source for NDXL user-level channel functions.
 *
 * Pre-checks as far as possible the input and output to all channel functions.
 *
 * @copyright Copyright (c) 2026, Ethan Codron.
 *
 * Licensed under the GNU General Public License v3.0 only.
 * See the LICENSE file in the repository root.
*/

#include "internal/internal.h"

// Constructor and destructor vvvvv

_NDXLEXP_ NDXLChannel* ndxlCreateChannel(NDXLNetwork* _Network, void* _Params)
{
	NDXLChannel* channel = 0;
	if (!_Network)
		__ndxlSetLastError(NDXL_ARG_ERR, "ndxlCreateChannel: null parent network.");
	else
		channel = __ndxlCreateChannel(_Network, _Params);
	return channel;
}

_NDXLEXP_ const int ndxlDestroyChannel(NDXLChannel* _Channel)
{
	int _Result = NDXL_ERR;
	if (!_Channel)
		__ndxlSetLastError(NDXL_ARG_ERR, "ndxlDestroyChannel: null channel.");
	else
	{
		__ndxlDestroyChannel(_Channel);
		_Result = NDXL_OK;
	}
	return _Result;
}

// ^^^^^ Constructor and destructor | getters vvvvv

_NDXLEXP_ const size_t ndxlGetChannelEdgeDimension(NDXLChannel* _Channel)
{
	if (_Channel)
		return __ndxlGetChannelEdgeDimension(_Channel);
	__ndxlSetLastError(NDXL_ARG_ERR, "ndxlGetChannelEdgeDimension: null channel.");
	return NDXL_ERR;
}

_NDXLEXP_ NDXLNetwork* ndxlGetChannelNetwork(NDXLChannel* _Channel)
{
	if (_Channel)
		return __ndxlGetChannelNetwork(_Channel);
	__ndxlSetLastError(NDXL_ARG_ERR, "ndxlGetChannelNetwork: null channel.");
	return 0;
}

_NDXLEXP_ void* ndxlGetChannelParams(NDXLChannel* _Channel)
{
	if (_Channel)
		return __ndxlGetChannelParams(_Channel);
	__ndxlSetLastError(NDXL_ARG_ERR, "ndxlGetChannelParams: null channel.");
	return 0;
}

_NDXLEXP_ const size_t ndxlGetChannelVertexDimension(NDXLChannel* _Channel)
{
	if (_Channel)
		return __ndxlGetChannelVertexDimension(_Channel);
	__ndxlSetLastError(NDXL_ARG_ERR, "ndxlGetChannelVertexDimension: null channel.");
	return NDXL_ERR;
}

// ^^^^^ Getters | setters vvvvv

_NDXLEXP_ const int ndxlSetChannelParams(NDXLChannel* _Channel, void* _Params)
{
	int _Result = NDXL_ERR;
	if (!_Channel)
		__ndxlSetLastError(NDXL_ARG_ERR, "ndxlSetChannelParams: null channel.");
	else
	{
		__ndxlSetChannelParams(_Channel, _Params);
		_Result = NDXL_OK;
	}
	return _Result;
}

// ^^^^^ Setters | functions vvvvv

_NDXLEXP_ NDXLEdge* ndxlChannelEdge(NDXLChannel* _Channel, const size_t _Idx)
{
	NDXLEdge* edge = 0;
	if (!_Channel)
		__ndxlSetLastError(NDXL_ARG_ERR, "ndxlChannelEdge: null channel.");
	else if (_Idx < 0)
		__ndxlSetLastError(NDXL_VALUE_ERR, "ndxlChannelEdge: negative array index.");
	else if (_Idx >= _Channel->ne)
		__ndxlSetLastError(NDXL_VALUE_ERR, "ndxlChannelEdge: array index out of bounds.");
	else
		edge = __ndxlChannelEdge(_Channel, _Idx);
	return edge;
}

_NDXLEXP_ NDXLVertex* ndxlChannelVertex(NDXLChannel* _Channel, const size_t _Idx)
{
	NDXLVertex* vertex = 0;
	if (!_Channel)
		__ndxlSetLastError(NDXL_ARG_ERR, "ndxlChannelVertex: null channel.");
	else if (_Idx < 0)
		__ndxlSetLastError(NDXL_VALUE_ERR, "ndxlChannelVertex: negative array index.");
	else if (_Idx >= _Channel->nv)
		__ndxlSetLastError(NDXL_VALUE_ERR, "ndxlChannelVertex: array index out of bounds.");
	else
		vertex = __ndxlChannelVertex(_Channel, _Idx);
	return vertex;
}

// ^^^^^ Functions