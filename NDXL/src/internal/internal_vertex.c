//--------------------------------------\\
// NDXL source file for vertex class	\\
// Copyright Ethan Codron 2026			\\
// GNU GPL-3.0							\\
//--------------------------------------\\
// Source file providing all vertex		\\
// functions used internally by the		\\
// API.									\\
//--------------------------------------\\

#include "internal.h"

// Vertex constructors and destructor vvvvv

NDXLVertexChannel* __ndxlCreateVertexChannel(NDXLVertex* _Vertex, NDXLChannel* _Channel)
{
	NDXLVertexChannel* vchannel = (NDXLVertexChannel*)malloc(sizeof(NDXLVertexChannel));
	vchannel->channel = _Channel;
	vchannel->channel->nv++;
	vchannel->prev = _Vertex->ctail;
	vchannel->next = 0;
	if (_Vertex->chead)
		_Vertex->ctail->next = vchannel;
	else
		_Vertex->chead = vchannel;
	_Vertex->ctail = vchannel;
	return vchannel;
}

void __ndxlDestroyVertexChannel(NDXLVertexChannel* _Vchannel)
{
	_Vchannel->channel->nv--;
	if (_Vchannel->prev)
		_Vchannel->prev->next = _Vchannel->next;
	else
		_Vchannel->cvert->vertex->chead = _Vchannel->cvert->vertex->chead->next;
	if (_Vchannel->next)
		_Vchannel->next->prev = _Vchannel->prev;
	else
		_Vchannel->cvert->vertex->ctail = _Vchannel->cvert->vertex->ctail->prev;
	free(_Vchannel);
}

NDXLVertex* __ndxlCreateVertex(NDXLNetwork* _Network, const double* _State, const size_t _Order, NDXLVertexDynamics _Func, NDXLVertexNoise _Noise, NDXLChannel** _Channels, const size_t _Nchannels, NDXLVertexCallback _Callback, void* _Params)
{
	const size_t _Datsz = _Order * sizeof(double);

	// Initialise explicit values
	NDXLVertex* vert = (NDXLVertex*)malloc(sizeof(NDXLVertex));
	vert->network = _Network;
	if (_Noise)
		vert->network->nsv++;
	else
		vert->network->ndv++;
	vert->order = _Order;
	vert->state = (double*)malloc(_Datsz);
	vert->func = _Func;
	vert->noise = _Noise;
	vert->m = ((_Noise && !_Nchannels) ? _Order : _Nchannels);
	vert->callback = _Callback;
	vert->params = _Params;

	// Initialise implicit values
	vert->network->jacn += _Order;
	vert->state0 = (double*)malloc(_Datsz);
	vert->stage_state[0] = (double*)malloc(_Datsz);
	vert->stage_state[1] = (double*)malloc(_Datsz);
	vert->null_state = (double*)calloc(_Order, sizeof(double));
	vert->interp_state = (double*)malloc(_Datsz);
	vert->stage = 0;
	vert->rkf[0] = (double*)malloc(_Datsz);
	vert->rkf[1] = (double*)malloc(_Datsz);
	vert->rkf[2] = (double*)malloc(_Datsz);
	vert->rkf[3] = (double*)malloc(_Datsz);
	vert->rkf[4] = (double*)malloc(_Datsz);
	vert->rkf[5] = (double*)malloc(_Datsz);
	vert->rkf[6] = (double*)malloc(_Datsz);
	vert->rkf[7] = (double*)malloc(_Datsz);
	vert->rkf[8] = (double*)malloc(_Datsz);
	vert->rkg[0] = (double*)malloc(_Datsz * _Nchannels);
	vert->rkg[1] = (double*)malloc(_Datsz * _Nchannels);
	vert->time = 0.0;
	vert->in_degree = 0;
	vert->out_degree = 0;
	vert->degree = 0;
	vert->chead = 0;
	vert->ctail = 0;
	vert->edge_head = 0;
	vert->edge_tail = 0;
	vert->prev = _Network->vertex_tail;
	vert->next = 0;

	// Initialise state vectors
	if (_State)
	{
		memcpy(vert->state, _State, _Datsz);
		memcpy(vert->state0, _State, _Datsz);
		memcpy(vert->stage_state[0], _State, _Datsz);
	}
	else
	{
		memset(vert->state, 0, _Datsz);
		memset(vert->state0, 0, _Datsz);
		memset(vert->stage_state[0], 0, _Datsz);
	}

	// Add to noise channels' and own stacks
	NDXLChannel* channel;
	NDXLChannelVertex* cvert;
	NDXLVertexChannel* vchannel;
	for (size_t i = 0; i < vert->m; i++)
	{
		channel = (!_Channels || !_Channels[i]) ? __ndxlCreateChannel(_Network, 0) : _Channels[i];
		cvert = __ndxlCreateChannelVertex(channel, vert);
		vchannel = __ndxlCreateVertexChannel(vert, channel);
		cvert->vchannel = vchannel;
		vchannel->cvert = cvert;
	}

	// Add to network vertex stack
	if (_Network->vertex_head)
	{
		_Network->vertex_tail->next = vert;
		_Network->vertex_tail = _Network->vertex_tail->next;
	}
	else
	{
		_Network->vertex_head = vert;
		_Network->vertex_tail = vert;
	}

	// ...what exactly do you want me to say here, it's the end of the function. We're done. Vertex is made. Go away now. Value already returned
	return vert;
}

