#include "../integration/integration.h"

// Because users cannot do anything in NDXL without a NDXLNetwork, they are all but guaranteed to use at least one function in this file
// We therefore do compiler-specific library initialisation here, for example, registering the garbage collection functions

#if UNI_COMPILER == UNI_COMPILER_MSVC
	#ifdef NDXL_STATIC
		#pragma section(".CRT$XCU", read)
		static void __cdecl init_library(void)
		{
			__ndxlInit();
		}
		__declspec(allocate(".CRT$XCU")) void (__cdecl *p_init)(void) = init_library;
	#else
		BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
		{
			if (fdwReason == DLL_PROCESS_ATTACH)
				__ndxlInit();
			return TRUE;
		}
	#endif	// NDXL_STATIC
#else
	__attribute__((constructor))
	static void init_library(void)
	{
		__ndxlInit();
	}
#endif	// MSVC

// ^^^^^ Library initialisation | constructor and destructor vvvvv

NDXLNetwork* __ndxlCreateNetwork(void* _Params)
{
	// Initialise explicit variables
	NDXLNetwork* nwk = (NDXLNetwork*)malloc(sizeof(NDXLNetwork));
	nwk->params = _Params;

	// Initialise implicit variables
	__ndxlResizeNetworkArray();
	nwk->idx = __ndxlNetworkCount;
	nwk->nv = 0;
	nwk->nsv = 0;
	nwk->ne = 0;
	nwk->nse = 0;
	nwk->nc = 0;
	nwk->vidx = 0;
	nwk->vcap = 7;
	nwk->eidx = 0;
	nwk->ecap = 7;
	nwk->cidx = 0;
	nwk->ccap = 3;
	nwk->aidx = 0;
	nwk->vertices = (NDXLVertex**)calloc(nwk->vcap, sizeof(NDXLVertex*));
	nwk->edges = (NDXLEdge**)calloc(nwk->ecap, sizeof(NDXLEdge*));
	nwk->channels = (NDXLChannel**)calloc(nwk->ccap, sizeof(NDXLChannel*));

	// Initialise simulation variables
	nwk->time = 0.0;
	nwk->h = -1.0;
	nwk->s = -1.0;
	nwk->atol = -1.0;
	nwk->rtol = -1.0;
	nwk->butcher = 0;
	nwk->stage = 0;
	nwk->method_explicit = -1;

	// Initialise Runge-Kutta variables
	nwk->jacn = 0;
	nwk->jacn_cap = (nwk->vcap << 1) + 1;
	nwk->state = (double*)malloc(nwk->jacn_cap * sizeof(double));
	nwk->rkf[0] = (double*)malloc(nwk->jacn_cap * sizeof(double));
	nwk->rkf[1] = (double*)malloc(nwk->jacn_cap * sizeof(double));
	nwk->rkf[2] = (double*)malloc(nwk->jacn_cap * sizeof(double));
	nwk->rkf[3] = (double*)malloc(nwk->jacn_cap * sizeof(double));
	nwk->rkf[4] = (double*)malloc(nwk->jacn_cap * sizeof(double));
	nwk->rkf[5] = (double*)malloc(nwk->jacn_cap * sizeof(double));
	nwk->rkf[6] = (double*)malloc(nwk->jacn_cap * sizeof(double));
	nwk->rkf[7] = (double*)malloc(nwk->jacn_cap * sizeof(double));
	nwk->rkf[8] = (double*)malloc(nwk->jacn_cap * sizeof(double));
	nwk->stage_state[0] = (double*)malloc(nwk->jacn_cap * sizeof(double));
	nwk->stage_state[1] = (double*)malloc(nwk->jacn_cap * sizeof(double));

	// Add created network to array and return
	__ndxlNetworks[__ndxlNetworkCount++] = nwk;
	return nwk;
}

void __ndxlDestroyNetwork(NDXLNetwork* _Network)
{
	// Destroy all vertices (automatically destroys associated edges)
	while (_Network->nv)
		__ndxlDestroyVertex(_Network->vertices[0]);

	// Swap last network index with current index so array remains contiguous
	__ndxlNetworkCount--;
	__ndxlNetworks[_Network->idx] = __ndxlNetworks[__ndxlNetworkCount];
	__ndxlNetworks[__ndxlNetworkCount] = 0;

	// Finally, free network memory
	free(_Network->vertices);
	free(_Network->edges);
	free(_Network->state);
	free(_Network->rkf[0]);
	free(_Network->rkf[1]);
	free(_Network->rkf[2]);
	free(_Network->rkf[3]);
	free(_Network->rkf[4]);
	free(_Network->rkf[5]);
	free(_Network->rkf[6]);
	free(_Network->rkf[7]);
	free(_Network->rkf[8]);
	free(_Network->stage_state[0]);
	free(_Network->stage_state[1]);
	free(_Network);
}

