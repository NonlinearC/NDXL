#include "integration.h"

// Stage equations vvvvv

const int __ndxlEulerStage0(NDXLNetwork* _Network)
{
	static size_t i, j, k, nidx;
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
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateEuler: external error encountered in vertex dynamics call.");
				return NDXL_ERR;
			}
		}
		else
			memset(_Network->rkf[0] + vert->nidx, 0, vert->datsz);
		if (vert->noise)
		{
			if (vert->noise(vert, vert->rkg[0], _Network->state + vert->nidx, vert->time, vert->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateEuler: external error encountered in vertex noise call.");
				return NDXL_ERR;
			}
		}
		else
			memset(vert->rkg[0], 0, vert->nc * vert->datsz);

		// Update network stage equation in same loop
		for (j = 0; j < vert->dimension; j++)
		{
			nidx = vert->nidx + j;
			_Network->stage_state[1][nidx] = _Network->state[nidx] + _Network->h * _Network->rkf[0][nidx];
			for (k = 0; k < vert->nc; k++)
				_Network->stage_state[1][nidx] += vert->channels[k]->channel->dw * vert->rkg[0][j * vert->nc + k];
		}
	}

	// Iterate over edges
	for (i = 0; i < _Network->ne; i++)
	{
		edge = _Network->edges[i];
		if (edge->dynamics)
		{
			if (edge->dynamics(edge, _Network->rkf[0] + edge->nidx, _Network->state + edge->nidx, edge->time, edge->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateEuler: external error encountered in edge dynamics call.");
				return NDXL_ERR;
			}
		}
		else
			memset(_Network->rkf[0] + edge->nidx, 0, edge->datsz);
		if (edge->noise)
		{
			if (edge->noise(edge, edge->rkg[0], _Network->state + edge->nidx, edge->time, edge->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateEuler: external error encountered in edge noise call.");
				return NDXL_ERR;
			}
		}
		else
			memset(edge->rkg[0], 0, edge->nc * edge->datsz);

		// Update network stage equation in same loop
		for (j = 0; j < edge->dimension; j++)
		{
			nidx = edge->nidx + j;
			_Network->stage_state[1][nidx] = _Network->state[nidx] + _Network->h * _Network->rkf[0][nidx];
			for (k = 0; k < edge->nc; k++)
				_Network->stage_state[1][nidx] += edge->channels[k]->channel->dw * edge->rkg[0][j * edge->nc + k];
		}
	}

	return NDXL_OK;
}

// ^^^^^ Stage equations