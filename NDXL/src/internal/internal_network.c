//--------------------------------------\\
// NDXL source file for network class	\\
// Copyright Ethan Codron 2026			\\
// GNU GPL-3.0							\\
//--------------------------------------\\
// Source file providing all network	\\
// functions used internally by the		\\
// API.									\\
//--------------------------------------\\

#include "internal.h"

// Network constructor and destructors vvvvv

NDXLNetwork* __ndxlCreateNetwork(void* _Params)
{
	// Initialise explicit values
	NDXLNetwork* nwk = (NDXLNetwork*)malloc(sizeof(NDXLNetwork));
	nwk->params = _Params;

	// Initialise implicit values
	nwk->ndv = 0;
	nwk->nde = 0;
	nwk->nsv = 0;
	nwk->nse = 0;
	nwk->nnc = 0;
	nwk->time = 0.0;
	nwk->h = 0.0;
	nwk->s = 0.0;
	nwk->butcher = 0;
	nwk->method_explicit = 0;
	nwk->jacn = 0;
	nwk->jacn_old = 0;
	nwk->pivots[0] = 0;
	nwk->pivots[1] = 0;
	nwk->jac = 0;
	nwk->erhs = 0;
	nwk->errloc = 0;
	nwk->mlhs_real = 0;
	nwk->mrhs_real = 0;
	nwk->decoup_real = 0;
	nwk->mlhs_imag = 0;
	nwk->mrhs_imag = 0;
	nwk->decoup_imag = 0;
	nwk->channel_head = 0;
	nwk->channel_tail = 0;
	nwk->vertex_head = 0;
	nwk->vertex_tail = 0;
	nwk->edge_head = 0;
	nwk->edge_tail = 0;
	nwk->prev = __ndxlNetworkTail;
	nwk->next = 0;

	// Add to network stack
	if (__ndxlNetworkHead)
	{
		__ndxlNetworkTail->next = nwk;
		__ndxlNetworkTail = __ndxlNetworkTail->next;
	}
	else
	{
		__ndxlNetworkHead = nwk;
		__ndxlNetworkTail = nwk;
	}

	// Increment network count and return network
	__ndxlNetworkCount++;
	return nwk;
}

void __ndxlDestroyNetwork(NDXLNetwork* _Network)
{
	// Decrement network count
	__ndxlNetworkCount--;

	// Free associated vertices (automatically frees any associated edges)
	size_t i = 0;
	NDXLVertex* vert = _Network->vertex_head;
	while (vert)
	{
		NDXLVertex* tmp = vert;
		vert = vert->next;
		__ndxlDestroyVertex(tmp);
	}

	// De-link from network stack
	if (_Network->prev)
		_Network->prev->next = _Network->next;
	else
		__ndxlNetworkHead = __ndxlNetworkHead->next;
	if (_Network->next)
		_Network->next->prev = _Network->prev;
	else
		__ndxlNetworkTail = __ndxlNetworkTail->prev;

	// Finally, free network memory
	free(_Network->jac);
	free(_Network->pivots[0]);
	free(_Network->pivots[1]);
	free(_Network->erhs);
	free(_Network->errloc);
	free(_Network->mlhs_real);
	free(_Network->mrhs_real);
	free(_Network->decoup_real);
	free(_Network->mlhs_imag);
	free(_Network->mrhs_imag);
	free(_Network->decoup_imag);
	free(_Network);
}

void __ndxlCleanupNetworks()
{
	NDXLNetwork* nwk = __ndxlNetworkHead;
	while (nwk)
	{
		NDXLNetwork* tmp = nwk;
		nwk = nwk->next;
		__ndxlDestroyNetwork(tmp);
	}
	__ndxlNetworkHead = 0;
	__ndxlNetworkTail = 0;
}

// ^^^^^ Network constructor and destructors | network getters vvvvv

const size_t __ndxlGetNetworkChannelCount(NDXLNetwork* _Network)
{
	return _Network->nnc;
}

void __ndxlGetNetworkEdges(NDXLNetwork* _Network, NDXLEdge** _Edges)
{
	NDXLEdge* edge = _Network->edge_head;
	size_t i = 0;
	while (edge)
	{
		_Edges[i++] = edge;
		edge = edge->next;
	}
}

NDXLChannel* __ndxlGetNetworkFirstChannel(NDXLNetwork* _Network)
{
	return _Network->channel_head;
}

NDXLEdge* __ndxlGetNetworkFirstEdge(NDXLNetwork* _Network)
{
	return _Network->edge_head;
}

NDXLVertex* __ndxlGetNetworkFirstVertex(NDXLNetwork* _Network)
{
	return _Network->vertex_head;
}

NDXLChannel* __ndxlGetNetworkLastChannel(NDXLNetwork* _Network)
{
	return _Network->channel_tail;
}

NDXLEdge* __ndxlGetNetworkLastEdge(NDXLNetwork* _Network)
{
	return _Network->edge_tail;
}

NDXLVertex* __ndxlGetNetworkLastVertex(NDXLNetwork* _Network)
{
	return _Network->vertex_tail;
}

NDXLChannel* __ndxlGetNetworkNextChannel(NDXLChannel* _Channel)
{
	return _Channel->next;
}

NDXLEdge* __ndxlGetNetworkNextEdge(NDXLEdge* _Edge)
{
	return _Edge->next;
}

NDXLVertex* __ndxlGetNetworkNextVertex(NDXLVertex* _Vertex)
{
	return _Vertex->next;
}

const size_t __ndxlGetNetworkOrder(NDXLNetwork* _Network)
{
	return _Network->ndv + _Network->nsv;
}

void* __ndxlGetNetworkParams(NDXLNetwork* _Network)
{
	return _Network->params;
}

NDXLChannel* __ndxlGetNetworkPreviousChannel(NDXLChannel* _Channel)
{
	return _Channel->prev;
}

NDXLEdge* __ndxlGetNetworkPreviousEdge(NDXLEdge* _Edge)
{
	return _Edge->prev;
}

NDXLVertex* __ndxlGetNetworkPreviousVertex(NDXLVertex* _Vertex)
{
	return _Vertex->prev;
}

const size_t __ndxlGetNetworkSize(NDXLNetwork* _Network)
{
	return _Network->nde + _Network->nse;
}

const double __ndxlGetNetworkTime(NDXLNetwork* _Network)
{
	return _Network->time;
}

void __ndxlGetNetworkVertices(NDXLNetwork* _Network, NDXLVertex** _Vertices)
{
	NDXLVertex* vert = _Network->vertex_head;
	size_t i = 0;
	while (vert)
	{
		_Vertices[i++] = vert;
		vert = vert->next;
	}
}

// ^^^^^ Network getters | network setters vvvvv

void __ndxlSetNetworkParams(NDXLNetwork* _Network, void* _Params)
{
	_Network->params = _Params;
}