// ^^^^^ Constructor and destructor | getters vvvvv

const size_t __ndxlGetNetworkNoiseDimension(NDXLNetwork* _Network)
{
	return _Network->nc;
}

const size_t __ndxlGetNetworkOrder(NDXLNetwork* _Network)
{
	return _Network->nv;
}

void* __ndxlGetNetworkParams(NDXLNetwork* _Network)
{
	return _Network->params;
}

const size_t __ndxlGetNetworkSize(NDXLNetwork* _Network)
{
	return _Network->ne;
}

const double __ndxlGetNetworkTime(NDXLNetwork* _Network)
{
	return _Network->time;
}

// ^^^^^ Getters | setters vvvvv

void __ndxlSetNetworkParams(NDXLNetwork* _Network, void* _Params)
{
	_Network->params = _Params;
}

// ^^^^^ Setters | functions vvvvv

NDXLChannel* __ndxlNetworkChannel(NDXLNetwork* _Network, const size_t _Idx)
{
	return _Network->channels[_Idx];
}

NDXLChannel* __ndxlNetworkChannelByIndex(NDXLNetwork* _Network, const size_t _Idx)
{
	for (size_t i = 0; i < _Network->nc; i++)
		if (_Network->channels[i]->idx == _Idx)
			_Network->channels[i];
	__ndxlSetLastError(NDXL_VALUE_ERR, "ndxlNetworkChannelByIndex: no channel with corresponding index found.");
	return 0;
}

NDXLEdge* __ndxlNetworkEdge(NDXLNetwork* _Network, const size_t _Idx)
{
	return _Network->edges[_Idx];
}

NDXLEdge* __ndxlNetworkEdgeByIndex(NDXLNetwork* _Network, const size_t _Idx)
{
	for (size_t i = 0; i < _Network->ne; i++)
		if (_Network->edges[i]->idx == _Idx)
			_Network->edges[i];
	__ndxlSetLastError(NDXL_VALUE_ERR, "ndxlNetworkEdgeByIndex: no edge with corresponding index found.");
	return 0;
}

NDXLVertex* __ndxlNetworkVertex(NDXLNetwork* _Network, const size_t _Idx)
{
	return _Network->vertices[_Idx];
}

NDXLVertex* __ndxlNetworkVertexByIndex(NDXLNetwork* _Network, const size_t _Idx)
{
	for (size_t i = 0; i < _Network->nv; i++)
		if (_Network->vertices[i]->idx == _Idx)
			_Network->vertices[i];
	__ndxlSetLastError(NDXL_VALUE_ERR, "ndxlNetworkVertexByIndex: no vertex with corresponding index found.");
	return 0;
}

void __ndxlNetworkResizeArrays(NDXLNetwork* _Network)
{
	// Verify capacity of vertex array
	if (_Network->nv == _Network->vcap)
	{
		_Network->vcap = (_Network->vcap << 1) + 1;
		_Network->vertices = (NDXLVertex**)realloc(_Network->vertices, _Network->vcap * sizeof(NDXLVertex*));
	}

	// Verify capacity of edge array
	if (_Network->ne == _Network->ecap)
	{
		_Network->ecap = (_Network->ecap << 1) + 1;
		_Network->edges = (NDXLEdge**)realloc(_Network->edges, _Network->ecap * sizeof(NDXLEdge*));
	}

	// Verify capacity of channel array
	if (_Network->nc == _Network->ccap)
	{
		_Network->ccap = (_Network->ccap << 1) + 1;
		_Network->channels = (NDXLChannel**)realloc(_Network->channels, _Network->ccap * sizeof(NDXLChannel*));
	}
}

void __ndxlNetworkResizeVectors(NDXLNetwork* _Network)
{
	// Verify capacity of state vectors
	if (_Network->jacn > _Network->jacn_cap)
	{
		_Network->jacn_cap = (_Network->jacn_cap << 1) + 1;
		_Network->state = (double*)realloc(_Network->state, _Network->jacn_cap * sizeof(double));
		_Network->stage_state[0] = (double*)realloc(_Network->stage_state[0], _Network->jacn_cap * sizeof(double));
		_Network->stage_state[1] = (double*)realloc(_Network->stage_state[1], _Network->jacn_cap * sizeof(double));
		_Network->rkf[0] = (double*)realloc(_Network->rkf[0], _Network->jacn_cap * sizeof(double));
		_Network->rkf[1] = (double*)realloc(_Network->rkf[1], _Network->jacn_cap * sizeof(double));
		_Network->rkf[2] = (double*)realloc(_Network->rkf[2], _Network->jacn_cap * sizeof(double));
		_Network->rkf[3] = (double*)realloc(_Network->rkf[3], _Network->jacn_cap * sizeof(double));
		_Network->rkf[4] = (double*)realloc(_Network->rkf[4], _Network->jacn_cap * sizeof(double));
		_Network->rkf[5] = (double*)realloc(_Network->rkf[5], _Network->jacn_cap * sizeof(double));
		_Network->rkf[6] = (double*)realloc(_Network->rkf[6], _Network->jacn_cap * sizeof(double));
		_Network->rkf[7] = (double*)realloc(_Network->rkf[7], _Network->jacn_cap * sizeof(double));
		_Network->rkf[8] = (double*)realloc(_Network->rkf[8], _Network->jacn_cap * sizeof(double));
	}
}

