from __future__ import annotations

from pathlib import Path
from typing import Union

import copy
import ctypes
import inspect
import numpy as np
import platform
import re
import sys
import traceback

# Load library from relative path
_cwd = Path(__file__).resolve().parent
_sysname = platform.system()
if _sysname == "Windows":
	libname = "NDXL.dll"
elif _sysname == "Linux":
	libname = "libNDXL.so"
elif (_sysname == "Darwin") and (platform.machine() == "x86_64"):
	libname = "libNDXL.dylib"
else:
	raise OSError(f"Unsupported platform: {_sysname}")
lib = ctypes.CDLL(str(_cwd / "lib" / libname))


# Error definitions from library header
NDXL_OK		 	= 0
NDXL_ERR		= -1
NDXL_VALUE_ERR  = 1
NDXL_ARG_ERR	= 2
NDXL_SIM_ERR	= 3


# NDXL object definitions
class NDXLNetwork(ctypes.Structure): pass
NDXLNetwork_p = ctypes.POINTER(NDXLNetwork)

class NDXLChannel(ctypes.Structure): pass
NDXLChannel_p = ctypes.POINTER(NDXLChannel)

class NDXLVertex(ctypes.Structure): pass
NDXLVertex_p = ctypes.POINTER(NDXLVertex)

class NDXLEdge(ctypes.Structure): pass
NDXLEdge_p = ctypes.POINTER(NDXLEdge)


# NDXL core functions
lib.ndxlGetLastErrorCode.restype = ctypes.c_int
lib.ndxlGetLastErrorCode.argtypes = []
ndxlGetLastErrorCode = lib.ndxlGetLastErrorCode

lib.ndxlGetLastErrorMsg.restype = ctypes.c_char_p
lib.ndxlGetLastErrorMsg.argtypes = []
ndxlGetLastErrorMsg = lib.ndxlGetLastErrorMsg

lib.ndxlGetNetworkCount.restype = ctypes.c_size_t
lib.ndxlGetNetworkCount.argtypes = []
ndxlGetNetworkCount = lib.ndxlGetNetworkCount

lib.ndxlPrintError.restype = None
lib.ndxlPrintError.argtypes = []
ndxlPrintError = lib.ndxlPrintError

lib.ndxlRaiseError.restype = None
lib.ndxlRaiseError.argtypes = [ctypes.c_int, ctypes.c_char_p]
ndxlRaiseError = lib.ndxlRaiseError

lib.ndxlGetNetwork.restype = NDXLNetwork_p
lib.ndxlGetNetwork.argtypes = [ctypes.c_size_t]
ndxlGetNetwork = lib.ndxlGetNetwork

lib.ndxlGetNetworkByIndex.restype = NDXLNetwork_p
lib.ndxlGetNetworkByIndex.argtypes = [ctypes.c_size_t]
ndxlGetNetworkByIndex = lib.ndxlGetNetworkByIndex

lib.ndxlGetNetworkCount.restype = ctypes.c_size_t
lib.ndxlGetNetworkCount.argtypes = []
ndxlGetNetworkCount = lib.ndxlGetNetworkCount

def get_last_error_code()->int:
	"""Gets the most recently raised error code.

	Returns
	-------
	int
		The most recently raised error code anywhre in the API.

	Notes
	-----
	This function is often used in conjunction with `get_last_error_msg()` for a full error diagnostic.

	See Also
	--------
	[`get_last_error_msg`][ndxl.get_last_error_msg] : Get the human-readable error string associated with the error code.
	"""
	return ndxlGetLastErrorCode()

def get_last_error_msg()->str:
	"""Gets the most recently raised error message.

	Returns
	-------
	str
		The most recently raised error message anywhre in the API.

	Notes
	-----
	This function is often used in conjunction with `get_last_error_code()` for a full error diagnostic.

	See Also
	--------
	[`get_last_error_code`][ndxl.get_last_error_code] : Get the error code associated with the error message.
	"""
	return ndxlGetLastErrorMsg().decode('utf-8')

def get_network(idx: int)->NDXLNetwork_p
	"""Gets the NDXLNetwork object at position `idx` in the underlying network array.

	Parameters
	----------
	idx : int
		Relative index (position) of network in the global array. Must be greater than or equal to zero, and less than `get_network_count()`.

	Returns
	-------
	NDXLNetwork_p
		The NDXLNetwork pointer at position `idx`, or `None` if either `idx` is out of bounds, or no networks exist yet.

	Notes
	-----
	A return value of `None` does not necessarily indicate an error, as no networks could exist yet. If unsure, use `get_last_error_code()` to check.

	See Also
	[`get_last_error_code`][ndxl.get_last_error_code] : Verify that the function raised no internal errors.
	[`get_network_count`][ndxl.get_network_count] : Get the total number of existing networks (also the length of the network array).
	"""
	_ptr = ndxlNetworkByIndex(idx)
	return _ptr if _ptr else None

def get_network_by_index(idx: int)->NDXLNetwork_p
	"""Gets the NDXLNetwork object whose id is `idx`.

	Parameters
	----------
	idx : int
		Absolute index (id) of the network. Equivalent to calling ndxl.network.index.

	Returns
	-------
	NDXLNetwork_p
		The NDXLNetwork pointer with id equal to `idx`, or `None` if either no network with a matching id is found, or no networks exist yet.

	Notes
	-----
	A return value of `None` does not necessarily indicate an error, as no networks could exist yet. If unsure, use `get_last_error_code()` to check.

	See Also
	--------
	[`get_last_error_code`][ndxl.get_last_error_code] : Verify that the function raised no internal errors.
	[`network.index`][ndxl.network.index] : Get the unique id of a network.
	"""
	_ptr = ndxlNetworkByIndex(idx)
	return _ptr if _ptr else None

def get_network_count()->int:
	"""Gets the number of individual NDXLNetwork objects currently alive.
	
	Returns
	-------
	int
		The number of networks that have not yet been destroyed.
	"""
	return ndxlGetNetworkCount()

def print_error()->None:
	"""Prints the most recent error code and message in an easily readable format.

	Returns
	-------
	None
	"""
	ndxlPrintError()

def raise_error(code: int, msg: str)->None:
	"""Manually sets the library's most recent error.

	Parameters
	----------
	code : int
		Error code to set.
	msg : str
		Error message associated with `code`.

	Returns
	-------
	None

	Notes
	-----
	Allows users to explicitly set an NDXL error code and associated error message. This method is used for experimental purposes and library debugging; use thereof is not recommended.
	"""
	ndxlRaiseError(code, msg.encode('utf-8'))