#include "internal.h"

// Constructors and destructors vvvvv

NDXLVertexChannel* __ndxlCreateVertexChannel(NDXLVertex* _Vertex, NDXLChannel* _Channel)
{
    NDXLVertexChannel* vchannel = (NDXLVertexChannel*)malloc(sizeof(NDXLVertexChannel));
    vchannel->channel = _Channel;
    vchannel->vidx = _Channel->nv;
    return vchannel;
}

void __ndxlDestroyVertexChannel(NDXLVertexChannel* _Vchannel)
{
    free(_Vchannel);
}

NDXLVertex* __ndxlCreateVertex(NDXLNetwork* _Network, const double* const _State, const size_t _Dimension, NDXLVertexDynamics _Dynamics, NDXLVertexNoise _Noise, NDXLChannel** _Channels, const size_t _Nchannels, NDXLVertexCallback _Callback, const double _Atol, const double _Rtol, void* _Params)
{
	// Initialise explicit variables
	NDXLVertex* vert = (NDXLVertex*)malloc(sizeof(NDXLVertex));
	vert->network = _Network;
	vert->dimension = _Dimension;
	vert->datsz = _Dimension * sizeof(double);
	vert->state0 = (double*)malloc(vert->datsz);
	vert->interp_state = (double*)calloc(_Dimension, sizeof(double));
	vert->dynamics = _Dynamics;
	vert->noise = _Noise;
	vert->callback = _Callback;
	vert->atol = _Atol;
	vert->rtol = _Rtol;
	vert->params = _Params;

	// Initialise implicit variables
	vert->idx = _Network->vidx;
    vert->vidx = _Network->nv;
    vert->aidx = _Network->aidx;
	vert->nidx = _Network->jacn;
	vert->ne = 0;
	vert->ecap = 3;
    vert->nc = 0;
    vert->ccap = 1;
	vert->indeg = 0;
	vert->outdeg = 0;
	vert->time = 0.0;
    vert->rkg[0] = (double*)malloc(_Nchannels * vert->datsz);
    vert->rkg[1] = (double*)malloc(_Nchannels * vert->datsz);
	vert->edges = (NDXLEdge**)calloc(vert->ecap, sizeof(NDXLEdge*));
	vert->channels = (NDXLVertexChannel**)calloc(vert->ccap, sizeof(NDXLVertexChannel*));

	// Add to network vertices array
	_Network->vidx++;
	__ndxlNetworkResizeArrays(_Network);
	_Network->vertices[_Network->nv++] = vert;

	// Add to network stage vectors
    _Network->aidx++;
	_Network->jacn += _Dimension;
	__ndxlNetworkResizeVectors(_Network);
    if (_State)
        memcpy(_Network->state + vert->nidx, _State, vert->datsz);
    else
        memset(_Network->state + vert->nidx, 0, vert->datsz);

    // Add noise channels
    if (_Noise)
    {
        _Network->nsv++;
        NDXLChannel* channel;
        NDXLChannelVertex* cvert;
        NDXLVertexChannel* vchannel;
        for (size_t i = 0; i < _Nchannels; i++)
        {
            channel = (!_Channels || !_Channels[i]) ? __ndxlCreateChannel(_Network, 0) : _Channels[i];
            cvert = __ndxlCreateChannelVertex(channel, vert);
            vchannel = __ndxlCreateVertexChannel(vert, channel);
            cvert->vchannel = vchannel;
            vchannel->cvert = cvert;
            __ndxlVertexResizeChannelArray(vert);
            vert->channels[vert->nc++] = vchannel;
            __ndxlChannelResizeVertexArray(channel);
            channel->vertices[channel->nv++] = cvert;
        }
    }

	// Return created vertex
	return vert;
}

