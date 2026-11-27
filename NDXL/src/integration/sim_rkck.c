#include "integration.h"

// Table initialisation vvvvv

double __butcherCashKarpA[15] = { 0.2, 0.075, 0.225, 0.3, -0.9, 1.2, -0.2037037037037037037037, 2.5, -2.59259259259259259259, 1.296296296296296296296, 0.0294958043981481481481, 0.341796875, 0.0415943287037037037037, 0.400345413773148148148, 0.061767578125 };
double __butcherCashKarpB5[4] = { 0.0978835978835978835979, 0.402576489533011272142, 0.21043771043771043771, 0.28910220214568040655 };
double __butcherCashKarpB4[5] = { 0.102177372685185185185, 0.383907903439153439153, 0.244592737268518518519, 0.0193219866071428571429, 0.25 };
double __butcherCashKarpC[6] = { 0.0, 0.2, 0.3, 0.6, 1.0, 0.875 };

// ^^^^^ Table initialisation | stage equations vvvvv

const int __ndxlRkckStage0(NDXLNetwork* _Network)
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
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRkck: external error encountered in vertex dynamics call.");
				return NDXL_ERR;
			}

			// Update network stage equation in same loop
			for (j = 0; j < vert->dimension; j++)
			{
				nidx = vert->nidx + j;
				_Network->stage_state[1][nidx] = _Network->state[nidx] + _Network->h * (__butcherCashKarpA[0] * _Network->rkf[0][nidx]);
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
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRkck: external error encountered in edge dynamics call.");
				return NDXL_ERR;
			}

			// Update network stage equation in same loop
			for (j = 0; j < edge->dimension; j++)
			{
				nidx = edge->nidx + j;
				_Network->stage_state[1][nidx] = _Network->state[nidx] + _Network->h * (__butcherCashKarpA[0] * _Network->rkf[0][nidx]);
			}
		}
		else
			memcpy(_Network->stage_state[1] + edge->nidx, _Network->state + edge->nidx, edge->dimension * sizeof(double));
	}

	_Network->stage = 1;
	return NDXL_OK;
}

const int __ndxlRkckStage1(NDXLNetwork* _Network)
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
			if (vert->dynamics(vert, _Network->rkf[1] + vert->nidx, _Network->stage_state[1] + vert->nidx, vert->time + __butcherCashKarpC[1] * _Network->h, vert->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRkck: external error encountered in vertex dynamics call.");
				return NDXL_ERR;
			}

			// Update network stage equation in same loop
			for (j = 0; j < vert->dimension; j++)
			{
				nidx = vert->nidx + j;
				_Network->stage_state[0][nidx] = _Network->state[nidx] + _Network->h * (__butcherCashKarpA[1] * _Network->rkf[0][nidx] + __butcherCashKarpA[2] * _Network->rkf[1][nidx]);
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
			if (edge->dynamics(edge, _Network->rkf[1] + edge->nidx, _Network->stage_state[1] + edge->nidx, edge->time + __butcherCashKarpC[1] * _Network->h, edge->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRkck: external error encountered in edge dynamics call.");
				return NDXL_ERR;
			}

			// Update network stage equation in same loop
			for (j = 0; j < edge->dimension; j++)
			{
				nidx = edge->nidx + j;
				_Network->stage_state[0][nidx] = _Network->state[nidx] + _Network->h * (__butcherCashKarpA[1] * _Network->rkf[0][nidx] + __butcherCashKarpA[2] * _Network->rkf[1][nidx]);
			}
		}
		else
			memcpy(_Network->stage_state[0] + edge->nidx, _Network->state + edge->nidx, edge->dimension * sizeof(double));
	}

	_Network->stage = 2;
	return NDXL_OK;
}

const int __ndxlRkckStage2(NDXLNetwork* _Network)
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
			if (vert->dynamics(vert, _Network->rkf[2] + vert->nidx, _Network->stage_state[0] + vert->nidx, vert->time + __butcherCashKarpC[2] * _Network->h, vert->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRkck: external error encountered in vertex dynamics call.");
				return NDXL_ERR;
			}

			// Update network stage equation in same loop
			for (j = 0; j < vert->dimension; j++)
			{
				nidx = vert->nidx + j;
				_Network->stage_state[1][nidx] = _Network->state[nidx] + _Network->h * (__butcherCashKarpA[3] * _Network->rkf[0][nidx] + __butcherCashKarpA[4] * _Network->rkf[1][nidx] + __butcherCashKarpA[5] * _Network->rkf[2][nidx]);
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
			if (edge->dynamics(edge, _Network->rkf[2] + edge->nidx, _Network->stage_state[0] + edge->nidx, edge->time + __butcherCashKarpC[2] * _Network->h, edge->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRkck: external error encountered in edge dynamics call.");
				return NDXL_ERR;
			}

			// Update network stage equation in same loop
			for (j = 0; j < edge->dimension; j++)
			{
				nidx = edge->nidx + j;
				_Network->stage_state[1][nidx] = _Network->state[nidx] + _Network->h * (__butcherCashKarpA[3] * _Network->rkf[0][nidx] + __butcherCashKarpA[4] * _Network->rkf[1][nidx] + __butcherCashKarpA[5] * _Network->rkf[2][nidx]);
			}
		}
		else
			memcpy(_Network->stage_state[1] + edge->nidx, _Network->state + edge->nidx, edge->dimension * sizeof(double));
	}

	_Network->stage = 3;
	return NDXL_OK;
}

