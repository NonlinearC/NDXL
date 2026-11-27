#include "internal.h"

// Constructors and destructors vvvvv

NDXLEdgeChannel* __ndxlCreateEdgeChannel(NDXLEdge* _Edge, NDXLChannel* _Channel)
{
	NDXLEdgeChannel* echannel = (NDXLEdgeChannel*)malloc(sizeof(NDXLEdgeChannel));
	echannel->channel = _Channel;
	echannel->eidx = _Channel->ne;
	return echannel;
}

void __ndxlDestroyEdgeChannel(NDXLEdgeChannel* _Echannel)
{
	free(_Echannel);
}

NDXLVertexState* __ndxlCreateVertexState(NDXLEdge* _Edge, const char _Which)
{
	NDXLVertexState* state = (NDXLVertexState*)malloc(sizeof(NDXLVertexState));
	NDXLVertex* vert;
	if (_Which == 1)
	{
		vert = _Edge->src;
		state->prev = _Edge->src_tail;
	}
	else
	{
		vert = _Edge->dest;
		state->prev = _Edge->dest_tail;
	}
	state->state = (double*)malloc(vert->datsz);
	memcpy(state->state, vert->network->state + vert->nidx, vert->datsz);
	state->slope = (double*)calloc(vert->dimension, sizeof(double));
	state->time = _Edge->time;
	state->next = 0;
	return state;
}

void __ndxlDestroyVertexState(NDXLVertexState* _State)
{
	free(_State->state);
	free(_State->slope);
	if (_State->next)
		_State->next->prev = _State->prev;
	free(_State);
}

NDXLEdge* __ndxlCreateEdge(NDXLVertex* _Source, NDXLVertex* _Destination, const double* const _State, const size_t _Dimension, const double _Delay, const char _Directed, NDXLEdgeDynamics _Dynamics, NDXLEdgeNoise _Noise, NDXLChannel** _Channels, const size_t _Nchannels, NDXLEdgeCallback _Callback, const double _Atol, const double _Rtol, void* _Params)
{
	// Deduce parent network from vertices
	NDXLNetwork* _Network = _Source->network;

	// Initialise explicit variables
	NDXLEdge* edge = (NDXLEdge*)malloc(sizeof(NDXLEdge));
	edge->src = _Source;
	edge->dest = _Destination;
	edge->dimension = _Dimension;
	edge->datsz = _Dimension * sizeof(double);
	edge->state0 = (double*)malloc(edge->datsz);
	edge->null_state = (double*)calloc(_Dimension, sizeof(double));
	edge->delay = _Delay;
	edge->directed = _Directed;
	edge->dynamics = _Dynamics;
	edge->noise = _Noise;
	edge->callback = _Callback;
	edge->atol = _Atol;
	edge->rtol = _Rtol;
	edge->params = _Params;

	// Initialise implicit variables
	edge->idx = _Network->eidx;
	edge->eidx = _Network->ne;
	edge->aidx = _Network->aidx;
	edge->nidx = _Network->jacn;
	edge->sidx = _Source->ne;
	edge->didx = _Destination->ne;
	edge->nc = 0;
	edge->ccap = 1;
	edge->time = 0.0;
	edge->rkg[0] = (double*)malloc(_Nchannels * edge->datsz);
	edge->rkg[1] = (double*)malloc(_Nchannels * edge->datsz);
	edge->channels = (NDXLEdgeChannel**)calloc(edge->ccap, sizeof(NDXLEdgeChannel*));

	// Add to vertex arrays
	__ndxlVertexResizeEdgeArray(_Source);
	_Source->edges[_Source->ne++] = edge;
	__ndxlVertexResizeEdgeArray(_Destination);
	_Destination->edges[_Destination->ne++] = edge;

	// Add to network edges array
	_Network->eidx++;
	__ndxlNetworkResizeArrays(_Network);
	_Network->edges[_Network->ne++] = edge;

	// Add to network stage vectors
	_Network->aidx++;
	_Network->jacn += _Dimension;
	__ndxlNetworkResizeVectors(_Network);
	if (_State)
		memcpy(_Network->state + edge->nidx, _State, edge->datsz);
	else
		memset(_Network->state + edge->nidx, 0, edge->datsz);

	// Update vertex variables based on directionality
	edge->src_head = __ndxlCreateVertexState(edge, 1);
	edge->src_tail = edge->src_head;
	if (_Directed)
	{
		edge->dest_head = 0;
		_Source->outdeg++;
		_Destination->indeg++;
	}
	else
	{
		edge->dest_head = __ndxlCreateVertexState(edge, 0);
	}
	edge->dest_tail = edge->dest_head;

	// Add noise channels
	if (_Noise)
	{
		_Network->nsv++;
		NDXLChannel* channel;
		NDXLChannelEdge* cedge;
		NDXLEdgeChannel* echannel;
		for (size_t i = 0; i < _Nchannels; i++)
		{
			channel = (!_Channels || !_Channels[i]) ? __ndxlCreateChannel(_Network, 0) : _Channels[i];
			cedge = __ndxlCreateChannelEdge(channel, edge);
			echannel = __ndxlCreateEdgeChannel(edge, channel);
			cedge->echannel = echannel;
			echannel->cedge = cedge;
			__ndxlEdgeResizeChannelArray(edge);
			edge->channels[edge->nc++] = echannel;
			__ndxlChannelResizeEdgeArray(channel);
			channel->edges[channel->ne++] = cedge;
		}
	}

	// Return created edge
	return edge;
}

