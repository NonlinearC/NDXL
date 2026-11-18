//--------------------------------------\\
// NDXL source file for network class	\\
// Copyright Ethan Codron 2026			\\
// GNU GPL-3.0							\\
//--------------------------------------\\
// Source file providing all network	\\
// functions used by the user.			\\
//--------------------------------------\\

#include "internal/internal.h"

// Constructor and destructor vvvvv

_NDXLEXP_ NDXLNetwork* ndxlCreateNetwork(void* _Params)
{
	return __ndxlCreateNetwork(_Params);
}

_NDXLEXP_ const int ndxlDestroyNetwork(NDXLNetwork** _Network)
{
	int _Result = NDXL_ERR;
	if (!_Network)
		__setLastError(NDXL_ARG_ERR, "ndxlDestroyNetwork: null address.");
	else if (!*_Network)
		__setLastError(NDXL_VALUE_ERR, "ndxlDestroyNetwork: null network.");
	else
	{
		__ndxlDestroyNetwork(*_Network);
		*_Network = 0;
		_Result = NDXL_OK;
	}
	return _Result;
}

// ^^^^^ Constructor and destructor | getters vvvvv

_NDXLEXP_ const size_t ndxlGetNetworkChannelCount(NDXLNetwork* _Network)
{
	if (_Network)
		return __ndxlGetNetworkChannelCount(_Network);
	__setLastError(NDXL_ARG_ERR, "ndxlGetNetworkChannelCount: null network.");
	return NDXL_ERR;
}

_NDXLEXP_ const int ndxlGetNetworkEdges(NDXLNetwork* _Network, NDXLEdge** _Edges)
{
	int _Result = NDXL_ERR;
	if (!_Network)
		__setLastError(NDXL_ARG_ERR, "ndxlGetNetworkEdges: null network.");
	else if (!_Edges)
		__setLastError(NDXL_ARG_ERR, "ndxlGetNetworkEdges: null edge output.");
	else
	{
		__ndxlGetNetworkEdges(_Network, _Edges);
		_Result = NDXL_OK;
	}
	return _Result;
}

_NDXLEXP_ const size_t ndxlGetNetworkOrder(NDXLNetwork* _Network)
{
	if (_Network)
		return __ndxlGetNetworkOrder(_Network);
	__setLastError(NDXL_ARG_ERR, "ndxlGetNetworkOrder: null network.");
	return NDXL_ERR;
}

_NDXLEXP_ void* ndxlGetNetworkParams(NDXLNetwork* _Network)
{
	if (_Network)
		return __ndxlGetNetworkParams(_Network);
	__setLastError(NDXL_ARG_ERR, "ndxlGetNetworkParams: null network");
	return 0;
}

_NDXLEXP_ const size_t ndxlGetNetworkSize(NDXLNetwork* _Network)
{
	if (_Network)
		return __ndxlGetNetworkSize(_Network);
	__setLastError(NDXL_ARG_ERR, "ndxlGetNetworkSize: null network.");
	return NDXL_ERR;
}

_NDXLEXP_ const double ndxlGetNetworkTime(NDXLNetwork* _Network)
{
	if (_Network)
		return __ndxlGetNetworkTime(_Network);
	__setLastError(NDXL_ARG_ERR, "ndxlGetNetworkTime: null network.");
	return (double)(NDXL_ERR);
}

_NDXLEXP_ const int ndxlGetNetworkVertices(NDXLNetwork* _Network, NDXLVertex** _Vertices)
{
	int _Result = NDXL_ERR;
	if (!_Network)
		__setLastError(NDXL_ARG_ERR, "ndxlGetNetworkVertices: null network.");
	else if (!_Vertices)
		__setLastError(NDXL_ARG_ERR, "ndxlGetNetworkVertices: null vertex output.");
	else
	{
		__ndxlGetNetworkVertices(_Network, _Vertices);
		_Result = NDXL_OK;
	}
	return _Result;
}

// ^^^^^ Getters | setters vvvvv

_NDXLEXP_ const int ndxlSetNetworkParams(NDXLNetwork* _Network, void* _Params)
{
	int _Result = NDXL_ERR;
	if (!_Network)
		__setLastError(NDXL_ARG_ERR, "ndxlSetNetworkParams: null network.");
	else
	{
		__ndxlSetNetworkParams(_Network, _Params);
		_Result = NDXL_OK;
	}
	return _Result;
}

// ^^^^^ Setters | traversal vvvvv

_NDXLEXP_ NDXLChannel* ndxlGetNetworkFirstChannel(NDXLNetwork* _Network)
{
	if (_Network)
		return __ndxlGetNetworkFirstChannel(_Network);
	__setLastError(NDXL_ARG_ERR, "ndxlGetNetworkFirstChannel: null network.");
	return 0;
}