void __ndxlDestroyVertex(NDXLVertex* _Vertex)
{
	// Grab handle to owning network
	NDXLNetwork* _Network = _Vertex->network;

    // Destroy connected edges
    size_t i = 0;
    while (_Vertex->ne)
        __ndxlDestroyEdge(_Vertex->edges[0]);

    // Update agent state and array indices
    for (i = _Vertex->vidx + 1; i < _Network->nv; i++)
    {
        _Network->vertices[i]->nidx -= _Vertex->dimension;
        _Network->vertices[i]->vidx--;
        _Network->vertices[i]->aidx--;
        _Network->vertices[i - 1] = _Network->vertices[i];
    }
    for (i = _Network->ne; i > 0; i--)
    {
        if (_Network->edges[i - 1]->aidx > _Vertex->aidx)
        {
            _Network->edges[i - 1]->nidx -= _Vertex->dimension;
            _Network->edges[i - 1]->aidx--;
        }
        else
            break;
    }
    _Network->aidx--;

    // Decrement vertex counts
    _Network->vertices[--_Network->nv] = 0;
    if (_Vertex->noise)
    {
        // Remove from channel arrays
        _Network->nsv--;
        size_t j;
        NDXLVertexChannel* vchannel;
        while (_Vertex->nc)
        {
            vchannel = _Vertex->channels[0];
            for (j = vchannel->vidx + 1; j < vchannel->channel->nv; j++)
            {
                vchannel->channel->vertices[j]->cidx--;
                vchannel->channel->vertices[j - 1] = vchannel->channel->vertices[j];
            }
            vchannel->channel->vertices[--vchannel->channel->nv] = 0;
            _Vertex->nc--;
        }
    }

    // Back-shift state array
    _Network->jacn -= _Vertex->dimension;
    memcpy(_Network->state + _Vertex->nidx, _Network->state + _Vertex->nidx + _Vertex->dimension, (_Network->jacn - _Vertex->nidx) * sizeof(double));

	// Finally, free vertex memory
	free(_Vertex->state0);
	free(_Vertex->interp_state);
    free(_Vertex->rkg[0]);
    free(_Vertex->rkg[1]);
	free(_Vertex->edges);
    free(_Vertex->channels);
	free(_Vertex);
}

// ^^^^^ Constructors and destructors | getters vvvvv

NDXLVertexCallback __ndxlGetVertexCallback(NDXLVertex* _Vertex)
{
	return _Vertex->callback;
}

const size_t __ndxlGetVertexDegree(NDXLVertex* _Vertex)
{
	return _Vertex->ne;
}

const size_t __ndxlGetVertexDegreeIn(NDXLVertex* _Vertex)
{
	return _Vertex->indeg;
}

const size_t __ndxlGetVertexDegreeOut(NDXLVertex* _Vertex)
{
	return _Vertex->outdeg;
}

const size_t __ndxlGetVertexDimension(NDXLVertex* _Vertex)
{
    return _Vertex->dimension;
}

NDXLVertexDynamics __ndxlGetVertexDynamics(NDXLVertex* _Vertex)
{
	return _Vertex->dynamics;
}

const double* __ndxlGetVertexInitialState(NDXLVertex* _Vertex)
{
	return _Vertex->state0;
}

NDXLVertex* __ndxlGetVertexNeighbour(NDXLVertex* _Vertex, NDXLEdge* _Edge)
{
	if (_Vertex == _Edge->src)
		return _Edge->dest;
	return _Edge->src;
}

const size_t __ndxlGetVertexNeighbourDimension(NDXLVertex* _Vertex, NDXLEdge* _Edge)
{
	if (_Vertex == _Edge->src)
		return _Edge->dest->dimension;
	return _Edge->src->dimension;
}

const size_t __ndxlGetVertexNeighbourNoiseDimension(NDXLVertex* _Vertex, NDXLEdge* _Edge)
{
    if (_Vertex == _Edge->src)
        return _Edge->dest->nc;
    return _Edge->src->nc;
}

const double* __ndxlGetVertexNeighbourState(NDXLVertex* _Vertex, NDXLEdge* _Edge)
{
    // Get handle to owning network
    NDXLNetwork* _Network = _Vertex->network;

    // Directed edges have information flow from src to dest
    if (_Vertex == _Edge->dest)
    {
        // If there is a delay...
        if (_Edge->delay)
        {
            // ...and the initial state hasn't yet propagated, return initial state, interpolate otherwise
            const double _Rtime = _Edge->src->time + _Network->h * (_Network->butcher ? _Network->butcher[_Network->stage] : _Network->stage) - _Edge->delay;
            if (_Rtime < 0.0)
                return _Edge->src->state0;

            // Find interval in state history where retarded time is
            NDXLVertexState* state = _Edge->src_head;
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
            for (size_t i = 0; i < _Edge->src->dimension; i++)
                _Edge->src->interp_state[i] = _H00 * state->prev->state[i] + _H01 * _Dtime * state->prev->slope[i] + _H10 * state->state[i] + _H11 * _Dtime * state->slope[i];
            return _Edge->src->interp_state;
        }

        // If delayless, return neighbour state at previous (current) RK stage for explicit (implicit) algorithms
        if (_Network->method_explicit)
            return (_Network->stage ? _Network->stage_state[_Network->stage % 2] : _Network->state) + _Edge->src->nidx;
        return _Network->rkf[_Network->stage] + _Edge->src->nidx;
    }
    else
    {
        // If directed, return zero vector
        if (_Edge->directed)
        {
            memset(_Edge->dest->interp_state, 0, _Edge->dest->datsz);
            return _Edge->dest->interp_state;
        }

        // If there is a delay...
        if (_Edge->delay)
        {
            // ...and the initial state hasn't yet propagated, return initial state, interpolate otherwise
            const double _Rtime = _Edge->dest->time + _Network->h * (_Network->butcher ? _Network->butcher[_Network->stage] : _Network->stage) - _Edge->delay;
            if (_Rtime < 0.0)
                return _Edge->dest->state0;

            // Find interval in state history where retarded time is
            NDXLVertexState* state = _Edge->dest_head;
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
            for (size_t i = 0; i < _Edge->dest->dimension; i++)
                _Edge->dest->interp_state[i] = _H00 * state->prev->state[i] + _H01 * _Dtime * state->prev->slope[i] + _H10 * state->state[i] + _H11 * _Dtime * state->slope[i];
            return _Edge->dest->interp_state;
        }

        // If delayless, return neighbour state at previous (current) RK stage for explicit (implicit) algorithms
        if (_Network->method_explicit)
            return (_Network->stage ? _Network->stage_state[_Network->stage % 2] : _Network->state) + _Edge->dest->nidx;
        return _Network->rkf[_Network->stage] + _Edge->dest->nidx;
    }
}

