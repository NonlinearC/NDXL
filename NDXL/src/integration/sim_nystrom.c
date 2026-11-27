#include "integration.h"

// Table initialisation vvvvv

double __butcherNystromA[14] = { 0.333333333333333333333, 0.16, 0.24, 0.25, -3.0, 3.75, 0.074074074074074074074074, 1.11111111111111111111, -0.617283950617283950617, 0.0987654320987654320988, 0.08, 0.48, 0.133333333333333333333, 0.106666666666666666667 };
double __butcherNystromB[4] = { 0.119791666666666666667, 0.651041666666666666667, -0.421875, 0.651041666666666666667 };
double __butcherNystromC[6] = { 0.0, 0.333333333333333333333, 0.4, 1.0, 0.666666666666666666667, 0.8 };

// ^^^^^ Table initialisation | stage equations vvvvv

const int __ndxlNystromStage0(NDXLNetwork* _Network)
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
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateNystrom: external error encountered in vertex dynamics call.");
				return NDXL_ERR;
			}

			// Update network stage equation in same loop
			for (j = 0; j < vert->dimension; j++)
			{
				nidx = vert->nidx + j;
				_Network->stage_state[1][nidx] = _Network->state[nidx] + _Network->h * (__butcherNystromA[0] * _Network->rkf[0][nidx]);
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
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateNystrom: external error encountered in edge dynamics call.");
				return NDXL_ERR;
			}

			// Update network stage equation in same loop
			for (j = 0; j < edge->dimension; j++)
			{
				nidx = edge->nidx + j;
				_Network->stage_state[1][nidx] = _Network->state[nidx] + _Network->h * (__butcherNystromA[0] * _Network->rkf[0][nidx]);
			}
		}
		else
			memcpy(_Network->stage_state[1] + edge->nidx, _Network->state + edge->nidx, edge->dimension * sizeof(double));
	}

	_Network->stage = 1;
	return NDXL_OK;
}

const int __ndxlNystromStage1(NDXLNetwork* _Network)
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
			if (vert->dynamics(vert, _Network->rkf[1] + vert->nidx, _Network->stage_state[1] + vert->nidx, vert->time + __butcherNystromC[1] * _Network->h, vert->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateNystrom: external error encountered in vertex dynamics call.");
				return NDXL_ERR;
			}

			// Update network stage equation in same loop
			for (j = 0; j < vert->dimension; j++)
			{
				nidx = vert->nidx + j;
				_Network->stage_state[0][nidx] = _Network->state[nidx] + _Network->h * (__butcherNystromA[1] * _Network->rkf[0][nidx] + __butcherNystromA[2] * _Network->rkf[1][nidx]);
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
			if (edge->dynamics(edge, _Network->rkf[1] + edge->nidx, _Network->stage_state[1] + edge->nidx, edge->time + __butcherNystromC[1] * _Network->h, edge->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateNystrom: external error encountered in edge dynamics call.");
				return NDXL_ERR;
			}

			// Update network stage equation in same loop
			for (j = 0; j < edge->dimension; j++)
			{
				nidx = edge->nidx + j;
				_Network->stage_state[0][nidx] = _Network->state[nidx] + _Network->h * (__butcherNystromA[1] * _Network->rkf[0][nidx] + __butcherNystromA[2] * _Network->rkf[1][nidx]);
			}
		}
		else
			memcpy(_Network->stage_state[0] + edge->nidx, _Network->state + edge->nidx, edge->dimension * sizeof(double));
	}

	_Network->stage = 2;
	return NDXL_OK;
}

const int __ndxlNystromStage2(NDXLNetwork* _Network)
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
			if (vert->dynamics(vert, _Network->rkf[2] + vert->nidx, _Network->stage_state[0] + vert->nidx, vert->time + __butcherNystromC[2] * _Network->h, vert->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateNystrom: external error encountered in vertex dynamics call.");
				return NDXL_ERR;
			}

			// Update network stage equation in same loop
			for (j = 0; j < vert->dimension; j++)
			{
				nidx = vert->nidx + j;
				_Network->stage_state[1][nidx] = _Network->state[nidx] + _Network->h * (__butcherNystromA[3] * _Network->rkf[0][nidx] + __butcherNystromA[4] * _Network->rkf[1][nidx] + __butcherNystromA[5] * _Network->rkf[2][nidx]);
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
			if (edge->dynamics(edge, _Network->rkf[2] + edge->nidx, _Network->stage_state[0] + edge->nidx, edge->time + __butcherNystromC[2] * _Network->h, edge->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateNystrom: external error encountered in edge dynamics call.");
				return NDXL_ERR;
			}

			// Update network stage equation in same loop
			for (j = 0; j < edge->dimension; j++)
			{
				nidx = edge->nidx + j;
				_Network->stage_state[1][nidx] = _Network->state[nidx] + _Network->h * (__butcherNystromA[3] * _Network->rkf[0][nidx] + __butcherNystromA[4] * _Network->rkf[1][nidx] + __butcherNystromA[5] * _Network->rkf[2][nidx]);
			}
		}
		else
			memcpy(_Network->stage_state[1] + edge->nidx, _Network->state + edge->nidx, edge->dimension * sizeof(double));
	}

	_Network->stage = 3;
	return NDXL_OK;
}