_NDXLEXP_ NDXLEdge* ndxlGetNetworkFirstEdge(NDXLNetwork* _Network)
{
	if (_Network)
		return __ndxlGetNetworkFirstEdge(_Network);
	__setLastError(NDXL_ARG_ERR, "ndxlGetNetworkFirstEdge: null network.");
	return 0;
}

_NDXLEXP_ NDXLVertex* ndxlGetNetworkFirstVertex(NDXLNetwork* _Network)
{
	if (_Network)
		return __ndxlGetNetworkFirstVertex(_Network);
	__setLastError(NDXL_ARG_ERR, "ndxlGetNetworkFirstVertex: null network.");
	return 0;
}

_NDXLEXP_ NDXLChannel* ndxlGetNetworkLastChannel(NDXLNetwork* _Network)
{
	if (_Network)
		return __ndxlGetNetworkLastChannel(_Network);
	__setLastError(NDXL_ARG_ERR, "ndxlGetNetworkLastChannel: null network.");
	return 0;
}

_NDXLEXP_ NDXLEdge* ndxlGetNetworkLastEdge(NDXLNetwork* _Network)
{
	if (_Network)
		return __ndxlGetNetworkLastEdge(_Network);
	__setLastError(NDXL_ARG_ERR, "ndxlGetNetworkLastEdge: null network.");
	return 0;
}

_NDXLEXP_ NDXLVertex* ndxlGetNetworkLastVertex(NDXLNetwork* _Network)
{
	if (_Network)
		return __ndxlGetNetworkLastVertex(_Network);
	__setLastError(NDXL_ARG_ERR, "ndxlGetNetworkLastVertex: null network.");
	return 0;
}

_NDXLEXP_ NDXLChannel* ndxlGetNetworkNextChannel(NDXLChannel* _Channel)
{
	if (_Channel)
		return __ndxlGetNetworkNextChannel(_Channel);
	__setLastError(NDXL_ARG_ERR, "ndxlGetNetworkNextChannel: null channel.");
	return 0;
}

_NDXLEXP_ NDXLEdge* ndxlGetNetworkNextEdge(NDXLEdge* _Edge)
{
	if (_Edge)
		return __ndxlGetNetworkNextEdge(_Edge);
	__setLastError(NDXL_ARG_ERR, "ndxlGetNetworkNextEdge: null edge.");
	return 0;
}

_NDXLEXP_ NDXLVertex* ndxlGetNetworkNextVertex(NDXLVertex* _Vertex)
{
	if (_Vertex)
		return __ndxlGetNetworkNextVertex(_Vertex);
	__setLastError(NDXL_ARG_ERR, "ndxlGetNetworkNextVertex: null vertex.");
	return 0;
}

_NDXLEXP_ NDXLChannel* ndxlGetNetworkPreviousChannel(NDXLChannel* _Channel)
{
	if (_Channel)
		return __ndxlGetNetworkPreviousChannel(_Channel);
	__setLastError(NDXL_ARG_ERR, "ndxlGetNetworkPreviousChannel: null channel.");
	return 0;
}

_NDXLEXP_ NDXLEdge* ndxlGetNetworkPreviousEdge(NDXLEdge* _Edge)
{
	if (_Edge)
		return __ndxlGetNetworkPreviousEdge(_Edge);
	__setLastError(NDXL_ARG_ERR, "ndxlGetNetworkPreviousEdge: null edge.");
	return 0;
}

_NDXLEXP_ NDXLVertex* ndxlGetNetworkPreviousVertex(NDXLVertex* _Vertex)
{
	if (_Vertex)
		return __ndxlGetNetworkPreviousVertex(_Vertex);
	__setLastError(NDXL_ARG_ERR, "ndxlGetNetworkPreviousVertex: null vertex.");
	return 0;
}

// ^^^^^ Traversal | simulation vvvvv

_NDXLEXP_ const int ndxlNetworkSimulateRalston(NDXLNetwork* _Network, const double _Time, const double _Step, NDXLNetworkCallback _Callback)
{
	int _Result = NDXL_ERR;
	if (!_Network)
		__setLastError(NDXL_ARG_ERR, "ndxlNetworkSimulateRalston: null network.");
	else if (_Time <= 0.0)
		__setLastError(NDXL_VALUE_ERR, "ndxlNetworkSimulateRalston: nonpositive simulation time.");
	else if (_Step <= 0.0)
		__setLastError(NDXL_VALUE_ERR, "ndxlNetworkSimulateRalston: nonpositive step size.");
	else if ((_Network->nsv != 0) || (_Network->nse != 0))
		__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRalston: network has stochastic agents. Either remove stochastic agents or use one of the stochastic methods.");
	else
		_Result = __ndxlNetworkSimulateRalston(_Network, _Time, _Step, _Callback);
	return _Result;
}