void __ndxlDestroyEdge(NDXLEdge* _Edge)
{
	// Grab handle to owning network and vertices
	NDXLNetwork* _Network = _Edge->src->network;
	NDXLVertex* src = _Edge->src;
	NDXLVertex* dest = _Edge->dest;

	// Destroy historical state information of source vertex
	NDXLVertexState* state = _Edge->src_head;
	NDXLVertexState* tmp;
	while (state)
	{
		tmp = state;
		state = state->next;
		__ndxlDestroyVertexState(tmp);
	}

	// If directed, do the same for destination vertex
	if (_Edge->directed)
	{
		state = _Edge->dest_head;
		while (state)
		{
			tmp = state;
			state = state->next;
			__ndxlDestroyVertexState(tmp);
		}
	}

	// Update agent state and array indices
	size_t i;
	for (i = _Edge->eidx + 1; i < _Network->ne; i++)
	{
		_Network->edges[i]->nidx -= _Edge->dimension;
		_Network->edges[i]->eidx--;
		_Network->edges[i]->aidx--;
		_Network->edges[i - 1] = _Network->edges[i];
	}
	for (i = _Network->nv; i > 0; i--)
	{
		if (_Network->vertices[i - 1]->aidx > _Edge->aidx)
		{
			_Network->vertices[i - 1]->nidx -= _Edge->dimension;
			_Network->vertices[i - 1]->aidx--;
		}
		else
			break;
	}
	_Network->aidx--;

	// Decrement edge counts
	_Network->edges[--_Network->ne] = 0;
	if (_Edge->noise)
	{
		// Remove from channel arrays
		_Network->nse--;
		size_t j;
		NDXLEdgeChannel* echannel;
		while (_Edge->nc)
		{
			echannel = _Edge->channels[0];
			for (j = echannel->eidx + 1; j < echannel->channel->ne; j++)
			{
				echannel->channel->vertices[j]->cidx--;
				echannel->channel->vertices[j - 1] = echannel->channel->vertices[j];
			}
			echannel->channel->vertices[--echannel->channel->ne] = 0;
			_Edge->nc--;
		}
	}

	// Remove edge from vertex arrays
	src->ne--;
	dest->ne--;
	for (i = _Edge->sidx; i < src->ne; i++)
	{
		src->edges[i] = src->edges[i + 1];
		if (src->edges[i]->src == src)
			src->edges[i]->sidx--;
		else
			src->edges[i]->didx--;
	}
	for (i = _Edge->didx; i < dest->ne; i++)
	{
		dest->edges[i] = dest->edges[i + 1];
		if (dest->edges[i]->dest == dest)
			dest->edges[i]->didx--;
		else
			dest->edges[i]->sidx--;
	}
	src->edges[src->ne] = 0;
	dest->edges[dest->ne] = 0;

	// Back-shift state array
	_Network->jacn -= _Edge->dimension;
	memcpy(_Network->state + _Edge->nidx, _Network->state + _Edge->nidx + _Edge->dimension, (_Network->jacn - _Edge->nidx) * sizeof(double));

	// Finally, free vertex memory
	free(_Edge->state0);
	free(_Edge->null_state);
	free(_Edge->rkg[0]);
	free(_Edge->rkg[1]);
	free(_Edge->channels);
	free(_Edge);
}

// ^^^^^ Constructors and destructors | getters vvvvv

NDXLEdgeCallback __ndxlGetEdgeCallback(NDXLEdge* _Edge)
{
	return _Edge->callback;
}

const double __ndxlGetEdgeDelay(NDXLEdge* _Edge)
{
	return _Edge->delay;
}

NDXLVertex* __ndxlGetEdgeDestination(NDXLEdge* _Edge)
{
	return _Edge->dest;
}

const size_t __ndxlGetEdgeDimension(NDXLEdge* _Edge)
{
	return _Edge->dimension;
}

const char __ndxlGetEdgeDirected(NDXLEdge* _Edge)
{
	return _Edge->directed;
}

NDXLEdgeDynamics __ndxlGetEdgeDynamics(NDXLEdge* _Edge)
{
	return _Edge->dynamics;
}

const double* __ndxlGetEdgeInitialState(NDXLEdge* _Edge)
{
	return _Edge->state0;
}