const int __ndxlNystromStage3(NDXLNetwork* _Network)
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
			if (vert->dynamics(vert, _Network->rkf[3] + vert->nidx, _Network->stage_state[1] + vert->nidx, vert->time + __butcherNystromC[3] * _Network->h, vert->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateNystrom: external error encountered in vertex dynamics call.");
				return NDXL_ERR;
			}

			// Update network stage equation in same loop
			for (j = 0; j < vert->dimension; j++)
			{
				nidx = vert->nidx + j;
				_Network->stage_state[0][nidx] = _Network->state[nidx] + _Network->h * (__butcherNystromA[6] * _Network->rkf[0][nidx] + __butcherNystromA[7] * _Network->rkf[1][nidx] + __butcherNystromA[8] * _Network->rkf[2][nidx] + __butcherNystromA[9] * _Network->rkf[3][nidx]);
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
			if (edge->dynamics(edge, _Network->rkf[3] + edge->nidx, _Network->stage_state[1] + edge->nidx, edge->time + __butcherNystromC[3] * _Network->h, edge->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateNystrom: external error encountered in edge dynamics call.");
				return NDXL_ERR;
			}

			// Update network stage equation in same loop
			for (j = 0; j < edge->dimension; j++)
			{
				nidx = edge->nidx + j;
				_Network->stage_state[0][nidx] = _Network->state[nidx] + _Network->h * (__butcherNystromA[6] * _Network->rkf[0][nidx] + __butcherNystromA[7] * _Network->rkf[1][nidx] + __butcherNystromA[8] * _Network->rkf[2][nidx] + __butcherNystromA[9] * _Network->rkf[3][nidx]);
			}
		}
		else
			memcpy(_Network->stage_state[0] + edge->nidx, _Network->state + edge->nidx, edge->dimension * sizeof(double));
	}

	_Network->stage = 4;
	return NDXL_OK;
}

const int __ndxlNystromStage4(NDXLNetwork* _Network)
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
			if (vert->dynamics(vert, _Network->rkf[4] + vert->nidx, _Network->stage_state[0] + vert->nidx, vert->time + __butcherNystromC[4] * _Network->h, vert->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateNystrom: external error encountered in vertex dynamics call.");
				return NDXL_ERR;
			}

			// Update network stage equation in same loop
			for (j = 0; j < vert->dimension; j++)
			{
				nidx = vert->nidx + j;
				_Network->stage_state[1][nidx] = _Network->state[nidx] + _Network->h * (__butcherNystromA[10] * _Network->rkf[0][nidx] + __butcherNystromA[11] * _Network->rkf[1][nidx] + __butcherNystromA[12] * _Network->rkf[2][nidx] + __butcherNystromA[13] * _Network->rkf[3][nidx]);
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
			if (edge->dynamics(edge, _Network->rkf[4] + edge->nidx, _Network->stage_state[0] + edge->nidx, edge->time + __butcherNystromC[4] * _Network->h, edge->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateNystrom: external error encountered in edge dynamics call.");
				return NDXL_ERR;
			}

			// Update network stage equation in same loop
			for (j = 0; j < edge->dimension; j++)
			{
				nidx = edge->nidx + j;
				_Network->stage_state[1][nidx] = _Network->state[nidx] + _Network->h * (__butcherNystromA[10] * _Network->rkf[0][nidx] + __butcherNystromA[11] * _Network->rkf[1][nidx] + __butcherNystromA[12] * _Network->rkf[2][nidx] + __butcherNystromA[13] * _Network->rkf[3][nidx]);
			}
		}
		else
			memcpy(_Network->stage_state[1] + edge->nidx, _Network->state + edge->nidx, edge->dimension * sizeof(double));
	}

	_Network->stage = 5;
	return NDXL_OK;
}

const int __ndxlNystromStage5(NDXLNetwork* _Network)
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
			if (vert->dynamics(vert, _Network->rkf[5] + vert->nidx, _Network->stage_state[1] + vert->nidx, vert->time + __butcherNystromC[5] * _Network->h, vert->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateNystrom: external error encountered in vertex dynamics call.");
				return NDXL_ERR;
			}

			// Update network stage equation in same loop
			for (j = 0; j < vert->dimension; j++)
			{
				nidx = vert->nidx + j;
				_Network->stage_state[0][nidx] = _Network->state[nidx] + _Network->h * (__butcherNystromB[0] * _Network->rkf[0][nidx] + __butcherNystromB[1] * _Network->rkf[2][nidx] + __butcherNystromB[2] * _Network->rkf[4][nidx] + __butcherNystromB[3] * _Network->rkf[5][nidx]);
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
			if (edge->dynamics(edge, _Network->rkf[5] + edge->nidx, _Network->stage_state[1] + edge->nidx, edge->time + __butcherNystromC[5] * _Network->h, edge->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateNystrom: external error encountered in edge dynamics call.");
				return NDXL_ERR;
			}

			// Update network stage equation in same loop
			for (j = 0; j < edge->dimension; j++)
			{
				nidx = edge->nidx + j;
				_Network->stage_state[0][nidx] = _Network->state[nidx] + _Network->h * (__butcherNystromB[0] * _Network->rkf[0][nidx] + __butcherNystromB[1] * _Network->rkf[2][nidx] + __butcherNystromB[2] * _Network->rkf[4][nidx] + __butcherNystromB[3] * _Network->rkf[5][nidx]);
			}
		}
		else
			memcpy(_Network->stage_state[0] + edge->nidx, _Network->state + edge->nidx, edge->dimension * sizeof(double));
	}

	_Network->stage = 0;
	return NDXL_OK;
}

// ^^^^^ Stage equations