const int __ndxlRkckStage3(NDXLNetwork* _Network)
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
			if (vert->dynamics(vert, _Network->rkf[3] + vert->nidx, _Network->stage_state[1] + vert->nidx, vert->time + __butcherCashKarpC[3] * _Network->h, vert->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRkck: external error encountered in vertex dynamics call.");
				return NDXL_ERR;
			}

			// Update network stage equation in same loop
			for (j = 0; j < vert->dimension; j++)
			{
				nidx = vert->nidx + j;
				_Network->stage_state[0][nidx] = _Network->state[nidx] + _Network->h * (__butcherCashKarpA[6] * _Network->rkf[0][nidx] + __butcherCashKarpA[7] * _Network->rkf[1][nidx] + __butcherCashKarpA[8] * _Network->rkf[2][nidx] + __butcherCashKarpA[9] * _Network->rkf[3][nidx]);
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
			if (edge->dynamics(edge, _Network->rkf[3] + edge->nidx, _Network->stage_state[1] + edge->nidx, edge->time + __butcherCashKarpC[3] * _Network->h, edge->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRkck: external error encountered in edge dynamics call.");
				return NDXL_ERR;
			}

			// Update network stage equation in same loop
			for (j = 0; j < edge->dimension; j++)
			{
				nidx = edge->nidx + j;
				_Network->stage_state[0][nidx] = _Network->state[nidx] + _Network->h * (__butcherCashKarpA[6] * _Network->rkf[0][nidx] + __butcherCashKarpA[7] * _Network->rkf[1][nidx] + __butcherCashKarpA[8] * _Network->rkf[2][nidx] + __butcherCashKarpA[9] * _Network->rkf[3][nidx]);
			}
		}
		else
			memcpy(_Network->stage_state[0] + edge->nidx, _Network->state + edge->nidx, edge->dimension * sizeof(double));
	}

	_Network->stage = 4;
	return NDXL_OK;
}

const int __ndxlRkckStage4(NDXLNetwork* _Network)
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
			if (vert->dynamics(vert, _Network->rkf[4] + vert->nidx, _Network->stage_state[0] + vert->nidx, vert->time + __butcherCashKarpC[4] * _Network->h, vert->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRkck: external error encountered in vertex dynamics call.");
				return NDXL_ERR;
			}

			// Update network stage equation in same loop
			for (j = 0; j < vert->dimension; j++)
			{
				nidx = vert->nidx + j;
				_Network->stage_state[1][nidx] = _Network->state[nidx] + _Network->h * (__butcherCashKarpA[10] * _Network->rkf[0][nidx] + __butcherCashKarpA[11] * _Network->rkf[1][nidx] + __butcherCashKarpA[12] * _Network->rkf[2][nidx] + __butcherCashKarpA[13] * _Network->rkf[3][nidx] + __butcherCashKarpA[14] * _Network->rkf[4][nidx]);
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
			if (edge->dynamics(edge, _Network->rkf[4] + edge->nidx, _Network->stage_state[0] + edge->nidx, edge->time + __butcherCashKarpC[4] * _Network->h, edge->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRkck: external error encountered in edge dynamics call.");
				return NDXL_ERR;
			}

			// Update network stage equation in same loop
			for (j = 0; j < edge->dimension; j++)
			{
				nidx = edge->nidx + j;
				_Network->stage_state[1][nidx] = _Network->state[nidx] + _Network->h * (__butcherCashKarpA[10] * _Network->rkf[0][nidx] + __butcherCashKarpA[11] * _Network->rkf[1][nidx] + __butcherCashKarpA[12] * _Network->rkf[2][nidx] + __butcherCashKarpA[13] * _Network->rkf[3][nidx] + __butcherCashKarpA[14] * _Network->rkf[4][nidx]);
			}
		}
		else
			memcpy(_Network->stage_state[1] + edge->nidx, _Network->state + edge->nidx, edge->dimension * sizeof(double));
	}

	_Network->stage = 5;
	return NDXL_OK;
}

