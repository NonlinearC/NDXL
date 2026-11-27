#include "integration.h"

// Table initialisation vvvvv

double __butcherDopriA[20] = { 0.2, 0.075, 0.225, 0.977777777777777777778, -3.73333333333333333333, 3.55555555555555555556, 2.9525986892242036275, -11.5957933241883859168, 9.82289285169943606158, -0.290809327846364883402, 2.84627525252525252525, -10.7575757575757575758, 8.90642271774347246045, 0.2784090909090909090909, -0.273531303602058319039, 0.0911458333333333333333, 0.449236298292902066487, 0.651041666666666666667, -0.322376179245283018868, 0.130952380952380952381 };
double __butcherDopriB[6] = { 0.0899131944444444444444, 0.45348906858340820605, 0.6140625, -0.271512382075471698113, 0.089047619047619047619, 0.025 };
double __butcherDopriC[7] = { 0.0, 0.2, 0.3, 0.8, 0.888888888888888888889, 1.0, 1.0 };

// ^^^^^ Table initialisation | stage equations vvvvv

const int __ndxlDopriStage0(NDXLNetwork* _Network)
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
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateDopri: external error encountered in vertex dynamics call.");
				return NDXL_ERR;
			}

			// Update network stage equation in same loop
			for (j = 0; j < vert->dimension; j++)
			{
				nidx = vert->nidx + j;
				_Network->stage_state[1][nidx] = _Network->state[nidx] + _Network->h * (__butcherDopriA[0] * _Network->rkf[0][nidx]);
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
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateDopri: external error encountered in edge dynamics call.");
				return NDXL_ERR;
			}

			// Update network stage equation in same loop
			for (j = 0; j < edge->dimension; j++)
			{
				nidx = edge->nidx + j;
				_Network->stage_state[1][nidx] = _Network->state[nidx] + _Network->h * (__butcherDopriA[0] * _Network->rkf[0][nidx]);
			}
		}
		else
			memcpy(_Network->stage_state[1] + edge->nidx, _Network->state + edge->nidx, edge->dimension * sizeof(double));
	}

	_Network->stage = 1;
	return NDXL_OK;
}

const int __ndxlDopriStage1(NDXLNetwork* _Network)
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
			if (vert->dynamics(vert, _Network->rkf[1] + vert->nidx, _Network->stage_state[1] + vert->nidx, vert->time + __butcherDopriC[1] * _Network->h, vert->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateDopri: external error encountered in vertex dynamics call.");
				return NDXL_ERR;
			}

			// Update network stage equation in same loop
			for (j = 0; j < vert->dimension; j++)
			{
				nidx = vert->nidx + j;
				_Network->stage_state[0][nidx] = _Network->state[nidx] + _Network->h * (__butcherDopriA[1] * _Network->rkf[0][nidx] + __butcherDopriA[2] * _Network->rkf[1][nidx]);
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
			if (edge->dynamics(edge, _Network->rkf[1] + edge->nidx, _Network->stage_state[1] + edge->nidx, edge->time + __butcherDopriC[1] * _Network->h, edge->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateDopri: external error encountered in edge dynamics call.");
				return NDXL_ERR;
			}

			// Update network stage equation in same loop
			for (j = 0; j < edge->dimension; j++)
			{
				nidx = edge->nidx + j;
				_Network->stage_state[0][nidx] = _Network->state[nidx] + _Network->h * (__butcherDopriA[1] * _Network->rkf[0][nidx] + __butcherDopriA[2] * _Network->rkf[1][nidx]);
			}
		}
		else
			memcpy(_Network->stage_state[0] + edge->nidx, _Network->state + edge->nidx, edge->dimension * sizeof(double));
	}

	_Network->stage = 2;
	return NDXL_OK;
}

const int __ndxlDopriStage2(NDXLNetwork* _Network)
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
			if (vert->dynamics(vert, _Network->rkf[2] + vert->nidx, _Network->stage_state[0] + vert->nidx, vert->time + __butcherDopriC[2] * _Network->h, vert->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateDopri: external error encountered in vertex dynamics call.");
				return NDXL_ERR;
			}

			// Update network stage equation in same loop
			for (j = 0; j < vert->dimension; j++)
			{
				nidx = vert->nidx + j;
				_Network->stage_state[1][nidx] = _Network->state[nidx] + _Network->h * (__butcherDopriA[3] * _Network->rkf[0][nidx] + __butcherDopriA[4] * _Network->rkf[1][nidx] + __butcherDopriA[5] * _Network->rkf[2][nidx]);
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
			if (edge->dynamics(edge, _Network->rkf[2] + edge->nidx, _Network->stage_state[0] + edge->nidx, edge->time + __butcherDopriC[2] * _Network->h, edge->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateDopri: external error encountered in edge dynamics call.");
				return NDXL_ERR;
			}

			// Update network stage equation in same loop
			for (j = 0; j < edge->dimension; j++)
			{
				nidx = edge->nidx + j;
				_Network->stage_state[1][nidx] = _Network->state[nidx] + _Network->h * (__butcherDopriA[3] * _Network->rkf[0][nidx] + __butcherDopriA[4] * _Network->rkf[1][nidx] + __butcherDopriA[5] * _Network->rkf[2][nidx]);
			}
		}
		else
			memcpy(_Network->stage_state[1] + edge->nidx, _Network->state + edge->nidx, edge->dimension * sizeof(double));
	}

	_Network->stage = 3;
	return NDXL_OK;
}

