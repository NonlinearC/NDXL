#include "integration.h"

// Initial step selection vvvvv

const int __ndxlSelectInitialStep(NDXLNetwork* _Network, const double _OrderInv)
{
	// Algorithm variables
	NDXLVertex* vert;
	NDXLEdge* edge;
	size_t i, j;
	double atol, rtol, scale, h0, h1;

	// Scaled L2 norms
	double _D0 = 0.0;
	double _D1 = 0.0;
	double _D2 = 0.0;

	// Iterate over vertices to determine state L2 d0 and derivative L2 d1
	for (i = 0; i < _Network->nv; i++)
	{
		vert = _Network->vertices[i];
		atol = (vert->atol ? vert->atol : _Network->atol);
		rtol = (vert->rtol ? vert->rtol : _Network->rtol);
		if (vert->dynamics)
		{
			if (vert->dynamics(vert, _Network->rkf[0] + vert->nidx, _Network->state + vert->nidx, vert->time, vert->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulate: external errour encountered in vertex dynamics call during initial step selection.");
				return NDXL_ERR;
			}

			// Compute d0 and d1 as slope may be nonzero
			for (j = 0; j < vert->dimension; j++)
			{
				scale = atol + rtol * fabs(_Network->state[vert->nidx + j]);
				_D0 += __ndxlSquare(_Network->state[vert->nidx + j] / scale);
				_D1 += __ndxlSquare(_Network->rkf[0][vert->nidx + j] / scale);
			}
		}
		else
		{
			// Compute only d0 as slope is zero
			for (j = 0; j < vert->dimension; j++)
			{
				scale = atol + rtol * fabs(_Network->state[vert->nidx + j]);
				_D0 += __ndxlSquare(_Network->state[vert->nidx + j] / scale);
			}
            memset(_Network->rkf[0] + vert->nidx, 0, vert->datsz);
		}
	}

	// Iterate over edges to determine state L2 d0 and derivative L2 d1
	for (i = 0; i < _Network->ne; i++)
	{
		edge = _Network->edges[i];
		atol = (edge->atol ? edge->atol : _Network->atol);
		rtol = (edge->rtol ? edge->rtol : _Network->rtol);
		if (edge->dynamics)
		{
			if (edge->dynamics(edge, _Network->rkf[0] + edge->nidx, _Network->state + edge->nidx, edge->time, edge->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulate: external errour encountered in edge dynamics call during initial step selection.");
				return NDXL_ERR;
			}

			// Compute d0 and d1 as slope may be nonzero
			for (j = 0; j < edge->dimension; j++)
			{
				scale = atol + rtol * fabs(_Network->state[edge->nidx + j]);
				_D0 += __ndxlSquare(_Network->state[edge->nidx + j] / scale);
				_D1 += __ndxlSquare(_Network->rkf[0][edge->nidx + j] / scale);
			}
		}
		else
		{
			// Compute only d0 as slope is zero
			for (j = 0; j < edge->dimension; j++)
			{
				scale = atol + rtol * fabs(_Network->state[edge->nidx + j]);
				_D0 += __ndxlSquare(_Network->state[edge->nidx + j] / scale);
			}
            memset(_Network->rkf[0] + edge->nidx, 0, edge->datsz);
		}
	}

	// Compute d0, d1, and initial step guess h0
	_D0 = sqrt(_D0 / _Network->jacn);
	_D1 = sqrt(_D1 / _Network->jacn);
	h0 = ((_D0 < 1e-5) || (_D1 < 1e-5)) ? 1e-6 : 1e-2 * (_D0 / _D1);
	_Network->stage = 1;
	for (i = 0; i < _Network->jacn; i++)
		_Network->stage_state[1][i] = _Network->state[i] + h0 * _Network->rkf[0][i];

	// Iterate over vertices to determine Euler state L2
	for (i = 0; i < _Network->nv; i++)
	{
		vert = _Network->vertices[i];
		atol = (vert->atol ? vert->atol : _Network->atol);
		rtol = (vert->rtol ? vert->rtol : _Network->rtol);
		if (vert->dynamics)
		{
			if (vert->dynamics(vert, _Network->rkf[1] + vert->nidx, _Network->stage_state[1] + vert->nidx, vert->time + h0, vert->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulate: external errour encountered in vertex dynamics call during initial step selection.");
				return NDXL_ERR;
			}

			// Compute d2 as slope may be nonzero
			for (j = 0; j < vert->dimension; j++)
			{
				scale = atol + rtol * fmax(fabs(_Network->state[vert->nidx + j]), fabs(_Network->stage_state[1][vert->nidx + j]));
				_D2 += __ndxlSquare((_Network->rkf[1][vert->nidx + j] - _Network->rkf[0][vert->nidx + j]) / scale);
			}
		}
	}

	// Iterate over edges to determine Euler state L2
	for (i = 0; i < _Network->ne; i++)
	{
		edge = _Network->edges[i];
		atol = (edge->atol ? edge->atol : _Network->atol);
		rtol = (edge->rtol ? edge->rtol : _Network->rtol);
		if (edge->dynamics)
		{
			if (edge->dynamics(edge, _Network->rkf[1] + edge->nidx, _Network->stage_state[1] + edge->nidx, edge->time + h0, edge->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulate: external errour encountered in edge dynamics call during initial step selection.");
				return NDXL_ERR;
			}

			// Compute d2 as slope may be nonzero
			for (j = 0; j < edge->dimension; j++)
			{
				scale = atol + rtol * fmax(fabs(_Network->state[edge->nidx + j]), fabs(_Network->stage_state[1][edge->nidx + j]));
				_D2 += __ndxlSquare((_Network->rkf[1][edge->nidx + j] - _Network->rkf[0][edge->nidx + j]) / scale);
			}
		}
	}

	// Compute L2 norm of Euler step and set network initial step size
	_D2 = sqrt(_D2 / _Network->jacn) / h0;
	_D0 = fmax(_D1, _D2);
	h1 = (_D0 <= 1e-15) ? fmax(1e-6, 1e-3 * h0) : pow(1e-2 / _D0, _OrderInv);
	_Network->h = fmin(100.0 * h0, h1);
	_Network->stage = 0;
	return NDXL_OK;
}

const int __ndxlSelectInitialStepStochastic(NDXLNetwork* _Network, const double _OrderInv)
{
	// Algorithm variables
	NDXLVertex* vert;
	NDXLEdge* edge;
	size_t i, j, k;
	double atol, rtol, scale, h0, h1, s0;

	// Scaled L2 norms
	double _D0 = 0.0;
	double _D1drift = 0.0, _D1diff = 0.0;
	double _D2drift = 0.0, _D2diff = 0.0;

	// Iterate over vertices to determine state L2 d0 and derivative L2 d1
	for (i = 0; i < _Network->nv; i++)
	{
		vert = _Network->vertices[i];
		atol = (vert->atol ? vert->atol : _Network->atol);
		rtol = (vert->rtol ? vert->rtol : _Network->rtol);
		if (vert->dynamics)
		{
			if (vert->dynamics(vert, _Network->rkf[0] + vert->nidx, _Network->state + vert->nidx, vert->time, vert->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulate: external errour encountered in vertex dynamics call during initial step selection.");
				return NDXL_ERR;
			}
		}
		else
			memset(_Network->rkf[0] + vert->nidx, 0, vert->datsz);
		if (vert->noise)
		{
			if (vert->noise(vert, vert->rkg[0], _Network->state + vert->nidx, vert->time, vert->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulate: external errour encountered in vertex noise call during initial step selection.");
				return NDXL_ERR;
			}
		}
		else
			memset(vert->rkg[0], 0, vert->nc * vert->datsz);
		for (j = 0; j < vert->dimension; j++)
		{
			scale = atol + rtol * fabs(_Network->state[vert->nidx + j]);
			_D0 += __ndxlSquare(_Network->state[vert->nidx + j] / scale);
			_D1drift += __ndxlSquare(_Network->rkf[0][vert->nidx + j] / scale);
			for (k = 0; k < vert->nc; k++)
				_D1diff += __ndxlSquare(vert->rkg[0][j * vert->nc + k] / scale);
		}
	}

	// Iterate over edges to determine state L2 d0 and derivative L2 d1
	for (i = 0; i < _Network->ne; i++)
	{
		edge = _Network->edges[i];
		atol = (edge->atol ? edge->atol : _Network->atol);
		rtol = (edge->rtol ? edge->rtol : _Network->rtol);
		if (edge->dynamics)
		{
			if (edge->dynamics(edge, _Network->rkf[0] + edge->nidx, _Network->state + edge->nidx, edge->time, edge->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulate: external errour encountered in edge dynamics call during initial step selection.");
				return NDXL_ERR;
			}
		}
		else
			memset(_Network->rkf[0] + edge->nidx, 0, edge->datsz);
		if (edge->noise)
		{
			if (edge->noise(edge, edge->rkg[0], _Network->state + edge->nidx, edge->time, edge->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulate: external errour encountered in edge noise call during initial step selection.");
				return NDXL_ERR;
			}
		}
		else
			memset(edge->rkg[0], 0, edge->nc * edge->datsz);
		for (j = 0; j < edge->dimension; j++)
		{
			scale = atol + rtol * fabs(_Network->state[edge->nidx + j]);
			_D0 += __ndxlSquare(_Network->state[edge->nidx + j] / scale);
			_D1drift += __ndxlSquare(_Network->rkf[0][edge->nidx + j] / scale);
			for (k = 0; k < edge->nc; k++)
				_D1diff += __ndxlSquare(edge->rkg[0][j * edge->nc + k] / scale);
		}
	}

	// Compute d0, d1, and initial step guess h0
	_D0 = sqrt(_D0 / _Network->jacn);
	_D1drift = sqrt(_D1drift / _Network->jacn);
	_D1diff = sqrt(_D1diff / _Network->jacn);
	h0 = fmin(((_D0 < 1e-5) || (_D1drift < 1e-5)) ? 1e-6 : 1e-2 * (_D0 / _D1drift), ((_D0 < 1e-5) || (_D1diff < 1e-5)) ? 1e-6 : __ndxlSquare(1e-2 * (_D0 / _D1diff)));
	s0 = sqrt(h0);
	_Network->stage = 1;
	for (i = 0; i < _Network->nv; i++)
	{
		vert = _Network->vertices[i];
		for (j = 0; j < vert->dimension; j++)
		{
			_Network->stage_state[1][vert->nidx + j] = _Network->state[vert->nidx + j] + h0 * _Network->rkf[0][vert->nidx + j];
			_Network->rkf[2][vert->nidx + j] = 0.0;
			for (k = 0; k < vert->nc; k++)
				_Network->rkf[2][vert->nidx + j] += _Network->stage_state[1][vert->nidx + j] + s0 * vert->rkg[0][j * vert->nc + k];
		}
	}
	for (i = 0; i < _Network->ne; i++)
	{
		edge = _Network->edges[i];
		for (j = 0; j < edge->dimension; j++)
		{
			_Network->stage_state[1][edge->nidx + j] = _Network->state[edge->nidx + j] + h0 * _Network->rkf[0][edge->nidx + j];
			_Network->rkf[2][edge->nidx + j] = 0.0;
			for (k = 0; k < edge->nc; k++)
				_Network->rkf[2][edge->nidx + j] += _Network->stage_state[1][edge->nidx + j] + s0 * edge->rkg[0][j * edge->nc + k];
		}
	}

	// Iterate over vertices to determine Euler state L2
	for (i = 0; i < _Network->nv; i++)
	{
		vert = _Network->vertices[i];
		atol = (vert->atol ? vert->atol : _Network->atol);
		rtol = (vert->rtol ? vert->rtol : _Network->rtol);
		if (vert->dynamics)
		{
			if (vert->dynamics(vert, _Network->rkf[1] + vert->nidx, _Network->stage_state[1] + vert->nidx, vert->time + h0, vert->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulate: external errour encountered in vertex dynamics call during initial step selection.");
				return NDXL_ERR;
			}
		}
		else
			memset(_Network->rkf[1] + vert->nidx, 0, vert->datsz);
		if (vert->noise)
		{
			if (vert->noise(vert, vert->rkg[1], _Network->stage_state[1] + vert->nidx, vert->time + h0, vert->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulate: external errour encountered in vertex noise call during initial step selection.");
				return NDXL_ERR;
			}
		}
		else
			memset(vert->rkg[1], 0, vert->nc * vert->datsz);
		for (j = 0; j < vert->dimension; j++)
		{
			scale = atol + rtol * fmax(fabs(_Network->stage_state[1][vert->nidx + j]), fabs(_Network->state[vert->nidx + j]));
			_D2drift += __ndxlSquare((_Network->rkf[1][vert->nidx + j] - _Network->rkf[0][vert->nidx + j]) / scale);
			for (k = 0; k < vert->nc; k++)
				_D2diff += __ndxlSquare((vert->rkg[1][j * vert->nc + k] - vert->rkg[0][j * vert->nc + k]) / scale);
		}
	}

	// Iterate over edges to determine Euler state L2
	for (i = 0; i < _Network->ne; i++)
	{
		edge = _Network->edges[i];
		atol = (edge->atol ? edge->atol : _Network->atol);
		rtol = (edge->rtol ? edge->rtol : _Network->rtol);
		if (edge->dynamics)
		{
			if (edge->dynamics(edge, _Network->rkf[1] + edge->nidx, _Network->stage_state[1] + edge->nidx, edge->time + h0, edge->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulate: external errour encountered in edge dynamics call during initial step selection.");
				return NDXL_ERR;
			}
		}
		else
			memset(_Network->rkf[1] + edge->nidx, 0, edge->datsz);
		if (edge->noise)
		{
			if (edge->noise(edge, edge->rkg[1], _Network->stage_state[1] + edge->nidx, edge->time + h0, edge->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulate: external errour encountered in edge noise call during initial step selection.");
				return NDXL_ERR;
			}
		}
		else
			memset(edge->rkg[1], 0, edge->nc * edge->datsz);
		for (j = 0; j < edge->dimension; j++)
		{
			scale = atol + rtol * fmax(fabs(_Network->stage_state[1][edge->nidx + j]), fabs(_Network->state[edge->nidx + j]));
			_D2drift += __ndxlSquare((_Network->rkf[1][edge->nidx + j] - _Network->rkf[0][edge->nidx + j]) / scale);
			for (k = 0; k < edge->nc; k++)
				_D2diff += __ndxlSquare((edge->rkg[1][j * edge->nc + k] - edge->rkg[0][j * edge->nc + k]) / scale);
		}
	}

	// Compute L2 norm of Euler step and set network initial step size
	_D2drift = sqrt(_D2drift / _Network->jacn) / h0;
	_D2diff = sqrt(_D2diff / _Network->jacn) / s0;
	_D0 = fmax(fmax(_D1drift, _D2drift), fmax(__ndxlSquare(_D1diff), __ndxlSquare(_D2diff)));
	h1 = (_D0 <= 1e-15) ? fmax(1e-6, 1e-3 * h0) : pow(1e-2 / _D0, _OrderInv);
	_Network->h = fmin(100.0 * h0, h1);
	_Network->s = sqrt(_Network->h);
	_Network->stage = 0;
	return NDXL_OK;
}

// ^^^^^ Initial step selection | Jacobian evaluation vvvvv

const int __ndxlNumericalJacobian(NDXLNetwork* _Network, double* _Jac)
{
	// Algorithm variables
	size_t _Jidx = 0;
	size_t i, j, k, l, nidx;
	double eps, y0;
	NDXLVertex* vert, * jvert;
	NDXLEdge* edge, * jedge;

	// Iteration over vertices
	for (i = 0; i < _Network->nv; i++)
	{
		vert = _Network->vertices[i];
		for (j = 0; j < vert->dimension; j++)
		{
			// Scale perturbation and preserve original state
			nidx = vert->nidx + j;
			eps = EPS * fmax(1.0, _Network->state[nidx]);
			y0 = _Network->state[nidx];

			// Compute forward perturbation, store in rkf[6]
			_Network->state[nidx] = y0 + eps;
			for (k = 0; k < _Network->nv; k++)
			{
				jvert = _Network->vertices[k];
				if (jvert->dynamics)
				{
					if (jvert->dynamics(jvert, _Network->rkf[6] + jvert->nidx, _Network->state + jvert->nidx, jvert->time, jvert->params))
					{
						__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulate: external error encountered in vertex dynamics call during Jacobian evaluation.");
						return NDXL_ERR;
					}
				}
				else
					memset(_Network->rkf[6] + jvert->nidx, 0, jvert->datsz);
			}
			for (k = 0; k < _Network->ne; k++)
			{
				jedge = _Network->edges[k];
				if (jedge->dynamics)
				{
					if (jedge->dynamics(jedge, _Network->rkf[6] + jedge->nidx, _Network->state + jedge->nidx, jedge->time, jedge->params))
					{
						__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulate: external error encountered in edge dynamics call during Jacobian evaluation.");
						return NDXL_ERR;
					}
				}
				else
					memset(_Network->rkf[6] + jedge->nidx, 0, jedge->datsz);
			}

			// Compute backward perturbation, store in rkf[7]
			_Network->state[nidx] = y0 - eps;
			for (k = 0; k < _Network->nv; k++)
			{
				jvert = _Network->vertices[k];
				if (jvert->dynamics)
				{
					if (jvert->dynamics(jvert, _Network->rkf[7] + jvert->nidx, _Network->state + jvert->nidx, jvert->time, jvert->params))
					{
						__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulate: external error encountered in vertex dynamics call during Jacobian evaluation.");
						return NDXL_ERR;
					}
				}
				else
					memset(_Network->rkf[7] + jvert->nidx, 0, jvert->datsz);
			}
			for (k = 0; k < _Network->ne; k++)
			{
				jedge = _Network->edges[k];
				if (jedge->dynamics)
				{
					if (jedge->dynamics(jedge, _Network->rkf[7] + jedge->nidx, _Network->state + jedge->nidx, jedge->time, jedge->params))
					{
						__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulate: external error encountered in edge dynamics call during Jacobian evaluation.");
						return NDXL_ERR;
					}
				}
				else
					memset(_Network->rkf[7] + jedge->nidx, 0, jedge->datsz);
			}

			// Restore state state and compute finite difference approximation
			_Network->state[nidx] = y0;
			for (k = 0; k < _Network->nv; k++)
			{
				jvert = _Network->vertices[k];
				for (l = 0; l < jvert->dimension; l++)
					_Jac[(jvert->nidx + l) * _Network->jacn + nidx] = (_Network->rkf[6][jvert->nidx + l] - _Network->rkf[7][jvert->nidx + l]) / (2.0 * eps);
			}
			for (k = 0; k < _Network->ne; k++)
			{
				jedge = _Network->edges[k];
				for (l = 0; l < jedge->dimension; l++)
					_Jac[(jedge->nidx + l) * _Network->jacn + nidx] = (_Network->rkf[6][jedge->nidx + l] - _Network->rkf[7][jedge->nidx + l]) / (2.0 * eps);
			}
		}
	}

	// Iteration over edges
	for (i = 0; i < _Network->ne; i++)
	{
		edge = _Network->edges[i];
		for (j = 0; j < edge->dimension; j++)
		{
			// Scale perturbation and preserve original state
			nidx = edge->nidx + j;
			eps = EPS * fmax(1.0, _Network->state[nidx]);
			y0 = _Network->state[nidx];

			// Compute forward perturbation, store in rkf[6]
			_Network->state[nidx] = y0 + eps;
			for (k = 0; k < _Network->nv; k++)
			{
				jvert = _Network->vertices[k];
				if (jvert->dynamics)
				{
					if (jvert->dynamics(jvert, _Network->rkf[6] + jvert->nidx, _Network->state + jvert->nidx, jvert->time, jvert->params))
					{
						__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulate: external error encountered in vertex dynamics call during Jacobian evaluation.");
						return NDXL_ERR;
					}
				}
				else
					memset(_Network->rkf[6] + jvert->nidx, 0, jvert->datsz);
			}
			for (k = 0; k < _Network->ne; k++)
			{
				jedge = _Network->edges[k];
				if (jedge->dynamics)
				{
					if (jedge->dynamics(jedge, _Network->rkf[6] + jedge->nidx, _Network->state + jedge->nidx, jedge->time, jedge->params))
					{
						__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulate: external error encountered in edge dynamics call during Jacobian evaluation.");
						return NDXL_ERR;
					}
				}
				else
					memset(_Network->rkf[6] + jedge->nidx, 0, jedge->datsz);
			}

			// Compute backward perturbation, store in rkf[7]
			_Network->state[nidx] = y0 - eps;
			for (k = 0; k < _Network->nv; k++)
			{
				jvert = _Network->vertices[k];
				if (jvert->dynamics)
				{
					if (jvert->dynamics(jvert, _Network->rkf[7] + jvert->nidx, _Network->state + jvert->nidx, jvert->time, jvert->params))
					{
						__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulate: external error encountered in vertex dynamics call during Jacobian evaluation.");
						return NDXL_ERR;
					}
				}
				else
					memset(_Network->rkf[7] + jvert->nidx, 0, jvert->datsz);
			}
			for (k = 0; k < _Network->ne; k++)
			{
				jedge = _Network->edges[k];
				if (jedge->dynamics)
				{
					if (jedge->dynamics(jedge, _Network->rkf[7] + jedge->nidx, _Network->state + jedge->nidx, jedge->time, jedge->params))
					{
						__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulate: external error encountered in edge dynamics call during Jacobian evaluation.");
						return NDXL_ERR;
					}
				}
				else
					memset(_Network->rkf[7] + jedge->nidx, 0, jedge->datsz);
			}

			// Restore state state and compute finite difference approximation
			_Network->state[nidx] = y0;
			for (k = 0; k < _Network->nv; k++)
			{
				jvert = _Network->vertices[k];
				for (l = 0; l < jvert->dimension; l++)
					_Jac[(jvert->nidx + l) * _Network->jacn + nidx] = (_Network->rkf[6][jvert->nidx + l] - _Network->rkf[7][jvert->nidx + l]) / (2.0 * eps);
			}
			for (k = 0; k < _Network->ne; k++)
			{
				jedge = _Network->edges[k];
				for (l = 0; l < jedge->dimension; l++)
					_Jac[(jedge->nidx + l) * _Network->jacn + nidx] = (_Network->rkf[6][jedge->nidx + l] - _Network->rkf[7][jedge->nidx + l]) / (2.0 * eps);
			}
		}
	}

	// Success
	return 0;
}

// ^^^^^ Jacobian evaluation | linear algebra vvvvv

void __ndxlPluDecompose(double* _Mat, size_t* _P, const size_t _Dim)
{
	// Algorithm variables
	size_t i, j, k, _Imax;
	double _Amax, _Aabs;
	double d;

	// Unit permutation matrix
	for (i = 0; i <= _Dim; i++)
		_P[i] = i;

	// Decompose
	for (i = 0; i < _Dim; i++)
	{
		_Amax = 0.0;
		_Imax = 0;

		// For complex values, pivoting condition is checked against magnitude of entry
		for (k = i; k < _Dim; k++)
			if ((_Aabs = fabs(_Mat[k * _Dim + i])) > _Amax)
			{
				_Amax = _Aabs;
				_Imax = k;
			}

		// Pivot rows and permutation matrix
		if (_Imax != i)
		{
			for (k = 0; k < _Dim; k++)
			{
				d = _Mat[i * _Dim + k];
				_Mat[i * _Dim + k] = _Mat[_Imax * _Dim + k];
				_Mat[_Imax * _Dim + k] = d;
			}
			j = _P[i];
			_P[i] = _P[_Imax];
			_P[_Imax] = j;
			_P[_Dim]++;
		}

		// Factorisation
		for (j = i + 1; j < _Dim; j++)
		{
			_Mat[j * _Dim + i] /= _Mat[i * _Dim + i];
			for (k = i + 1; k < _Dim; k++)
				_Mat[j * _Dim + k] -= (_Mat[j * _Dim + i] * _Mat[i * _Dim + k]);
		}
	}
}

void __ndxlPluDecomposeZ(complex_t* _Mat, size_t* _P, const size_t _Dim)
{
	// Algorithm variables
	size_t i, j, k, _Imax;
	double _Amax, _Aabs;
	complex_t z;

	// Unit permutation matrix
	for (i = 0; i <= _Dim; i++)
		_P[i] = i;

	// Decompose
	for (i = 0; i < _Dim; i++)
	{
		_Amax = 0.0;
		_Imax = 0;

		// For complex values, pivoting condition is checked against magnitude of entry
		for (k = i; k < _Dim; k++)
			if ((_Aabs = uniMag(_Mat[k * _Dim + i])) > _Amax)
			{
				_Amax = _Aabs;
				_Imax = k;
			}

		// Pivot rows and permutation matrix
		if (_Imax != i)
		{
			for (k = 0; k < _Dim; k++)
			{
				z = _Mat[i * _Dim + k];
				_Mat[i * _Dim + k] = _Mat[_Imax * _Dim + k];
				_Mat[_Imax * _Dim + k] = z;
			}
			j = _P[i];
			_P[i] = _P[_Imax];
			_P[_Imax] = j;
			_P[_Dim]++;
		}

		// Factorisation
		for (j = i + 1; j < _Dim; j++)
		{
			_Mat[j * _Dim + i] = uniDivComplexComplex(_Mat[j * _Dim + i], _Mat[i * _Dim + i]);
			for (k = i + 1; k < _Dim; k++)
				_Mat[j * _Dim + k] = uniSubComplexComplex(_Mat[j * _Dim + k], uniMulComplexComplex(_Mat[j * _Dim + i], _Mat[i * _Dim + k]));
		}
	}
}

void __ndxlPluSolve(double* _Ans, double* _Mat, double* _Rhs, size_t* _P, const size_t _Dim)
{
	// Algorithm variables
	size_t i, j;

	// Forward substitution Ly = Pb
	for (i = 0; i < _Dim; i++)
	{
		_Ans[i] = _Rhs[_P[i]];
		for (j = 0; j < i; j++)
			_Ans[i] -= (_Mat[i * _Dim + j] * _Ans[j]);
	}

	// Backward substitution Ux = y
	for (i = _Dim - 1; i >= 0; i--)
	{
		for (j = i + 1; j < _Dim; j++)
			_Ans[i] -= (_Mat[i * _Dim + j] * _Ans[j]);
		_Ans[i] /= _Mat[i * _Dim + i];
		if (i == 0)
			break;
	}
}

void __ndxlPluSolveZ(complex_t* _Ans, complex_t* _Mat, complex_t* _Rhs, size_t* _P, const size_t _Dim)
{
	// Algorithm variables
	size_t i, j;

	// Forward substitution Ly = Pb
	for (i = 0; i < _Dim; i++)
	{
		_Ans[i] = _Rhs[_P[i]];
		for (j = 0; j < i; j++)
			_Ans[i] = uniSubComplexComplex(_Ans[i], uniMulComplexComplex(_Mat[i * _Dim + j], _Ans[j]));
	}

	// Backward substitution Ux = y
	for (i = _Dim - 1; i >= 0; i--)
	{
		for (j = i + 1; j < _Dim; j++)
			_Ans[i] = uniSubComplexComplex(_Ans[i], uniMulComplexComplex(_Mat[i * _Dim + j], _Ans[j]));
		_Ans[i] = uniDivComplexComplex(_Ans[i], _Mat[i * _Dim + i]);
		if (i == 0)
			break;
	}
}

// ^^^^^ Linear algebra