const int __ndxlSetNetworkInitialStep(NDXLNetwork* _Network, const double _Atol, const double _Rtol, const double _OrderInv)
{
	// Algorithm variables
	NDXLVertex* vert;
	NDXLEdge* edge;
	size_t i;
	double _Scale, _Yscale, _H0, _H1;

	// Scaled L2 norms
	double _D0 = 0.0;
	double _D1 = 0.0;
	double _D2 = 0.0;

	// Iterate over vertices to determine state L2 d0 and derivative L2 d1
	for (vert = _Network->vertex_head; vert; vert = vert->next)
	{
		if (vert->func)
		{
			if (vert->func(vert, vert->rkf[0], vert->state, vert->time, vert->params))
			{
				__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulate: external error encountered during vertex dynamics call in initial step size calculation.");
                return NDXL_ERR;
			}
		}
		else
			memset(vert->rkf[0], 0, vert->order * sizeof(double));
		for (i = 0; i < vert->order; i++)
		{
			_Scale = _Atol + _Rtol * fabs(vert->state[i]);
			_Yscale = vert->state[i] / _Scale;
			_D0 += (_Yscale * _Yscale);
			_Yscale = vert->rkf[0][i] / _Scale;
			_D1 += (_Yscale * _Yscale);
		}
	}

	// Iterate over edges to determine state L2 d0 and derivative L2 d1
	for (edge = _Network->edge_head; edge; edge = edge->next)
	{
		if (edge->func)
		{
			if (edge->func(edge, edge->rkf[0], edge->state, edge->time, edge->params))
			{
				__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulate: external error encountered during edge dynamics call in initial step size calculation.");
                return NDXL_ERR;
			}
		}
		else
			memset(edge->rkf[0], 0, edge->order * sizeof(double));
		for (i = 0; i < edge->order; i++)
		{
			_Scale = _Atol + _Rtol * fabs(edge->state[i]);
			_Yscale = edge->state[i] / _Scale;
			_D0 += (_Yscale * _Yscale);
			_Yscale = edge->rkf[0][i] / _Scale;
			_D1 += (_Yscale * _Yscale);
		}
	}

	// Compute d0, d1, and initial step guess h0
	_D0 = sqrt(_D0 / _Network->jacn);
	_D1 = sqrt(_D1 / _Network->jacn);
	_H0 = ((_D0 < 1e-5) || (_D1 < 1e-5)) ? 1e-6 : 1e-2 * (_D0 / _D1);

	// Iterate over vertices to compute Euler step, and set stage = 1 for nearest neighbour interactions
	for (vert = _Network->vertex_head; vert; vert = vert->next)
	{
		for (i = 0; i < vert->order; i++)
			vert->stage_state[1][i] = vert->state[i] + _H0 * vert->rkf[0][i];
		vert->stage = 1;
	}

	// Iterate over edges to compute Euler step, and set stage = 1 for relative state interactions
	for (edge = _Network->edge_head; edge; edge = edge->next)
	{
		for (i = 0; i < edge->order; i++)
			edge->stage_state[1][i] = edge->state[i] + _H0 * edge->rkf[0][i];
		edge->stage = 1;
	}

	// Iterate over vertices to determine Euler state L2, and reset stage = 0 for integration
	for (vert = _Network->vertex_head; vert; vert = vert->next)
	{
		if (vert->func)
		{
			if (vert->func(vert, vert->rkf[1], vert->stage_state[1], vert->time + _H0, vert->params))
			{
				__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulate: external error encountered during vertex dynamics call in initial step size calculation.");
                return NDXL_ERR;
			}
		}
		else
			memset(vert->rkf[1], 0, vert->order * sizeof(double));
		for (i = 0; i < vert->order; i++)
		{
			_Scale = _Atol + _Rtol * fmax(fabs(vert->stage_state[1][i]), fabs(vert->state[i]));
			_Yscale = (vert->rkf[1][i] - vert->rkf[0][i]) / _Scale;
			_D2 += (_Yscale * _Yscale);
		}
		vert->stage = 0;
	}

	// Iterate over edges to determine Euler state L2, and reset stage = 0 for integration
	for (edge = _Network->edge_head; edge; edge = edge->next)
	{
		if (edge->func)
		{
			if (edge->func(edge, edge->rkf[1], edge->stage_state[1], edge->time + _H0, edge->params))
			{
				__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulate: external error encountered during edge dynamics call in initial step size calculation.");
                return NDXL_ERR;
			}
		}
		else
			memset(edge->rkf[1], 0, edge->order * sizeof(double));
		for (i = 0; i < edge->order; i++)
		{
			_Scale = _Atol + _Rtol * fmax(fabs(edge->stage_state[1][i]), fabs(edge->state[i]));
			_Yscale = (edge->rkf[1][i] - edge->rkf[0][i]) / _Scale;
			_D2 += (_Yscale * _Yscale);
		}
		edge->stage = 0;
	}

	// Compute L2 norm of Euler step and set network initial step size
	_D2 = sqrt(_D2 / _Network->jacn) / _H0;
	_D0 = fmax(_D1, _D2);
	_H1 = (_D0 <= 1e-15) ? fmax(1e-6, 1e-3 * _H0) : pow(1e-2 / _D0, _OrderInv);
    _Network->h = fmin(100.0 * _H0, _H1);
    return NDXL_OK;
}