_NDXLEXP_ const int ndxlNetworkSimulateNystrom(NDXLNetwork* _Network, const double _Time, const double _Step, NDXLNetworkCallback _Callback)
{
	int _Result = NDXL_ERR;
	if (!_Network)
		__setLastError(NDXL_ARG_ERR, "ndxlNetworkSimulateNystrom: null network.");
	else if (_Time <= 0.0)
		__setLastError(NDXL_VALUE_ERR, "ndxlNetworkSimulateNystrom: nonpositive simulation time.");
	else if (_Step <= 0.0)
		__setLastError(NDXL_VALUE_ERR, "ndxlNetworkSimulateNystrom: nonpositive step size.");
	else if ((_Network->nsv != 0) || (_Network->nse != 0))
		__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateNystrom: network has stochastic agents. Either remove stochastic agents or use one of the stochastic methods.");
	else
		_Result = __ndxlNetworkSimulateNystrom(_Network, _Time, _Step, _Callback);
	return _Result;
}

_NDXLEXP_ const int ndxlNetworkSimulateDopri(NDXLNetwork* _Network, const double _Time, const double _Atol, const double _Rtol, const double* _Tstops, const size_t _Tcount, NDXLNetworkCallback _Callback)
{
	int _Result = NDXL_ERR;
	if (!_Network)
		__setLastError(NDXL_ARG_ERR, "ndxlNetworkSimulateDopri: null network.");
	else if (_Time <= 0.0)
		__setLastError(NDXL_VALUE_ERR, "ndxlNetworkSimulateDopri: nonpositive simulation time.");
	else if (_Atol <= 0.0)
		__setLastError(NDXL_VALUE_ERR, "ndxlNetworkSimulateDopri: nonpositive absolute tolerance.");
	else if (_Rtol <= 0.0)
		__setLastError(NDXL_VALUE_ERR, "ndxlNetworkSimulateDopri: nonpositive relative tolerance.");
	else if ((_Tstops == 0) ^ (_Tcount == 0))
		__setLastError(NDXL_VALUE_ERR, "ndxlNetworkSimulateDopri: time stops specified without count.");
	else if ((_Network->nsv != 0) || (_Network->nse != 0))
		__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateDopri: network has stochastic agents. Either remove stochastic agents or use one of the stochastic methods.");
	else
		_Result = __ndxlNetworkSimulateDopri(_Network, _Time, _Atol, _Rtol, _Tstops, _Tcount, _Callback);
	return _Result;
}

_NDXLEXP_ const int ndxlNetworkSimulateRkck(NDXLNetwork* _Network, const double _Time, const double _Atol, const double _Rtol, const double* _Tstops, const size_t _Tcount, NDXLNetworkCallback _Callback)
{
	int _Result = NDXL_ERR;
	if (!_Network)
		__setLastError(NDXL_ARG_ERR, "ndxlNetworkSimulateRkck: null network.");
	else if (_Time <= 0.0)
		__setLastError(NDXL_VALUE_ERR, "ndxlNetworkSimulateRkck: nonpositive simulation time.");
	else if (_Atol <= 0.0)
		__setLastError(NDXL_VALUE_ERR, "ndxlNetworkSimulateRkck: nonpositive absolute tolerance.");
	else if (_Rtol <= 0.0)
		__setLastError(NDXL_VALUE_ERR, "ndxlNetworkSimulateRkck: nonpositive relative tolerance.");
	else if ((_Tstops == 0) ^ (_Tcount == 0))
		__setLastError(NDXL_VALUE_ERR, "ndxlNetworkSimulateRkck: time stops specified without count.");
	else if ((_Network->nsv != 0) || (_Network->nse != 0))
		__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRkck: network has stochastic agents. Either remove stochastic agents or use one of the stochastic methods.");
	else
		_Result = __ndxlNetworkSimulateRkck(_Network, _Time, _Atol, _Rtol, _Tstops, _Tcount, _Callback);
	return _Result;
}

