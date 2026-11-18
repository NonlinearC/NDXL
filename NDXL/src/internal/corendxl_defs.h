//----------------------------------------------\\
// NDXL internal header for API-wide functions	\\
// Copyright Ethan Codron 2026					\\
// GNU GPL-3.0									\\
//----------------------------------------------\\
// Contains internal state variables and macros	\\
// used throughout the API. Includes all header	\\
// files used everywhere else.					\\
//----------------------------------------------\\

#ifndef _CORENDXL_DEFS_
#define _CORENDXL_DEFS_

#include <float.h>	// DBL_EPSILON
#include <stdio.h>	// printf()
#include <stdlib.h>	// calloc(), free(), malloc(), rand(), realloc(), srand()
#include <string.h>	// memcpy(), memset()
#include <time.h>	// struct timespec, clock_gettime()

#include "../../include/unicpp/unicpp.h"	// complex_t, TWO_PI

#ifdef NDXL_STATIC
	#define _NDXLEXP_
#else
	#ifdef _MSC_VER
		#define _NDXLEXP_ __declspec(dllexport)
	#else
		#define _NDXLEXP_ __attribute__((visibility("default")))
	#endif // MSVC
#endif // NDXL_STATIC

// Internal global variables vvvvv

extern bool __ndxlIsInitialised;
extern int __ndxlLastErrorCode;
extern char* __ndxlLastErrorMsg;
static double __ndxlSqrtEpsilon = 1.4901161193847656e-08;

static double __butcherRalstonA[6] = { 0.4, 0.296977609247753600071, 0.158759644971035831853, 0.218100388225920467596, -3.05096514869293080535, 3.83286476046701033776 };
static double __butcherRalstonB[4] = { 0.174760282262690371255, -0.551480662878732940546, 1.20553559939652353503, 0.171184781219519034263 };
static double __butcherRalstonC[4] = { 0.0, 0.4, 0.455737254218789431923, 1.0 };

static double __butcherNystromA[14] = { 0.333333333333333333333, 0.16, 0.24, 0.25, -3.0, 3.75, 0.074074074074074074074074, 1.11111111111111111111, -0.617283950617283950617, 0.0987654320987654320988, 0.08, 0.48, 0.133333333333333333333, 0.106666666666666666667 };
static double __butcherNystromB[4] = { 0.119791666666666666667, 0.651041666666666666667, -0.421875, 0.651041666666666666667 };
static double __butcherNystromC[6] = { 0.0, 0.333333333333333333333, 0.4, 1.0, 0.666666666666666666667, 0.8 };

static double __butcherDopriA[20] = { 0.2, 0.075, 0.225, 0.977777777777777777778, -3.73333333333333333333, 3.55555555555555555556, 2.9525986892242036275, -11.5957933241883859168, 9.82289285169943606158, -0.290809327846364883402, 2.84627525252525252525, -10.7575757575757575758, 8.90642271774347246045, 0.2784090909090909090909, -0.273531303602058319039, 0.0911458333333333333333, 0.449236298292902066487, 0.651041666666666666667, -0.322376179245283018868, 0.130952380952380952381 };
static double __butcherDopriB[6] = { 0.0899131944444444444444, 0.45348906858340820605, 0.6140625, -0.271512382075471698113, 0.089047619047619047619, 0.025 };
static double __butcherDopriC[7] = { 0.0, 0.2, 0.3, 0.8, 0.888888888888888888889, 1.0, 1.0 };

static double __butcherCashKarpA[15] = { 0.2, 0.075, 0.225, 0.3, -0.9, 1.2, -0.2037037037037037037037, 2.5, -2.59259259259259259259, 1.296296296296296296296, 0.0294958043981481481481, 0.341796875, 0.0415943287037037037037, 0.400345413773148148148, 0.061767578125 };
static double __butcherCashKarpB5[4] = { 0.0978835978835978835979, 0.402576489533011272142, 0.21043771043771043771, 0.28910220214568040655 };
static double __butcherCashKarpB4[5] = { 0.102177372685185185185, 0.383907903439153439153, 0.244592737268518518519, 0.0193219866071428571429, 0.25 };
static double __butcherCashKarpC[6] = { 0.0, 0.2, 0.3, 0.6, 1.0, 0.875 };

static double __butcherNorsettA[6] = { 1.06857902130162880642, -0.568579021301628806419, 1.06857902130162880642, 2.13715804260325761284, -3.27431608520651522568, 1.06857902130162880642 };
static double __butcherNorsettB[3] = { 0.128886400515720422365, 0.742227198968559155271, 0.128886400515720422365 };
static double __butcherNorsettC[3] = { 1.06857902130162880642, 0.5, -0.0685790213016288064188 };