const int __ndxlSetNetworkInitialStepStochastic(NDXLNetwork* _Network, const double _Atol, const double _Rtol, const double _OrderInv)
{
	// Algorithm variables
	NDXLVertex* vert;
	NDXLEdge* edge;
	size_t i, j;
	double _Scale, _Yscale, _H0, _H1, _S0;

	// Scaled L2 norms
	double _D0 = 0.0;
	double _D1drift = 0.0, _D1diff = 0.0;
	double _D2drift = 0.0, _D2diff = 0.0;

	// Iterate over vertices to determine state L2 d0 and derivative L2 d1
	for (vert = _Network->vertex_head; vert; vert = vert->next)
	{
		if (vert->func)
		{
			if (vert->func(vert, vert->rkf[0], vert->state, vert->time, vert->params))
			{
				__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulate: external error encountered during vertex dynamics call in initial step size calculation.");
                return NDXL_ERR;
			}
		}
		else
			memset(vert->rkf[0], 0, vert->order * sizeof(double));
		if (vert->noise)
		{
			if (vert->func(vert, vert->rkg[0], vert->state, vert->time, vert->params))
			{
				__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulate: external error encountered during vertex noise call in initial step size calculation.");
                return NDXL_ERR;
			}
		}
		else
			memset(vert->rkg[0], 0, vert->order * vert->m * sizeof(double));
		for (i = 0; i < vert->order; i++)
		{
			_Scale = _Atol + _Rtol * fabs(vert->state[i]);
			_Yscale = vert->state[i] / _Scale;
			_D0 += (_Yscale * _Yscale);
			_Yscale = vert->rkf[0][i] / _Scale;
			_D1drift += (_Yscale * _Yscale);
			for (j = 0; j < vert->m; j++)
			{
				_Yscale = vert->rkg[0][i * vert->m + j] / _Scale;
				_D1diff += (_Yscale * _Yscale);
			}
		}
	}

	// Iterate over edges to determine state L2 d0 and derivative L2 d1
	for (edge = _Network->edge_head; edge; edge = edge->next)
	{
		if (edge->func)
		{
			if (edge->func(edge, edge->rkf[0], edge->state, edge->time, edge->params))
			{
				__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulate: external error encountered during edge dynamics call in initial step size calculation.");
                return NDXL_ERR;
			}
		}
		else
			memset(edge->rkf[0], 0, edge->order * sizeof(double));
		if (edge->noise)
		{
			if (edge->func(edge, edge->rkg[0], edge->state, edge->time, edge->params))
			{
				__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulate: external error encountered during edge noise call in initial step size calculation.");
                return NDXL_ERR;
			}
		}
		else
			memset(edge->rkg[0], 0, edge->order * edge->m * sizeof(double));
		for (i = 0; i < edge->order; i++)
		{
			_Scale = _Atol + _Rtol * fabs(edge->state[i]);
			_Yscale = edge->state[i] / _Scale;
			_D0 += (_Yscale * _Yscale);
			_Yscale = edge->rkf[0][i] / _Scale;
			_D1drift += (_Yscale * _Yscale);
			for (j = 0; j < edge->m; j++)
			{
				_Yscale = edge->rkg[0][i * edge->m + j] / _Scale;
				_D1diff += (_Yscale * _Yscale);
			}
		}
	}

	// Compute d0, d1, and initial step guess h0
	_D0 = sqrt(_D0 / _Network->jacn);
	_D1drift = sqrt(_D1drift / _Network->jacn);
	_D1diff = sqrt(_D1diff / _Network->jacn);
	_H0 = fmin(((_D0 < 1e-5) || (_D1drift < 1e-5)) ? 1e-6 : 1e-2 * (_D0 / _D1drift), ((_D0 < 1e-5) || (_D1diff < 1e-5)) ? 1e-6 : __square(1e-2 * (_D0 / _D1diff)));
	_S0 = sqrt(_H0);

	// Iterate over vertices to compute Euler step, and set stage = 1 for nearest neighbour interactions
	for (vert = _Network->vertex_head; vert; vert = vert->next)
	{
		for (i = 0; i < vert->order; i++)
		{
			vert->stage_state[1][i] = vert->state[i] + _H0 * vert->rkf[0][i];
            vert->rkf[2][i] = 0.0;
			for (j = 0; j < vert->m; j++)
				vert->rkf[2][i] += vert->stage_state[1][i] + _S0 * vert->rkg[0][i * vert->m + j];
		}
		vert->stage = 1;
	}

	// Iterate over edges to compute Euler step, and set stage = 1 for relative state interactions
	for (edge = _Network->edge_head; edge; edge = edge->next)
	{
		for (i = 0; i < edge->order; i++)
		{
			edge->stage_state[1][i] = edge->state[i] + _H0 * edge->rkf[0][i];
            edge->rkf[2][i] = 0.0;
			for (j = 0; j < edge->order; j++)
				edge->rkf[2][i] += edge->stage_state[1][i] + _S0 * edge->rkg[0][i * edge->m + j];
		}
		edge->stage = 1;
	}

	// Iterate over vertices to determine Euler state L2, and reset stage = 0 for integration
	for (vert = _Network->vertex_head; vert; vert = vert->next)
	{
		if (vert->func)
		{
			if (vert->func(vert, vert->rkf[1], vert->stage_state[1], vert->time + _H0, vert->params))
			{
				__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulate: external error encountered during vertex dynamics call in initial step size calculation.");
                return NDXL_ERR;
			}
		}
		else
			memset(vert->rkf[1], 0, vert->order * sizeof(double));
		if (vert->noise)
		{
			if (vert->noise(vert, vert->rkg[1], vert->rkf[2], vert->time + _H0, vert->params))
			{
				__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulate: external error encountered during vertex noise call in initial step size calculation.");
                return NDXL_ERR;
			}
		}
		else
			memset(vert->rkg[1], 0, vert->order * vert->m * sizeof(double));
		for (i = 0; i < vert->order; i++)
		{
			_Scale = _Atol + _Rtol * fmax(fabs(vert->stage_state[1][i]), fabs(vert->state[i]));
			_Yscale = (vert->rkf[1][i] - vert->rkf[0][i]) / _Scale;
			_D2drift += (_Yscale * _Yscale);
			for (j = 0; j < vert->m; j++)
			{
				_Yscale = (vert->rkg[1][i * vert->m + j] - vert->rkg[0][i * vert->m + j]) / _Scale;
				_D2diff += (_Yscale * _Yscale);
			}
		}
		vert->stage = 0;
	}

	// Iterate over edges to determine Euler state L2, and reset stage = 0 for integration
	for (edge = _Network->edge_head; edge; edge = edge->next)
	{
		if (edge->func)
		{
			if (edge->func(edge, edge->rkf[1], edge->stage_state[1], edge->time + _H0, edge->params))
			{
				__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulate: external error encountered during edge dynamics call in initial step size calculation.");
                return NDXL_ERR;
			}
		}
		else
			memset(edge->rkf[1], 0, edge->order * sizeof(double));
		if (edge->noise)
		{
			if (edge->noise(edge, edge->rkg[1], edge->rkf[2], edge->time + _H0, edge->params))
			{
				__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulate: external error encountered during edge noise call in initial step size calculation.");
                return NDXL_ERR;
			}
		}
		else
			memset(edge->rkg[1], 0, edge->order * edge->m * sizeof(double));
		for (i = 0; i < edge->order; i++)
		{
			_Scale = _Atol + _Rtol * fmax(fabs(edge->stage_state[1][i]), fabs(edge->state[i]));
			_Yscale = (edge->rkf[1][i] - edge->rkf[0][i]) / _Scale;
			_D2drift += (_Yscale * _Yscale);
			for (j = 0; j < edge->m; j++)
			{
				_Yscale = (edge->rkg[1][i * edge->m + j] - edge->rkg[0][i * edge->m + j]) / _Scale;
				_D2diff += (_Yscale * _Yscale);
			}
		}
		edge->stage = 0;
	}

	// Compute L2 norm of Euler step and set network initial step size
	_D2drift = sqrt(_D2drift / _Network->jacn) / _H0;
	_D2diff = sqrt(_D2diff / _Network->jacn) / _S0;
	_D0 = fmax(fmax(_D1drift, _D2drift), fmax(__square(_D1diff), __square(_D2diff)));
	_H1 = (_D0 <= 1e-15) ? fmax(1e-6, 1e-3 * _H0) : pow(1e-2 / _D0, _OrderInv);
	_Network->h = fmin(100.0 * _H0, _H1);
    _Network->s = sqrt(_Network->h);
    return NDXL_OK;
}

// ^^^^^ Network setters | network functions vvvvv

const int __ndxlNetworkSimulateRalston(NDXLNetwork* _Network, const double _Time, const double _Step, NDXLNetworkCallback _Callback)
{
	// Ensure there are agents in the network
	if (!_Network->jacn)
	{
		__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRalston: no agents to simulate.");
		return NDXL_ERR;
	}

	// Process network callback at start, if any
	if (_Callback && _Network->time == 0.0)
		if (_Callback(_Network, 0.0, _Network->params))
		{
			__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRalston: external error encountered in network callback.");
			return NDXL_ERR;
		}

	// Setup network parameters
	_Network->h = _Step;
	_Network->butcher = __butcherRalstonC;
	_Network->method_explicit = 1;

	// Setup simulatiuon variables
	const size_t _Steps = (size_t)(_Time / _Step);
	NDXLVertex* vert, *vtmp;
	NDXLEdge* edge, *etmp;

	for (size_t i = 1; i <= _Steps; i++)
	{
		// Calculate time as integer multiple of step to minimise accumulated floating point rounding errors
		const double t = _Network->time + i * _Step;

		// Ralston is a four stage method
		for (char j = 0; j < 4; j++)
		{
			// Tick each vertex RK stage
			for (vert = _Network->vertex_head; vert; vert = vert->next)
				if (__ndxlVertexTickRalston(vert, _Step))
				{
					__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRalston: external error encountered in vertex dynamics call.");
					return NDXL_ERR;
				}

			// Tick each edge RK stage
			for (edge = _Network->edge_head; edge; edge = edge->next)
				if (__ndxlEdgeTickRalston(edge, _Step))
				{
					__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRalston: external error encountered in edge dynamics call.");
					return NDXL_ERR;
				}
		}

		// Update vertex states and resolve callbacks
        vert = _Network->vertex_head;
		while (vert)
		{
            vtmp = vert;
            vert = vert->next;
			if (vtmp->func)
				memcpy(vtmp->state, vtmp->stage_state[0], vtmp->order * sizeof(double));
            vtmp->time += _Step;
			if (__ndxlVertexProcessCallback(vtmp))
			{
				__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRalston: external error encountered in vertex callback.");
				return NDXL_ERR;
			}
		}

		// Update edge states, resolve callbacks, and accumulate vertex states
        edge = _Network->edge_head;
		while (edge)
		{
            etmp = edge;
            edge = edge->next;
			if (etmp->func)
				memcpy(etmp->state, etmp->stage_state[0], etmp->order * sizeof(double));
			__ndxlEdgeAccumulateEffects(etmp, _Step);
			if (__ndxlEdgeProcessCallback(etmp))
			{
				__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRalston: external error encountered in edge callback.");
				return NDXL_ERR;
			}
		}

		// Process network callback, if any
		if (_Callback)
			if (_Callback(_Network, t, _Network->params))
			{
				__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRalston: external error encountered in network callback.");
				return NDXL_ERR;
			}
	}
	_Network->time += (_Steps * _Step);
	return NDXL_OK;
}