NDXLNetwork* __ndxlGetEdgeNetwork(NDXLEdge* _Edge)
{
	return _Edge->src->network;
}

NDXLEdgeNoise __ndxlGetEdgeNoise(NDXLEdge* _Edge)
{
	return _Edge->noise;
}

void __ndxlGetEdgeNoiseChannels(NDXLEdge* _Edge, NDXLChannel** _Channels)
{
	for (size_t i = 0; i < _Edge->nc; i++)
		_Channels[i] = _Edge->channels[i]->channel;
}

const size_t __ndxlGetEdgeNoiseDimension(NDXLEdge* _Edge)
{
	return _Edge->nc;
}

void* __ndxlGetEdgeParams(NDXLEdge* _Edge)
{
	return _Edge->params;
}

const double* __ndxlGetEdgeRelativeState(NDXLEdge* _Edge, NDXLVertex* _Vertex)
{
	NDXLNetwork* _Network = _Vertex->network;
	if (_Edge->time < _Edge->delay)
		return _Edge->null_state;
	if ((_Vertex == _Edge->dest) || (!_Edge->directed))
		return (_Network->stage ? _Network->stage_state[_Network->stage % 2] : _Network->state) + _Edge->nidx;
	return _Edge->null_state;
}

NDXLVertex* __ndxlGetEdgeSource(NDXLEdge* _Edge)
{
	return _Edge->src;
}

const double __ndxlGetEdgeTime(NDXLEdge* _Edge)
{
	return _Edge->time;
}

// ^^^^^ Getters | setters vvvvv

void __ndxlSetEdgeCallback(NDXLEdge* _Edge, NDXLEdgeCallback _Callback)
{
	_Edge->callback = _Callback;
}

void __ndxlSetEdgeDynamics(NDXLEdge* _Edge, NDXLEdgeDynamics _Dynamics)
{
	_Edge->dynamics = _Dynamics;
}

void __ndxlSetEdgeNoise(NDXLEdge* _Edge, NDXLEdgeNoise _Noise)
{
	if (_Edge->noise && !_Noise)
		_Edge->src->network->nse--;
	else if (!_Edge->noise && _Noise)
		_Edge->src->network->nse++;
	_Edge->noise = _Noise;
}

void __ndxlSetEdgeParams(NDXLEdge* _Edge, void* _Params)
{
	_Edge->params = _Params;
}

// ^^^^^ Setters | functions vvvvv

NDXLChannel* __ndxlEdgeChannel(NDXLEdge* _Edge, const size_t _Idx)
{
	return _Edge->channels[_Idx]->channel;
}

void __ndxlEdgeAccumulateEffects(NDXLEdge* _Edge, const double _Delta)
{
	_Edge->time += _Delta;

	// Accumulate source vertex state
	NDXLVertexState* state = __ndxlCreateVertexState(_Edge, 1);
	_Edge->src_tail->next = state;
	_Edge->src_tail = _Edge->src_tail->next;
	while ((_Edge->src_tail->time - _Edge->src_head->next->time) > _Edge->delay)
	{
		_Edge->src_head = _Edge->src_head->next;
		__ndxlDestroyVertexState(_Edge->src_head->prev);
	}

	// Accumulate destination vertex state
	if (!(_Edge->directed))
	{
		state = __ndxlCreateVertexState(_Edge, 0);
		_Edge->dest_tail->next = state;
		_Edge->dest_tail = _Edge->dest_tail->next;
		while ((_Edge->dest_tail->time - _Edge->dest_head->next->time) > _Edge->delay)
		{
			_Edge->dest_head = _Edge->dest_head->next;
			__ndxlDestroyVertexState(_Edge->dest_head->prev);
		}
	}
}

const char __ndxlEdgeConnectsVertex(NDXLEdge* _Edge, NDXLVertex* _Vertex)
{
	char _Flags = 0;
	if (_Vertex == _Edge->src)
	{
		_Flags |= 1;
		if (_Edge->directed)
			_Flags |= 2;
	}
	if (_Vertex == _Edge->dest)
	{
		_Flags |= 1;
		if (_Edge->directed)
			_Flags |= 4;
	}
	return _Flags;
}

void __ndxlEdgeResizeChannelArray(NDXLEdge* _Edge)
{
	// Verify capacity of channel array
	if (_Edge->nc == _Edge->ccap)
	{
		_Edge->ccap = (_Edge->ccap << 1) + 1;
		_Edge->channels = (NDXLEdgeChannel**)realloc(_Edge->channels, _Edge->ccap * sizeof(NDXLEdgeChannel*));
	}
}

double* __ndxlEdgeState(NDXLEdge* _Edge)
{
	NDXLNetwork* _Network = _Edge->src->network;
	return (_Network->stage ? _Network->stage_state[_Network->stage % 2] : _Network->state) + _Edge->nidx;
}

// ^^^^^ Functions