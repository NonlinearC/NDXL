//----------------------------------\\
// NDXL source file for edge class	\\
// Copyright Ethan Codron 2026		\\
// GNU GPL-3.0						\\
//----------------------------------\\
// Source file providing all edge	\\
// functions used internally by the	\\
// API.								\\
//----------------------------------\\

#include "internal.h"

// Vertex state constructor and destructor vvvvv

NDXLVertexState* __ndxlCreateVertexState(NDXLVertex* _Vertex, const double _Time)
{
	const size_t _Datsz = _Vertex->order * sizeof(double);
	NDXLVertexState* state = (NDXLVertexState*)malloc(sizeof(NDXLVertexState));
	state->state = (double*)malloc(_Datsz);
	memcpy(state->state, _Vertex->state, _Datsz);
	state->slope = (double*)calloc(_Vertex->order, sizeof(double));
	state->time = _Time;
	state->prev = 0;
	state->next = 0;
	return state;
}

void __ndxlDestroyVertexState(NDXLVertexState* _State)
{
	free(_State->state);
	free(_State->slope);
	if (_State->prev)
		_State->prev->next = _State->next;
	if (_State->next)
		_State->next->prev = _State->prev;
	free(_State);
}

NDXLEdgeNode* __ndxlCreateEdgeNode(NDXLEdge* _Edge, NDXLVertex* _Vertex)
{
	NDXLEdgeNode* node = (NDXLEdgeNode*)malloc(sizeof(NDXLEdgeNode));
	node->edge = _Edge;
	node->prev = _Vertex->edge_tail;
	node->next = 0;
	if (_Vertex->edge_head)
		_Vertex->edge_tail->next = node;
	else
		_Vertex->edge_head = node;
	_Vertex->edge_tail = node;
	return node;
}

void __ndxlDestroyEdgeNode(NDXLEdgeNode* _Node)
{
	NDXLEdge* edge = _Node->edge;
	if (_Node == edge->vert1->edge_head)
	{
		edge->vert1->edge_head = edge->vert1->edge_head->next;
		if (edge->vert1->edge_head)
			edge->vert1->edge_head->prev = 0;
	}
	else if (_Node == edge->vert1->edge_tail)
	{
		edge->vert1->edge_tail = edge->vert1->edge_tail->prev;
		if (edge->vert1->edge_tail)
			edge->vert1->edge_tail->next = 0;
	}
	else if (_Node == edge->vert2->edge_head)
	{
		edge->vert2->edge_head = edge->vert2->edge_head->next;
		if (edge->vert2->edge_head)
			edge->vert2->edge_head->prev = 0;
	}
	else if (_Node == edge->vert2->edge_tail)
	{
		edge->vert2->edge_tail = edge->vert2->edge_tail->prev;
		if (edge->vert2->edge_tail)
			edge->vert2->edge_tail->next = 0;
	}
	else
	{
		_Node->prev->next = _Node->next;
		_Node->next->prev = _Node->prev;
	}
	free(_Node);
}

// ^^^^^ Vertex state constructor and destructor | edge constructors and destructor vvvvv

NDXLEdgeChannel* __ndxlCreateEdgeChannel(NDXLEdge* _Edge, NDXLChannel* _Channel)
{
	NDXLEdgeChannel* echannel = (NDXLEdgeChannel*)malloc(sizeof(NDXLEdgeChannel));
	echannel->channel = _Channel;
	echannel->channel->ne++;
	echannel->prev = _Edge->ctail;
	echannel->next = 0;
	if (_Edge->chead)
		_Edge->ctail->next = echannel;
	else
		_Edge->chead = echannel;
	_Edge->ctail = echannel;
	return echannel;
}

void __ndxlDestroyEdgeChannel(NDXLEdgeChannel* _Echannel)
{
	_Echannel->channel->ne--;
	if (_Echannel->prev)
		_Echannel->prev->next = _Echannel->next;
	else
		_Echannel->cedge->edge->chead = _Echannel->cedge->edge->chead->next;
	if (_Echannel->next)
		_Echannel->next->prev = _Echannel->prev;
	else
		_Echannel->cedge->edge->ctail = _Echannel->cedge->edge->ctail->prev;
	free(_Echannel);
}