_NDXLEXP_ const int ndxlNetworkSimulateEuler(NDXLNetwork* _Network, const double _Time, const double _Step, NDXLNetworkCallback _Callback)
{
	int _Result = NDXL_ERR;
	if (!_Network)
		__setLastError(NDXL_ARG_ERR, "ndxlNetworkSimulateEuler: null network.");
	else if (_Time <= 0.0)
		__setLastError(NDXL_VALUE_ERR, "ndxlNetworkSimulateEuler: nonpositive simulation time.");
	else if (_Step <= 0.0)
		__setLastError(NDXL_VALUE_ERR, "ndxlNetworkSimulateEuler: nonpositive step size.");
	else if((_Network->nsv == 0) && (_Network->nse == 0))
		__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateEuler: network has no stochastic agents. Either add stochastic agents or use one of the deterministic methods.");
	else
		_Result = __ndxlNetworkSimulateEuler(_Network, _Time, _Step, _Callback);
	return _Result;
}

_NDXLEXP_ const int ndxlNetworkSimulateHeun(NDXLNetwork* _Network, const double _Time, const double _Atol, const double _Rtol, const double* _Tstops, const size_t _Tcount, NDXLNetworkCallback _Callback)
{
	int _Result = NDXL_ERR;
	if (!_Network)
		__setLastError(NDXL_ARG_ERR, "ndxlNetworkSimulateHeun: null network.");
	else if (_Time <= 0.0)
		__setLastError(NDXL_VALUE_ERR, "ndxlNetworkSimulateHeun: nonpositive simulation time.");
	else if (_Atol <= 0.0)
		__setLastError(NDXL_VALUE_ERR, "ndxlNetworkSimulateHeun: nonpositive absolute tolerance.");
	else if (_Rtol <= 0.0)
		__setLastError(NDXL_VALUE_ERR, "ndxlNetworkSimulateHeun: nonpositive relative tolerance.");
	else if ((_Tstops == 0) ^ (_Tcount == 0))
		__setLastError(NDXL_VALUE_ERR, "ndxlNetworkSimulateHeun: time stops specified without count.");
	else if ((_Network->nsv == 0) && (_Network->nse == 0))
		__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateHeun: network has no stochastic agents. Either add stochastic agents or use one of the deterministic methods.");
	else
		_Result = __ndxlNetworkSimulateHeun(_Network, _Time, _Atol, _Rtol, _Tstops, _Tcount, _Callback);
	return _Result;
}

_NDXLEXP_ const int ndxlNetworkSimulateNorsett(NDXLNetwork* _Network, const double _Time, const double _Step, NDXLNetworkCallback _Callback)
{
	int _Result = NDXL_ERR;
	if (!_Network)
		__setLastError(NDXL_ARG_ERR, "ndxlNetworkSimulateNorsett: null network.");
	else if (_Time <= 0.0)
		__setLastError(NDXL_VALUE_ERR, "ndxlNetworkSimulateNorsett: nonpositive simulation time.");
	else if (_Step <= 0.0)
		__setLastError(NDXL_VALUE_ERR, "ndxlNetworkSimulateNorsett: nonpositive step size.");
	else if ((_Network->nsv != 0) || (_Network->nse != 0))
		__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateNorsett: network has stochastic agents. Either remove stochastic agents or use one of the stochastic methods.");
	else
		_Result = __ndxlNetworkSimulateNorsett(_Network, _Time, _Step, _Callback);
	return _Result;
}

_NDXLEXP_ const int ndxlNetworkSimulateRadau(NDXLNetwork* _Network, const double _Time, const double _Atol, const double _Rtol, const double* _Tstops, const size_t _Tcount, NDXLNetworkCallback _Callback)
{
	int _Result = NDXL_ERR;
	if (!_Network)
		__setLastError(NDXL_ARG_ERR, "ndxlNetworkSimulateRadau: null network.");
	else if (_Time <= 0.0)
		__setLastError(NDXL_VALUE_ERR, "ndxlNetworkSimulateRadau: nonpositive simulation time.");
	else if (_Atol <= 0.0)
		__setLastError(NDXL_VALUE_ERR, "ndxlNetworkSimulateRadau: nonpositive absolute tolerance.");
	else if (_Rtol <= 0.0)
		__setLastError(NDXL_VALUE_ERR, "ndxlNetworkSimulateRadau: nonpositive relative tolerance.");
	else if ((_Tstops == 0) ^ (_Tcount == 0))
		__setLastError(NDXL_VALUE_ERR, "ndxlNetworkSimulateRadau: time stops specified without count.");
	else if ((_Network->nsv != 0) || (_Network->nse != 0))
		__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRadau: network has stochastic agents. Either remove stochastic agents or use one of the stochastic methods.");
	else
		_Result = __ndxlNetworkSimulateRadau(_Network, _Time, _Atol, _Rtol, _Tstops, _Tcount, _Callback);
	return _Result;
}

// ^^^^^ Simulation