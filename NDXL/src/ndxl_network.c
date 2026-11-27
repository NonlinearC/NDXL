/**
 * @file ndxl_network.c
 *
 * @brief API source for NDXL user-level network functions.
 *
 * Pre-checks as far as possible the input and output to all network functions.
 *
 * @copyright Copyright (c) 2026, Ethan Codron.
 *
 * Licensed under the GNU General Public License v3.0 only.
 * See the LICENSE file in the repository root.
*/

#include "internal/internal.h"

// Constructor and destructor vvvvv

_NDXLEXP_ NDXLNetwork* ndxlCreateNetwork(void* _Params)
{
	return __ndxlCreateNetwork(_Params);
}

_NDXLEXP_ const int ndxlDestroyNetwork(NDXLNetwork* _Network)
{
	int _Result = NDXL_ERR;
	if (!_Network)
		__ndxlSetLastError(NDXL_ARG_ERR, "ndxlDestroyNetwork: null network.");
	else
	{
		__ndxlDestroyNetwork(_Network);
		_Result = NDXL_OK;
	}
	return _Result;
}

// ^^^^^ Constructor and destructor | getters vvvvv

_NDXLEXP_ const size_t ndxlGetNetworkNoiseDimension(NDXLNetwork* _Network)
{
	if (_Network)
		return __ndxlGetNetworkNoiseDimension(_Network);
	__ndxlSetLastError(NDXL_ARG_ERR, "ndxlGetNetworkNoiseDimension: null network.");
	return NDXL_ERR;
}

_NDXLEXP_ const size_t ndxlGetNetworkOrder(NDXLNetwork* _Network)
{
	if (_Network)
		return __ndxlGetNetworkOrder(_Network);
	__ndxlSetLastError(NDXL_ARG_ERR, "ndxlGetNetworkOrder: null network.");
	return NDXL_ERR;
}

_NDXLEXP_ void* ndxlGetNetworkParams(NDXLNetwork* _Network)
{
	if (_Network)
		return __ndxlGetNetworkParams(_Network);
	__ndxlSetLastError(NDXL_ARG_ERR, "ndxlGetNetworkParams: null network");
	return 0;
}

_NDXLEXP_ const size_t ndxlGetNetworkSize(NDXLNetwork* _Network)
{
	if (_Network)
		return __ndxlGetNetworkSize(_Network);
	__ndxlSetLastError(NDXL_ARG_ERR, "ndxlGetNetworkSize: null network.");
	return NDXL_ERR;
}

_NDXLEXP_ const double ndxlGetNetworkTime(NDXLNetwork* _Network)
{
	if (_Network)
		return __ndxlGetNetworkTime(_Network);
	__ndxlSetLastError(NDXL_ARG_ERR, "ndxlGetNetworkTime: null network.");
	return (double)(NDXL_ERR);
}

// ^^^^^ Getters | setters vvvvv

_NDXLEXP_ const int ndxlSetNetworkParams(NDXLNetwork* _Network, void* _Params)
{
	int _Result = NDXL_ERR;
	if (!_Network)
		__ndxlSetLastError(NDXL_ARG_ERR, "ndxlSetNetworkParams: null network.");
	else
	{
		__ndxlSetNetworkParams(_Network, _Params);
		_Result = NDXL_OK;
	}
	return _Result;
}

// ^^^^^ Setters | functions vvvvv

_NDXLEXP_ NDXLChannel* ndxlNetworkChannel(NDXLNetwork* _Network, const size_t _Idx)
{
	NDXLChannel* channel = 0;
	if (!_Network)
		__ndxlSetLastError(NDXL_ARG_ERR, "ndxlNetworkChannel: null network.");
	else if (_Idx < 0)
		__ndxlSetLastError(NDXL_VALUE_ERR, "ndxlNetworkChannel: negative array index.");
	else if (_Idx >= _Network->nc)
		__ndxlSetLastError(NDXL_VALUE_ERR, "ndxlNetworkChannel: array index out of bounds.");
	else
		channel = __ndxlNetworkChannel(_Network, _Idx);
	return channel;
}

