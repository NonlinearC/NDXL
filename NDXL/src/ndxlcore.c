//------------------------------------------\\
// NDXL source file for core functionality	\\
// Copyright Ethan Codron 2026				\\
// GNU GPL-3.0								\\
//------------------------------------------\\
// Source file providing all core internal	\\
// functions like error raising.			\\
//------------------------------------------\\

#include "internal/internal.h"

bool __ndxlIsInitialised = 0;
NDXLNetwork* __ndxlNetworkHead = 0;
NDXLNetwork* __ndxlNetworkTail = 0;
size_t __ndxlNetworkCount = 0;
int __ndxlLastErrorCode = 0;
char* __ndxlLastErrorMsg = 0;

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
	__setLastError(_Code, _Msg);
}

_NDXLEXP_ const int ndxlInit()
{
	if (!__ndxlIsInitialised)
	{
		atexit(__onExit);
		atexit(__ndxlCleanupNetworks);
		__setLastError(NDXL_OK, "Successfully initialised Network Dynamics eXperimentation Library.");
		__ndxlIsInitialised = 1;
	}
	return NDXL_OK;
}

_NDXLEXP_ const size_t ndxlGetNetworkCount()
{
	return __ndxlNetworkCount;
}