void __ndxlDestroyVertex(NDXLVertex* _Vertex)
{
	// Decrement network Jacobian and vertex counts
	_Vertex->network->jacn -= _Vertex->order;
	if (_Vertex->noise)
		_Vertex->network->nsv--;
	else
		_Vertex->network->ndv--;

	// Remove all associated edges
	NDXLEdgeNode* node = _Vertex->edge_head;
	while (node)
	{
		NDXLEdgeNode* tmp = node;
		node = node->next;
		__ndxlDestroyEdge(tmp->edge);
	}

	// De-link from channel stacks
	NDXLVertexChannel* vchannel = _Vertex->chead;
	while (vchannel)
	{
		NDXLVertexChannel* tmp = vchannel;
		vchannel = vchannel->next;
		__ndxlDestroyChannelVertex(tmp->cvert);
		__ndxlDestroyVertexChannel(tmp);
	}

	// De-link from vertex stack
	if (_Vertex->prev)
		_Vertex->prev->next = _Vertex->next;
	else
		_Vertex->network->vertex_head = _Vertex->network->vertex_head->next;
	if (_Vertex->next)
		_Vertex->next->prev = _Vertex->prev;
	else
		_Vertex->network->vertex_tail = _Vertex->network->vertex_tail->prev;

	// Finally, free vertex memory
	free(_Vertex->state);
	free(_Vertex->state0);
	free(_Vertex->stage_state[0]);
	free(_Vertex->stage_state[1]);
	free(_Vertex->interp_state);
	free(_Vertex->rkf[0]);
	free(_Vertex->rkf[1]);
	free(_Vertex->rkf[2]);
	free(_Vertex->rkf[3]);
	free(_Vertex->rkf[4]);
	free(_Vertex->rkf[5]);
	free(_Vertex->rkf[6]);
	free(_Vertex->rkf[7]);
	free(_Vertex->rkf[8]);
	free(_Vertex->rkg[0]);
	free(_Vertex->rkg[1]);
	free(_Vertex);
}

// ^^^^^ Vertex constructor and destructor | vertex functions vvvvv

const int __ndxlVertexProcessCallback(NDXLVertex* _Vertex)
{
	if (_Vertex->callback)
		return _Vertex->callback(_Vertex, _Vertex->state, _Vertex->time, _Vertex->params);
	return 0;
}

const int __ndxlVertexTickRalston(NDXLVertex* _Vertex, const double _Delta)
{
	return __ndxlIntegrateRalstonVertex(_Vertex, _Delta);
}