NDXLEdge* __ndxlCreateEdge(NDXLVertex* _Vert1, NDXLVertex* _Vert2, const double* _State, const size_t _Order, const double _Delay, const char _Directed, NDXLEdgeDynamics _Func, NDXLEdgeDynamics _Noise, NDXLChannel** _Channels, const size_t _Nchannels, NDXLEdgeCallback _Callback, void* _Params)
{
	if (_Noise)
		_Vert1->network->nse++;
	else
		_Vert1->network->nde++;
	const size_t _Datsz = _Order * sizeof(double);

	// Initialise explicit values
	NDXLEdge* edge = (NDXLEdge*)malloc(sizeof(NDXLEdge));
	edge->vert1 = _Vert1;
	edge->vert2 = _Vert2;
	edge->state = (double*)malloc(_Datsz);
	edge->order = _Order;
	edge->delay = _Delay;
	edge->directed = (_Directed ? 1 : 0);
	edge->func = _Func;
	edge->noise = _Noise;
	edge->m = ((_Noise && !_Nchannels) ? _Order : _Nchannels);
	edge->callback = _Callback;
	edge->params = _Params;
	

	// Initialise implicit values
	edge->vert1->network->jacn += _Order;
	edge->state0 = (double*)malloc(_Datsz);
	edge->stage_state[0] = (double*)malloc(_Datsz);
	edge->stage_state[1] = (double*)malloc(_Datsz);
	edge->null_state = (double*)calloc(_Order, sizeof(double));
	edge->stage = 0;
	edge->rkf[0] = (double*)malloc(_Datsz);
	edge->rkf[1] = (double*)malloc(_Datsz);
	edge->rkf[2] = (double*)malloc(_Datsz);
	edge->rkf[3] = (double*)malloc(_Datsz);
	edge->rkf[4] = (double*)malloc(_Datsz);
	edge->rkf[5] = (double*)malloc(_Datsz);
	edge->rkf[6] = (double*)malloc(_Datsz);
	edge->rkf[7] = (double*)malloc(_Datsz);
	edge->rkf[8] = (double*)malloc(_Datsz);
	edge->rkg[0] = (double*)malloc(_Datsz);
	edge->rkg[1] = (double*)malloc(_Datsz);
	edge->time = 0.0;
	edge->prev = _Vert1->network->edge_tail;
	edge->next = 0;

	// Initialise state vectors
	if (_State)
	{
		memcpy(edge->state, _State, _Datsz);
		memcpy(edge->state0, _State, _Datsz);
		memcpy(edge->stage_state[0], _State, _Datsz);
	}
	else
	{
		memset(edge->state, 0, _Datsz);
		memset(edge->state0, 0, _Datsz);
		memset(edge->stage_state[0], 0, _Datsz);
	}

	// Add to noise channels' and own stacks
	NDXLChannel* channel;
	NDXLChannelEdge* cedge;
	NDXLEdgeChannel* echannel;
	for (size_t i = 0; i < _Nchannels; i++)
	{
		channel = (!_Channels || !_Channels[i]) ? __ndxlCreateChannel(_Vert1->network, 0) : _Channels[i];
		cedge = __ndxlCreateChannelEdge(channel, edge);
		echannel = __ndxlCreateEdgeChannel(edge, channel);
		cedge->echannel = echannel;
		echannel->cedge = cedge;
	}

	// Initialise vertex state and channel stacks
	edge->chead = 0;
	edge->ctail = 0;
	edge->vert1_head = __ndxlCreateVertexState(_Vert1, 0.0);
	edge->vert1_tail = edge->vert1_head;

	// Increment vertex degrees
	if (_Directed)
	{
		edge->vert2_head = 0;
		edge->vert2_tail = 0;
		_Vert2->in_degree++;
		_Vert1->out_degree++;
	}
	else
	{
		edge->vert2_head = __ndxlCreateVertexState(_Vert2, 0.0);
		edge->vert2_tail = edge->vert2_head;
		_Vert2->degree++;
		_Vert1->degree++;
	}

	// Add to vertex stacks
	edge->node1 = __ndxlCreateEdgeNode(edge, _Vert1);
	edge->node2 = (_Vert1 == _Vert2 ? 0 : __ndxlCreateEdgeNode(edge, _Vert2));

	// Add to noise channels' stacks
	for (size_t i = 0; i < _Nchannels; i++)
		__ndxlCreateChannelEdge((!_Channels || !_Channels[i]) ? __ndxlCreateChannel(_Vert1->network, 0) : _Channels[i], edge);

	// Add to network edge stack
	if (_Vert1->network->edge_head)
		_Vert1->network->edge_tail->next = edge;
	else
		_Vert1->network->edge_head = edge;
	_Vert1->network->edge_tail = edge;

	// Return edge
	return edge;
}