_NDXLEXP_ NDXLChannel* ndxlNetworkChannelByIndex(NDXLNetwork* _Network, const size_t _Idx)
{
	if (_Network)
		return __ndxlNetworkChannelByIndex(_Network, _Idx);
	__ndxlSetLastError(NDXL_ARG_ERR, "ndxlNetworkChannelByIndex: null network.");
	return 0;
}

_NDXLEXP_ NDXLEdge* ndxlNetworkEdge(NDXLNetwork* _Network, const size_t _Idx)
{
	NDXLEdge* edge = 0;
	if (!_Network)
		__ndxlSetLastError(NDXL_ARG_ERR, "ndxlNetworkEdge: null network.");
	else if (_Idx < 0)
		__ndxlSetLastError(NDXL_VALUE_ERR, "ndxlNetworkEdge: negative array index.");
	else if (_Idx >= _Network->ne)
		__ndxlSetLastError(NDXL_VALUE_ERR, "ndxlNetworkEdge: array index out of bounds.");
	else
		edge = __ndxlNetworkEdge(_Network, _Idx);
	return edge;
}

_NDXLEXP_ NDXLEdge* ndxlNetworkEdgeByIndex(NDXLNetwork* _Network, const size_t _Idx)
{
	if (_Network)
		return __ndxlNetworkEdgeByIndex(_Network, _Idx);
	__ndxlSetLastError(NDXL_ARG_ERR, "ndxlNetworkEdgeByIndex: null network.");
	return 0;
}

_NDXLEXP_ NDXLVertex* ndxlNetworkVertex(NDXLNetwork* _Network, const size_t _Idx)
{
	NDXLVertex* vertex = 0;
	if (!_Network)
		__ndxlSetLastError(NDXL_ARG_ERR, "ndxlNetworkVertex: null network.");
	else if (_Idx < 0)
		__ndxlSetLastError(NDXL_VALUE_ERR, "ndxlNetworkVertex: negative array index.");
	else if (_Idx >= _Network->nv)
		__ndxlSetLastError(NDXL_VALUE_ERR, "ndxlNetworkVertex: array index out of bounds.");
	else
		vertex = __ndxlNetworkVertex(_Network, _Idx);
	return vertex;
}

_NDXLEXP_ NDXLVertex* ndxlNetworkVertexByIndex(NDXLNetwork* _Network, const size_t _Idx)
{
	if (_Network)
		return __ndxlNetworkVertexByIndex(_Network, _Idx);
	__ndxlSetLastError(NDXL_ARG_ERR, "ndxlNetworkVertexByIndex: null network.");
	return 0;
}

// ^^^^^ Functions | simulation vvvvv

_NDXLEXP_ const int ndxlNetworkSimulateRalston(NDXLNetwork* _Network, const double _Time, const double _Step, NDXLNetworkCallback _Callback)
{
	int _Result = NDXL_ERR;
	if (!_Network)
		__ndxlSetLastError(NDXL_ARG_ERR, "ndxlNetworkSimulateRalston: null network.");
	else if (_Time <= 0.0)
		__ndxlSetLastError(NDXL_VALUE_ERR, "ndxlNetworkSimulateRalston: negative simulation time.");
	else if (_Step < 0.0)
		__ndxlSetLastError(NDXL_VALUE_ERR, "ndxlNetworkSimulateRalston: nonpositive absolute tolerance.");
	else if (_Network->nsv || _Network->nse)
		__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRalston: network has stochastic agents. Either remove stochastic agents or use one of the stochastic methods.");
	else
		_Result = __ndxlNetworkSimulateRalston(_Network, _Time, _Step, _Callback);
	return _Result;
}