const int __ndxlVertexTickNystrom(NDXLVertex* _Vertex, const double _Delta)
{
	return __ndxlIntegrateNystromVertex(_Vertex, _Delta);
}

const int __ndxlVertexTickDopri(NDXLVertex* _Vertex, const double _Delta, const double _Atol, const double _Rtol, double* _Errsum)
{
	return __ndxlIntegrateDopriVertex(_Vertex, _Delta, _Atol, _Rtol, _Errsum);
}

const int __ndxlVertexTickRkck(NDXLVertex* _Vertex, const double _Delta, const double _Atol, const double _Rtol, double* _Errsum)
{
	return __ndxlIntegrateRkckVertex(_Vertex, _Delta, _Atol, _Rtol, _Errsum);
}

const int __ndxlVertexTickEuler(NDXLVertex* _Vertex, const double _Delta)
{
	return __ndxlIntegrateEulerVertex(_Vertex, _Delta);
}

const int __ndxlVertexTickHeun(NDXLVertex* _Vertex, const double _Delta, const double _Atol, const double _Rtol, double* _Errsum)
{
	return __ndxlIntegrateHeunVertex(_Vertex, _Delta, _Atol, _Rtol, _Errsum);
}

const int __ndxlVertexTickNorsett(NDXLVertex* _Vertex, const double _Delta)
{
	return 0;
}

// ^^^^^ Vertex functions | vertes getters vvvvv

NDXLVertexCallback __ndxlGetVertexCallback(NDXLVertex* _Vertex)
{
	return _Vertex->callback;
}

const size_t __ndxlGetVertexChannelCount(NDXLVertex* _Vertex)
{
	return _Vertex->m;
}

void __ndxlGetVertexChannels(NDXLVertex* _Vertex, NDXLChannel** _Channels)
{
	size_t i = 0;
	for (NDXLVertexChannel* vchannel = _Vertex->chead; vchannel; i++, vchannel = vchannel->next)
		_Channels[i] = vchannel->channel;
}

const size_t __ndxlGetVertexDegree(NDXLVertex* _Vertex)
{
	return _Vertex->degree;
}

const size_t __ndxlGetVertexDegreeIn(NDXLVertex* _Vertex)
{
	return _Vertex->in_degree;
}

const size_t __ndxlGetVertexDegreeOut(NDXLVertex* _Vertex)
{
	return _Vertex->out_degree;
}

NDXLVertexDynamics __ndxlGetVertexDynamics(NDXLVertex* _Vertex)
{
	return _Vertex->func;
}

void __ndxlGetVertexEdges(NDXLVertex* _Vertex, NDXLEdge** _Edges)
{
	size_t i = 0;
	NDXLEdgeNode* node = _Vertex->edge_head;
	while (node)
	{
		_Edges[i++] = node->edge;
		node = node->next;
	}
}

NDXLEdge* __ndxlGetVertexFirstEdge(NDXLVertex* _Vertex)
{
	return (_Vertex->edge_head ? _Vertex->edge_head->edge : 0);
}

NDXLEdge* __ndxlGetVertexLastEdge(NDXLVertex* _Vertex)
{
	return (_Vertex->edge_tail ? _Vertex->edge_tail->edge : 0);
}

NDXLVertex* __ndxlGetVertexNeighbour(NDXLVertex* _Vertex, NDXLEdge* _Edge)
{
	return (_Vertex == _Edge->vert1 ? _Edge->vert2 : _Edge->vert1);
}

const size_t __ndxlGetVertexNeighbourOrder(NDXLVertex* _Vertex, NDXLEdge* _Edge)
{
	return (_Vertex == _Edge->vert1 ? _Edge->vert2->order : _Edge->vert1->order);
}

void __ndxlGetVertexNeighbours(NDXLVertex* _Vertex, NDXLVertex** _Neigbours)
{
	size_t i = 0;
	NDXLEdgeNode* node = _Vertex->edge_head;
	while (node)
	{
		if (_Vertex == node->edge->vert1)
			_Neigbours[i++] = node->edge->vert2;
		else
			_Neigbours[i++] = node->edge->vert1;
		node = node->next;
	}
}
 