void __ndxlDestroyEdge(NDXLEdge* _Edge)
{
	// Decrement relevant counts
	_Edge->vert1->network->jacn -= _Edge->order;
	if (_Edge->noise)
		_Edge->vert1->network->nse--;
	else
		_Edge->vert1->network->nde--;
	if (_Edge->directed)
	{
		_Edge->vert1->out_degree--;
		_Edge->vert1->in_degree--;
	}
	else
	{
		_Edge->vert1->degree--;
		_Edge->vert2->degree--;
	}

	// Clear state stacks
	NDXLVertexState* state = _Edge->vert1_head;
	while (state)
	{
		NDXLVertexState* tmp = state;
		state = state->next;
		__ndxlDestroyVertexState(tmp);
	}

	state = _Edge->vert2_head;
	while (state)
	{
		NDXLVertexState* tmp = state;
		state = state->next;
		__ndxlDestroyVertexState(tmp);
	}

	// De-link from vertex stacks
	__ndxlDestroyEdgeNode(_Edge->node1);
	__ndxlDestroyEdgeNode(_Edge->node2);

	// De-link from channel stacks
	NDXLEdgeChannel* echannel = _Edge->chead;
	while (echannel)
	{
		NDXLEdgeChannel* tmp = echannel;
		echannel = echannel->next;
		__ndxlDestroyChannelEdge(tmp->cedge);
		__ndxlDestroyEdgeChannel(tmp);
	}

	// De-link from edge stack
	if (_Edge->prev)
		_Edge->prev->next = _Edge->next;
	else
		_Edge->vert1->network->edge_head = _Edge->vert1->network->edge_head->next;
	if (_Edge->next)
		_Edge->next->prev = _Edge->prev;
	else
		_Edge->vert1->network->edge_tail = _Edge->vert1->network->edge_tail->prev;

	// Free edge memory
	free(_Edge->state);
	free(_Edge->state0);
	free(_Edge->stage_state[0]);
	free(_Edge->stage_state[1]);
	free(_Edge->rkf[0]);
	free(_Edge->rkf[1]);
	free(_Edge->rkf[2]);
	free(_Edge->rkf[3]);
	free(_Edge->rkf[4]);
	free(_Edge->rkf[5]);
	free(_Edge->rkf[6]);
	free(_Edge->rkf[7]);
	free(_Edge->rkf[8]);
	free(_Edge->rkg[0]);
	free(_Edge->rkg[1]);
	free(_Edge);
}

// ^^^^^ Edge constructors and destructor | edge functions vvvvv

const int __ndxlEdgeProcessCallback(NDXLEdge* _Edge)
{
	if (_Edge->callback)
		return _Edge->callback(_Edge, _Edge->state, _Edge->time, _Edge->params);
	return 0;
}

void __ndxlEdgeAccumulateEffects(NDXLEdge* _Edge, const double _Delta)
{
	_Edge->time += _Delta;

	// Accumulate vertex 1 state
	NDXLVertexState* state = __ndxlCreateVertexState(_Edge->vert1, _Edge->time);
	state->prev = _Edge->vert1_tail;
	_Edge->vert1_tail->next = state;
	_Edge->vert1_tail = _Edge->vert1_tail->next;
	while ((_Edge->vert1_tail->time - _Edge->vert1_head->next->time) > _Edge->delay)
	{
		_Edge->vert1_head = _Edge->vert1_head->next;
		__ndxlDestroyVertexState(_Edge->vert1_head->prev);
	}
	
	// Accumulate vertex 2 state
	if (!(_Edge->directed))
	{
		state = __ndxlCreateVertexState(_Edge->vert2, _Edge->time);
		state->prev = _Edge->vert2_tail;
		_Edge->vert2_tail->next = state;
		_Edge->vert2_tail = _Edge->vert2_tail->next;
		while ((_Edge->vert2_tail->time - _Edge->vert2_head->next->time) > _Edge->delay)
		{
			_Edge->vert2_head = _Edge->vert2_head->next;
			__ndxlDestroyVertexState(_Edge->vert2_head->prev);
		}
	}
}

const int __ndxlEdgeTickRalston(NDXLEdge* _Edge, const double _Delta)
{
	return __ndxlIntegrateRalstonEdge(_Edge, _Delta);
}

const int __ndxlEdgeTickNystrom(NDXLEdge* _Edge, const double _Delta)
{
	return __ndxlIntegrateNystromEdge(_Edge, _Delta);
}

const int __ndxlEdgeTickDopri(NDXLEdge* _Edge, const double _Delta, const double _Atol, const double _Rtol, double* _Errsum)
{
	return __ndxlIntegrateDopriEdge(_Edge, _Delta, _Atol, _Rtol, _Errsum);
}

const int __ndxlEdgeTickRkck(NDXLEdge* _Edge, const double _Delta, const double _Atol, const double _Rtol, double* _Errsum)
{
	return __ndxlIntegrateRkckEdge(_Edge, _Delta, _Atol, _Rtol, _Errsum);
}

const int __ndxlEdgeTickEuler(NDXLEdge* _Edge, const double _Delta)
{
	return __ndxlIntegrateEulerEdge(_Edge, _Delta);
}