_NDXLEXP_ const int ndxlNetworkSimulateNystrom(NDXLNetwork* _Network, const double _Time, const double _Step, NDXLNetworkCallback _Callback)
{
	int _Result = NDXL_ERR;
	if (!_Network)
		__ndxlSetLastError(NDXL_ARG_ERR, "ndxlNetworkSimulateNystrom: null network.");
	else if (_Time <= 0.0)
		__ndxlSetLastError(NDXL_VALUE_ERR, "ndxlNetworkSimulateNystrom: negative simulation time.");
	else if (_Step < 0.0)
		__ndxlSetLastError(NDXL_VALUE_ERR, "ndxlNetworkSimulateNystrom: nonpositive absolute tolerance.");
	else if (_Network->nsv || _Network->nse)
		__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateNystrom: network has stochastic agents. Either remove stochastic agents or use one of the stochastic methods.");
	else
		_Result = __ndxlNetworkSimulateNystrom(_Network, _Time, _Step, _Callback);
	return _Result;
}

_NDXLEXP_ const int ndxlNetworkSimulateDopri(NDXLNetwork* _Network, const double _Time, const double _Atol, const double _Rtol, const double* _Tstops, const size_t _Tcount, NDXLNetworkCallback _Callback)
{
	int _Result = NDXL_ERR;
	if (!_Network)
		__ndxlSetLastError(NDXL_ARG_ERR, "ndxlNetworkSimulateDopri: null network.");
	else if (_Time <= 0.0)
		__ndxlSetLastError(NDXL_VALUE_ERR, "ndxlNetworkSimulateDopri: negative simulation time.");
	else if (_Atol <= 0.0)
		__ndxlSetLastError(NDXL_VALUE_ERR, "ndxlNetworkSimulateDopri: nonpositive absolute tolerance.");
	else if (_Rtol <= 0.0)
		__ndxlSetLastError(NDXL_VALUE_ERR, "ndxlNetworkSimulateDopri: nonpositive relative tolerance.");
	else if ((_Tstops == 0) ^ (_Tcount == 0))
		__ndxlSetLastError(NDXL_VALUE_ERR, "ndxlNetworkSimulateDopri: time stops specified without count.");
	else if (_Network->nsv || _Network->nse)
		__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateDopri: network has stochastic agents. Either remove stochastic agents or use one of the stochastic methods.");
	else
		_Result = __ndxlNetworkSimulateDopri(_Network, _Time, _Atol, _Rtol, _Tstops, _Tcount, _Callback);
	return _Result;
}

_NDXLEXP_ const int ndxlNetworkSimulateRkck(NDXLNetwork* _Network, const double _Time, const double _Atol, const double _Rtol, const double* _Tstops, const size_t _Tcount, NDXLNetworkCallback _Callback)
{
	int _Result = NDXL_ERR;
	if (!_Network)
		__ndxlSetLastError(NDXL_ARG_ERR, "ndxlNetworkSimulateRkck: null network.");
	else if (_Time <= 0.0)
		__ndxlSetLastError(NDXL_VALUE_ERR, "ndxlNetworkSimulateRkck: negative simulation time.");
	else if (_Atol <= 0.0)
		__ndxlSetLastError(NDXL_VALUE_ERR, "ndxlNetworkSimulateRkck: nonpositive absolute tolerance.");
	else if (_Rtol <= 0.0)
		__ndxlSetLastError(NDXL_VALUE_ERR, "ndxlNetworkSimulateRkck: nonpositive relative tolerance.");
	else if ((_Tstops == 0) ^ (_Tcount == 0))
		__ndxlSetLastError(NDXL_VALUE_ERR, "ndxlNetworkSimulateRkck: time stops specified without count.");
	else if (_Network->nsv || _Network->nse)
		__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRkck: network has stochastic agents. Either remove stochastic agents or use one of the stochastic methods.");
	else
		_Result = __ndxlNetworkSimulateRkck(_Network, _Time, _Atol, _Rtol, _Tstops, _Tcount, _Callback);
	return _Result;
}