const double* __ndxlGetVertexNeighbourState(NDXLVertex* _Vertex, NDXLEdge* _Edge)
{
	// Directed edges have information flow from vertex 1 to vertex 2
	if (_Vertex == _Edge->vert2)
	{
		// If there is a delay...
		if (_Edge->delay)
		{
			// ...and the initial state hasn't yet propagated, return y0, interpolate otherwise
			const double _Rtime = _Edge->vert1->time + _Vertex->network->h * (_Vertex->network->butcher ? _Vertex->network->butcher[_Vertex->stage] : _Vertex->stage) - _Edge->delay;
			if (_Rtime < 0.0)
				return _Edge->vert1->state0;

			// Find interval in state history where retarded time is
			NDXLVertexState* state = _Edge->vert1_head;
			while (state->next && (_Rtime >= state->time))
				state = state->next;

			// state and state->prev now contain the times surrounding _Rtime, or the nearest interval that can be used for extrapolation
			const double _Dtime = state->time - state->prev->time;

			// Cubic Hermite terms
			const double _T = (_Rtime - state->prev->time) / _Dtime;
			const double _T2 = _T * _T;
			const double _T3 = _T2 * _T;
			const double _H00 = 2.0 * _T3 - 3.0 * _T2 + 1.0;
			const double _H01 = _T3 - 2.0 * _T2 + _T;
			const double _H10 = -2.0 * _T3 + 3.0 * _T2;
			const double _H11 = _T3 - _T2;

			// Interpolation
			for (size_t i = 0; i < _Edge->vert1->order; i++)
				_Edge->vert1->interp_state[i] = _H00 * state->prev->state[i] + _H01 * _Dtime * state->prev->slope[i] + _H10 * state->state[i] + _H11 * _Dtime * state->slope[i];
			return _Edge->vert1->interp_state;
		}

		// If delayless, return neighbour state at previous (current) RK stage for explicit (implicit) algorithms
		if (_Vertex->network->method_explicit)
			return _Edge->vert1->func ? (_Vertex->stage ? _Edge->vert1->stage_state[_Vertex->stage % 2] : _Edge->vert1->state) : _Edge->vert1->state;
		return _Edge->vert1->rkf[_Vertex->stage];
	}
	else
	{
		// If directed, return null
		if (_Edge->directed)
			return _Edge->vert2->null_state;

		// If there is a delay...
		if (_Edge->delay)
		{
			// ...and the initial state hasn't yet propagated, return y0, interpolate otherwise
			const double _Rtime = _Edge->vert2->time + _Vertex->network->h * (_Vertex->network->butcher ? _Vertex->network->butcher[_Vertex->stage] : _Vertex->stage) - _Edge->delay;
			if (_Rtime < 0.0)
				return _Edge->vert2->state0;

			// Find interval in state history where retarded time is
			NDXLVertexState* state = _Edge->vert2_head;
			while (state->next && (_Rtime >= state->time))
				state = state->next;

			// state and state->prev now contain the times surrounding _Rtime, or the nearest interval that can be used for extrapolation
			const double _Dtime = state->time - state->prev->time;

			// Cubic Hermite terms
			const double _T = (_Rtime - state->prev->time) / _Dtime;
			const double _T2 = _T * _T;
			const double _T3 = _T2 * _T;
			const double _H00 = 2.0 * _T3 - 3.0 * _T2 + 1.0;
			const double _H01 = _T3 - 2.0 * _T2 + _T;
			const double _H10 = -2.0 * _T3 + 3.0 * _T2;
			const double _H11 = _T3 - _T2;

			// Interpolation
			for (size_t i = 0; i < _Edge->vert2->order; i++)
				_Edge->vert2->interp_state[i] = _H00 * state->prev->state[i] + _H01 * _Dtime * state->prev->slope[i] + _H10 * state->state[i] + _H11 * _Dtime * state->slope[i];
			return _Edge->vert2->interp_state;
		}

		// If delayless, return neighbour state at previous (current) RK stage for explicit (implicit) algorithms
		if (_Vertex->network->method_explicit)
			return _Edge->vert2->func ? (_Vertex->stage ? _Edge->vert2->stage_state[_Vertex->stage % 2] : _Edge->vert2->state) : _Edge->vert2->state;
		return _Edge->vert2->rkf[_Vertex->stage];
	}
}