static double __butcherRadauA[9] = { 0.196815477223660425868, -0.0655354258501983881085, 0.0237709743482201524204, 0.394424314739087276997, 0.292073411665228463021, -0.0415487521259979301982, 0.37640306270046727505, 0.512485826188421613839, 0.111111111111111111111 };
static double __butcherRadauC[3] = { 0.15505102572168219018, 0.64494897427831780982, 1.0 };
static double __radauEig[3] = { 0.274888829595677, 0.162555585202161, 0.184949324407141 };
static double __radauMat[9] = { 0.094438762488975, -0.141255295020954,  0.030029194105147, 0.250213122965333,  0.204129352293800, -0.382942112757262, 1.0,  1.0,  0.0 };
static double __radauInv[9] = { 4.178718591551905, 0.327682820761062,  0.523376445499450, -4.178718591551905, -0.327682820761062,  0.476623554500550, 0.502872634945787, -2.571926949855605,  0.596039204828225 };
static double __radauErr[3] = { -10.048809399827414, 1.382142733160748, -0.333333333333333 };

// ^^^^^ Internal global variables | internal macros vvvvv

#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#define EPS		__ndxlSqrtEpsilon
#define ITERMAX	5

#define INTEGRATOR_RALSTON(objname) static inline int __ndxlIntegrateRalston##objname(NDXL##objname* _##objname, const double _Delta) {\
	int _Result = 0;\
	if (_##objname->func) {\
		_Result |= _##objname->func(_##objname, _##objname->rkf[_##objname->stage], _##objname->stage_state[_##objname->stage % 2], _##objname->time + __butcherRalstonC[_##objname->stage] * _Delta, _##objname->params);\
		switch (_##objname->stage) {\
		case 0:\
			for (size_t i = 0; i < _##objname->order; i++) {\
				_##objname->stage_state[1][i] = _##objname->state[i] + _Delta * (__butcherRalstonA[0] * _##objname->rkf[0][i]);\
			}\
			break;\
		case 1:\
			for (size_t i = 0; i < _##objname->order; i++) {\
				_##objname->stage_state[0][i] = _##objname->state[i] + _Delta * (__butcherRalstonA[1] * _##objname->rkf[0][i] + __butcherRalstonA[2] * _##objname->rkf[1][i]);\
			}\
			break;\
		case 2:\
			for (size_t i = 0; i < _##objname->order; i++) {\
				_##objname->stage_state[1][i] = _##objname->state[i] + _Delta * (__butcherRalstonA[3] * _##objname->rkf[0][i] + __butcherRalstonA[4] * _##objname->rkf[1][i] + __butcherRalstonA[5] * _##objname->rkf[2][i]);\
			}\
			break;\
		case 3:\
			for (size_t i = 0; i < _##objname->order; i++) {\
				_##objname->stage_state[0][i] = _##objname->state[i] + _Delta * (__butcherRalstonB[0] * _##objname->rkf[0][i] + __butcherRalstonB[1] * _##objname->rkf[1][i] + __butcherRalstonB[2] * _##objname->rkf[2][i] + __butcherRalstonB[3] * _##objname->rkf[3][i]);\
			}\
			break;\
		}\
	}\
	_##objname->stage = (_##objname->stage + 1) % 4;\
	return _Result;\
}

#define INTEGRATOR_NYSTROM(objname) static inline int __ndxlIntegrateNystrom##objname(NDXL##objname* _##objname, const double _Delta) {\
	int _Result = 0;\
	if (_##objname->func) {\
		_Result |= _##objname->func(_##objname, _##objname->rkf[_##objname->stage], _##objname->stage_state[_##objname->stage % 2], _##objname->time + __butcherNystromC[_##objname->stage] * _Delta, _##objname->params);\
		switch (_##objname->stage) {\
		case 0:\
			for (size_t i = 0; i < _##objname->order; i++) {\
				_##objname->stage_state[1][i] = _##objname->state[i] + _Delta * (__butcherNystromA[0] * _##objname->rkf[0][i]);\
			}\
			break;\
		case 1:\
			for (size_t i = 0; i < _##objname->order; i++) {\
				_##objname->stage_state[0][i] = _##objname->state[i] + _Delta * (__butcherNystromA[1] * _##objname->rkf[0][i] + __butcherNystromA[2] * _##objname->rkf[1][i]);\
			}\
			break;\
		case 2:\
			for (size_t i = 0; i < _##objname->order; i++) {\
				_##objname->stage_state[1][i] = _##objname->state[i] + _Delta * (__butcherNystromA[3] * _##objname->rkf[0][i] + __butcherNystromA[4] * _##objname->rkf[1][i] + __butcherNystromA[5] * _##objname->rkf[2][i]);\
			}\
			break;\
		case 3:\
			for (size_t i = 0; i < _##objname->order; i++) {\
				_##objname->stage_state[0][i] = _##objname->state[i] + _Delta * (__butcherNystromA[6] * _##objname->rkf[0][i] + __butcherNystromA[7] * _##objname->rkf[1][i] + __butcherNystromA[8] * _##objname->rkf[2][i] + __butcherNystromA[9] * _##objname->rkf[3][i]);\
			}\
			break;\
		case 4:\
			for (size_t i = 0; i < _##objname->order; i++) {\
				_##objname->stage_state[1][i] = _##objname->state[i] + _Delta * (__butcherNystromA[10] * _##objname->rkf[0][i] + __butcherNystromA[11] * _##objname->rkf[1][i] + __butcherNystromA[12] * _##objname->rkf[2][i] + __butcherNystromA[13] * _##objname->rkf[3][i]);\
			}\
			break;\
		case 5:\
			for (size_t i = 0; i < _##objname->order; i++) {\
				_##objname->stage_state[0][i] = _##objname->state[i] + _Delta * (__butcherNystromB[0] * _##objname->rkf[0][i] + __butcherNystromB[1] * _##objname->rkf[2][i] + __butcherNystromB[2] * _##objname->rkf[4][i] + __butcherNystromB[3] * _##objname->rkf[5][i]);\
			}\
			break;\
		}\
	}\
	_##objname->stage = (_##objname->stage + 1) % 6;\
	return _Result;\
}

