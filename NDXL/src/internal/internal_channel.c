//------------------------------------------\\
// NDXL source file for noise channel class	\\
// Copyright Ethan Codron 2026				\\
// GNU GPL-3.0								\\
//------------------------------------------\\
// Source file providing all noise channel	\\
// functions used internally by the API.	\\
//------------------------------------------\\

#include "internal.h"

// Channel node constructors and destructors vvvvv

NDXLChannelEdge* __ndxlCreateChannelEdge(NDXLChannel* _Channel, NDXLEdge* _Edge)
{
	NDXLChannelEdge* cedge = (NDXLChannelEdge*)malloc(sizeof(NDXLChannelEdge));
	cedge->edge = _Edge;
	cedge->prev = _Channel->edge_tail;
	cedge->next = 0;
	if (_Channel->edge_head)
		_Channel->edge_tail->next = cedge;
	else
		_Channel->edge_head = cedge;
	_Channel->edge_tail = cedge;
	return cedge;
}

NDXLChannelVertex* __ndxlCreateChannelVertex(NDXLChannel* _Channel, NDXLVertex* _Vertex)
{
	NDXLChannelVertex* cvert = (NDXLChannelVertex*)malloc(sizeof(NDXLChannelVertex));
	cvert->vertex = _Vertex;
	cvert->prev = _Channel->vertex_tail;
	cvert->next = 0;
	if (_Channel->vertex_head)
		_Channel->vertex_tail->next = cvert;
	else
		_Channel->vertex_head = cvert;
	_Channel->vertex_tail = cvert;
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
	if (_Cedge->prev)
		_Cedge->prev->next = _Cedge->next;
	else
		_Cedge->echannel->channel->edge_head = _Cedge->echannel->channel->edge_head->next;
	if (_Cedge->next)
		_Cedge->next->prev = _Cedge->prev;
	else
		_Cedge->echannel->channel->edge_tail = _Cedge->echannel->channel->edge_tail->prev;
	free(_Cedge);
}

void __ndxlDestroyChannelVertex(NDXLChannelVertex* _Cvertex)
{
	if (_Cvertex->prev)
		_Cvertex->prev->next = _Cvertex->next;
	else
		_Cvertex->vchannel->channel->vertex_head = _Cvertex->vchannel->channel->vertex_head->next;
	if (_Cvertex->next)
		_Cvertex->next->prev = _Cvertex->prev;
	else
		_Cvertex->vchannel->channel->vertex_tail = _Cvertex->vchannel->channel->vertex_tail->prev;
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

// ^^^^^ Channel node constructors and destructors | channel constructor and destructor vvvvv

NDXLChannel* __ndxlCreateChannel(NDXLNetwork* _Network, void* _Params)
{
	// Initialise explicit values
	NDXLChannel* channel = (NDXLChannel*)malloc(sizeof(NDXLChannel));
	channel->network = _Network;
	channel->params = _Params;

	// Initialise implicit values
	channel->network->nnc++;
	channel->cache = 1;
	channel->nv = 0;
	channel->ne = 0;
	channel->b_head = 0;
	channel->b_tail = 0;
	channel->vertex_head = 0;
	channel->vertex_tail = 0;
	channel->prev = _Network->channel_tail;
	channel->next = 0;

	// Add to network channel stack
	if (_Network->channel_head)
		_Network->channel_tail->next = channel;
	else
		_Network->channel_head = channel;
	_Network->channel_tail = channel;

	// It is important to always remember to return your variables after creating them, I totally never forgot to do this
	return channel;
}

void __ndxlDestroyChannel(NDXLChannel* _Channel)
{
	// Decrement network channel count
	_Channel->network->nnc--;

	// De-link from vertex stacks
	NDXLChannelVertex* cvert = _Channel->vertex_head;
	while (cvert)
	{
		cvert->vertex->m--;
		cvert->vertex->rkg[0] = (double*)realloc(cvert->vertex->rkg[0], cvert->vertex->order * cvert->vertex->m * sizeof(double));
		cvert->vertex->rkg[1] = (double*)realloc(cvert->vertex->rkg[1], cvert->vertex->order * cvert->vertex->m * sizeof(double));
		NDXLChannelVertex* tmp = cvert;
		cvert = cvert->next;
		__ndxlDestroyVertexChannel(cvert->vchannel);
		__ndxlDestroyChannelVertex(tmp);
	}

	// De-link from edge stacks
	NDXLChannelEdge* cedge = _Channel->edge_head;
	while (cedge)
	{
		cedge->edge->m--;
		cedge->edge->rkg[0] = (double*)realloc(cedge->edge->rkg[0], cedge->edge->order * cedge->edge->m * sizeof(double));
		cedge->edge->rkg[1] = (double*)realloc(cedge->edge->rkg[1], cedge->edge->order * cedge->edge->m * sizeof(double));
		NDXLChannelEdge* tmp = cedge;
		cedge = cedge->next;
		__ndxlDestroyEdgeChannel(cedge->echannel);
		__ndxlDestroyChannelEdge(tmp);
	}

	// De-link from network channel stack
	if (_Channel->prev)
		_Channel->prev->next = _Channel->next;
	else
		_Channel->network->channel_head = _Channel->network->channel_head->next;
	if (_Channel->next)
		_Channel->next->prev = _Channel->prev;
	else
		_Channel->network->channel_tail = _Channel->network->channel_tail->prev;

	// Free channel memory
	free(_Channel);
}

// ^^^^^ Channel constructor and destructor | getters vvvvv

const size_t __ndxlGetChannelEdgeCount(NDXLChannel* _Channel)
{
	return _Channel->ne;
}

void __ndxlGetChannelEdges(NDXLChannel* _Channel, NDXLEdge** _Edges)
{
	size_t i = 0;
	for (NDXLChannelEdge* cedge = _Channel->edge_head; cedge; i++, cedge = cedge->next)
		_Edges[i] = cedge->edge;
}

NDXLNetwork* __ndxlGetChannelNetwork(NDXLChannel* _Channel)
{
	return _Channel->network;
}

void* __ndxlGetChannelParams(NDXLChannel* _Channel)
{
	return _Channel->params;
}

const size_t __ndxlGetChannelVertexCount(NDXLChannel* _Channel)
{
	return _Channel->nv;
}

void __ndxlGetChannelVertices(NDXLChannel* _Channel, NDXLVertex** _Vertices)
{
	size_t i = 0;
	for (NDXLChannelVertex* cvert = _Channel->vertex_head; cvert; i++, cvert = cvert->next)
		_Vertices[i] = cvert->vertex;
}

// ^^^^^ Getters | setters vvvvv

void __ndxlSetChannelParams(NDXLChannel* _Channel, void* _Params)
{
	_Channel->params = _Params;
}

// ^^^^^ Setters | channel functions vvvvv

const double __ndxlChannelNormal(NDXLChannel* _Channel)
{
	_Channel->cache = !_Channel->cache;
	if (_Channel->cache)
		return _Channel->z;

	double _U1 = __randomDouble();
	double _U2 = __randomDouble();
	while ((_U1 == 0.0) || (_U2 == 0.0))
	{
		_U1 = __randomDouble();
		_U2 = __randomDouble();
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

// ^^^^^ Channel functions