NDXLNetwork* __ndxlGetVertexNetwork(NDXLVertex* _Vertex)
{
	return _Vertex->network;
}

NDXLEdge* __ndxlGetVertexNextEdge(NDXLVertex* _Vertex, NDXLEdge* _Edge)
{
	if (_Vertex == _Edge->vert1)
		return (_Edge->node1->next ? _Edge->node1->next->edge : 0);
	return (_Edge->node2->next ? _Edge->node2->next->edge : 0);
}

NDXLVertexNoise __ndxlGetVertexNoise(NDXLVertex* _Vertex)
{
	return _Vertex->noise;
}

const size_t __ndxlGetVertexOrder(NDXLVertex* _Vertex)
{
	return _Vertex->order;
}

void* __ndxlGetVertexParams(NDXLVertex* _Vertex)
{
	return _Vertex->params;
}

NDXLEdge* __ndxlGetVertexPreviousEdge(NDXLVertex* _Vertex, NDXLEdge* _Edge)
{
	if (_Vertex == _Edge->vert1)
		return (_Edge->node1->prev ? _Edge->node1->prev->edge : 0);
	return (_Edge->node2->prev ? _Edge->node2->prev->edge : 0);
}

const double* __ndxlGetVertexState(NDXLVertex* _Vertex)
{
	return _Vertex->state;
}

const double* __ndxlGetVertexState0(NDXLVertex* _Vertex)
{
	return _Vertex->state0;
}

const double __ndxlGetVertexTime(NDXLVertex* _Vertex)
{
	return _Vertex->time;
}

// ^^^^^ Vertex getters | vertex setters vvvvv

void __ndxlSetVertexCallback(NDXLVertex* _Vertex, NDXLVertexCallback _Callback)
{
	_Vertex->callback = _Callback;
}

void __ndxlSetVertexDynamics(NDXLVertex* _Vertex, NDXLVertexDynamics _Dynamics)
{
	_Vertex->func = _Dynamics;
}

void __ndxlSetVertexNoise(NDXLVertex* _Vertex, NDXLVertexNoise _Noise)
{
	if (_Vertex->noise && !_Noise)
	{
		_Vertex->network->nsv--;
		_Vertex->network->ndv++;
	}
	else if (!_Vertex->noise && _Noise)
	{
		_Vertex->network->ndv--;
		_Vertex->network->nsv++;
	}
	_Vertex->noise = _Noise;
}

void __ndxlSetVertexParams(NDXLVertex* _Vertex, void* _Params)
{
	_Vertex->params = _Params;
}

// ^^^^^ Vertex setters | vertex functions vvvvv

char __ndxlVertexHasNeighbour(NDXLVertex* _Source, NDXLVertex* _Destination)
{
	char _Flags = 0;
	for (NDXLEdgeNode* node = _Source->edge_head; node; node = node->next)
	{
		if (__ndxlEdgeConnectsVertex(node->edge, _Destination))
		{
			if (node->edge->directed)
			{
				if (_Source == node->edge->vert1)
					_Flags |= 2;
				else
					_Flags |= 4;
			}
			else
				_Flags |= 1;
		}
	}
	return _Flags;
}

double* __ndxlVertexState(NDXLVertex* _Vertex)
{
	return _Vertex->func ? (_Vertex->stage ? _Vertex->stage_state[_Vertex->stage % 2] : _Vertex->state) : _Vertex->state;
}

// ^^^^^ Vertex functions