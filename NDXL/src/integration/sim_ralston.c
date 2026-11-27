#include "integration.h"

// Table initialisation vvvvv

double __butcherRalstonA[6] = { 0.4, 0.296977609247753600071, 0.158759644971035831853, 0.218100388225920467596, -3.05096514869293080535, 3.83286476046701033776 };
double __butcherRalstonB[4] = { 0.174760282262690371255, -0.551480662878732940546, 1.20553559939652353503, 0.171184781219519034263 };
double __butcherRalstonC[4] = { 0.0, 0.4, 0.455737254218789431923, 1.0 };

// ^^^^^ Table initialisation | stage equations vvvvv

const int __ndxlRalstonStage0(NDXLNetwork* _Network)
{
	static size_t i, j, nidx;
	static NDXLVertex* vert;
	static NDXLEdge* edge;

	// Iterate over vertices
	for (i = 0; i < _Network->nv; i++)
	{
		vert = _Network->vertices[i];
		if (vert->dynamics)
		{
			if (vert->dynamics(vert, _Network->rkf[0] + vert->nidx, _Network->state + vert->nidx, vert->time, vert->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRalston: external error encountered in vertex dynamics call.");
				return NDXL_ERR;
			}

			// Update network stage equation in same loop
			for (j = 0; j < vert->dimension; j++)
			{
				nidx = vert->nidx + j;
				_Network->stage_state[1][nidx] = _Network->state[nidx] + _Network->h * (__butcherRalstonA[0] * _Network->rkf[0][nidx]);
			}
		}
		else
			memcpy(_Network->stage_state[1] + vert->nidx, _Network->state + vert->nidx, vert->dimension * sizeof(double));
	}

	// Iterate over edges
	for (i = 0; i < _Network->ne; i++)
	{
		edge = _Network->edges[i];
		if (edge->dynamics)
		{
			if (edge->dynamics(edge, _Network->rkf[0] + edge->nidx, _Network->state + edge->nidx, edge->time, edge->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRalston: external error encountered in edge dynamics call.");
				return NDXL_ERR;
			}

			// Update network stage equation in same loop
			for (j = 0; j < edge->dimension; j++)
			{
				nidx = edge->nidx + j;
				_Network->stage_state[1][nidx] = _Network->state[nidx] + _Network->h * (__butcherRalstonA[0] * _Network->rkf[0][nidx]);
			}
		}
		else
			memcpy(_Network->stage_state[1] + edge->nidx, _Network->state + edge->nidx, edge->dimension * sizeof(double));
	}

	_Network->stage = 1;
	return NDXL_OK;
}

const int __ndxlRalstonStage1(NDXLNetwork* _Network)
{
	static size_t i, j, nidx;
	static NDXLVertex* vert;
	static NDXLEdge* edge;

	// Iterate over vertices
	for (i = 0; i < _Network->nv; i++)
	{
		vert = _Network->vertices[i];
		if (vert->dynamics)
		{
			if (vert->dynamics(vert, _Network->rkf[1] + vert->nidx, _Network->stage_state[1] + vert->nidx, vert->time + __butcherRalstonC[1] * _Network->h, vert->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRalston: external error encountered in vertex dynamics call.");
				return NDXL_ERR;
			}

			// Update network stage equation in same loop
			for (j = 0; j < vert->dimension; j++)
			{
				nidx = vert->nidx + j;
				_Network->stage_state[0][nidx] = _Network->state[nidx] + _Network->h * (__butcherRalstonA[1] * _Network->rkf[0][nidx] + __butcherRalstonA[2] * _Network->rkf[1][nidx]);
			}
		}
		else
			memcpy(_Network->stage_state[0] + vert->nidx, _Network->state + vert->nidx, vert->dimension * sizeof(double));
	}

	// Iterate over edges
	for (i = 0; i < _Network->ne; i++)
	{
		edge = _Network->edges[i];
		if (edge->dynamics)
		{
			if (edge->dynamics(edge, _Network->rkf[1] + edge->nidx, _Network->stage_state[1] + edge->nidx, edge->time + __butcherRalstonC[1] * _Network->h, edge->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRalston: external error encountered in edge dynamics call.");
				return NDXL_ERR;
			}

			// Update network stage equation in same loop
			for (j = 0; j < edge->dimension; j++)
			{
				nidx = edge->nidx + j;
				_Network->stage_state[0][nidx] = _Network->state[nidx] + _Network->h * (__butcherRalstonA[1] * _Network->rkf[0][nidx] + __butcherRalstonA[2] * _Network->rkf[1][nidx]);
			}
		}
		else
			memcpy(_Network->stage_state[0] + edge->nidx, _Network->state + edge->nidx, edge->dimension * sizeof(double));
	}

	_Network->stage = 2;
	return NDXL_OK;
}

const int __ndxlRalstonStage2(NDXLNetwork* _Network)
{
	static size_t i, j, nidx;
	static NDXLVertex* vert;
	static NDXLEdge* edge;

	// Iterate over vertices
	for (i = 0; i < _Network->nv; i++)
	{
		vert = _Network->vertices[i];
		if (vert->dynamics)
		{
			if (vert->dynamics(vert, _Network->rkf[2] + vert->nidx, _Network->stage_state[0] + vert->nidx, vert->time + __butcherRalstonC[2] * _Network->h, vert->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRalston: external error encountered in vertex dynamics call.");
				return NDXL_ERR;
			}

			// Update network stage equation in same loop
			for (j = 0; j < vert->dimension; j++)
			{
				nidx = vert->nidx + j;
				_Network->stage_state[1][nidx] = _Network->state[nidx] + _Network->h * (__butcherRalstonA[3] * _Network->rkf[0][nidx] + __butcherRalstonA[4] * _Network->rkf[1][nidx] + __butcherRalstonA[5] * _Network->rkf[2][nidx]);
			}
		}
		else
			memcpy(_Network->stage_state[1] + vert->nidx, _Network->state + vert->nidx, vert->dimension * sizeof(double));
	}

	// Iterate over edges
	for (i = 0; i < _Network->ne; i++)
	{
		edge = _Network->edges[i];
		if (edge->dynamics)
		{
			if (edge->dynamics(edge, _Network->rkf[2] + edge->nidx, _Network->stage_state[0] + edge->nidx, edge->time + __butcherRalstonC[2] * _Network->h, edge->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRalston: external error encountered in edge dynamics call.");
				return NDXL_ERR;
			}

			// Update network stage equation in same loop
			for (j = 0; j < edge->dimension; j++)
			{
				nidx = edge->nidx + j;
				_Network->stage_state[1][nidx] = _Network->state[nidx] + _Network->h * (__butcherRalstonA[3] * _Network->rkf[0][nidx] + __butcherRalstonA[4] * _Network->rkf[1][nidx] + __butcherRalstonA[5] * _Network->rkf[2][nidx]);
			}
		}
		else
			memcpy(_Network->stage_state[1] + edge->nidx, _Network->state + edge->nidx, edge->dimension * sizeof(double));
	}

	_Network->stage = 3;
	return NDXL_OK;
}

const int __ndxlRalstonStage3(NDXLNetwork* _Network)
{
	static size_t i, j, nidx;
	static NDXLVertex* vert;
	static NDXLEdge* edge;

	// Iterate over vertices
	for (i = 0; i < _Network->nv; i++)
	{
		vert = _Network->vertices[i];
		if (vert->dynamics)
		{
			if (vert->dynamics(vert, _Network->rkf[3] + vert->nidx, _Network->stage_state[1] + vert->nidx, vert->time + __butcherRalstonC[3] * _Network->h, vert->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRalston: external error encountered in vertex dynamics call.");
				return NDXL_ERR;
			}

			// Update network stage equation in same loop
			for (j = 0; j < vert->dimension; j++)
			{
				nidx = vert->nidx + j;
				_Network->stage_state[0][nidx] = _Network->state[nidx] + _Network->h * (__butcherRalstonB[0] * _Network->rkf[0][nidx] + __butcherRalstonB[1] * _Network->rkf[1][nidx] + __butcherRalstonB[2] * _Network->rkf[2][nidx] + __butcherRalstonB[3] * _Network->rkf[3][nidx]);
			}
		}
		else
			memcpy(_Network->stage_state[0] + vert->nidx, _Network->state + vert->nidx, vert->dimension * sizeof(double));
	}

	// Iterate over edges
	for (i = 0; i < _Network->ne; i++)
	{
		edge = _Network->edges[i];
		if (edge->dynamics)
		{
			if (edge->dynamics(edge, _Network->rkf[3] + edge->nidx, _Network->stage_state[1] + edge->nidx, edge->time + __butcherRalstonC[3] * _Network->h, edge->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRalston: external error encountered in edge dynamics call.");
				return NDXL_ERR;
			}

			// Update network stage equation in same loop
			for (j = 0; j < edge->dimension; j++)
			{
				nidx = edge->nidx + j;
				_Network->stage_state[0][nidx] = _Network->state[nidx] + _Network->h * (__butcherRalstonB[0] * _Network->rkf[0][nidx] + __butcherRalstonB[1] * _Network->rkf[1][nidx] + __butcherRalstonB[2] * _Network->rkf[2][nidx] + __butcherRalstonB[3] * _Network->rkf[3][nidx]);
			}
		}
		else
			memcpy(_Network->stage_state[0] + edge->nidx, _Network->state + edge->nidx, edge->dimension * sizeof(double));
	}

	_Network->stage = 0;
	return NDXL_OK;
}

// ^^^^^ Stage equations