NDXLNetwork* __ndxlGetVertexNetwork(NDXLVertex* _Vertex)
{
    return _Vertex->network;
}

NDXLVertexNoise __ndxlGetVertexNoise(NDXLVertex* _Vertex)
{
    return _Vertex->noise;
}

const size_t __ndxlGetVertexNoiseDimension(NDXLVertex* _Vertex)
{
    return _Vertex->nc;
}

void* __ndxlGetVertexParams(NDXLVertex* _Vertex)
{
    return _Vertex->params;
}

const double __ndxlGetVertexTime(NDXLVertex* _Vertex)
{
    return _Vertex->time;
}

// ^^^^^ Getters | setters vvvvv

void __ndxlSetVertexCallback(NDXLVertex* _Vertex, NDXLVertexCallback _Callback)
{
    _Vertex->callback = _Callback;
}

void __ndxlSetVertexDynamics(NDXLVertex* _Vertex, NDXLVertexDynamics _Dynamics)
{
    _Vertex->dynamics = _Dynamics;
}

void __ndxlSetVertexNoise(NDXLVertex* _Vertex, NDXLVertexNoise _Noise)
{
    if (_Vertex->noise && !_Noise)
        _Vertex->network->nsv--;
    else if (!_Vertex->noise && _Noise)
        _Vertex->network->nsv++;
    _Vertex->noise = _Noise;
}

void __ndxlSetVertexParams(NDXLVertex* _Vertex, void* _Params)
{
    _Vertex->params = _Params;
}

// ^^^^^ Setters | functions vvvvv

NDXLChannel* __ndxlVertexChannel(NDXLVertex* _Vertex, const size_t _Idx)
{
    return _Vertex->channels[_Idx]->channel;
}

NDXLEdge* __ndxlVertexEdge(NDXLVertex* _Vertex, const size_t _Idx)
{
    return _Vertex->edges[_Idx];
}

const char __ndxlVertexHasNeighbour(NDXLVertex* _Source, NDXLVertex* _Destination)
{
    char _Flags = 0;
    NDXLEdge* edge;
    for (size_t i = 0; i < _Source->ne; i++)
    {
        edge = _Source->edges[i];
        if (__ndxlEdgeConnectsVertex(edge, _Destination))
        {
            if (edge->directed)
            {
                if (_Source == edge->src)
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

void __ndxlVertexResizeChannelArray(NDXLVertex* _Vertex)
{
    // Verify capacity of channel array
    if (_Vertex->nc == _Vertex->ccap)
    {
        _Vertex->ccap = (_Vertex->ccap << 1) + 1;
        _Vertex->channels = (NDXLVertexChannel**)realloc(_Vertex->channels, _Vertex->ccap * sizeof(NDXLVertexChannel*));
    }
}

void __ndxlVertexResizeEdgeArray(NDXLVertex* _Vertex)
{
    // Verify capacity of edge array
    if (_Vertex->ne == _Vertex->ecap)
    {
        _Vertex->ecap = (_Vertex->ecap << 1) + 1;
        _Vertex->edges = (NDXLEdge**)realloc(_Vertex->edges, _Vertex->ecap * sizeof(NDXLEdge*));
    }
}

double* __ndxlVertexState(NDXLVertex* _Vertex)
{
    NDXLNetwork* _Network = _Vertex->network;
    return (_Network->stage ? _Network->stage_state[_Network->stage % 2] : _Network->state) + _Vertex->nidx;
}

// ^^^^^ Functions