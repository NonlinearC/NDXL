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

lib.ndxlGetLastErrorMsg.restype = ctypes.c_char_p
lib.ndxlGetLastErrorMsg.argtypes = []

lib.ndxlGetNetworkCount.restype = ctypes.c_size_t
lib.ndxlGetNetworkCount.argtypes = []

lib.ndxlPrintError.restype = None
lib.ndxlPrintError.argtypes = []

lib.ndxlRaiseError.restype = None
lib.ndxlRaiseError.argtypes = [ctypes.c_int, ctypes.c_char_p]

lib.ndxlInit.restype = ctypes.c_int
lib.ndxlInit.argtypes = []


# Wrapper functions
def libinit()->int:
	'''
	Initialises the library's internal variables and garbage collection.
	'''
	return lib.ndxlInit()

def get_last_error_code()->int:
	'''
	Gets the most recently raised error code.
	'''
	return lib.ndxlGetLastErrorCode()

def get_last_error_msg()->str:
	'''
	Gets the most recently raised error message.
	'''
	return lib.ndxlGetLastErrorMsg().decode('utf-8')

def network_count()->int:
	'''
	Gets the number of individual NDXLNetwork objects currently alive.
	'''
	return lib.ndxlGetNetworkCount()

def print_error()->None:
	'''
	Prints the most recent error code and message in an easily readable format.
	'''
	lib.ndxlPrintError()

def raise_error(code: int, msg: str)->None:
	'''
	Manually sets the library's most recent error.
	Allows users to explicitly set an NDXL error code and associated error message.
	
	:param code: Error code to set.
	:param msg: Error message associated with `code`.
	'''
	lib.ndxlRaiseError(code, msg.encode('utf-8'))