// ^^^^^ Functions | simulation vvvvv

const int __ndxlNetworkSimulateRalston(NDXLNetwork* _Network, const double _Time, const double _Step, NDXLNetworkCallback _Callback)
{
	// Ensure there are agents in the network
	if (!_Network->jacn)
	{
		__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRalston: no agents to simulate.");
		return NDXL_ERR;
	}

	// Process callback at start, if any
	if (_Callback && _Network->time == 0.0)
		if (_Callback(_Network, 0.0, _Network->params))
		{
			__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRalston: external error encountered in network callback.");
			return NDXL_ERR;
		}

	// Setup network parameters
	_Network->butcher = __butcherRalstonC;
	_Network->method_explicit = 1;
	_Network->h = _Step;

	// Setup simulatiuon variables
	const size_t _Steps = (size_t)(_Time / _Step);
	const double _Endtime = _Network->time + _Steps * _Step;
	double _T;
	size_t i, j;
	NDXLVertex* vert;
	NDXLEdge* edge;

	for (i = 1; i <= _Steps; i++)
	{
		// Calculate time as integer multiple of step to minimise accumulated floating point rounding errors
		_T = _Network->time + i * _Step;

		// Ralston is a four stage method
		if (__ndxlRalstonStage0(_Network))
		{
			_Network->stage = 0;
			return NDXL_ERR;
		}
		if (__ndxlRalstonStage1(_Network))
		{
			_Network->stage = 0;
			return NDXL_ERR;
		}
		if (__ndxlRalstonStage2(_Network))
		{
			_Network->stage = 0;
			return NDXL_ERR;
		}
		if (__ndxlRalstonStage3(_Network))
		{
			_Network->stage = 0;
			return NDXL_ERR;
		}

		// Accept new state
		memcpy(_Network->state, _Network->stage_state[0], _Network->jacn * sizeof(double));

		// Resolve vertex callbacks
		for (j = _Network->nv; j > 0; j--)
		{
			vert = _Network->vertices[j - 1];
			vert->time += _Network->h;
			if (vert->callback)
			{
				if (vert->callback(vert, _Network->state + vert->nidx, vert->time, vert->params))
				{
					__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRalston: external error encountered in vertex callback.");
					return NDXL_ERR;
				}
			}
		}

		// Resolve edge callbacks
		for (j = _Network->ne; j > 0; j--)
		{
			edge = _Network->edges[j - 1];
			__ndxlEdgeAccumulateEffects(edge, _Network->h);
			if (edge->callback)
			{
				if (edge->callback(edge, _Network->state + edge->nidx, edge->time, edge->params))
				{
					__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRalston: external error encountered in edge callback.");
					return NDXL_ERR;
				}
			}
		}

		// Process callback, if any
		if (_Callback)
			if (_Callback(_Network, _T, _Network->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRalston: external error encountered in network callback.");
				return NDXL_ERR;
			}
	}
	_Network->time = _Endtime;
	return NDXL_OK;
}

const int __ndxlNetworkSimulateNystrom(NDXLNetwork* _Network, const double _Time, const double _Step, NDXLNetworkCallback _Callback)
{
	// Ensure there are agents in the network
	if (!_Network->jacn)
	{
		__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateNystrom: no agents to simulate.");
		return NDXL_ERR;
	}

	// Process callback at start, if any
	if (_Callback && _Network->time == 0.0)
		if (_Callback(_Network, 0.0, _Network->params))
		{
			__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateNystrom: external error encountered in network callback.");
			return NDXL_ERR;
		}

	// Setup network parameters
	_Network->butcher = __butcherNystromC;
	_Network->method_explicit = 1;
	_Network->h = _Step;

	// Setup simulatiuon variables
	const size_t _Steps = (size_t)(_Time / _Step);
	const double _Endtime = _Network->time + _Steps * _Step;
	double _T;
	size_t i, j;
	NDXLVertex* vert;
	NDXLEdge* edge;

	for (i = 1; i <= _Steps; i++)
	{
		// Calculate time as integer multiple of step to minimise accumulated floating point rounding errors
		_T = _Network->time + i * _Step;

		// Nystrom is a six stage method
		if (__ndxlNystromStage0(_Network))
		{
			_Network->stage = 0;
			return NDXL_ERR;
		}
		if (__ndxlNystromStage1(_Network))
		{
			_Network->stage = 0;
			return NDXL_ERR;
		}
		if (__ndxlNystromStage2(_Network))
		{
			_Network->stage = 0;
			return NDXL_ERR;
		}
		if (__ndxlNystromStage3(_Network))
		{
			_Network->stage = 0;
			return NDXL_ERR;
		}
		if (__ndxlNystromStage4(_Network))
		{
			_Network->stage = 0;
			return NDXL_ERR;
		}
		if (__ndxlNystromStage5(_Network))
		{
			_Network->stage = 0;
			return NDXL_ERR;
		}

		// Accept new state
		memcpy(_Network->state, _Network->stage_state[0], _Network->jacn * sizeof(double));

		// Resolve vertex callbacks
		for (j = _Network->nv; j > 0; j--)
		{
			vert = _Network->vertices[j - 1];
			vert->time += _Network->h;
			if (vert->callback)
			{
				if (vert->callback(vert, _Network->state + vert->nidx, vert->time, vert->params))
				{
					__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateNystrom: external error encountered in vertex callback.");
					return NDXL_ERR;
				}
			}
		}

		// Resolve edge callbacks
		for (j = _Network->ne; j > 0; j--)
		{
			edge = _Network->edges[j - 1];
			__ndxlEdgeAccumulateEffects(edge, _Network->h);
			if (edge->callback)
			{
				if (edge->callback(edge, _Network->state + edge->nidx, edge->time, edge->params))
				{
					__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateNystrom: external error encountered in edge callback.");
					return NDXL_ERR;
				}
			}
		}

		// Process callback, if any
		if (_Callback)
			if (_Callback(_Network, _T, _Network->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateNystrom: external error encountered in network callback.");
				return NDXL_ERR;
			}
	}
	_Network->time = _Endtime;
	return NDXL_OK;
}

const int __ndxlNetworkSimulateDopri(NDXLNetwork* _Network, const double _Time, const double _Atol, const double _Rtol, const double* _Tstops, const size_t _Tcount, NDXLNetworkCallback _Callback)
{
	// Ensure there are agents in the network
	if (!_Network->jacn)
	{
		__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateDopri: no agents to simulate.");
		return NDXL_ERR;
	}

	// Process callback at start, if any
	if (_Callback && _Network->time == 0.0)
		if (_Callback(_Network, 0.0, _Network->params))
		{
			__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateDopri: external error encountered in network callback.");
			return NDXL_ERR;
		}

	// Setup network parameters
	_Network->butcher = __butcherDopriC;
	_Network->method_explicit = 1;
	_Network->atol = _Atol;
	_Network->rtol = _Rtol;
	if (__ndxlSelectInitialStep(_Network, 0.2))
		return NDXL_ERR;

	// Setup simulatiuon variables
	const double _Endtime = _Network->time + _Time;
	double _T = _Network->time;
	size_t _Tidx = 0;
	double errsum, hnew, tnext;
	size_t i, j;
	NDXLVertex* vert;
	NDXLEdge* edge;

	while (_T < _Endtime)
	{
		// Reset step rejection
		char _Rejected = 1;

		// Continue until step is successful
		while (_Rejected)
		{
			// Reset error sum
			errsum = 0.0;

			// DOPRI is a seven stage method
			if (__ndxlDopriStage0(_Network))
			{
				_Network->stage = 0;
				return NDXL_ERR;
			}
			if (__ndxlDopriStage1(_Network))
			{
				_Network->stage = 0;
				return NDXL_ERR;
			}
			if (__ndxlDopriStage2(_Network))
			{
				_Network->stage = 0;
				return NDXL_ERR;
			}
			if (__ndxlDopriStage3(_Network))
			{
				_Network->stage = 0;
				return NDXL_ERR;
			}
			if (__ndxlDopriStage4(_Network))
			{
				_Network->stage = 0;
				return NDXL_ERR;
			}
			if (__ndxlDopriStage5(_Network))
			{
				_Network->stage = 0;
				return NDXL_ERR;
			}
			if (__ndxlDopriStage6(_Network, &errsum))
			{
				_Network->stage = 0;
				return NDXL_ERR;
			}

			// Global tolerance is based on global max error from all agents
			errsum = fmax(sqrt(errsum / _Network->jacn), EPS);
			hnew = fmax(fmin(0.9 * pow(errsum, -0.2), 10.0), 0.2) * _Network->h;

			// Check step success
			if (errsum >= 1.0)
				_Network->h = hnew;
			else
			{
				_T += _Network->h;
				_Rejected = 0;
			}
		}

		// Accept new state
		memcpy(_Network->state, _Network->stage_state[0], _Network->jacn * sizeof(double));

		// Resolve vertex callbacks
		for (i = _Network->nv; i > 0; i--)
		{
			vert = _Network->vertices[i - 1];
			vert->time += _Network->h;
			if (vert->callback)
			{
				if (vert->callback(vert, _Network->state + vert->nidx, vert->time, vert->params))
				{
					__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateDopri: external error encountered in vertex callback.");
					return NDXL_ERR;
				}
			}
		}

		// Resolve edge callbacks
		for (i = _Network->ne; i > 0; i--)
		{
			edge = _Network->edges[i - 1];
			__ndxlEdgeAccumulateEffects(edge, _Network->h);
			if (edge->callback)
			{
				if (edge->callback(edge, _Network->state + edge->nidx, edge->time, edge->params))
				{
					__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateDopri: external error encountered in edge callback.");
					return NDXL_ERR;
				}
			}
		}

		// Check for time stops/end of simulation time and adapt h accordingly
		tnext = _T + hnew;
		if ((_Tidx < _Tcount) && (_T >= _Tstops[_Tidx]))
			_Tidx++;
		if ((_Tidx < _Tcount) && (tnext >= _Tstops[_Tidx]))
			hnew = _Tstops[_Tidx] - _T;
		if (_T + hnew > _Endtime)
			hnew = _Endtime - _T;
		_Network->h = hnew;

		// Process callback, if any
		if (_Callback)
			if (_Callback(_Network, _T, _Network->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateDopri: external error encountered in network callback.");
				return NDXL_ERR;
			}
	}
	_Network->time = _Endtime;
	return NDXL_OK;
}

const int __ndxlNetworkSimulateRkck(NDXLNetwork* _Network, const double _Time, const double _Atol, const double _Rtol, const double* _Tstops, const size_t _Tcount, NDXLNetworkCallback _Callback)
{
	// Ensure there are agents in the network
	if (!_Network->jacn)
	{
		__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRkck: no agents to simulate.");
		return NDXL_ERR;
	}

	// Process callback at start, if any
	if (_Callback && _Network->time == 0.0)
		if (_Callback(_Network, 0.0, _Network->params))
		{
			__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRkck: external error encountered in network callback.");
			return NDXL_ERR;
		}

	// Setup network parameters
	_Network->butcher = __butcherCashKarpC;
	_Network->method_explicit = 1;
	_Network->atol = _Atol;
	_Network->rtol = _Rtol;
	if (__ndxlSelectInitialStep(_Network, 0.2))
		return NDXL_ERR;

	// Setup simulatiuon variables
	const double _Endtime = _Network->time + _Time;
	double _T = _Network->time;
	size_t _Tidx = 0;
	double errsum, hnew, tnext;
	size_t i, j;
	NDXLVertex* vert;
	NDXLEdge* edge;

	while (_T < _Endtime)
	{
		// Reset step rejection
		char _Rejected = 1;

		// Continue until step is successful
		while (_Rejected)
		{
			// Reset error sum
			errsum = 0.0;

			// DOPRI is a seven stage method
			if (__ndxlRkckStage0(_Network))
			{
				_Network->stage = 0;
				return NDXL_ERR;
			}
			if (__ndxlRkckStage1(_Network))
			{
				_Network->stage = 0;
				return NDXL_ERR;
			}
			if (__ndxlRkckStage2(_Network))
			{
				_Network->stage = 0;
				return NDXL_ERR;
			}
			if (__ndxlRkckStage3(_Network))
			{
				_Network->stage = 0;
				return NDXL_ERR;
			}
			if (__ndxlRkckStage4(_Network))
			{
				_Network->stage = 0;
				return NDXL_ERR;
			}
			if (__ndxlRkckStage5(_Network, &errsum))
			{
				_Network->stage = 0;
				return NDXL_ERR;
			}

			// Global tolerance is based on global max error from all agents
			errsum = fmax(sqrt(errsum / _Network->jacn), EPS);
			hnew = fmax(fmin(0.9 * pow(errsum, -0.2), 10.0), 0.2) * _Network->h;

			// Check step success
			if (errsum >= 1.0)
				_Network->h = hnew;
			else
			{
				_T += _Network->h;
				_Rejected = 0;
			}
		}

		// Accept new state
		memcpy(_Network->state, _Network->stage_state[0], _Network->jacn * sizeof(double));

		// Resolve vertex callbacks
		for (i = _Network->nv; i > 0; i--)
		{
			vert = _Network->vertices[i - 1];
			vert->time += _Network->h;
			if (vert->callback)
			{
				if (vert->callback(vert, _Network->state + vert->nidx, vert->time, vert->params))
				{
					__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRkck: external error encountered in vertex callback.");
					return NDXL_ERR;
				}
			}
		}

		// Resolve edge callbacks
		for (i = _Network->ne; i > 0; i--)
		{
			edge = _Network->edges[i - 1];
			__ndxlEdgeAccumulateEffects(edge, _Network->h);
			if (edge->callback)
			{
				if (edge->callback(edge, _Network->state + edge->nidx, edge->time, edge->params))
				{
					__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRkck: external error encountered in edge callback.");
					return NDXL_ERR;
				}
			}
		}

		// Check for time stops/end of simulation time and adapt h accordingly
		tnext = _T + hnew;
		if ((_Tidx < _Tcount) && (_T >= _Tstops[_Tidx]))
			_Tidx++;
		if ((_Tidx < _Tcount) && (tnext >= _Tstops[_Tidx]))
			hnew = _Tstops[_Tidx] - _T;
		if (_T + hnew > _Endtime)
			hnew = _Endtime - _T;
		_Network->h = hnew;

		// Process callback, if any
		if (_Callback)
			if (_Callback(_Network, _T, _Network->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRkck: external error encountered in network callback.");
				return NDXL_ERR;
			}
	}
	_Network->time = _Endtime;
	return NDXL_OK;
}

const int __ndxlNetworkSimulateEuler(NDXLNetwork* _Network, const double _Time, const double _Step, NDXLNetworkCallback _Callback)
{
	// Ensure there are agents in the network
	if (!_Network->jacn)
	{
		__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateEuler: no agents to simulate.");
		return NDXL_ERR;
	}

	// Process callback at start, if any
	if (_Callback && _Network->time == 0.0)
		if (_Callback(_Network, 0.0, _Network->params))
		{
			__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateEuler: external error encountered in network callback.");
			return NDXL_ERR;
		}

	// Setup network parameters
	_Network->butcher = 0;
	_Network->method_explicit = 1;
	_Network->h = _Step;

	// Setup simulatiuon variables
	const size_t _Steps = (size_t)(_Time / _Step);
	const double _Endtime = _Network->time + _Steps * _Step;
	double _T;
	size_t i, j;
	NDXLVertex* vert;
	NDXLEdge* edge;

	for (i = 1; i <= _Steps; i++)
	{
		// Calculate time as integer multiple of step to minimise accumulated floating point rounding errors
		_T = _Network->time + i * _Step;

		// Euler is a one stage method
		if (__ndxlEulerStage0(_Network))
		{
			_Network->stage = 0;
			return NDXL_ERR;
		}

		// Accept new state
		memcpy(_Network->state, _Network->stage_state[0], _Network->jacn * sizeof(double));

		// Resolve vertex callbacks
		for (j = _Network->nv; j > 0; j--)
		{
			vert = _Network->vertices[j - 1];
			vert->time += _Network->h;
			if (vert->callback)
			{
				if (vert->callback(vert, _Network->state + vert->nidx, vert->time, vert->params))
				{
					__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateEuler: external error encountered in vertex callback.");
					return NDXL_ERR;
				}
			}
		}

		// Resolve edge callbacks
		for (j = _Network->ne; j > 0; j--)
		{
			edge = _Network->edges[j - 1];
			__ndxlEdgeAccumulateEffects(edge, _Network->h);
			if (edge->callback)
			{
				if (edge->callback(edge, _Network->state + edge->nidx, edge->time, edge->params))
				{
					__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateEuler: external error encountered in edge callback.");
					return NDXL_ERR;
				}
			}
		}

		// Process callback, if any
		if (_Callback)
			if (_Callback(_Network, _T, _Network->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateEuler: external error encountered in network callback.");
				return NDXL_ERR;
			}
	}
	_Network->time = _Endtime;
	return NDXL_OK;
}

const int __ndxlNetworkSimulateHeun(NDXLNetwork* _Network, const double _Time, const double _Atol, const double _Rtol, const double* _Tstops, const size_t _Tcount, NDXLNetworkCallback _Callback)
{
	// Ensure there are agents in the network
	if (!_Network->jacn)
	{
		__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateHeun: no agents to simulate.");
		return NDXL_ERR;
	}

	// Process callback at start, if any
	if (_Callback && _Network->time == 0.0)
		if (_Callback(_Network, 0.0, _Network->params))
		{
			__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateHeun: external error encountered in network callback.");
			return NDXL_ERR;
		}

	// Setup network parameters
	_Network->butcher = 0;
	_Network->method_explicit = 1;
	_Network->atol = _Atol;
	_Network->rtol = _Rtol;
	if (__ndxlSelectInitialStepStochastic(_Network, 1.0))
		return NDXL_ERR;

	// Setup simulatiuon variables
	const double _Endtime = _Network->time + _Time;
	double _T = _Network->time;
	size_t _Tidx = 0;
	double errsum, hnew, tnext;
	size_t i, j;
	NDXLVertex* vert;
	NDXLEdge* edge;

	while (_T < _Endtime)
	{
		// Reset step rejection
		char _Rejected = 1;
		for (i = 0; i < _Network->nc; i++)
			_Network->h = __ndxlChannelRedraw(_Network->channels[i]);

		// Continue until step is successful
		while (_Rejected)
		{
			// Reset error sum
			errsum = 0.0;

			// Heun-Euler is a two stage method
			if (__ndxlHeunStage0(_Network))
			{
				_Network->stage = 0;
				return NDXL_ERR;
			}
			if (__ndxlHeunStage1(_Network, &errsum))
			{
				_Network->stage = 0;
				return NDXL_ERR;
			}

			// Global tolerance is based on global max error from all agents
			errsum = sqrt(errsum / _Network->jacn);
			hnew = fmax(fmin(0.9 * errsum, 10.0), 0.2) * _Network->h;

			// Check step success
			if (errsum < 1.0)
			{
				for (i = 0; i < _Network->nc; i++)
					__ndxlCreateChannelIncrement(_Network->channels[i], _Network->h, hnew);
				_Network->h = hnew;
			}
			else
			{
				_T += _Network->h;
				_Rejected = 0;
			}
		}

		// Accept new state
		memcpy(_Network->state, _Network->stage_state[0], _Network->jacn * sizeof(double));

		// Resolve vertex callbacks
		for (i = _Network->nv; i > 0; i--)
		{
			vert = _Network->vertices[i - 1];
			vert->time += _Network->h;
			if (vert->callback)
			{
				if (vert->callback(vert, _Network->state + vert->nidx, vert->time, vert->params))
				{
					__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateHeun: external error encountered in vertex callback.");
					return NDXL_ERR;
				}
			}
		}

		// Resolve edge callbacks
		for (i = _Network->ne; i > 0; i--)
		{
			edge = _Network->edges[i - 1];
			__ndxlEdgeAccumulateEffects(edge, _Network->h);
			if (edge->callback)
			{
				if (edge->callback(edge, _Network->state + edge->nidx, edge->time, edge->params))
				{
					__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateHeun: external error encountered in edge callback.");
					return NDXL_ERR;
				}
			}
		}

		// Check for time stops/end of simulation time and adapt h accordingly
		tnext = _T + hnew;
		if ((_Tidx < _Tcount) && (_T >= _Tstops[_Tidx]))
			_Tidx++;
		if ((_Tidx < _Tcount) && (tnext >= _Tstops[_Tidx]))
			hnew = _Tstops[_Tidx] - _T;
		if (_T + hnew > _Endtime)
			hnew = _Endtime - _T;
		_Network->h = hnew;
		_Network->s = sqrt(_Network->h);

		// Process callback, if any
		if (_Callback)
			if (_Callback(_Network, _T, _Network->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateHeun: external error encountered in network callback.");
				return NDXL_ERR;
			}
	}
	_Network->time = _Endtime;
	return NDXL_OK;
}

const int __ndxlNetworkSimulateRadau(NDXLNetwork* _Network, const double _Time, const double _Atol, const double _Rtol, const double* _Tstops, const size_t _Tcount, NDXLNetworkCallback _Callback)
{
	// Ensure there are agents in the network
	if (!_Network->jacn)
	{
		__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRadau: no agents to simulate.");
		return NDXL_ERR;
	}

	// Process callback at start, if any
	if (_Callback && _Network->time == 0.0)
		if (_Callback(_Network, 0.0, _Network->params))
		{
			__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRadau: external error encountered in network callback.");
			return NDXL_ERR;
		}

	// Setup network parameters
	_Network->butcher = __butcherRadauC;
	_Network->method_explicit = 0;
	_Network->atol = _Atol;
	_Network->rtol = _Rtol;
	if (__ndxlSelectInitialStep(_Network, 0.25))
		return NDXL_ERR;

	// Setup simulatiuon variables
	const double _Endtime = _Network->time + _Time;
	double _T = _Network->time;
	size_t _Tidx = 0;
	size_t _Jacn = 0;
	double* _Jac = 0;
	double* _Mlhsreal = 0;
	double* _Mrhsreal = 0;
	double* _Decoupreal = 0;
	complex_t* _Mlhsimag = 0;
	complex_t* _Mrhsimag = 0;
	complex_t* _Decoupimag = 0;
	double* _Erhs = 0;
	double* _Errloc = 0;
	size_t* _Pivots[2] = {0, 0};
	size_t i;
	int solve;
	double err, hnew, tnext;
	NDXLVertex* vert;
	NDXLEdge* edge;

	while (_T < _Endtime)
	{
		// Reset step rejection
		char _Rejected = 1;

		// Prepare Jacobian and network vectors
		if (_Jacn < _Network->jacn_cap)
		{
			_Jacn = _Network->jacn_cap;
			_Jac = (double*)realloc(_Jac, _Network->jacn_cap * _Network->jacn_cap * sizeof(double));
			_Mlhsreal = (double*)realloc(_Mlhsreal, _Network->jacn_cap * _Network->jacn_cap * sizeof(double));
			_Mrhsreal = (double*)realloc(_Mrhsreal, _Network->jacn_cap * sizeof(double));
			_Decoupreal = (double*)realloc(_Decoupreal, _Network->jacn_cap * sizeof(double));
			_Mlhsimag = (complex_t*)realloc(_Mlhsimag, _Network->jacn_cap * _Network->jacn_cap * sizeof(complex_t));
			_Mrhsimag = (complex_t*)realloc(_Mrhsimag, _Network->jacn_cap * sizeof(complex_t));
			_Decoupimag = (complex_t*)realloc(_Decoupimag, _Network->jacn_cap * sizeof(complex_t));
			_Erhs = (double*)realloc(_Erhs, _Network->jacn_cap * sizeof(double));
			_Errloc = (double*)realloc(_Errloc, _Network->jacn_cap * sizeof(double));
			_Pivots[0] = (size_t*)realloc(_Pivots[0], (_Network->jacn_cap + 1) * sizeof(size_t));
			_Pivots[1] = (size_t*)realloc(_Pivots[1], (_Network->jacn_cap + 1) * sizeof(size_t));
		}
		if (__ndxlRadauSlopes(_Network, _Jac, _Mlhsreal, _Mrhsreal, _Decoupreal, _Mlhsimag, _Mrhsimag, _Decoupimag, _Erhs, _Errloc, _Pivots))
			return NDXL_ERR;

		// Continue until step is successful
		while (_Rejected)
		{
			// PLU-decompose decoupled mass matrix, and prepare initial stage guesses
			__ndxlRadauGuesses(_Network, _Jac, _Mlhsreal, _Mlhsimag, _Pivots);

			// Optimise stage equation residuals; if not converged, halve step size and retry
			solve = __ndxlRadauSolve(_Network, _Mlhsreal, _Mrhsreal, _Decoupreal, _Mlhsimag, _Mrhsimag, _Decoupimag, _Pivots);
			if (solve == NDXL_ERR)
				return NDXL_ERR;
			if (solve == 1)
				_Network->h *= 0.5;
			else
			{
				// Compute error and next step size
				err = __ndxlRadauError(_Network, _Mlhsreal, _Erhs, _Errloc, _Pivots);
				hnew = fmin(fmax(0.9 * pow(err, -0.25), 0.2), 10.0) * _Network->h;

				// Check step success
				if (err >= 1.0)
					_Network->h = hnew;
				else
				{
					_T += _Network->h;
					_Rejected = 0;
				}
			}
		}

		// Accept new state
		memcpy(_Network->state, _Network->rkf[2], _Network->jacn * sizeof(double));

		// Resolve vertex callbacks
		for (i = _Network->nv; i > 0; i--)
		{
			vert = _Network->vertices[i - 1];
			vert->time += _Network->h;
			if (vert->callback)
			{
				if (vert->callback(vert, _Network->state + vert->nidx, vert->time, vert->params))
				{
					__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRadau: external error encountered in vertex callback.");
					return NDXL_ERR;
				}
			}
		}

		// Resolve edge callbacks
		for (i = _Network->ne; i > 0; i--)
		{
			edge = _Network->edges[i - 1];
			__ndxlEdgeAccumulateEffects(edge, _Network->h);
			if (edge->callback)
			{
				if (edge->callback(edge, _Network->state + edge->nidx, edge->time, edge->params))
				{
					__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRadau: external error encountered in edge callback.");
					return NDXL_ERR;
				}
			}
		}

		// Check for time stops/end of simulation time and adapt h accordingly
		tnext = _T + hnew;
		if ((_Tidx < _Tcount) && (_T >= _Tstops[_Tidx]))
			_Tidx++;
		if ((_Tidx < _Tcount) && (tnext >= _Tstops[_Tidx]))
			hnew = _Tstops[_Tidx] - _T;
		if (_T + hnew > _Endtime)
			hnew = _Endtime - _T;
		_Network->h = hnew;

		// Process callback, if any
		if (_Callback)
			if (_Callback(_Network, _T, _Network->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRadau: external error encountered in network callback.");
				return NDXL_ERR;
			}
	}
	_Network->time = _Endtime;
	return NDXL_OK;
}

// ^^^^^ Simulation
