#include "integration.h"

// Table initialisation vvvvv

double __butcherRadauA[9] = { 0.196815477223660425868, -0.0655354258501983881085, 0.0237709743482201524204, 0.394424314739087276997, 0.292073411665228463021, -0.0415487521259979301982, 0.37640306270046727505, 0.512485826188421613839, 0.111111111111111111111 };
double __butcherRadauC[3] = { 0.15505102572168219018, 0.64494897427831780982, 1.0 };
double __radauEig[3] = { 0.274888829595677, 0.162555585202161, 0.184949324407141 };
double __radauMat[9] = { 0.094438762488975, -0.141255295020954,  0.030029194105147, 0.250213122965333,  0.204129352293800, -0.382942112757262, 1.0,  1.0,  0.0 };
double __radauInv[9] = { 4.178718591551905, 0.327682820761062,  0.523376445499450, -4.178718591551905, -0.327682820761062,  0.476623554500550, 0.502872634945787, -2.571926949855605,  0.596039204828225 };
double __radauErr[3] = { -10.048809399827414, 1.382142733160748, -0.333333333333333 };

// ^^^^^ Table initialisation | stage equations vvvvv

const int __ndxlRadauSlopes(NDXLNetwork* _Network, double* _Jac, double *_Mlhsreal, double* _Mrhsreal, double* _Decoupreal, complex_t* _Mlhsimag, complex_t* _Mrhsimag, complex_t* _Decoupimag, double* _Erhs, double* _Errloc, size_t** _Pivots)
{
	// Algorithm variables
	size_t i, j;
	NDXLVertex* vert;
	NDXLEdge* edge;

	// Make network explicit so nearest-neighbour interactions return the current state instead of the guessed stage
	_Network->method_explicit = 1;
	if (__ndxlNumericalJacobian(_Network, _Jac))
		return NDXL_SIM_ERR;

	// Compute f(y, t) and store in stage_state[0]
	for (i = 0; i < _Network->nv; i++)
	{
		vert = _Network->vertices[i];
		if (vert->dynamics)
		{
			if (vert->dynamics(vert, _Network->stage_state[0] + vert->nidx, _Network->state + vert->nidx, vert->time, vert->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRadau: external error encountered in vertex dynamics call.");
				return NDXL_SIM_ERR;
			}
		}
		else
			memset(_Network->stage_state[0] + vert->nidx, 0, vert->datsz);
	}
	for (i = 0; i < _Network->ne; i++)
	{
		edge = _Network->edges[i];
		if (edge->dynamics)
		{
			if (edge->dynamics(edge, _Network->stage_state[0] + edge->nidx, _Network->state + edge->nidx, edge->time, edge->params))
			{
				__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRadau: external error encountered in edge dynamics call.");
				return NDXL_SIM_ERR;
			}
		}
		else
			memset(_Network->stage_state[0] + edge->nidx, 0, edge->datsz);
	}

	// Restore method explicit flag
	_Network->method_explicit = 0;
	return NDXL_OK;
}

void __ndxlRadauGuesses(NDXLNetwork* _Network, double* _Jac, double* _Mlhsreal, complex_t* _Mlhsimag, size_t** _Pivots)
{
	// Algorithm variables
	size_t i, j, jrow, jidx;

	// PLU-decompose real and complex subsystems once since LHS does not change during Newton-Raphson iteration
	for (i = 0; i < _Network->jacn; i++)
	{
		jrow = i * _Network->jacn;
		for (j = 0; j < _Network->jacn; j++)
		{
			jidx = jrow + j;
			_Mlhsreal[jidx] = (i == j) - _Network->h * __radauEig[0] * _Jac[jidx];
			_Mlhsimag[jidx] = uniComplex((i == j) - _Network->h * __radauEig[1] * _Jac[jidx], -_Network->h * __radauEig[2] * _Jac[jidx]);
		}
	}
	__ndxlPluDecompose(_Mlhsreal, _Pivots[0], _Network->jacn);
	__ndxlPluDecomposeZ(_Mlhsimag, _Pivots[1], _Network->jacn);

	// Initial guesses for stage derivatives rkf[3]..rkf[5] -> k0..k2; initial guess of f(y, t)
	memcpy(_Network->rkf[3], _Network->stage_state[0], _Network->jacn * sizeof(double));
	memcpy(_Network->rkf[4], _Network->stage_state[0], _Network->jacn * sizeof(double));
	memcpy(_Network->rkf[5], _Network->stage_state[0], _Network->jacn * sizeof(double));
}

const int __ndxlRadauSolve(NDXLNetwork* _Network, double* _Mlhsreal, double* _Mrhsreal, double* _Decoupreal, complex_t* _Mlhsimag, complex_t* _Mrhsimag, complex_t* _Decoupimag, size_t** _Pivots)
{
	// Algorithm variables
	int _Rejected = 1;
	double dk[3], err;
	size_t i, j, nidx;
	NDXLVertex* vert;
	NDXLEdge* edge;

	// Newton-Raphson iteration to solve stage derivatives
	for (char NEWTON_ITER = 0; NEWTON_ITER < ITERMAX; NEWTON_ITER++)
	{
		// Compute stage equations (rkf[0]..rkf[2] -> Y0..Y2)
		for (i = 0; i < _Network->jacn; i++)
		{
			_Network->rkf[0][i] = _Network->state[i] + _Network->h * (__butcherRadauA[0] * _Network->rkf[3][i] + __butcherRadauA[1] * _Network->rkf[4][i] + __butcherRadauA[2] * _Network->rkf[5][i]);
			_Network->rkf[1][i] = _Network->state[i] + _Network->h * (__butcherRadauA[3] * _Network->rkf[3][i] + __butcherRadauA[4] * _Network->rkf[4][i] + __butcherRadauA[5] * _Network->rkf[5][i]);
			_Network->rkf[2][i] = _Network->state[i] + _Network->h * (__butcherRadauA[6] * _Network->rkf[3][i] + __butcherRadauA[7] * _Network->rkf[4][i] + __butcherRadauA[8] * _Network->rkf[5][i]);
		}

		// Compute residuals (rkf[6]..rkf[8] -> R0..R2)
		for (i = 0; i < _Network->nv; i++)
		{
			vert = _Network->vertices[i];
			if (vert->dynamics)
			{
				if (vert->dynamics(vert, _Network->rkf[6] + vert->nidx, _Network->rkf[0] + vert->nidx, vert->time + __butcherRadauC[0] * _Network->h, vert->params))
				{
					__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRadau: external error encountered in vertex dynamics call.");
					return NDXL_SIM_ERR;
				}
				_Network->stage = 1;
				if (vert->dynamics(vert, _Network->rkf[7] + vert->nidx, _Network->rkf[1] + vert->nidx, vert->time + __butcherRadauC[1] * _Network->h, vert->params))
				{
					__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRadau: external error encountered in vertex dynamics call.");
					return NDXL_SIM_ERR;
				}
				_Network->stage = 2;
				if (vert->dynamics(vert, _Network->rkf[8] + vert->nidx, _Network->rkf[2] + vert->nidx, vert->time + __butcherRadauC[2] * _Network->h, vert->params))
				{
					__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRadau: external error encountered in vertex dynamics call.");
					return NDXL_SIM_ERR;
				}
				_Network->stage = 0;
				for (j = 0; j < vert->dimension; j++)
				{
					nidx = vert->nidx + j;
					_Network->rkf[6][nidx] -= _Network->rkf[3][nidx];
					_Network->rkf[7][nidx] -= _Network->rkf[4][nidx];
					_Network->rkf[8][nidx] -= _Network->rkf[5][nidx];
				}
			}
			else
			{
				memset(_Network->rkf[6] + vert->nidx, 0, vert->datsz);
				memset(_Network->rkf[7] + vert->nidx, 0, vert->datsz);
				memset(_Network->rkf[8] + vert->nidx, 0, vert->datsz);
			}
		}
		for (i = 0; i < _Network->ne; i++)
		{
			edge = _Network->edges[i];
			if (edge->dynamics)
			{
				if (edge->dynamics(edge, _Network->rkf[6] + edge->nidx, _Network->rkf[0] + edge ->nidx, edge->time + __butcherRadauC[0] * _Network->h, edge->params))
				{
					__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRadau: external error encountered in edge dynamics call.");
					return NDXL_SIM_ERR;
				}
				_Network->stage = 1;
				if (edge->dynamics(edge, _Network->rkf[7] + edge->nidx, _Network->rkf[1] + edge->nidx, edge->time + __butcherRadauC[1] * _Network->h, edge->params))
				{
					__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRadau: external error encountered in edge dynamics call.");
					return NDXL_SIM_ERR;
				}
				_Network->stage = 2;
				if (edge->dynamics(edge, _Network->rkf[8] + edge->nidx, _Network->rkf[2] + edge->nidx, edge->time + __butcherRadauC[2] * _Network->h, edge->params))
				{
					__ndxlSetLastError(NDXL_SIM_ERR, "ndxlNetworkSimulateRadau: external error encountered in edge dynamics call.");
					return NDXL_SIM_ERR;
				}
				_Network->stage = 0;
				for (j = 0; j < edge->dimension; j++)
				{
					nidx = edge->nidx + j;
					_Network->rkf[6][nidx] -= _Network->rkf[3][nidx];
					_Network->rkf[7][nidx] -= _Network->rkf[4][nidx];
					_Network->rkf[8][nidx] -= _Network->rkf[5][nidx];
				}
			}
			else
			{
				memset(_Network->rkf[6] + edge->nidx, 0, edge->datsz);
				memset(_Network->rkf[7] + edge->nidx, 0, edge->datsz);
				memset(_Network->rkf[8] + edge->nidx, 0, edge->datsz);
			}
		}

		// Populate right hand side intermediate vectors
		for (i = 0; i < _Network->jacn; i++)
		{
			_Mrhsreal[i] = __radauInv[0] * _Network->rkf[6][i] + __radauInv[1] * _Network->rkf[7][i] + __radauInv[2] * _Network->rkf[8][i];
			_Mrhsimag[i] = uniComplex(__radauInv[3] * _Network->rkf[6][i] + __radauInv[4] * _Network->rkf[7][i] + __radauInv[5] * _Network->rkf[8][i], __radauInv[6] * _Network->rkf[6][i] + __radauInv[7] * _Network->rkf[7][i] + __radauInv[8] * _Network->rkf[8][i]);
		}

		// PLU-solve decoupled increments
		__ndxlPluSolve(_Decoupreal, _Mlhsreal, _Mrhsreal, _Pivots[0], _Network->jacn);
		__ndxlPluSolveZ(_Decoupimag, _Mlhsimag, _Mrhsimag, _Pivots[1], _Network->jacn);

		// Calculate stage derivative increments and L2 error norm
		err = 0.0;
		for (i = 0; i < _Network->jacn; i++)
		{
			dk[0] = __radauMat[0] * _Decoupreal[i] + __radauMat[1] * _Decoupimag[i].real + __radauMat[2] * _Decoupimag[i].imag;
			dk[1] = __radauMat[3] * _Decoupreal[i] + __radauMat[4] * _Decoupimag[i].real + __radauMat[5] * _Decoupimag[i].imag;
			dk[2] = __radauMat[6] * _Decoupreal[i] + __radauMat[7] * _Decoupimag[i].real + __radauMat[8] * _Decoupimag[i].imag;
			err += __ndxlSquare((_Network->h * (__butcherRadauA[6] * dk[0] + __butcherRadauA[7] * dk[1] + __butcherRadauA[8] * dk[2])) / (1e-6 + 1e-3 * fmax(fabs(_Network->state[i]), fabs(_Network->rkf[2][i]))));
			_Network->rkf[3][i] += dk[0];
			_Network->rkf[4][i] += dk[1];
			_Network->rkf[5][i] += dk[2];
		}

		// Check for convergence
		err = sqrt(err / _Network->jacn);
		if (err < 1.0)
		{
			_Rejected = 0;
			break;
		}
	}

	// Return -1 if error; 0 if converged; 1 if not converged
	return _Rejected;
}

const double __ndxlRadauError(NDXLNetwork* _Network, double* _Mlhsreal, double* _Erhs, double* _Errloc, size_t** _Pivots)
{
	// Algorithm variables
	double atol = _Network->atol, rtol = _Network->rtol, err = 0.0;
	size_t i, j, nidx;
	NDXLVertex* vert;
	NDXLEdge* edge;

	// PLU-solve error estimator
	for (i = 0; i < _Network->jacn; i++)
		_Erhs[i] = _Network->h * _Network->stage_state[0][i] + __radauErr[0] * (_Network->rkf[0][i] - _Network->state[i]) + __radauErr[1] * (_Network->rkf[1][i] - _Network->state[i]) + __radauErr[2] * (_Network->rkf[2][i] - _Network->state[i]);
	__ndxlPluSolve(_Errloc, _Mlhsreal, _Erhs, _Pivots[0], _Network->jacn);

	// Compute L2 error norm
	for (i = 0; i < _Network->nv; i++)
	{
		vert = _Network->vertices[i];
		atol = (vert->atol ? vert->atol : _Network->atol);
		atol = (vert->rtol ? vert->rtol : _Network->rtol);
		for (j = 0; j < vert->dimension; j++)
		{
			nidx = vert->nidx + j;
			err += __ndxlSquare((__radauEig[0] * _Errloc[i]) / (atol + rtol * fmax(fabs(_Network->state[nidx]), fabs(_Network->rkf[2][nidx]))));
		}
	}

	// Return error
	return fmax(sqrt(err / _Network->jacn), EPS);
}