const int __ndxlRkckStage5(NDXLNetwork* _Network, double* _Errsum)
{
	static size_t i, j, nidx;
	static NDXLVertex* vert;
	static NDXLEdge* edge;
	static double atol, rtol, err;

	// Iterate over vertices
	for (i = 0; i < _Network->nv; i++)
	{
		vert = _Network->vertices[i];
		if (vert->dynamics)
		{
			if (vert->dynamics(vert, _Network->rkf[5] + vert->nidx, _Network->stage_state[1] + vert->nidx, vert->time + __butcherCashKarpC[5] * _Network->h, vert->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRkck: external error encountered in vertex dynamics call.");
				return NDXL_ERR;
			}

			// Update network error in same loop
			atol = (vert->atol ? vert->atol : _Network->atol);
			rtol = (vert->rtol ? vert->rtol : _Network->rtol);
			for (j = 0; j < vert->dimension; j++)
			{
				nidx = vert->nidx + j;
				_Network->stage_state[0][nidx] = _Network->state[nidx] + _Network->h * (__butcherCashKarpB5[0] * _Network->rkf[0][nidx] + __butcherCashKarpB5[1] * _Network->rkf[2][nidx] + __butcherCashKarpB5[2] * _Network->rkf[3][nidx] + __butcherCashKarpB5[3] * _Network->rkf[5][nidx]);
				_Network->rkf[7][nidx] = _Network->state[nidx] + _Network->h * (__butcherCashKarpB4[0] * _Network->rkf[0][nidx] + __butcherCashKarpB4[1] * _Network->rkf[2][nidx] + __butcherCashKarpB4[2] * _Network->rkf[3][nidx] + __butcherCashKarpB4[3] * _Network->rkf[4][nidx] + __butcherCashKarpB4[4] * _Network->rkf[5][nidx]);
				err = (_Network->stage_state[0][nidx] - _Network->rkf[7][nidx]) / (atol + rtol * fmax(fabs(_Network->stage_state[0][nidx]), fabs(_Network->state[nidx])));
				*_Errsum += (err * err);
			}
		}
		else
		{
			memcpy(_Network->stage_state[0] + vert->nidx, _Network->state + vert->nidx, vert->dimension * sizeof(double));
			memcpy(_Network->rkf[7] + vert->nidx, _Network->state + vert->nidx, vert->dimension * sizeof(double));
		}
	}

	// Iterate over edges
	for (i = 0; i < _Network->ne; i++)
	{
		edge = _Network->edges[i];
		if (edge->dynamics)
		{
			if (edge->dynamics(edge, _Network->rkf[5] + edge->nidx, _Network->stage_state[1] + edge->nidx, edge->time + __butcherCashKarpC[5] * _Network->h, edge->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRkck: external error encountered in edge dynamics call.");
				return NDXL_ERR;
			}

			// Update network error in same loop
			atol = (edge->atol ? edge->atol : _Network->atol);
			rtol = (edge->rtol ? edge->rtol : _Network->rtol);
			for (j = 0; j < edge->dimension; j++)
			{
				nidx = edge->nidx + j;
				_Network->stage_state[0][nidx] = _Network->state[nidx] + _Network->h * (__butcherCashKarpB5[0] * _Network->rkf[0][nidx] + __butcherCashKarpB5[1] * _Network->rkf[2][nidx] + __butcherCashKarpB5[2] * _Network->rkf[3][nidx] + __butcherCashKarpB5[3] * _Network->rkf[5][nidx]);
				_Network->rkf[7][nidx] = _Network->state[nidx] + _Network->h * (__butcherCashKarpB4[0] * _Network->rkf[0][nidx] + __butcherCashKarpB4[1] * _Network->rkf[2][nidx] + __butcherCashKarpB4[2] * _Network->rkf[3][nidx] + __butcherCashKarpB4[3] * _Network->rkf[4][nidx] + __butcherCashKarpB4[4] * _Network->rkf[5][nidx]);
				err = (_Network->stage_state[0][nidx] - _Network->rkf[7][nidx]) / (atol + rtol * fmax(fabs(_Network->stage_state[0][nidx]), fabs(_Network->state[nidx])));
				*_Errsum += (err * err);
			}
		}
		else
		{
			memcpy(_Network->stage_state[0] + edge->nidx, _Network->state + edge->nidx, edge->dimension * sizeof(double));
			memcpy(_Network->rkf[7] + edge->nidx, _Network->state + edge->nidx, edge->dimension * sizeof(double));
		}
	}

	_Network->stage = 0;
	return NDXL_OK;
}

// ^^^^^ Stage equations