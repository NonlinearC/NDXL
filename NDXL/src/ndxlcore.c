#include "internal/internal.h"

char __ndxlIsInitialised = 0;
int __ndxlLastErrorCode = 0;
char* __ndxlLastErrorMsg = 0;
double __ndxlSqrtEpsilon = 1.4901161193847656e-08;
NDXLNetwork** __ndxlNetworks = 0;
size_t __ndxlNetworkCount = 0;
size_t __ndxlNetworkCap = 3;

// Internal functions vvvvv

static inline void __onExit()
{
	if (__ndxlLastErrorMsg) free(__ndxlLastErrorMsg);
	while (__ndxlNetworkCount)
		__ndxlDestroyNetwork(__ndxlNetworks[0]);
	if (__ndxlNetworks) free(__ndxlNetworks);
	__ndxlIsInitialised = 0;
}

void __ndxlInit()
{
	atexit(__onExit);
	__ndxlNetworks = (NDXLNetwork**)malloc(__ndxlNetworkCap * sizeof(NDXLNetwork*));
	__ndxlSetLastError(NDXL_OK, "Successfully initialised Network Dynamics eXperimentation Library.");
	__ndxlIsInitialised = 1;
}

const double __ndxlRandomDouble()
{
	double _Time = uniTime();
	size_t _Sec = (size_t)_Time;
	srand(_Sec ^ (size_t)((_Time - _Sec) * 1e9));
	return ((double)rand()) / ((double)RAND_MAX);
}

void __ndxlResizeNetworkArray()
{
	// Verify capacity of network array
	if (__ndxlNetworkCount > __ndxlNetworkCap)
	{
		__ndxlNetworkCap = (__ndxlNetworkCap << 1) + 1;
		__ndxlNetworks = (NDXLNetwork**)realloc(__ndxlNetworks, __ndxlNetworkCap * sizeof(NDXLNetwork*));
	}
}

void __ndxlSetLastError(const int _Code, const char* const _Msg)
{
	__ndxlLastErrorCode = _Code;
	const size_t _Len = (strlen(_Msg) + 1) * sizeof(char);
	__ndxlLastErrorMsg = (char*)realloc(__ndxlLastErrorMsg, _Len);
	memcpy(__ndxlLastErrorMsg, _Msg, _Len);
}

const double __ndxlSquare(const double _X)
{
	return _X * _X;
}

// ^^^^^ Internal functions | external functions vvvvv

_NDXLEXP_ const int ndxlGetLastErrorCode()
{
	return __ndxlLastErrorCode;
}

_NDXLEXP_ const char* const ndxlGetLastErrorMsg()
{
	return __ndxlLastErrorMsg;
}

_NDXLEXP_ void ndxlPrintError()
{
	if (stderr)
	{
		if (__ndxlLastErrorCode == NDXL_OK)
			fprintf(stderr, __ndxlLastErrorMsg);
		else
			fprintf(stderr, "ERROR %d: %s", __ndxlLastErrorCode, __ndxlLastErrorMsg);
		fprintf(stderr, "\n");
	}
	else if (stdout)
	{
		if (__ndxlLastErrorCode == NDXL_OK)
			printf(__ndxlLastErrorMsg);
		else
			printf("ERROR %d: %s", __ndxlLastErrorCode, __ndxlLastErrorMsg);
		printf("\n");
	}
}

_NDXLEXP_ void ndxlRaiseError(const int _Code, const char* const _Msg)
{
	__ndxlSetLastError(_Code, _Msg);
}

// ^^^^^ External functions