const int __ndxlDopriStage3(NDXLNetwork* _Network)
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
			if (vert->dynamics(vert, _Network->rkf[3] + vert->nidx, _Network->stage_state[1] + vert->nidx, vert->time + __butcherDopriC[3] * _Network->h, vert->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateDopri: external error encountered in vertex dynamics call.");
				return NDXL_ERR;
			}

			// Update network stage equation in same loop
			for (j = 0; j < vert->dimension; j++)
			{
				nidx = vert->nidx + j;
				_Network->stage_state[0][nidx] = _Network->state[nidx] + _Network->h * (__butcherDopriA[6] * _Network->rkf[0][nidx] + __butcherDopriA[7] * _Network->rkf[1][nidx] + __butcherDopriA[8] * _Network->rkf[2][nidx] + __butcherDopriA[9] * _Network->rkf[3][nidx]);
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
			if (edge->dynamics(edge, _Network->rkf[3] + edge->nidx, _Network->stage_state[1] + edge->nidx, edge->time + __butcherDopriC[3] * _Network->h, edge->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateDopri: external error encountered in edge dynamics call.");
				return NDXL_ERR;
			}

			// Update network stage equation in same loop
			for (j = 0; j < edge->dimension; j++)
			{
				nidx = edge->nidx + j;
				_Network->stage_state[0][nidx] = _Network->state[nidx] + _Network->h * (__butcherDopriA[6] * _Network->rkf[0][nidx] + __butcherDopriA[7] * _Network->rkf[1][nidx] + __butcherDopriA[8] * _Network->rkf[2][nidx] + __butcherDopriA[9] * _Network->rkf[3][nidx]);
			}
		}
		else
			memcpy(_Network->stage_state[0] + edge->nidx, _Network->state + edge->nidx, edge->dimension * sizeof(double));
	}

	_Network->stage = 4;
	return NDXL_OK;
}

const int __ndxlDopriStage4(NDXLNetwork* _Network)
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
			if (vert->dynamics(vert, _Network->rkf[4] + vert->nidx, _Network->stage_state[0] + vert->nidx, vert->time + __butcherDopriC[4] * _Network->h, vert->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateDopri: external error encountered in vertex dynamics call.");
				return NDXL_ERR;
			}

			// Update network stage equation in same loop
			for (j = 0; j < vert->dimension; j++)
			{
				nidx = vert->nidx + j;
				_Network->stage_state[1][nidx] = _Network->state[nidx] + _Network->h * (__butcherDopriA[10] * _Network->rkf[0][nidx] + __butcherDopriA[11] * _Network->rkf[1][nidx] + __butcherDopriA[12] * _Network->rkf[2][nidx] + __butcherDopriA[13] * _Network->rkf[3][nidx] + __butcherDopriA[14] * _Network->rkf[4][nidx]);
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
			if (edge->dynamics(edge, _Network->rkf[4] + edge->nidx, _Network->stage_state[0] + edge->nidx, edge->time + __butcherDopriC[4] * _Network->h, edge->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateDopri: external error encountered in edge dynamics call.");
				return NDXL_ERR;
			}

			// Update network stage equation in same loop
			for (j = 0; j < edge->dimension; j++)
			{
				nidx = edge->nidx + j;
				_Network->stage_state[1][nidx] = _Network->state[nidx] + _Network->h * (__butcherDopriA[10] * _Network->rkf[0][nidx] + __butcherDopriA[11] * _Network->rkf[1][nidx] + __butcherDopriA[12] * _Network->rkf[2][nidx] + __butcherDopriA[13] * _Network->rkf[3][nidx] + __butcherDopriA[14] * _Network->rkf[4][nidx]);
			}
		}
		else
			memcpy(_Network->stage_state[1] + edge->nidx, _Network->state + edge->nidx, edge->dimension * sizeof(double));
	}

	_Network->stage = 5;
	return NDXL_OK;
}