_NDXLEXP_ const int ndxlNetworkSimulateHeun(NDXLNetwork* _Network, const double _Time, const double _Atol, const double _Rtol, const double* _Tstops, const size_t _Tcount, NDXLNetworkCallback _Callback)
{
	int _Result = NDXL_ERR;
	if (!_Network)
		__ndxlSetLastError(NDXL_ARG_ERR, "ndxlNetworkSimulateHeun: null network.");
	else if (_Time <= 0.0)
		__ndxlSetLastError(NDXL_VALUE_ERR, "ndxlNetworkSimulateHeun: negative simulation time.");
	else if (_Atol <= 0.0)
		__ndxlSetLastError(NDXL_VALUE_ERR, "ndxlNetworkSimulateHeun: nonpositive absolute tolerance.");
	else if (_Rtol <= 0.0)
		__ndxlSetLastError(NDXL_VALUE_ERR, "ndxlNetworkSimulateHeun: nonpositive relative tolerance.");
	else if ((_Tstops == 0) ^ (_Tcount == 0))
		__ndxlSetLastError(NDXL_VALUE_ERR, "ndxlNetworkSimulateHeun: time stops specified without count.");
	else if ((_Network->nsv == 0) && (_Network->nse == 0))
		__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateHeun: network has no stochastic agents. Either add stochastic agents or use one of the deterministic methods.");
	else
		_Result = __ndxlNetworkSimulateHeun(_Network, _Time, _Atol, _Rtol, _Tstops, _Tcount, _Callback);
	return _Result;
}

_NDXLEXP_ const int ndxlNetworkSimulateEuler(NDXLNetwork* _Network, const double _Time, const double _Step, NDXLNetworkCallback _Callback)
{
	int _Result = NDXL_ERR;
	if (!_Network)
		__ndxlSetLastError(NDXL_ARG_ERR, "ndxlNetworkSimulateEuler: null network.");
	else if (_Time <= 0.0)
		__ndxlSetLastError(NDXL_VALUE_ERR, "ndxlNetworkSimulateEuler: negative simulation time.");
	else if (_Step < 0.0)
		__ndxlSetLastError(NDXL_VALUE_ERR, "ndxlNetworkSimulateEuler: nonpositive absolute tolerance.");
	else if ((_Network->nsv == 0) && (_Network->nse == 0))
		__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateEuler: network has no stochastic agents. Either add stochastic agents or use one of the deterministic methods.");
	else
		_Result = __ndxlNetworkSimulateEuler(_Network, _Time, _Step, _Callback);
	return _Result;
}

_NDXLEXP_ const int ndxlNetworkSimulateRadau(NDXLNetwork* _Network, const double _Time, const double _Atol, const double _Rtol, const double* _Tstops, const size_t _Tcount, NDXLNetworkCallback _Callback)
{
	int _Result = NDXL_ERR;
	if (!_Network)
		__ndxlSetLastError(NDXL_ARG_ERR, "ndxlNetworkSimulateRadau: null network.");
	else if (_Time <= 0.0)
		__ndxlSetLastError(NDXL_VALUE_ERR, "ndxlNetworkSimulateRadau: negative simulation time.");
	else if (_Atol <= 0.0)
		__ndxlSetLastError(NDXL_VALUE_ERR, "ndxlNetworkSimulateRadau: nonpositive absolute tolerance.");
	else if (_Rtol <= 0.0)
		__ndxlSetLastError(NDXL_VALUE_ERR, "ndxlNetworkSimulateRadau: nonpositive relative tolerance.");
	else if ((_Tstops == 0) ^ (_Tcount == 0))
		__ndxlSetLastError(NDXL_VALUE_ERR, "ndxlNetworkSimulateRadau: time stops specified without count.");
	else if (_Network->nsv || _Network->nse)
		__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRadau: network has stochastic agents. Either remove stochastic agents or use one of the stochastic methods.");
	else
		_Result = __ndxlNetworkSimulateRadau(_Network, _Time, _Atol, _Rtol, _Tstops, _Tcount, _Callback);
	return _Result;
}

// ^^^^^ Simulation