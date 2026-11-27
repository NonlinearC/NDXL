#ifndef _INTEGRATION_NDXL_
#define _INTEGRATION_NDXL_

#include "../internal/internal.h"

// Ralston vvvvv

extern double __butcherRalstonA[6];
extern double __butcherRalstonB[4];
extern double __butcherRalstonC[4];

_NDXLEXP_ const int __ndxlRalstonStage0(NDXLNetwork* _Network);
_NDXLEXP_ const int __ndxlRalstonStage1(NDXLNetwork* _Network);
_NDXLEXP_ const int __ndxlRalstonStage2(NDXLNetwork* _Network);
_NDXLEXP_ const int __ndxlRalstonStage3(NDXLNetwork* _Network);

// ^^^^^ Ralston | Nystrom vvvvv

extern double __butcherNystromA[14];
extern double __butcherNystromB[4];
extern double __butcherNystromC[6];

_NDXLEXP_ const int __ndxlNystromStage0(NDXLNetwork* _Network);
_NDXLEXP_ const int __ndxlNystromStage1(NDXLNetwork* _Network);
_NDXLEXP_ const int __ndxlNystromStage2(NDXLNetwork* _Network);
_NDXLEXP_ const int __ndxlNystromStage3(NDXLNetwork* _Network);
_NDXLEXP_ const int __ndxlNystromStage4(NDXLNetwork* _Network);
_NDXLEXP_ const int __ndxlNystromStage5(NDXLNetwork* _Network);

// ^^^^^ Nystrom | Dormand-Prince vvvvv

extern double __butcherDopriA[20];
extern double __butcherDopriB[6];
extern double __butcherDopriC[7];

_NDXLEXP_ const int __ndxlDopriStage0(NDXLNetwork* _Network);
_NDXLEXP_ const int __ndxlDopriStage1(NDXLNetwork* _Network);
_NDXLEXP_ const int __ndxlDopriStage2(NDXLNetwork* _Network);
_NDXLEXP_ const int __ndxlDopriStage3(NDXLNetwork* _Network);
_NDXLEXP_ const int __ndxlDopriStage4(NDXLNetwork* _Network);
_NDXLEXP_ const int __ndxlDopriStage5(NDXLNetwork* _Network);
_NDXLEXP_ const int __ndxlDopriStage6(NDXLNetwork* _Network, double* _Errsum);

// ^^^^^ Dormand-Prince | Cash-Karp vvvvv

extern double __butcherCashKarpA[15];
extern double __butcherCashKarpB5[4];
extern double __butcherCashKarpB4[5];
extern double __butcherCashKarpC[6];

_NDXLEXP_ const int __ndxlRkckStage0(NDXLNetwork* _Network);
_NDXLEXP_ const int __ndxlRkckStage1(NDXLNetwork* _Network);
_NDXLEXP_ const int __ndxlRkckStage2(NDXLNetwork* _Network);
_NDXLEXP_ const int __ndxlRkckStage3(NDXLNetwork* _Network);
_NDXLEXP_ const int __ndxlRkckStage4(NDXLNetwork* _Network);
_NDXLEXP_ const int __ndxlRkckStage5(NDXLNetwork* _Network, double* _Errsum);

// ^^^^^ Cash-Karp | Euler-Maruyama vvvvv

_NDXLEXP_ const int __ndxlEulerStage0(NDXLNetwork* _Network);

// Euler-Maruyama | Heun-Euler vvvvv

_NDXLEXP_ const int __ndxlHeunStage0(NDXLNetwork* _Network);
_NDXLEXP_ const int __ndxlHeunStage1(NDXLNetwork* _Network, double* _Errsum);

// ^^^^^ Heun-Euler | Radau IIA vvvvv

extern double __butcherRadauA[9];
extern double __butcherRadauC[3];
extern double __radauEig[3];
extern double __radauMat[9];
extern double __radauInv[9];
extern double __radauErr[3];

_NDXLEXP_ const int __ndxlRadauSlopes(NDXLNetwork* _Network, double* _Jac, double* _Mlhsreal, double* _Mrhsreal, double* _Decoupreal, complex_t* _Mlhsimag, complex_t* _Mrhsimag, complex_t* _Decoupimag, double* _Erhs, double* _Errloc, size_t** _Pivots);
_NDXLEXP_ void __ndxlRadauGuesses(NDXLNetwork* _Network, double* _Jac, double* _Mlhsreal, complex_t* _Mlhsimag, size_t** _Pivots);
_NDXLEXP_ const int __ndxlRadauSolve(NDXLNetwork* _Network, double* _Mlhsreal, double* _Mrhsreal, double* _Decoupreal, complex_t* _Mlhsimag, complex_t* _Mrhsimag, complex_t* _Decoupimag, size_t** _Pivots);
_NDXLEXP_ const double __ndxlRadauError(NDXLNetwork* _Network, double* _Mlhsreal, double* _Erhs, double* _Errloc, size_t** _Pivots);

// ^^^^^ Radau IIA | integration helpers vvvvv

_NDXLEXP_ const int __ndxlSelectInitialStep(NDXLNetwork* _Network, const double _OrderInv);
_NDXLEXP_ const int __ndxlSelectInitialStepStochastic(NDXLNetwork* _Network, const double _OrderInv);
_NDXLEXP_ const int __ndxlNumericalJacobian(NDXLNetwork* _Network, double* _Jac);
_NDXLEXP_ void __ndxlPluDecompose(double* _Mat, size_t* _P, const size_t _Dim);
_NDXLEXP_ void __ndxlPluDecomposeZ(complex_t* _Mat, size_t* _P, const size_t _Dim);
_NDXLEXP_ void __ndxlPluSolve(double* _Ans, double* _Mat, double* _Rhs, size_t* _P, const size_t _Dim);
_NDXLEXP_ void __ndxlPluSolveZ(complex_t* _Ans, complex_t* _Mat, complex_t* _Rhs, size_t* _P, const size_t _Dim);

// ^^^^^ Integration helpers

#endif // _INTEGRATION_NDXL_