#define INTEGRATOR_DOPRI(objname) static inline int __ndxlIntegrateDopri##objname(NDXL##objname* _##objname, const double _Delta, const double _Atol, const double _Rtol, double* _Errsum) {\
	int _Result = 0;\
	if (_##objname->func) {\
		_Result |= _##objname->func(_##objname, _##objname->rkf[_##objname->stage], _##objname->stage ? _##objname->stage_state[_##objname->stage % 2] : _##objname->state, _##objname->time + __butcherDopriC[_##objname->stage] * _Delta, _##objname->params);\
		switch (_##objname->stage) {\
		case 0:\
			for (size_t i = 0; i < _##objname->order; i++) {\
				_##objname->rkf[7][i] = __butcherDopriB[0] * _##objname->rkf[0][i];\
				_##objname->stage_state[1][i] = _##objname->state[i] + _Delta * (__butcherDopriA[0] * _##objname->rkf[0][i]);\
			}\
			break;\
		case 1:\
			for (size_t i = 0; i < _##objname->order; i++) {\
				_##objname->stage_state[0][i] = _##objname->state[i] + _Delta * (__butcherDopriA[1] * _##objname->rkf[0][i] + __butcherDopriA[2] * _##objname->rkf[1][i]);\
			}\
			break;\
		case 2:\
			for (size_t i = 0; i < _##objname->order; i++) {\
				_##objname->rkf[7][i] += __butcherDopriB[1] * _##objname->rkf[2][i];\
				_##objname->stage_state[1][i] = _##objname->state[i] + _Delta * (__butcherDopriA[3] * _##objname->rkf[0][i] + __butcherDopriA[4] * _##objname->rkf[1][i] + __butcherDopriA[5] * _##objname->rkf[2][i]);\
			}\
			break;\
		case 3:\
			for (size_t i = 0; i < _##objname->order; i++) {\
				_##objname->rkf[7][i] += __butcherDopriB[2] * _##objname->rkf[3][i];\
				_##objname->stage_state[0][i] = _##objname->state[i] + _Delta * (__butcherDopriA[6] * _##objname->rkf[0][i] + __butcherDopriA[7] * _##objname->rkf[1][i] + __butcherDopriA[8] * _##objname->rkf[2][i] + __butcherDopriA[9] * _##objname->rkf[3][i]);\
			}\
			break;\
		case 4:\
			for (size_t i = 0; i < _##objname->order; i++) {\
				_##objname->rkf[7][i] += __butcherDopriB[3] * _##objname->rkf[4][i];\
				_##objname->stage_state[1][i] = _##objname->state[i] + _Delta * (__butcherDopriA[10] * _##objname->rkf[0][i] + __butcherDopriA[11] * _##objname->rkf[1][i] + __butcherDopriA[12] * _##objname->rkf[2][i] + __butcherDopriA[13] * _##objname->rkf[3][i] + __butcherDopriA[14] * _##objname->rkf[4][i]);\
			}\
			break;\
		case 5:\
			for (size_t i = 0; i < _##objname->order; i++) {\
				_##objname->rkf[7][i] += __butcherDopriB[4] * _##objname->rkf[5][i];\
				_##objname->stage_state[0][i] = _##objname->state[i] + _Delta * (__butcherDopriA[15] * _##objname->rkf[0][i] + __butcherDopriA[16] * _##objname->rkf[2][i] + __butcherDopriA[17] * _##objname->rkf[3][i] + __butcherDopriA[18] * _##objname->rkf[4][i] + __butcherDopriA[19] * _##objname->rkf[5][i]);\
			}\
			break;\
		case 6:\
			for (size_t i = 0; i < _##objname->order; i++) {\
				_##objname->rkf[7][i] = _##objname->state[i] + _Delta * (_##objname->rkf[7][i] + __butcherDopriB[5] * _##objname->rkf[6][i]);\
				const double _Err = (_Atol + _Rtol * fmax(fabs(_##objname->stage_state[0][i]), fabs(_##objname->state[i]))) / (_##objname->stage_state[0][i] - _##objname->rkf[7][i]);\
				*_Errsum += (_Err * _Err);\
			}\
			break;\
		}\
	}\
	_##objname->stage = (_##objname->stage + 1) % 7;\
	return _Result;\
}

#define INTEGRATOR_RKCK(objname) static inline int __ndxlIntegrateRkck##objname(NDXL##objname* _##objname, const double _Delta, const double _Atol, const double _Rtol, double* _Errsum) {\
	int _Result = 0;\
	if (_##objname->func) {\
		_Result |= _##objname->func(_##objname, _##objname->rkf[_##objname->stage], _##objname->stage ? _##objname->stage_state[_##objname->stage % 2] : _##objname->state, _##objname->time + __butcherCashKarpC[_##objname->stage] * _Delta, _##objname->params);\
		switch (_##objname->stage) {\
		case 0:\
			for (size_t i = 0; i < _##objname->order; i++) {\
				_##objname->rkf[7][i] = __butcherCashKarpB4[0] * _##objname->rkf[0][i];\
				_##objname->stage_state[1][i] = _##objname->state[i] + _Delta * (__butcherCashKarpA[0] * _##objname->rkf[0][i]);\
			}\
			break;\
		case 1:\
			for (size_t i = 0; i < _##objname->order; i++) {\
				_##objname->stage_state[0][i] = _##objname->state[i] + _Delta * (__butcherCashKarpA[1] * _##objname->rkf[0][i] + __butcherCashKarpA[2] * _##objname->rkf[1][i]);\
			}\
			break;\
		case 2:\
			for (size_t i = 0; i < _##objname->order; i++) {\
				_##objname->rkf[7][i] += __butcherCashKarpB4[1] * _##objname->rkf[2][i];\
				_##objname->stage_state[1][i] = _##objname->state[i] + _Delta * (__butcherCashKarpA[3] * _##objname->rkf[0][i] + __butcherCashKarpA[4] * _##objname->rkf[1][i] + __butcherCashKarpA[5] * _##objname->rkf[2][i]);\
			}\
			break;\
		case 3:\
			for (size_t i = 0; i < _##objname->order; i++) {\
				_##objname->rkf[7][i] += __butcherCashKarpB4[2] * _##objname->rkf[3][i];\
				_##objname->stage_state[0][i] = _##objname->state[i] + _Delta * (__butcherCashKarpA[6] * _##objname->rkf[0][i] + __butcherCashKarpA[7] * _##objname->rkf[1][i] + __butcherCashKarpA[8] * _##objname->rkf[2][i] + __butcherCashKarpA[9] * _##objname->rkf[3][i]);\
			}\
			break;\
		case 4:\
			for (size_t i = 0; i < _##objname->order; i++) {\
				_##objname->rkf[7][i] += __butcherCashKarpB4[3] * _##objname->rkf[4][i];\
				_##objname->stage_state[1][i] = _##objname->state[i] + _Delta * (__butcherCashKarpA[10] * _##objname->rkf[0][i] + __butcherCashKarpA[11] * _##objname->rkf[1][i] + __butcherCashKarpA[12] * _##objname->rkf[2][i] + __butcherCashKarpA[13] * _##objname->rkf[3][i] + __butcherCashKarpA[14] * _##objname->rkf[4][i]);\
			}\
			break;\
		case 5:\
			for (size_t i = 0; i < _##objname->order; i++) {\
				_##objname->rkf[7][i] = _##objname->state[i] + _Delta * (_##objname->rkf[7][i] + __butcherCashKarpB4[4] * _##objname->rkf[5][i]);\
				_##objname->stage_state[0][i] = _##objname->state[i] + _Delta * (__butcherCashKarpB5[0] * _##objname->rkf[0][i] + __butcherCashKarpB5[1] * _##objname->rkf[2][i] + __butcherCashKarpB5[2] * _##objname->rkf[3][i] + __butcherCashKarpB5[3] * _##objname->rkf[5][i]);\
				const double _Err = (_Atol + _Rtol * fmax(fabs(_##objname->stage_state[0][i]), fabs(_##objname->state[i]))) / (_##objname->stage_state[0][i] - _##objname->rkf[7][i]);\
				*_Errsum += (_Err * _Err);\
			}\
			break;\
		}\
	}\
	_##objname->stage = (_##objname->stage + 1) % 6;\
	return _Result;\
}

#define INTEGRATOR_EULER(objname) static inline int __ndxlIntegrateEuler##objname(NDXL##objname* _##objname, const double _Delta) {\
	int _Result = 0;\
	if (_##objname->func)\
		_Result |= _##objname->func(_##objname, _##objname->rkf[_##objname->stage], _##objname->stage ? _##objname->stage_state[_##objname->stage % 2] : _##objname->state, _##objname->time + _##objname->stage * _Delta, _##objname->params);\
	else\
		memset(_##objname->rkf[_##objname->stage], 0, _##objname->order * sizeof(double));\
	if (_##objname->noise)\
		_Result |= _##objname->noise(_##objname, _##objname->rkg[_##objname->stage], _##objname->stage ? _##objname->stage_state[_##objname->stage % 2] : _##objname->state, _##objname->time + _##objname->stage * _Delta, _##objname->params);\
	else\
		memset(_##objname->rkg[_##objname->stage], 0, _##objname->order * sizeof(double));\
	if (_##objname->stage) {\
		for (size_t i = 0; i < _##objname->order; i++) {\
			_##objname->stage_state[0][i] = _##objname->state[i] + 0.5 * _Delta * (_##objname->rkf[0][i] + _##objname->rkf[1][i]);\
			size_t j = 0;\
			for (NDXL##objname##Channel* citer = _##objname->chead; citer; j++, citer = citer->next)\
				_##objname->stage_state[0][i] += 0.5 * citer->channel->dw * (_##objname->rkg[0][i * _##objname->m + j] + _##objname->rkg[1][i * _##objname->m + j]);\
		}\
		_##objname->stage = 0;\
	} else {\
		for (size_t i = 0; i < _##objname->order; i++) {\
			_##objname->stage_state[1][i] = _##objname->state[i] + _Delta * _##objname->rkf[0][i];\
			size_t j = 0;\
			for (NDXL##objname##Channel* citer = _##objname->chead; citer; j++, citer = citer->next)\
				_##objname->stage_state[1][i] += citer->channel->dw * _##objname->rkg[0][i * _##objname->m + j];\
		}\
		_##objname->stage = 1;\
	}\
	return _Result;\
}

#define INTEGRATOR_HEUN(objname) static inline int __ndxlIntegrateHeun##objname(NDXL##objname* _##objname, const double _Delta, const double _Atol, const double _Rtol, double* _Errsum) {\
	int _Result = 0;\
	if (_##objname->func)\
		_Result |= _##objname->func(_##objname, _##objname->rkf[_##objname->stage], _##objname->stage ? _##objname->stage_state[_##objname->stage % 2] : _##objname->state, _##objname->time + _##objname->stage * _Delta, _##objname->params);\
	else\
		memset(_##objname->rkf[_##objname->stage], 0, _##objname->order * sizeof(double));\
	if (_##objname->noise)\
		_Result |= _##objname->noise(_##objname, _##objname->rkg[_##objname->stage], _##objname->stage ? _##objname->stage_state[_##objname->stage % 2] : _##objname->state, _##objname->time + _##objname->stage * _Delta, _##objname->params);\
	else\
		memset(_##objname->rkg[_##objname->stage], 0, _##objname->order * sizeof(double));\
	if (_##objname->stage) {\
		for (size_t i = 0; i < _##objname->order; i++) {\
			_##objname->stage_state[0][i] = _##objname->state[i] + 0.5 * _Delta * (_##objname->rkf[0][i] + _##objname->rkf[1][i]);\
			size_t j = 0;\
			for (NDXL##objname##Channel* citer = _##objname->chead; citer; j++, citer = citer->next)\
				_##objname->stage_state[0][i] += 0.5 * citer->channel->dw * (_##objname->rkg[0][i * _##objname->m + j] + _##objname->rkg[1][i * _##objname->m + j]);\
			const double _Err = (_Atol + _Rtol * fmax(fabs(_##objname->stage_state[0][i]), fabs(_##objname->state[i]))) / (_##objname->stage_state[0][i] - _##objname->rkf[2][i]);\
			*_Errsum += (_Err * _Err);\
		}\
		_##objname->stage = 0;\
	} else {\
		for (size_t i = 0; i < _##objname->order; i++) {\
			_##objname->stage_state[1][i] = _##objname->state[i] + _Delta * _##objname->rkf[0][i];\
			size_t j = 0;\
			for (NDXL##objname##Channel* citer = _##objname->chead; citer; j++, citer = citer->next)\
				_##objname->stage_state[1][i] += citer->channel->dw * _##objname->rkg[0][i * _##objname->m + j];\
			_##objname->rkf[2][i] = _##objname->stage_state[1][i];\
		}\
		_##objname->stage = 1;\
	}\
	return _Result;\
}

#define INTEGRATOR_NORSETT(objname) static inline int __ndxlIntegrateNorsett##objname(NDXL##objname* _##objname, const double _Delta) {\
	int _Result = 0;\
	if (_##objname->func) {\
		const size_t _Datsz = _##objname->order * sizeof(double);\
		const size_t _Jacsz = _##objname->order * sizeof(double*);\
		memcpy(_##objname->rkf[3], _##objname->state, _Datsz);\
		memcpy(_##objname->rkf[4], _##objname->state, _Datsz);\
		memcpy(_##objname->rkf[5], _##objname->state, _Datsz);\
		memcpy(_##objname->rkg[3], _##objname->state, _Datsz);\
		_##objname->rkg[3][0] += 1e-10;\
		memcpy(_##objname->rkg[4], _##objname->state, _Datsz);\
		_##objname->rkg[4][0] -= 1e-10;\
		const double _Tevals[3] = {\
			_##objname->time + __butcherNorsettC[0] * _Delta,\
			_##objname->time + __butcherNorsettC[1] * _Delta,\
			_##objname->time + __butcherNorsettC[2] * _Delta\
		};\
		if (_##objname->noise) {\
			for (size_t _NR = 0; _NR < 20; _NR++) {\
				double _Maxerr = 0.0;\
				double _Abserr;\
				for (size_t i = 0; i < _##objname->order; i++) {\
					for (size_t j = 0; j < _##objname->order; i++)\
						_##objname->rkf[6][j] = _##objname->state[j] + _Delta * (__butcherNorsettA[0] * _##objname->rkf[3][j]) + _##objname->rkg[7][i] * (__butcherNorsettA[0] * _##objname->rkg[0][j]);\
					_Result |= _##objname->func(_##objname, _##objname->rkf[7], _##objname->rkf[6], _Tevals[0], _##objname->params);\
					_Result |= _##objname->func(_##objname, _##objname->rkg[5], _##objname->rkg[3], _Tevals[0], _##objname->params);\
					_Result |= _##objname->func(_##objname, _##objname->rkg[6], _##objname->rkg[4], _Tevals[0], _##objname->params);\
					const double _Newt = (_##objname->state[i] + _##objname->rkf[7][i] - _##objname->rkf[3][i]) / ((_##objname->rkg[3][i] - _##objname->rkg[4][i]) * 5e9);\
					if ((_Abserr = fabs(_##objname->rkf[3][i]) - fabs(_Newt)) > _Maxerr)\
						_Maxerr = _Abserr;\
					_##objname->rkf[3][i] -= _Newt;\
					_##objname->rkg[3][i] -= 1e-10;\
					_##objname->rkg[3][(i + 1) % _##objname->order] += 1e-10;\
					_##objname->rkg[4][i] += 1e-10;\
					_##objname->rkg[4][(i + 1) % _##objname->order] -= 1e-10;\
				}\
				if (_Maxerr < 1e-10)\
					break;\
			}\
			for (size_t _NR = 0; _NR < 20; _NR++) {\
				double _Maxerr = 0.0;\
				double _Abserr;\
				for (size_t i = 0; i < _##objname->order; i++) {\
					for (size_t j = 0; j < _##objname->order; i++)\
						_##objname->rkf[6][j] = _##objname->state[j] + _Delta * (__butcherNorsettA[1] * _##objname->rkf[3][j] + __butcherNorsettA[2] * _##objname->rkf[4][j]) + _##objname->rkg[7][i] * (__butcherNorsettA[1] * _##objname->rkg[0][j] + __butcherNorsettA[2] * _##objname->rkg[1][j]);\
					_Result |= _##objname->func(_##objname, _##objname->rkf[7], _##objname->rkf[6], _Tevals[1], _##objname->params);\
					_Result |= _##objname->func(_##objname, _##objname->rkg[5], _##objname->rkg[3], _Tevals[1], _##objname->params);\
					_Result |= _##objname->func(_##objname, _##objname->rkg[6], _##objname->rkg[4], _Tevals[1], _##objname->params);\
					const double _Newt = (_##objname->state[i] + _##objname->rkf[7][i] - _##objname->rkf[4][i]) / ((_##objname->rkg[3][i] - _##objname->rkg[4][i]) * 5e9);\
					if ((_Abserr = fabs(_##objname->rkf[4][i]) - fabs(_Newt)) > _Maxerr)\
						_Maxerr = _Abserr;\
					_##objname->rkf[4][i] -= _Newt;\
					_##objname->rkg[3][i] -= 1e-10;\
					_##objname->rkg[3][(i + 1) % _##objname->order] += 1e-10;\
					_##objname->rkg[4][i] += 1e-10;\
					_##objname->rkg[4][(i + 1) % _##objname->order] -= 1e-10;\
				}\
				if (_Maxerr < 1e-10)\
					break;\
			}\
			for (size_t _NR = 0; _NR < 20; _NR++) {\
				double _Maxerr = 0.0;\
				double _Abserr;\
				for (size_t i = 0; i < _##objname->order; i++) {\
					for (size_t j = 0; j < _##objname->order; i++)\
						_##objname->rkf[6][j] = _##objname->state[j] + _Delta * (__butcherNorsettA[3] * _##objname->rkf[3][j] + __butcherNorsettA[4] * _##objname->rkf[4][j] + __butcherNorsettA[5] * _##objname->rkf[5][j]) + _##objname->rkg[7][i] * (__butcherNorsettA[3] * _##objname->rkg[0][j] + __butcherNorsettA[4] * _##objname->rkg[1][j] + __butcherNorsettA[5] * _##objname->rkg[2][i]);\
					_Result |= _##objname->func(_##objname, _##objname->rkf[7], _##objname->rkf[6], _Tevals[2], _##objname->params);\
					_Result |= _##objname->func(_##objname, _##objname->rkg[5], _##objname->rkg[3], _Tevals[2], _##objname->params);\
					_Result |= _##objname->func(_##objname, _##objname->rkg[6], _##objname->rkg[4], _Tevals[2], _##objname->params);\
					const double _Newt = (_##objname->state[i] + _##objname->rkf[7][i] - _##objname->rkf[5][i]) / ((_##objname->rkg[3][i] - _##objname->rkg[4][i]) * 5e9);\
					if ((_Abserr = fabs(_##objname->rkf[5][i]) - fabs(_Newt)) > _Maxerr)\
						_Maxerr = _Abserr;\
					_##objname->rkf[5][i] -= _Newt;\
					_##objname->rkg[3][i] -= 1e-10;\
					_##objname->rkg[3][(i + 1) % _##objname->order] += 1e-10;\
					_##objname->rkg[4][i] += 1e-10;\
					_##objname->rkg[4][(i + 1) % _##objname->order] -= 1e-10;\
				}\
				if (_Maxerr < 1e-10)\
					break;\
			}\
			for (size_t i = 0; i < _##objname->order; i++)\
				_##objname->temp_state[i] = _##objname->state[i] + _Delta * (__butcherNorsettB[0] * _##objname->rkf[3][i] + __butcherNorsettB[1] * _##objname->rkf[4][i] + __butcherNorsettB[2] * _##objname->rkf[5][i]) + _##objname->rkg[7][i] * (__butcherNorsettB[0] * _##objname->rkg[0][i] + __butcherNorsettB[1] * _##objname->rkg[1][i] + __butcherNorsettB[2] * _##objname->rkg[2][i]);\
		} else {\
			for (size_t _NR = 0; _NR < 20; _NR++) {\
				double _Maxerr = 0.0;\
				double _Abserr;\
				for (size_t i = 0; i < _##objname->order; i++) {\
					for (size_t j = 0; j < _##objname->order; i++)\
						_##objname->rkf[6][j] = _##objname->state[j] + _Delta * (__butcherNorsettA[0] * _##objname->rkf[3][j]);\
					_Result |= _##objname->func(_##objname, _##objname->rkf[7], _##objname->rkf[6], _Tevals[0], _##objname->params);\
					_Result |= _##objname->func(_##objname, _##objname->rkg[5], _##objname->rkg[3], _Tevals[0], _##objname->params);\
					_Result |= _##objname->func(_##objname, _##objname->rkg[6], _##objname->rkg[4], _Tevals[0], _##objname->params);\
					const double _Newt = (_##objname->state[i] + _##objname->rkf[7][i] - _##objname->rkf[3][i]) / ((_##objname->rkg[3][i] - _##objname->rkg[4][i]) * 5e9);\
					if ((_Abserr = fabs(_##objname->rkf[3][i]) - fabs(_Newt)) > _Maxerr)\
						_Maxerr = _Abserr;\
					_##objname->rkf[3][i] -= _Newt;\
					_##objname->rkg[3][i] -= 1e-10;\
					_##objname->rkg[3][(i + 1) % _##objname->order] += 1e-10;\
					_##objname->rkg[4][i] += 1e-10;\
					_##objname->rkg[4][(i + 1) % _##objname->order] -= 1e-10;\
				}\
				if (_Maxerr < 1e-10)\
					break;\
			}\
			for (size_t _NR = 0; _NR < 20; _NR++) {\
				double _Maxerr = 0.0;\
				double _Abserr;\
				for (size_t i = 0; i < _##objname->order; i++) {\
					for (size_t j = 0; j < _##objname->order; i++)\
						_##objname->rkf[6][j] = _##objname->state[j] + _Delta * (__butcherNorsettA[1] * _##objname->rkf[3][j] + __butcherNorsettA[2] * _##objname->rkf[4][j]);\
					_Result |= _##objname->func(_##objname, _##objname->rkf[7], _##objname->rkf[6], _Tevals[1], _##objname->params);\
					_Result |= _##objname->func(_##objname, _##objname->rkg[5], _##objname->rkg[3], _Tevals[1], _##objname->params);\
					_Result |= _##objname->func(_##objname, _##objname->rkg[6], _##objname->rkg[4], _Tevals[1], _##objname->params);\
					const double _Newt = (_##objname->state[i] + _##objname->rkf[7][i] - _##objname->rkf[4][i]) / ((_##objname->rkg[3][i] - _##objname->rkg[4][i]) * 5e9);\
					if ((_Abserr = fabs(_##objname->rkf[4][i]) - fabs(_Newt)) > _Maxerr)\
						_Maxerr = _Abserr;\
					_##objname->rkf[4][i] -= _Newt;\
					_##objname->rkg[3][i] -= 1e-10;\
					_##objname->rkg[3][(i + 1) % _##objname->order] += 1e-10;\
					_##objname->rkg[4][i] += 1e-10;\
					_##objname->rkg[4][(i + 1) % _##objname->order] -= 1e-10;\
				}\
				if (_Maxerr < 1e-10)\
					break;\
			}\
			for (size_t _NR = 0; _NR < 20; _NR++) {\
				double _Maxerr = 0.0;\
				double _Abserr;\
				for (size_t i = 0; i < _##objname->order; i++) {\
					for (size_t j = 0; j < _##objname->order; i++)\
						_##objname->rkf[6][j] = _##objname->state[j] + _Delta * (__butcherNorsettA[3] * _##objname->rkf[3][j] + __butcherNorsettA[4] * _##objname->rkf[4][j] + __butcherNorsettA[5] * _##objname->rkf[5][j]);\
					_Result |= _##objname->func(_##objname, _##objname->rkf[7], _##objname->rkf[6], _Tevals[2], _##objname->params);\
					_Result |= _##objname->func(_##objname, _##objname->rkg[5], _##objname->rkg[3], _Tevals[2], _##objname->params);\
					_Result |= _##objname->func(_##objname, _##objname->rkg[6], _##objname->rkg[4], _Tevals[2], _##objname->params);\
					const double _Newt = (_##objname->state[i] + _##objname->rkf[7][i] - _##objname->rkf[5][i]) / ((_##objname->rkg[3][i] - _##objname->rkg[4][i]) * 5e9);\
					if ((_Abserr = fabs(_##objname->rkf[5][i]) - fabs(_Newt)) > _Maxerr)\
						_Maxerr = _Abserr;\
					_##objname->rkf[5][i] -= _Newt;\
					_##objname->rkg[3][i] -= 1e-10;\
					_##objname->rkg[3][(i + 1) % _##objname->order] += 1e-10;\
					_##objname->rkg[4][i] += 1e-10;\
					_##objname->rkg[4][(i + 1) % _##objname->order] -= 1e-10;\
				}\
				if (_Maxerr < 1e-10)\
					break;\
			}\
			for (size_t i = 0; i < _##objname->order; i++)\
				_##objname->temp_state[i] = _##objname->state[i] + _Delta * (__butcherNorsettB[0] * _##objname->rkf[3][i] + __butcherNorsettB[1] * _##objname->rkf[4][i] + __butcherNorsettB[2] * _##objname->rkf[5][i]);\
		}\
	}\
	return _Result;\
}

// ^^^^^ Internal macros | Internal global functions vvvvv

static inline void __setLastError(const int _Code, const char* const _Msg)
{
	__ndxlLastErrorCode = _Code;
	const size_t _Len = (strlen(_Msg) + 1) * sizeof(char);
	__ndxlLastErrorMsg = (char*)realloc(__ndxlLastErrorMsg, _Len);
	memcpy(__ndxlLastErrorMsg, _Msg, _Len);
}

static inline void __onExit()
{
	__ndxlIsInitialised = 0;
	if (__ndxlLastErrorMsg) free(__ndxlLastErrorMsg);
}

static inline double __randomDouble()
{
	static struct timespec _Ts;
	timespec_get(&_Ts, TIME_UTC);
	srand(_Ts.tv_sec ^ _Ts.tv_nsec);
	return ((double)rand()) / ((double)RAND_MAX);
}

static inline int __isNear(const double _X, const double _Y)
{
	return fabs(_X - _Y) < EPS;
}

static inline double __square(const double _X)
{
	return _X * _X;
}

// ^^^^^ Internal global functions

#endif // _CORENDXL_DEFS_