const int __ndxlNetworkSimulateNystrom(NDXLNetwork* _Network, const double _Time, const double _Step, NDXLNetworkCallback _Callback)
{
	// Ensure there are agents in the network
	if (!_Network->jacn)
	{
		__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRalston: no agents to simulate.");
		return NDXL_ERR;
	}

	// Process network callback at start, if any
	if (_Callback && _Network->time == 0.0)
		if (_Callback(_Network, 0.0, _Network->params))
		{
			__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateNystrom: external error encountered in network callback.");
			return NDXL_ERR;
		}

	// Setup network parameters
	_Network->h = _Step;
	_Network->butcher = __butcherNystromC;
	_Network->method_explicit = 1;

	// Setup simulatiuon variables
	const size_t _Steps = (size_t)(_Time / _Step);
	NDXLVertex* vert, *vtmp;
	NDXLEdge* edge, *etmp;

	for (size_t i = 1; i <= _Steps; i++)
	{
		// Calculate time as integer multiple of step to minimise accumulated floating point rounding errors
		const double t = _Network->time + i * _Step;

		// Nystrom is a six stage method
		for (char j = 0; j < 6; j++)
		{
			// Tick each vertex RK stage
			for (vert = _Network->vertex_head; vert; vert = vert->next)
				if (__ndxlVertexTickNystrom(vert, _Step))
				{
					__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateNystrom: external error encountered in vertex dynamics call.");
					return NDXL_ERR;
				}

			// Tick each edge RK stage
			for (edge = _Network->edge_head; edge; edge = edge->next)
				if (__ndxlEdgeTickNystrom(edge, _Step))
				{
					__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateNystrom: external error encountered in edge dynamics call.");
					return NDXL_ERR;
				}
		}

        // Update vertex states and resolve callbacks
        vert = _Network->vertex_head;
        while (vert)
        {
            vtmp = vert;
            vert = vert->next;
            if (vtmp->func)
                memcpy(vtmp->state, vtmp->stage_state[0], vtmp->order * sizeof(double));
            vtmp->time += _Step;
            if (__ndxlVertexProcessCallback(vtmp))
            {
                __setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateNystrom: external error encountered in vertex callback.");
                return NDXL_ERR;
            }
        }

        // Update edge states, resolve callbacks, and accumulate vertex states
        edge = _Network->edge_head;
        while (edge)
        {
            etmp = edge;
            edge = edge->next;
            if (etmp->func)
                memcpy(etmp->state, etmp->stage_state[0], etmp->order * sizeof(double));
            __ndxlEdgeAccumulateEffects(etmp, _Step);
            if (__ndxlEdgeProcessCallback(etmp))
            {
                __setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateNystrom: external error encountered in edge callback.");
                return NDXL_ERR;
            }
        }

		// Process network callback, if any
		if (_Callback)
			if (_Callback(_Network, t, _Network->params))
			{
				__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateNystrom: external error encountered in network callback.");
				return NDXL_ERR;
			}
	}
	_Network->time += (_Steps * _Step);
	return NDXL_OK;
}