const int __ndxlDopriStage5(NDXLNetwork* _Network)
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
			if (vert->dynamics(vert, _Network->rkf[5] + vert->nidx, _Network->stage_state[1] + vert->nidx, vert->time + __butcherDopriC[5] * _Network->h, vert->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateDopri: external error encountered in vertex dynamics call.");
				return NDXL_ERR;
			}

			// Update network stage equation in same loop
			for (j = 0; j < vert->dimension; j++)
			{
				nidx = vert->nidx + j;
				_Network->stage_state[0][nidx] = _Network->state[nidx] + _Network->h * (__butcherDopriA[15] * _Network->rkf[0][nidx] + __butcherDopriA[16] * _Network->rkf[2][nidx] + __butcherDopriA[17] * _Network->rkf[3][nidx] + __butcherDopriA[18] * _Network->rkf[4][nidx] + __butcherDopriA[19] * _Network->rkf[5][nidx]);
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
			if (edge->dynamics(edge, _Network->rkf[5] + edge->nidx, _Network->stage_state[1] + edge->nidx, edge->time + __butcherDopriC[5] * _Network->h, edge->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateDopri: external error encountered in edge dynamics call.");
				return NDXL_ERR;
			}

			// Update network stage equation in same loop
			for (j = 0; j < edge->dimension; j++)
			{
				nidx = edge->nidx + j;
				_Network->stage_state[0][nidx] = _Network->state[nidx] + _Network->h * (__butcherDopriA[15] * _Network->rkf[0][nidx] + __butcherDopriA[16] * _Network->rkf[2][nidx] + __butcherDopriA[17] * _Network->rkf[3][nidx] + __butcherDopriA[18] * _Network->rkf[4][nidx] + __butcherDopriA[19] * _Network->rkf[5][nidx]);
			}
		}
		else
			memcpy(_Network->stage_state[0] + edge->nidx, _Network->state + edge->nidx, edge->dimension * sizeof(double));
	}

	_Network->stage = 6;
	return NDXL_OK;
}

const int __ndxlDopriStage6(NDXLNetwork* _Network, double* _Errsum)
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
			if (vert->dynamics(vert, _Network->rkf[6] + vert->nidx, _Network->stage_state[0] + vert->nidx, vert->time + __butcherDopriC[6] * _Network->h, vert->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateDopri: external error encountered in vertex dynamics call.");
				return NDXL_ERR;
			}

			// Update network error in same loop
			atol = (vert->atol ? vert->atol : _Network->atol);
			rtol = (vert->rtol ? vert->rtol : _Network->rtol);
			for (j = 0; j < vert->dimension; j++)
			{
				nidx = vert->nidx + j;
				_Network->rkf[7][nidx] = _Network->state[nidx] + _Network->h * (__butcherDopriB[0] * _Network->rkf[0][nidx] + __butcherDopriB[1] * _Network->rkf[2][nidx] + __butcherDopriB[2] * _Network->rkf[3][nidx] + __butcherDopriB[3] * _Network->rkf[4][nidx] + __butcherDopriB[4] * _Network->rkf[5][nidx] + __butcherDopriB[5] * _Network->rkf[6][nidx]);
				err = (_Network->stage_state[0][nidx] - _Network->rkf[7][nidx]) / (atol + rtol * fmax(fabs(_Network->stage_state[0][nidx]), fabs(_Network->state[nidx])));
				*_Errsum += (err * err);
			}
		}
		else
			memcpy(_Network->rkf[7] + vert->nidx, _Network->state + vert->nidx, vert->dimension * sizeof(double));
	}

	// Iterate over edges
	for (i = 0; i < _Network->ne; i++)
	{
		edge = _Network->edges[i];
		if (edge->dynamics)
		{
			if (edge->dynamics(edge, _Network->rkf[6] + edge->nidx, _Network->stage_state[0] + edge->nidx, edge->time + __butcherDopriC[6] * _Network->h, edge->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateDopri: external error encountered in edge dynamics call.");
				return NDXL_ERR;
			}

			// Update network error in same loop
			atol = (edge->atol ? edge->atol : _Network->atol);
			rtol = (edge->rtol ? edge->rtol : _Network->rtol);
			for (j = 0; j < edge->dimension; j++)
			{
				nidx = edge->nidx + j;
				_Network->rkf[7][nidx] = _Network->state[nidx] + _Network->h * (__butcherDopriB[0] * _Network->rkf[0][nidx] + __butcherDopriB[1] * _Network->rkf[2][nidx] + __butcherDopriB[2] * _Network->rkf[3][nidx] + __butcherDopriB[3] * _Network->rkf[4][nidx] + __butcherDopriB[4] * _Network->rkf[5][nidx] + __butcherDopriB[5] * _Network->rkf[6][nidx]);
				err = (_Network->stage_state[0][nidx] - _Network->rkf[7][nidx]) / (atol + rtol * fmax(fabs(_Network->stage_state[0][nidx]), fabs(_Network->state[nidx])));
				*_Errsum += (err * err);
			}
		}
		else
			memcpy(_Network->rkf[7] + edge->nidx, _Network->state + edge->nidx, edge->dimension * sizeof(double));
	}

	_Network->stage = 0;
	return NDXL_OK;
}

// ^^^^^ Stage equations