const int __ndxlEdgeTickHeun(NDXLEdge* _Edge, const double _Delta, const double _Atol, const double _Rtol, double* _Errsum)
{
	return __ndxlIntegrateHeunEdge(_Edge, _Delta, _Atol, _Rtol, _Errsum);
}

const int __ndxlEdgeTickNorsett(NDXLEdge* _Edge, const double _Delta)
{
	return 0;
}

// ^^^^^ Edge functions | edge getters vvvvv

NDXLEdgeCallback __ndxlGetEdgeCallback(NDXLEdge* _Edge)
{
	return _Edge->callback;
}

const size_t __ndxlGetEdgeChannelCount(NDXLEdge* _Edge)
{
	return _Edge->m;
}

void __ndxlGetEdgeChannels(NDXLEdge* _Edge, NDXLChannel** _Channels)
{
	size_t i = 0;
	for (NDXLEdgeChannel* echannel = _Edge->chead; echannel; i++, echannel = echannel->next)
		_Channels[i] = echannel->channel;
}

const double __ndxlGetEdgeDelay(NDXLEdge* _Edge)
{
	return _Edge->delay;
}

const char __ndxlGetEdgeDirected(NDXLEdge* _Edge)
{
	return _Edge->directed;
}

NDXLEdgeDynamics __ndxlGetEdgeDynamics(NDXLEdge* _Edge)
{
	return _Edge->func;
}

NDXLVertex* __ndxlGetEdgeFirstVertex(NDXLEdge* _Edge)
{
	return _Edge->vert1;
}

NDXLNetwork* __ndxlGetEdgeNetwork(NDXLEdge* _Edge)
{
	return _Edge->vert1->network;
}

NDXLEdgeNoise __ndxlGetEdgeNoise(NDXLEdge* _Edge)
{
	return _Edge->noise;
}

const size_t __ndxlGetEdgeOrder(NDXLEdge* _Edge)
{
	return _Edge->order;
}

void* __ndxlGetEdgeParams(NDXLEdge* _Edge)
{
	return _Edge->params;
}

const double* __ndxlGetEdgeRelativeState(NDXLEdge* _Edge, NDXLVertex* _Vertex)
{
	if (_Edge->time < _Edge->delay)
		return _Edge->null_state;
	if ((_Vertex == _Edge->vert2) || (!_Edge->directed))
		return _Edge->func ? (_Edge->stage ? _Edge->stage_state[_Edge->stage % 2] : _Edge->state) : _Edge->state;
	return _Edge->null_state;
}

NDXLVertex* __ndxlGetEdgeSecondVertex(NDXLEdge* _Edge)
{
	return _Edge->vert2;
}

const double* __ndxlGetEdgeState0(NDXLEdge* _Edge)
{
	return _Edge->state0;
}

const double __ndxlGetEdgeTime(NDXLEdge* _Edge)
{
	return _Edge->time;
}

// ^^^^^ Edge getters | edge setters vvvvv

void __ndxlSetEdgeCallback(NDXLEdge* _Edge, NDXLEdgeCallback _Callback)
{
	_Edge->callback = _Callback;
}

void __ndxlSetEdgeDynamics(NDXLEdge* _Edge, NDXLEdgeDynamics _Func)
{
	_Edge->func = _Func;
}

void __ndxlSetEdgeNoise(NDXLEdge* _Edge, NDXLEdgeNoise _Noise)
{
	if (_Edge->noise && !_Noise)
	{
		_Edge->vert1->network->nse--;
		_Edge->vert1->network->nde++;
	}
	else if (!_Edge->noise && _Noise)
	{
		_Edge->vert1->network->nde--;
		_Edge->vert1->network->nse++;
	}
	_Edge->noise = _Noise;
}

void __ndxlSetEdgeParams(NDXLEdge* _Edge, void* _Params)
{
	_Edge->params = _Params;
}

// ^^^^^ Edge setters | edge functions vvvvv

const char __ndxlEdgeConnectsVertex(NDXLEdge* _Edge, NDXLVertex* _Vertex)
{
	char _Flags = 0;
	if (_Vertex == _Edge->vert1)
	{
		_Flags |= 1;
		if (_Edge->directed)
			_Flags |= 2;
	}
	if (_Vertex == _Edge->vert2)
	{
		_Flags |= 1;
		if (_Edge->directed)
			_Flags |= 4;
	}
	return _Flags;
}

double* __ndxlEdgeState(NDXLEdge* _Edge)
{
	return _Edge->func ? (_Edge->stage ? _Edge->stage_state[_Edge->stage % 2] : _Edge->state) : _Edge->state;
}

// ^^^^^ Edge functions