const int __ndxlNetworkSimulateDopri(NDXLNetwork* _Network, const double _Time, const double _Atol, const double _Rtol, const double* _Tstops, const size_t _Tcount, NDXLNetworkCallback _Callback)
{
	// Ensure there are agents in the network
	if (!_Network->jacn)
	{
		__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRalston: no agents to simulate.");
		return NDXL_ERR;
	}

	// Process callback at start, if any
	if (_Callback && _Network->time == 0.0)
		if (_Callback(_Network, 0.0, _Network->params))
		{
			__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateDopri: external error encountered in network callback.");
			return NDXL_ERR;
		}

	// Setup network parameters
	_Network->butcher = __butcherDopriC;
	_Network->method_explicit = 1;
	if (__ndxlSetNetworkInitialStep(_Network, _Atol, _Rtol, 0.2))
		return NDXL_ERR;

	// Setup simulatiuon variables
	const double _Endtime = _Network->time + _Time;
	double t = _Network->time;
	size_t _Tidx = 0;
	double _Errsum, _Hnew, _Tnext;
	NDXLVertex* vert, *vtmp;
	NDXLEdge* edge, *etmp;

	while (t < _Endtime)
	{
		// Reset step rejection
		char _Rejected = 1;

		// Continue until step is successful
		while (_Rejected)
		{
			// Reset error sum
			_Errsum = 0.0;

			// DOPRI is a seven stage method
			for (char j = 0; j < 7; j++)
			{
				// Tick each vertex RK stage
				for (vert = _Network->vertex_head; vert; vert = vert->next)
					if (__ndxlVertexTickDopri(vert, _Network->h, _Atol, _Rtol, &_Errsum))
					{
						__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateDopri: external error encountered in vertex dynamics call.");
						return NDXL_ERR;
					}

				// Tick each edge RK stage
				for (edge = _Network->edge_head; edge; edge = edge->next)
					if (__ndxlEdgeTickDopri(edge, _Network->h, _Atol, _Rtol, &_Errsum))
					{
						__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateDopri: external error encountered in edge dynamics call.");
						return NDXL_ERR;
					}
			}

			// Global tolerance is based on global max error from all agents
			_Errsum = fmax(sqrt(_Errsum / _Network->jacn), EPS);
			_Hnew = fmax(fmin(0.9 * pow(_Errsum, -0.2), 10.0), 0.2) * _Network->h;

			// Check step success
			if (_Errsum >= 1.0)
				_Network->h = _Hnew;
			else
			{
				t += _Network->h;
				_Rejected = 0;
			}
		}

        // Update vertex states and resolve callbacks
        vert = _Network->vertex_head;
        while (vert)
        {
            vtmp = vert;
            vert = vert->next;
            if (vtmp->func)
                memcpy(vtmp->state, vtmp->stage_state[0], vtmp->order * sizeof(double));
            vtmp->time += _Network->h;
            if (__ndxlVertexProcessCallback(vtmp))
            {
                __setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateDopri: external error encountered in vertex callback.");
                return NDXL_ERR;
            }
        }

        // Update edge states, resolve callbacks, and accumulate vertex states
        edge = _Network->edge_head;
        while (edge)
        {
            etmp = edge;
            edge = edge->next;
            if (etmp->func)
                memcpy(etmp->state, etmp->stage_state[0], etmp->order * sizeof(double));
            __ndxlEdgeAccumulateEffects(etmp, _Network->h);
            if (__ndxlEdgeProcessCallback(etmp))
            {
                __setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateDopri: external error encountered in edge callback.");
                return NDXL_ERR;
            }
        }

		// Check for time stops/end of simulation time and adapt h accordingly
		_Tnext = t + _Hnew;
		if ((_Tidx < _Tcount) && (t >= _Tstops[_Tidx]))
			_Tidx++;
		if ((_Tidx < _Tcount) && (_Tnext >= _Tstops[_Tidx]))
			_Hnew = _Tstops[_Tidx] - t;
		if (t + _Hnew > _Endtime)
			_Hnew = _Endtime - t;
		_Network->h = _Hnew;

		// Process callback, if any
		if (_Callback)
			if (_Callback(_Network, t, _Network->params))
			{
				__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateDopri: external error encountered in network callback.");
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
		__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRalston: no agents to simulate.");
		return NDXL_ERR;
	}

	// Process callback at start, if any
	if (_Callback && _Network->time == 0.0)
		if (_Callback(_Network, 0.0, _Network->params))
		{
			__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRkck: external error encountered in network callback.");
			return NDXL_ERR;
		}

	// Setup network parameters
	_Network->butcher = __butcherCashKarpC;
	_Network->method_explicit = 1;
	if (__ndxlSetNetworkInitialStep(_Network, _Atol, _Rtol, 0.2))
		return NDXL_ERR;

	// Setup simulatiuon variables
	const double _Endtime = _Network->time + _Time;
	double t = _Network->time;
	size_t _Tidx = 0;
	double _Errsum, _Hnew, _Tnext;
	NDXLVertex* vert, *vtmp;
	NDXLEdge* edge, *etmp;

	while (t < _Endtime)
	{
		// Reset step rejection
		char _Rejected = 1;

		// Continue until step is successful
		while (_Rejected)
		{
			// Reset error sum
			_Errsum = 0.0;

			// RKCK is a six stage method
			for (char j = 0; j < 6; j++)
			{
				// Tick each vertex RK stage
				for (vert = _Network->vertex_head; vert; vert = vert->next)
					if (__ndxlVertexTickRkck(vert, _Network->h, _Atol, _Rtol, &_Errsum))
					{
						__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRkck: external error encountered in vertex dynamics call.");
						return NDXL_ERR;
					}

				// Tick each edge RK stage
				for (edge = _Network->edge_head; edge; edge = edge->next)
					if (__ndxlEdgeTickRkck(edge, _Network->h, _Atol, _Rtol, &_Errsum))
					{
						__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRkck: external error encountered in edge dynamics call.");
						return NDXL_ERR;
					}
			}

			// Global tolerance is based on global max error from all agents
			_Errsum = sqrt(_Errsum / _Network->jacn);
			_Hnew = fmax(fmin(0.9 * pow(_Errsum, -0.2), 10.0), 0.2) * _Network->h;

			// Check step success
			if (_Errsum >= 1.0)
				_Network->h = _Hnew;
			else
			{
				t += _Network->h;
				_Rejected = 0;
			}
		}

        // Update vertex states and resolve callbacks
        vert = _Network->vertex_head;
        while (vert)
        {
            vtmp = vert;
            vert = vert->next;
            if (vtmp->func)
                memcpy(vtmp->state, vtmp->stage_state[0], vtmp->order * sizeof(double));
            vtmp->time += _Network->h;
            if (__ndxlVertexProcessCallback(vtmp))
            {
                __setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRkck: external error encountered in vertex callback.");
                return NDXL_ERR;
            }
        }

        // Update edge states, resolve callbacks, and accumulate vertex states
        edge = _Network->edge_head;
        while (edge)
        {
            etmp = edge;
            edge = edge->next;
            if (etmp->func)
                memcpy(etmp->state, etmp->stage_state[0], etmp->order * sizeof(double));
            __ndxlEdgeAccumulateEffects(etmp, _Network->h);
            if (__ndxlEdgeProcessCallback(etmp))
            {
                __setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRkck: external error encountered in edge callback.");
                return NDXL_ERR;
            }
        }

		// Check for time stops/end of simulation time and adapt h accordingly
		_Tnext = t + _Hnew;
		if ((_Tidx < _Tcount) && (t >= _Tstops[_Tidx]))
			_Tidx++;
		if ((_Tidx < _Tcount) && (_Tnext >= _Tstops[_Tidx]))
			_Hnew = _Tstops[_Tidx] - t;
		if (t + _Hnew > _Endtime)
			_Hnew = _Endtime - t;
		_Network->h = _Hnew;

		// Process callback, if any
		if (_Callback)
			if (_Callback(_Network, t, _Network->params))
			{
				__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRkck: external error encountered in network callback.");
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
		__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRalston: no agents to simulate.");
		return NDXL_ERR;
	}

	// Process network callback at start, if any
	if (_Callback && _Network->time == 0.0)
		if (_Callback(_Network, 0.0, _Network->params))
		{
			__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateEuler: external error encountered in network callback.");
			return NDXL_ERR;
		}

	// Setup network parameters
	_Network->h = _Step;
	_Network->s = sqrt(_Step);
	_Network->butcher = 0;
	_Network->method_explicit = 1;

	// Setup simulatiuon variables
	const size_t _Steps = (size_t)(_Time / _Step);
	NDXLVertex* vert, *vtmp;
	NDXLEdge* edge, *etmp;
	NDXLChannel* channel;
	NDXLVertexChannel* vchannel;
	NDXLEdgeChannel* echannel;
	size_t _Pidx;

	for (size_t i = 1; i <= _Steps; i++)
	{
		// Calculate time as integer multiple of step to minimise accumulated floating point rounding errors
		const double t = _Network->time + i * _Step;

		// Redraw Wiener incrememts
		_Pidx = 0;
		for (channel = _Network->channel_head; channel; _Pidx++, channel = channel->next)
			__ndxlChannelRedraw(channel);

		// Heun-Euler is a two stage method
		for (char j = 0; j < 2; j++)
		{
			// Tick each vertex RK stage
			for (vert = _Network->vertex_head; vert; vert = vert->next)
				if (__ndxlVertexTickEuler(vert, _Network->h))
				{
					__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateEuler: external error encountered in vertex dynamics/noise call.");
					return NDXL_ERR;
				}

			// Tick each edge RK stage
			for (edge = _Network->edge_head; edge; edge = edge->next)
				if (__ndxlEdgeTickEuler(edge, _Network->h))
				{
					__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateEuler: external error encountered in edge dynamics/noise call.");
					return NDXL_ERR;
				}
		}

        // Update vertex states and resolve callbacks
        vert = _Network->vertex_head;
        while (vert)
        {
            vtmp = vert;
            vert = vert->next;
            if (vtmp->func)
                memcpy(vtmp->state, vtmp->stage_state[0], vtmp->order * sizeof(double));
            vtmp->time += _Step;
            if (__ndxlVertexProcessCallback(vtmp))
            {
                __setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateEuler: external error encountered in vertex callback.");
                return NDXL_ERR;
            }
        }

        // Update edge states, resolve callbacks, and accumulate vertex states
        edge = _Network->edge_head;
        while (edge)
        {
            etmp = edge;
            edge = edge->next;
            if (etmp->func)
                memcpy(etmp->state, etmp->stage_state[0], etmp->order * sizeof(double));
            __ndxlEdgeAccumulateEffects(etmp, _Step);
            if (__ndxlEdgeProcessCallback(etmp))
            {
                __setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateEuler: external error encountered in edge callback.");
                return NDXL_ERR;
            }
        }

		// Process network callback, if any
		if (_Callback)
			if (_Callback(_Network, t, _Network->params))
			{
				__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateEuler: external error encountered in network callback.");
				return NDXL_ERR;
			}
	}
	_Network->time += (_Steps * _Step);
	return NDXL_OK;
}

const int __ndxlNetworkSimulateHeun(NDXLNetwork* _Network, const double _Time, const double _Atol, const double _Rtol, const double* _Tstops, const size_t _Tcount, NDXLNetworkCallback _Callback)
{
	// Ensure there are agents in the network
	if (!_Network->jacn)
	{
		__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRalston: no agents to simulate.");
		return NDXL_ERR;
	}

	// Process callback at start, if any
	if (_Callback && _Network->time == 0.0)
		if (_Callback(_Network, 0.0, _Network->params))
		{
			__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateHeun: external error encountered in network callback.");
			return NDXL_ERR;
		}

	// Setup network parameters
	_Network->butcher = 0;
	_Network->method_explicit = 1;
	if (__ndxlSetNetworkInitialStepStochastic(_Network, _Atol, _Rtol, 1.0))
		return NDXL_ERR;

	// Setup simulatiuon variables
	const double _Endtime = _Network->time + _Time;
	double t = _Network->time;
	size_t _Tidx = 0;
	double _Errsum, _Hnew, _Tnext;
	NDXLVertex* vert, *vtmp;
	NDXLEdge* edge, *etmp;
	NDXLChannel* channel;

	while (t < _Endtime)
	{
		// Reset step rejection and obtain Wiener increments
		char _Rejected = 1;
		for (channel = _Network->channel_head; channel; channel = channel->next)
			_Network->h = __ndxlChannelRedraw(channel);

		// Continue until step is successful
		while (_Rejected)
		{
			// Reset error sum and Wiener scalar
			_Errsum = 0.0;

			// Heun-Euler is a two stage method
			for (char j = 0; j < 2; j++)
			{
				// Tick each vertex RK stage
				for (vert = _Network->vertex_head; vert; vert = vert->next)
					if (__ndxlVertexTickHeun(vert, _Network->h, _Atol, _Rtol, &_Errsum))
					{
						__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateHeun: external error encountered in vertex dynamics/noise call.");
						return NDXL_ERR;
					}

				// Tick each edge RK stage
				for (edge = _Network->edge_head; edge; edge = edge->next)
					if (__ndxlEdgeTickHeun(edge, _Network->h, _Atol, _Rtol, &_Errsum))
					{
						__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateHeun: external error encountered in edge dynamics/noise call.");
						return NDXL_ERR;
					}
			}

			// Global tolerance is based on global max error from all agents
			_Errsum = sqrt(_Errsum / _Network->jacn);
			_Hnew = fmax(fmin(0.9 * _Errsum, 10.0), 0.2) * _Network->h;

			// Check step success
			if (_Errsum < 1.0)
			{
				for (channel = _Network->channel_head; channel; channel = channel->next)
					__ndxlCreateChannelIncrement(channel, _Network->h, _Hnew);
				_Network->h = _Hnew;
			}
			else
			{
				t += _Network->h;
				_Rejected = 0;
			}
		}

        // Update vertex states and resolve callbacks
        vert = _Network->vertex_head;
        while (vert)
        {
            vtmp = vert;
            vert = vert->next;
            if (vtmp->func)
                memcpy(vtmp->state, vtmp->stage_state[0], vtmp->order * sizeof(double));
            vtmp->time += _Network->h;
            if (__ndxlVertexProcessCallback(vtmp))
            {
                __setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateHeun: external error encountered in vertex callback.");
                return NDXL_ERR;
            }
        }

        // Update edge states, resolve callbacks, and accumulate vertex states
        edge = _Network->edge_head;
        while (edge)
        {
            etmp = edge;
            edge = edge->next;
            if (etmp->func)
                memcpy(etmp->state, etmp->stage_state[0], etmp->order * sizeof(double));
            __ndxlEdgeAccumulateEffects(etmp, _Network->h);
            if (__ndxlEdgeProcessCallback(etmp))
            {
                __setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateHeun: external error encountered in edge callback.");
                return NDXL_ERR;
            }
        }

		// Check for time stops/end of simulation time and adapt h accordingly
		_Tnext = t + _Hnew;
		if ((_Tidx < _Tcount) && (t >= _Tstops[_Tidx]))
			_Tidx++;
		if ((_Tidx < _Tcount) && (_Tnext >= _Tstops[_Tidx]))
			_Hnew = _Tstops[_Tidx] - t;
		if (t + _Hnew > _Endtime)
			_Hnew = _Endtime - t;
		_Network->h = _Hnew;
        _Network->s = sqrt(_Network->h);

		// Process callback, if any
		if (_Callback)
			if (_Callback(_Network, t, _Network->params))
			{
				__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateHeun: external error encountered in network callback.");
				return NDXL_ERR;
			}
	}
	_Network->time = _Endtime;
	return NDXL_OK;
}

const int __ndxlNetworkSimulateNorsett(NDXLNetwork* _Network, const double _Time, const double _Step, NDXLNetworkCallback _Callback)
{
	__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateNorsett: method not implemented.");
	return NDXL_ERR;
}

const int __ndxlNetworkSimulateRadau(NDXLNetwork* _Network, const double _Time, const double _Atol, const double _Rtol, const double* _Tstops, const size_t _Tcount, NDXLNetworkCallback _Callback)
{
	// Ensure there are agents in the network
	if (!_Network->jacn)
	{
		__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRalston: no agents to simulate.");
		return NDXL_ERR;
	}

	// Process callback at start, if any
	if (_Callback && _Network->time == 0.0)
		if (_Callback(_Network, 0.0, _Network->params))
		{
			__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRadau: external error encountered in network callback.");
			return NDXL_ERR;
		}

	// Setup network parameters
	_Network->butcher = __butcherRadauC;
	_Network->method_explicit = 0;
	if (__ndxlSetNetworkInitialStep(_Network, _Atol, _Rtol, 0.25))
		return NDXL_ERR;

	// Setup simulatiuon variables
	const double _Endtime = _Network->time + _Time;
	double t = _Network->time;
	size_t _Tidx = 0;
	size_t _Aidx, _Datsz;
	double _Dk[3], _Dy, _Err, _Errsum, _Hnew, _Tnext;
	char NEWTON_ITER;
	NDXLVertex* vert, *vtmp;
	NDXLEdge* edge, *etmp;

	while (t < _Endtime)
	{
		// Reset step rejection and define method as explicit for vertex nearest neighbour and edge relative state interactions
		char _Rejected = 1;
		_Network->method_explicit = 1;

		// Prepare Jacobian and network vectors
		if (_Network->jacn != _Network->jacn_old)
		{
			_Network->jacn_old = _Network->jacn;
			_Network->jac = (double*)realloc(_Network->jac, _Network->jacn * _Network->jacn * sizeof(double));
			_Network->mlhs_real = (double*)realloc(_Network->mlhs_real, _Network->jacn * _Network->jacn * sizeof(double));
			_Network->mrhs_real = (double*)realloc(_Network->mrhs_real, _Network->jacn * sizeof(double));
			_Network->decoup_real = (double*)realloc(_Network->decoup_real, _Network->jacn * sizeof(double));
			_Network->mlhs_imag = (complex_t*)realloc(_Network->mlhs_imag, _Network->jacn * _Network->jacn * sizeof(complex_t));
			_Network->mrhs_imag = (complex_t*)realloc(_Network->mrhs_imag, _Network->jacn * sizeof(complex_t));
			_Network->decoup_imag = (complex_t*)realloc(_Network->decoup_imag, _Network->jacn * sizeof(complex_t));
			_Network->erhs = (double*)realloc(_Network->erhs, _Network->jacn * sizeof(double));
			_Network->errloc = (double*)realloc(_Network->errloc, _Network->jacn * sizeof(double));
			_Network->pivots[0] = (size_t*)realloc(_Network->pivots[0], (_Network->jacn + 1) * sizeof(size_t));
			_Network->pivots[1] = (size_t*)realloc(_Network->pivots[1], (_Network->jacn + 1) * sizeof(size_t));
		}
		if (__ndxlNetworkJacobian(_Network))
		{
			__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRadau: external error encountered in obtaining network Jacobian.");
			return NDXL_SIM_ERR;
		}

		// Compute f(y, t) and store in stage_state[0]
		for (vert = _Network->vertex_head; vert; vert = vert->next)
		{
			if (vert->func)
			{
				if (vert->func(vert, vert->stage_state[0], vert->state, vert->time, vert->params))
				{
					__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRadau: external error encountered in vertex dynamics call.");
					return NDXL_SIM_ERR;
				}
			}
			else
				memset(vert->stage_state[0], 0, vert->order * sizeof(double));
		}
		for (edge = _Network->edge_head; edge; edge = edge->next)
		{
			if (edge->func)
			{
				if (edge->func(edge, edge->stage_state[0], edge->state, edge->time, edge->params))
				{
					__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRadau: external error encountered in edge dynamics call.");
					return NDXL_SIM_ERR;
				}
			}
			else
				memset(edge->stage_state[0], 0, edge->order * sizeof(double));
		}
		_Network->method_explicit = 0;

		while (_Rejected)
		{
			// PLU-decompose real and complex subsystems once since LHS does not change during Newton-Raphson iteration
			for (size_t i = 0; i < _Network->jacn; i++)
			{
				const size_t _Jrow = i * _Network->jacn;
				for (size_t j = 0; j < _Network->jacn; j++)
				{
					const size_t _Jidx = _Jrow + j;
					_Network->mlhs_real[_Jidx] = (i == j) - _Network->h * __radauEig[0] * _Network->jac[_Jidx];
					_Network->mlhs_imag[_Jidx] = uniComplex((i == j) - _Network->h * __radauEig[1] * _Network->jac[_Jidx], -_Network->h * __radauEig[2] * _Network->jac[_Jidx]);
				}
			}
			__pluDecompose(_Network->mlhs_real, _Network->pivots[0], _Network->jacn);
			__pluDecomposeZ(_Network->mlhs_imag, _Network->pivots[1], _Network->jacn);

			// Initial guesses for stage derivatives rkf[3]..rkf[5] -> k0..k2; initial guess of f(y, t)
			_Network->method_explicit = 1;
			for (vert = _Network->vertex_head; vert; vert = vert->next)
			{
				_Datsz = vert->order * sizeof(double);
				memcpy(vert->rkf[3], vert->stage_state[0], _Datsz);
				memcpy(vert->rkf[4], vert->stage_state[0], _Datsz);
				memcpy(vert->rkf[5], vert->stage_state[0], _Datsz);
			}
			for (edge = _Network->edge_head; edge; edge = edge->next)
			{
				_Datsz = edge->order * sizeof(double);
				memcpy(edge->rkf[3], edge->stage_state[0], _Datsz);
				memcpy(edge->rkf[4], edge->stage_state[0], _Datsz);
				memcpy(edge->rkf[5], edge->stage_state[0], _Datsz);
			}
			_Network->method_explicit = 0;

			// Newton-Raphson iteration to solve stage derivatives
			char _Newton = 0;
			for (NEWTON_ITER = 0; NEWTON_ITER < ITERMAX; NEWTON_ITER++)
			{
				// Compute stage equations (rkf[0]..rkf[2] -> Y0..Y2)
				for (vert = _Network->vertex_head; vert; vert = vert->next)
				{
					for (size_t i = 0; i < vert->order; i++)
					{
						vert->rkf[0][i] = vert->state[i] + _Network->h * (__butcherRadauA[0] * vert->rkf[3][i] + __butcherRadauA[1] * vert->rkf[4][i] + __butcherRadauA[2] * vert->rkf[5][i]);
						vert->rkf[1][i] = vert->state[i] + _Network->h * (__butcherRadauA[3] * vert->rkf[3][i] + __butcherRadauA[4] * vert->rkf[4][i] + __butcherRadauA[5] * vert->rkf[5][i]);
						vert->rkf[2][i] = vert->state[i] + _Network->h * (__butcherRadauA[6] * vert->rkf[3][i] + __butcherRadauA[7] * vert->rkf[4][i] + __butcherRadauA[8] * vert->rkf[5][i]);
					}
				}
				for (edge = _Network->edge_head; edge; edge = edge->next)
				{
					for (size_t i = 0; i < edge->order; i++)
					{
						edge->rkf[0][i] = edge->state[i] + _Network->h * (__butcherRadauA[0] * edge->rkf[3][i] + __butcherRadauA[1] * edge->rkf[4][i] + __butcherRadauA[2] * edge->rkf[5][i]);
						edge->rkf[1][i] = edge->state[i] + _Network->h * (__butcherRadauA[3] * edge->rkf[3][i] + __butcherRadauA[4] * edge->rkf[4][i] + __butcherRadauA[5] * edge->rkf[5][i]);
						edge->rkf[2][i] = edge->state[i] + _Network->h * (__butcherRadauA[6] * edge->rkf[3][i] + __butcherRadauA[7] * edge->rkf[4][i] + __butcherRadauA[8] * edge->rkf[5][i]);
					}
				}

				// Compute residuals (rkf[6]..rkf[8] -> R0..R2)
				for (vert = _Network->vertex_head; vert; vert = vert->next)
				{
					if (vert->func)
					{
						if (vert->func(vert, vert->rkf[6], vert->rkf[0], vert->time + __butcherRadauC[0] * _Network->h, vert->params))
						{
							__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRadau: external error encountered in vertex dynamics call.");
							return NDXL_SIM_ERR;
						}
						vert->stage = 1;
						if (vert->func(vert, vert->rkf[7], vert->rkf[1], vert->time + __butcherRadauC[1] * _Network->h, vert->params))
						{
							__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRadau: external error encountered in vertex dynamics call.");
							return NDXL_SIM_ERR;
						}
						vert->stage = 2;
						if (vert->func(vert, vert->rkf[8], vert->rkf[2], vert->time + __butcherRadauC[2] * _Network->h, vert->params))
						{
							__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRadau: external error encountered in vertex dynamics call.");
							return NDXL_SIM_ERR;
						}
						vert->stage = 0;
						for (size_t i = 0; i < vert->order; i++)
						{
							vert->rkf[6][i] -= vert->rkf[3][i];
							vert->rkf[7][i] -= vert->rkf[4][i];
							vert->rkf[8][i] -= vert->rkf[5][i];
						}
					}
					else
					{
						memset(vert->rkf[6], 0, _Datsz);
						memset(vert->rkf[7], 0, _Datsz);
						memset(vert->rkf[8], 0, _Datsz);
					}
				}
				for (edge = _Network->edge_head; edge; edge = edge->next)
				{
					if (edge->func)
					{
						if (edge->func(edge, edge->rkf[6], edge->rkf[0], edge->time + __butcherRadauC[0] * _Network->h, edge->params))
						{
							__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRadau: external error encountered in vertex dynamics call.");
							return NDXL_SIM_ERR;
						}
						edge->stage = 1;
						if (edge->func(edge, edge->rkf[7], edge->rkf[1], edge->time + __butcherRadauC[1] * _Network->h, edge->params))
						{
							__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRadau: external error encountered in vertex dynamics call.");
							return NDXL_SIM_ERR;
						}
						edge->stage = 2;
						if (edge->func(edge, edge->rkf[8], edge->rkf[2], edge->time + __butcherRadauC[2] * _Network->h, edge->params))
						{
							__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRadau: external error encountered in vertex dynamics call.");
							return NDXL_SIM_ERR;
						}
						edge->stage = 0;
						for (size_t i = 0; i < edge->order; i++)
						{
							edge->rkf[6][i] -= edge->rkf[3][i];
							edge->rkf[7][i] -= edge->rkf[4][i];
							edge->rkf[8][i] -= edge->rkf[5][i];
						}
					}
					else
					{
						memset(edge->rkf[6], 0, _Datsz);
						memset(edge->rkf[7], 0, _Datsz);
						memset(edge->rkf[8], 0, _Datsz);
					}
				}

				// Populate right hand side intermediate vectors
				_Aidx = 0;
				for (vert = _Network->vertex_head; vert; _Aidx += vert->order, vert = vert->next)
				{
					for (size_t i = 0; i < vert->order; i++)
					{
						_Network->mrhs_real[_Aidx + i] = __radauInv[0] * vert->rkf[6][i] + __radauInv[1] * vert->rkf[7][i] + __radauInv[2] * vert->rkf[8][i];
						_Network->mrhs_imag[_Aidx + i] = uniComplex(__radauInv[3] * vert->rkf[6][i] + __radauInv[4] * vert->rkf[7][i] + __radauInv[5] * vert->rkf[8][i], __radauInv[6] * vert->rkf[6][i] + __radauInv[7] * vert->rkf[7][i] + __radauInv[8] * vert->rkf[8][i]);
					}
				}
				for (edge = _Network->edge_head; edge; _Aidx += edge->order, edge = edge->next)
				{
					for (size_t i = 0; i < edge->order; i++)
					{
						_Network->mrhs_real[_Aidx + i] = __radauInv[0] * edge->rkf[6][i] + __radauInv[1] * edge->rkf[7][i] + __radauInv[2] * edge->rkf[8][i];
						_Network->mrhs_imag[_Aidx + i] = uniComplex(__radauInv[3] * edge->rkf[6][i] + __radauInv[4] * edge->rkf[7][i] + __radauInv[5] * edge->rkf[8][i], __radauInv[6] * edge->rkf[6][i] + __radauInv[7] * edge->rkf[7][i] + __radauInv[8] * edge->rkf[8][i]);
					}
				}

				// PLU-solve decoupled increments
				__pluSolve(_Network->decoup_real, _Network->mlhs_real, _Network->mrhs_real, _Network->pivots[0], _Network->jacn);
				__pluSolveZ(_Network->decoup_imag, _Network->mlhs_imag, _Network->mrhs_imag, _Network->pivots[1], _Network->jacn);

				// Calculate stage derivative increments and L2 error norm
				_Aidx = 0;
				_Errsum = 0.0;
				for (vert = _Network->vertex_head; vert; _Aidx += vert->order, vert = vert->next)
				{
					for (size_t i = 0; i < vert->order; i++)
					{
						_Dk[0] = __radauMat[0] * _Network->decoup_real[_Aidx + i] + __radauMat[1] * _Network->decoup_imag[_Aidx + i].real + __radauMat[2] * _Network->decoup_imag[_Aidx + i].imag;
						_Dk[1] = __radauMat[3] * _Network->decoup_real[_Aidx + i] + __radauMat[4] * _Network->decoup_imag[_Aidx + i].real + __radauMat[5] * _Network->decoup_imag[_Aidx + i].imag;
						_Dk[2] = __radauMat[6] * _Network->decoup_real[_Aidx + i] + __radauMat[7] * _Network->decoup_imag[_Aidx + i].real + __radauMat[8] * _Network->decoup_imag[_Aidx + i].imag;
						_Dy = _Network->h * (__butcherRadauA[6] * _Dk[0] + __butcherRadauA[7] * _Dk[1] + __butcherRadauA[8] * _Dk[2]);
						_Err = _Dy / (1e-6 + 1e-3 * fmax(fabs(vert->state[i]), fabs(vert->rkf[2][i])));
						_Errsum += (_Err * _Err);
						vert->rkf[3][i] += _Dk[0];
						vert->rkf[4][i] += _Dk[1];
						vert->rkf[5][i] += _Dk[2];
					}
				}
				for (edge = _Network->edge_head; edge; _Aidx += edge->order, edge = edge->next)
				{
					for (size_t i = 0; i < edge->order; i++)
					{
						_Dk[0] = __radauMat[0] * _Network->decoup_real[_Aidx + i] + __radauMat[1] * _Network->decoup_imag[_Aidx + i].real + __radauMat[2] * _Network->decoup_imag[_Aidx + i].imag;
						_Dk[1] = __radauMat[3] * _Network->decoup_real[_Aidx + i] + __radauMat[4] * _Network->decoup_imag[_Aidx + i].real + __radauMat[5] * _Network->decoup_imag[_Aidx + i].imag;
						_Dk[2] = __radauMat[6] * _Network->decoup_real[_Aidx + i] + __radauMat[7] * _Network->decoup_imag[_Aidx + i].real + __radauMat[8] * _Network->decoup_imag[_Aidx + i].imag;
						_Dy = _Network->h * (__butcherRadauA[6] * _Dk[0] + __butcherRadauA[7] * _Dk[1] + __butcherRadauA[8] * _Dk[2]);
						_Err = _Dy / (1e-6 + 1e-3 * fmax(fabs(edge->state[i]), fabs(edge->rkf[2][i])));
						_Errsum += (_Err * _Err);
						edge->rkf[3][i] += _Dk[0];
						edge->rkf[4][i] += _Dk[1];
						edge->rkf[5][i] += _Dk[2];
					}
				}

				// Check for convergence
				_Err = sqrt(_Errsum / _Network->jacn);
				if (_Err < 1.0)
				{
					_Newton = 1;
					break;
				}
			}

			// If Newton-Raphson didn't converge, halve step size and try again
			if (!_Newton)
				_Network->h *= 0.5;
			else
			{
				// PLU-solve error estimator
				_Aidx = 0;
				for (vert = _Network->vertex_head; vert; _Aidx += vert->order, vert = vert->next)
					for (size_t i = 0; i < vert->order; i++)
						_Network->erhs[_Aidx + i] = _Network->h * vert->stage_state[0][i] + __radauErr[0] * (vert->rkf[0][i] - vert->state[i]) + __radauErr[1] * (vert->rkf[1][i] - vert->state[i]) + __radauErr[2] * (vert->rkf[2][i] - vert->state[i]);
				for (edge = _Network->edge_head; edge; _Aidx += edge->order, edge = edge->next)
					for (size_t i = 0; i < edge->order; i++)
						_Network->erhs[_Aidx + i] = _Network->h * edge->stage_state[0][i] + __radauErr[0] * (edge->rkf[0][i] - edge->state[i]) + __radauErr[1] * (edge->rkf[1][i] - edge->state[i]) + __radauErr[2] * (edge->rkf[2][i] - edge->state[i]);
				__pluSolve(_Network->errloc, _Network->mlhs_real, _Network->erhs, _Network->pivots[0], _Network->jacn);

				// Compute L2 error norm
				_Aidx = 0;
				_Errsum = 0.0;
				for (vert = _Network->vertex_head; vert; _Aidx += vert->order, vert = vert->next)
				{
					for (size_t i = 0; i < vert->order; i++)
					{
						_Err = (__radauEig[0] * _Network->errloc[_Aidx + i]) / (_Atol + _Rtol * fmax(fabs(vert->state[i]), fabs(vert->rkf[2][i])));
						_Errsum += (_Err * _Err);
					}
				}
				for (edge = _Network->edge_head; edge; _Aidx += edge->order, edge = edge->next)
				{
					for (size_t i = 0; i < edge->order; i++)
					{
						_Err = (__radauEig[0] * _Network->errloc[_Aidx + i]) / (_Atol + _Rtol * fmax(fabs(edge->state[i]), fabs(edge->rkf[2][i])));
						_Errsum += (_Err * _Err);
					}
				}

				// Predict step size and check error convergence
				_Err = sqrt(_Errsum / _Network->jacn);
				_Hnew = fmin(fmax(0.9 * pow(_Err, -0.25), 0.2), 10.0) * _Network->h;

				// Check step success
				if (_Err >= 1.0)
					_Network->h = _Hnew;
				else
				{
					t += _Network->h;
					_Rejected = 0;
				}
			}
		}

		// Update vertex state vectors
        vert = _Network->vertex_head;
		while (vert)
		{
            vtmp = vert;
            vert = vert->next;
			if (vtmp->func)
			{
				memcpy(vtmp->state, vtmp->rkf[2], vtmp->order * sizeof(double));
				memcpy(vtmp->stage_state[0], vtmp->rkf[2], vtmp->order * sizeof(double));
			}
            vtmp->time += _Network->h;
			if (__ndxlVertexProcessCallback(vtmp))
			{
				__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRadau: external error encountered in vertex callback.");
				return NDXL_ERR;
			}
		}

		// Update edge state vectors
        edge = _Network->edge_head;
		while (edge)
		{
            etmp = edge;
            edge = edge->next;
			if (etmp->func)
			{
				memcpy(etmp->state, etmp->rkf[2], etmp->order * sizeof(double));
				memcpy(etmp->stage_state[0], etmp->rkf[2], etmp->order * sizeof(double));
			}
			__ndxlEdgeAccumulateEffects(etmp, _Network->h);
			if (__ndxlEdgeProcessCallback(etmp))
			{
				__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRadau: external error encountered in edge callback.");
				return NDXL_ERR;
			}
		}

		// Check for time stops/end of simulation time and adapt h accordingly
		_Tnext = t + _Hnew;
		if ((_Tidx < _Tcount) && (t >= _Tstops[_Tidx]))
			_Tidx++;
		if ((_Tidx < _Tcount) && (_Tnext >= _Tstops[_Tidx]))
			_Hnew = _Tstops[_Tidx] - t;
		if (t + _Hnew > _Endtime)
			_Hnew = _Endtime - t;
		_Network->h = _Hnew;

		// Process callback, if any
		if (_Callback)
			if (_Callback(_Network, t, _Network->params))
			{
				__setLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRadau: external error encountered in network callback.");
				return NDXL_ERR;
			}
	}
	_Network->time = _Endtime;
	return NDXL_OK;
}

// ^^^^^ Network functions
