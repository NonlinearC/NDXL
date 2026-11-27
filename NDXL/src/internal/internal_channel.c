#include "internal.h"

// Constructors and destructors vvvvv

NDXLChannelEdge* __ndxlCreateChannelEdge(NDXLChannel* _Channel, NDXLEdge* _Edge)
{
	NDXLChannelEdge* cedge = (NDXLChannelEdge*)malloc(sizeof(NDXLChannelEdge));
	cedge->edge = _Edge;
	cedge->cidx = _Edge->nc;
	return cedge;
}

NDXLChannelVertex* __ndxlCreateChannelVertex(NDXLChannel* _Channel, NDXLVertex* _Vertex)
{
	NDXLChannelVertex* cvert = (NDXLChannelVertex*)malloc(sizeof(NDXLChannelVertex));
	cvert->vertex = _Vertex;
	cvert->cidx = _Vertex->nc;
	return cvert;
}

NDXLChannelIncrement* __ndxlCreateChannelIncrement(NDXLChannel* _Channel, const double _Hold, const double _Hnew)
{
	const double _Hratio = _Hnew / _Hold;
	const double _dWleft = _Channel->dw * _Hratio + sqrt(_Hratio * (_Hold - _Hnew)) * __ndxlChannelNormal(_Channel);
	NDXLChannelIncrement* increment = (NDXLChannelIncrement*)malloc(sizeof(NDXLChannelIncrement));
	increment->dw = _Channel->dw - _dWleft;
	increment->h = _Hold - _Hnew;
	increment->channel = _Channel;
	increment->prev = _Channel->b_tail;
	increment->next = 0;
	if (_Channel->b_head)
		_Channel->b_tail->next = increment;
	else
		_Channel->b_head = increment;
	_Channel->b_tail = increment;
	_Channel->dw = _dWleft;
	return increment;
}

void __ndxlDestroyChannelEdge(NDXLChannelEdge* _Cedge)
{
	free(_Cedge);
}

void __ndxlDestroyChannelVertex(NDXLChannelVertex* _Cvertex)
{
	free(_Cvertex);
}

void __ndxlDestroyChannelIncrement(NDXLChannelIncrement* _Increment)
{
	if (_Increment->prev)
		_Increment->prev->next = _Increment->next;
	else
		_Increment->channel->b_head = _Increment->next;
	if (_Increment->next)
		_Increment->next->prev = _Increment->prev;
	else
		_Increment->channel->b_tail = _Increment->prev;
	free(_Increment);
}

NDXLChannel* __ndxlCreateChannel(NDXLNetwork* _Network, void* _Params)
{
	// Initialise explicit values
	NDXLChannel* channel = (NDXLChannel*)malloc(sizeof(NDXLChannel));
	channel->network = _Network;
	channel->params = _Params;

	// Initialise implicit values
	channel->cache = 1;
	channel->z = 0.0;
	channel->dw = 0.0;
	channel->idx = _Network->cidx;
	channel->cidx = _Network->nc;
	channel->nv = 0;
	channel->vcap = 7;
	channel->ne = 0;
	channel->ecap = 7;
	channel->vertices = (NDXLChannelVertex**)calloc(channel->vcap, sizeof(NDXLChannelVertex*));
	channel->edges = (NDXLChannelEdge**)calloc(channel->ecap, sizeof(NDXLChannelEdge*));
	channel->b_head = 0;
	channel->b_tail = 0;
	
	// Add to network channel array
	_Network->cidx++;
	__ndxlNetworkResizeArrays(_Network);
	_Network->channels[_Network->nc++] = channel;

	// Return created channel
	return channel;
}

void __ndxlDestroyChannel(NDXLChannel* _Channel)
{
	// Remove from network array
	NDXLNetwork* _Network = _Channel->network;
	size_t i;
	for (i = _Channel->cidx + 1; i < _Network->nc; i++)
	{
		_Network->channels[i]->cidx--;
		_Network->channels[i - 1] = _Network->channels[i];
	}
	_Network->channels[--_Network->nc] = 0;

	// Remove from vertex arrays
	size_t j;
	NDXLChannelVertex* cvert;
	NDXLChannelEdge* cedge;
	for (i = 0; i < _Channel->nv; i++)
	{
		cvert = _Channel->vertices[i];
		for (j = cvert->cidx + 1; j < cvert->vertex->nc; j++)
		{
			cvert->vertex->channels[j]->vidx--;
			cvert->vertex->channels[j - 1] = cvert->vertex->channels[j];
		}
		cvert->vertex->channels[--cvert->vertex->nc] = 0;
	}
	for (i = 0; i < _Channel->ne; i++)
	{
		cedge = _Channel->edges[i];
		for (j = cedge->cidx + 1; j < cedge->edge->nc; j++)
		{
			cedge->edge->channels[j]->eidx--;
			cedge->edge->channels[j - 1] = cedge->edge->channels[j];
		}
		cedge->edge->channels[--cedge->edge->nc] = 0;
	}

	// Finally, free channel memory
	for (i = 0; i < _Channel->nv; i++)
		__ndxlDestroyChannelVertex(_Channel->vertices[i]);
	for (i = 0; i < _Channel->ne; i++)
		__ndxlDestroyChannelEdge(_Channel->edges[i]);
	free(_Channel->vertices);
	free(_Channel->edges);
}

// ^^^^^ Constructors and destructors | getters vvvvv

const size_t __ndxlGetChannelEdgeDimension(NDXLChannel* _Channel)
{
	return _Channel->ne;
}

NDXLNetwork* __ndxlGetChannelNetwork(NDXLChannel* _Channel)
{
	return _Channel->network;
}

void* __ndxlGetChannelParams(NDXLChannel* _Channel)
{
	return _Channel->params;
}

const size_t __ndxlGetChannelVertexDimension(NDXLChannel* _Channel)
{
	return _Channel->nv;
}

// ^^^^^ Getters | setters vvvvv

void __ndxlSetChannelParams(NDXLChannel* _Channel, void* _Params)
{
	_Channel->params = _Params;
}

// ^^^^^ Setters | functions vvvvv

NDXLEdge* __ndxlChannelEdge(NDXLChannel* _Channel, const size_t _Idx)
{
	return _Channel->edges[_Idx]->edge;
}

const double __ndxlChannelNormal(NDXLChannel* _Channel)
{
	_Channel->cache = !_Channel->cache;
	if (_Channel->cache)
		return _Channel->z;

	double _U1 = __ndxlRandomDouble();
	double _U2 = __ndxlRandomDouble();
	while ((_U1 == 0.0) || (_U2 == 0.0))
	{
		_U1 = __ndxlRandomDouble();
		_U2 = __ndxlRandomDouble();
	}

	_Channel->z = sqrt(-2.0 * log(_U1)) * sin(TWO_PI * _U2);
	return sqrt(-2.0 * log(_U1)) * cos(TWO_PI * _U2);
}

const double __ndxlChannelRedraw(NDXLChannel* _Channel)
{
	// If has RSwM stack, use right dW, otherwise redraw fresh increment
	NDXLChannelIncrement* _Tmp = _Channel->b_tail;
	double _H = _Channel->network->h;
	if (_Tmp)
	{
		_H = _Tmp->h;
		_Channel->dw = _Channel->b_tail->dw;
		__ndxlDestroyChannelIncrement(_Tmp);
	}
	else
		_Channel->dw = __ndxlChannelNormal(_Channel) * _Channel->network->s;
	return _H;
}

void __ndxlChannelResizeEdgeArray(NDXLChannel* _Channel)
{
	// Verify capacity of channel array
	if (_Channel->ne == _Channel->ecap)
	{
		_Channel->ecap = (_Channel->ecap << 1) + 1;
		_Channel->edges = (NDXLChannelEdge**)realloc(_Channel->edges, _Channel->ecap * sizeof(NDXLChannelEdge*));
	}
}

void __ndxlChannelResizeVertexArray(NDXLChannel* _Channel)
{
	// Verify capacity of channel array
	if (_Channel->nv == _Channel->vcap)
	{
		_Channel->vcap = (_Channel->vcap << 1) + 1;
		_Channel->vertices = (NDXLChannelVertex**)realloc(_Channel->vertices, _Channel->vcap * sizeof(NDXLChannelVertex*));
	}
}

NDXLVertex* __ndxlChannelVertex(NDXLChannel* _Channel, const size_t _Idx)
{
	return _Channel->vertices[_Idx]->vertex;
}

// ^^^^^ Functions