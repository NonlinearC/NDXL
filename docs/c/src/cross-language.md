# Cross-Language Interoperability {#crosslang}
As we eluded to earlier, the architecture of NDXL is designed with two core concepts in mind, namely, maximum user control, and programming language-independence.
The raw C functions therefore expose many parameters, most of which are unusued most of the time, and the whole API is structured around only primitive and opaque types, making direct usage thereof very verbose.
For these reasons, the intended use of NDXL is a standalone library that can be imported into and used from other programming languages, with those language-specific bindings handling much of the boilerplate code.
This page demonstrates this process, using the [Python version](https://pypi.org/project/ndxl/) of NDXL as an example, the source code of which can be found on [GitHub](https://github.com/NonlinearC/NDXL/tree/main/bindings/python).

\note To avoid clutter and repetition, we will not show every file and function in the library. Instead, we stick to core concepts, design philosophy, and the basics of the "how to import" procedure.

## Design Framework
To import NDXL into another programming language, we need a foreign library interface in that language that handles external library loading, symbol exposure, and data type conversion.
This interface is usually in the form of language-specific functionality (e.g., `dllimport` in C/C++), or a package that mediates the operations (e.g., [ctypes](https://docs.python.org/3/library/ctypes.html) in Python; [Libdl](https://docs.julialang.org/en/v1/stdlib/Libdl/) in Julia).
We then make use of the interface to

- Define the opaque types and function signatures exported in NDXL
- Construct wrapper classes that operate on the memory held by the opaque objects
- Construct wrapper functions that interface with the API functions using the wrapper classes
- Export the wrapper classes and functions as a more user-friendly interface so the user does not concern about things like memory management

\note Perhaps most crucially, because NDXL manages its own memory, we do not want our wrapper classes "managing" the underlying memory so much as "simply using" it. We distinguish between the destructor of a wrapper class, which simply releases its handle on the underlying object memory, and the C destructor of the object itself e.g., @ref ndxlDestroyNetwork().

Almost always will the dynamic versions (`NDXL.dll`, `libNDXL.so`, `libNDXL.dylib`) of NDXL be used from the foreign library interface.
Since NDXL supports Windows, Unix, and x86-64 macOS operating systems, we also need to detect the operating system and load the corresponding library file.
The wrapper code is ideally platform independent maintatining NDXL's "write once, run anywhere" design semantic.

## Loading the Library
In Python, we make use of the `pathlib` and `platform` libraries to detect, respectively

- Where on the computer we are (path)
- What operating system is currently running

Using that, we extract the appropriate library file name relative to the current path, warning the user if an unsupported operating system is detected.
In `core.py`, we load the dynamic library once using the `ctypes` library; we will use the `lib` variable throughout the rest of the API to interface with the library symbols.

```python
from pathlib import Path

import ctypes
import numpy as np
import platform

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
```

Since we're in `core.py`, this is the ideal place to put all "core library functionality" such as error codes and opaque object definitions.
We use `ctypes.Structure` to define a C `struct`, but leave the corresponding Python class empty to indicate its definition is opaque.
We then create the "pointer to object" types using `ctypes.POINTER` on the empty classes.

```python
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
```

When we loaded the library using `ctypes.CDLL`, all exported symbols were automatically detected and added to `lib` by name.
However, `ctypes` knows only of the functions' existence, not their signatures or return values.
We must specify these by setting each function's `restype` (return type) and `argtypes` (parameters) properties.

```python
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
```

Of course, we do not want to use these functions with pure C types like `const char*`, but with Python types like `str`.
This is where our wrapper functions come in, mediating the translation between the languages.

```python
# Wrapper functions
def libinit():
	return lib.ndxlInit()

def get_last_error_code():
	return lib.ndxlGetLastErrorCode()

def get_last_error_msg():
	return lib.ndxlGetLastErrorMsg().decode('utf-8')

def network_count():
	return lib.ndxlGetNetworkCount()

def print_error():
	lib.ndxlPrintError()

def raise_error(code, msg):
	lib.ndxlRaiseError(code, msg.encode('utf-8'))
```

## Defining Class-Specific Functions
The files `network.py`, `channel.py`, `vertex.py`, and `edge.py` each define their own wrapper classes for the corresponding NDXL objects.
They begin by specifying the `restype` and `argtypes` properties of each function pertaining to the eventual wrapper object; basically, whatever is in `ndxl_network.h` will have a corresponding `ctypes` signature and wrapper in `network.py`, and so forth.
Again, to avoid clutter, we will only show a few functions here, but the design philosophy remains the same throughout all the Python wrapper files.

```python
from __future__ import annotations

from .core import *

# Callbacks vvvvv

NDXLNetworkCallback = ctypes.CFUNCTYPE(ctypes.c_int, NDXLNetwork_p, ctypes.c_double, ctypes.c_void_p)
NoneNCb = NDXLNetworkCallback(0)

# ^^^^^ Callbacks | constructor and destructor vvvvv

lib.ndxlCreateNetwork.restype = NDXLNetwork_p
lib.ndxlCreateNetwork.argtypes = [ctypes.c_void_p]

lib.ndxlDestroyNetwork.restype = ctypes.c_int
lib.ndxlDestroyNetwork.argtypes = [ctypes.POINTER(NDXLNetwork_p)]

# ^^^^^ Constructor and destructor | getters vvvvv

lib.ndxlGetNetworkOrder.restype = ctypes.c_size_t
lib.ndxlGetNetworkOrder.argtypes = [NDXLNetwork_p]

lib.ndxlGetNetworkParams.restype = ctypes.c_void_p
lib.ndxlGetNetworkParams.argtypes = [NDXLNetwork_p]

lib.ndxlGetNetworkTime.restype = ctypes.c_double
lib.ndxlGetNetworkTime.argtypes = [NDXLNetwork_p]

lib.ndxlGetNetworkVertices.restype = ctypes.c_int
lib.ndxlGetNetworkVertices.argtypes = [NDXLNetwork_p, ctypes.POINTER(NDXLVertex_p)]

# ^^^^^ Getters | setters vvvvv

lib.ndxlSetNetworkParams.restype = ctypes.c_int
lib.ndxlSetNetworkParams.argtypes = [NDXLNetwork_p, ctypes.c_void_p]

# ^^^^^ Setters | traversal vvvvv

lib.ndxlGetNetworkFirstVertex.restype = NDXLVertex_p
lib.ndxlGetNetworkFirstVertex.argtypes = [NDXLNetwork_p]

lib.ndxlGetNetworkLastVertex.restype = NDXLVertex_p
lib.ndxlGetNetworkLastVertex.argtypes = [NDXLNetwork_p]

lib.ndxlGetNetworkNextVertex.restype = NDXLVertex_p
lib.ndxlGetNetworkNextVertex.argtypes = [NDXLVertex_p]

lib.ndxlGetNetworkPreviousVertex.restype = NDXLVertex_p
lib.ndxlGetNetworkPreviousVertex.argtypes = [NDXLVertex_p]

# ^^^^^ Traversal | simulation vvvvv

lib.ndxlNetworkSimulateRalston.restype = ctypes.c_int
lib.ndxlNetworkSimulateRalston.argtypes = [NDXLNetwork_p, ctypes.c_double, ctypes.c_double, NDXLNetworkCallback]

lib.ndxlNetworkSimulateDopri.restype = ctypes.c_int
lib.ndxlNetworkSimulateDopri.argtypes = [NDXLNetwork_p, ctypes.c_double, ctypes.c_double, ctypes.c_double, ctypes.POINTER(ctypes.c_double), ctypes.c_size_t, NDXLNetworkCallback]

lib.ndxlNetworkSimulateHeun.restype = ctypes.c_int
lib.ndxlNetworkSimulateHeun.argtypes = [NDXLNetwork_p, ctypes.c_double, ctypes.c_double, ctypes.c_double, ctypes.POINTER(ctypes.c_double), ctypes.c_size_t, NDXLNetworkCallback]

lib.ndxlNetworkSimulateRadau.restype = ctypes.c_int
lib.ndxlNetworkSimulateRadau.argtypes = [NDXLNetwork_p, ctypes.c_double, ctypes.c_double, ctypes.c_double, ctypes.POINTER(ctypes.c_double), ctypes.c_size_t, NDXLNetworkCallback]

# ^^^^^ Simulation
```

This is where the personal flavour element of wrapper library design comes in: one needs to decide how and when to call constructors, destructors, iterators, etc, so that the interface remains memory safe but intuitive.
For example, because agents and noise channels in NDXL all belong to a particular host network, we made the network class in Python responsible for:

- Creating agents and noise channels
- Storing created objects so that the user can ignore constructor return values
- Preserving the `ctypes` wrapper pointers
- Handling simulation parameters via a common `simulate()` function

Each object is also responsible for storing its own parameters pointer to by their `void* params` to ensure the pointer remains valid as long as the object is alive.
Repeated references to the same object simply share the underlying `void* params` memory, which only gets garbage collected when the final object pointing to it releases its handle on it.

The final bullet was an ease-of-use choice; the average user does not necessarily care what the underlying numerical method is, nor its numerical properties.
They just want to get a simulation up and running as quickly and efficiently as possible, worrying about the details of the procedure later (e.g., switching to a stiff solver if the dynamics necessitate it).
To facilitate this process, we had to ensure that

- The user supplied a valid simulation method for the `method` argument
- An appropriate default method is chosen if none is provided
- That method-specific parameters are passed (for example, one cannot supply a step size to an adaptive method, or an absolute tolerance to a static method)

We therefore defined several helper classes that combine the simulation method and arguments it requires, along with default values for those arguments where applicable.

```python
class simulationmethod:
	def __init__(self, func, static, kwdef):
		self._func = func
		self._static = static
		self._kwdef = kwdef

	@property
	def func(self):
		return self._func

	@property
	def kwdef(self):
		return self._kwdef

	@property
	def static(self):
		return self._static

class methodnames(type):
	@property
	def ralston(self):
		return simulationmethod(lib.ndxlNetworkSimulateRalston, True, (0.1,))

	@property
	def dopri(self):
		return simulationmethod(lib.ndxlNetworkSimulateDopri, False, (1e-6, 1e-3))

	@property
	def heun(self):
		return simulationmethod(lib.ndxlNetworkSimulateHeun, False, (1e-3, 1e-3))

	@property
	def radau(self):
		return simulationmethod(lib.ndxlNetworkSimulateRadau, False, (1e-8, 1e-6))

	def _Get_method_by_name(self, name):
		if re.match(r"(?i)ralston", name):
			return self.ralston
		if re.match(r"(?i)(dopri)|(dormand(-| )?prince)", name):
			return self.dopri
		if re.match(r"(?i)heun((-| )?euler)?", name):
			return self.heun
		if re.match(r"(?i)radau((ii)|2a)?", name):
			return self.radau
		return None

class methods(metaclass=methodnames):
	pass
```

The `methods` interface allows the user to pass either a `simulationmethod` directly e.g., `ndxl.methods.dopri`, or by name e.g., `"ralston"`.

## Defining the Class
The `network` class can now be designed.
The constructor takes in a raw @ref NDXLNetwork pointer and its corresponding `void* params` for reasons we will see later with callbacks.
It also registers the network with the class' internal `nwkdict` dictionary, but only if the pointer does not exist yet.
The destructor then simply releases handles on the underlying C memory, in line with the design choice of "memory ownership" vs "memory usage".
Networks are created using an external factory function, `create_network()`.

```python
class network:
	nwkdict = dict()

	def __init__(self, ptr, args):
		self._ptr = ptr
		self._pptr = args
		self._params = ctypes.cast(self._pptr, ctypes.POINTER(ctypes.py_object)).contents.value
		self._vdict = self._params[1]
		self._edict = self._params[2]
		self._cdict = self._params[3]
		if self._Int_ptr not in network.nwkdict:
			network.nwkdict[self._Int_ptr] = self

	def __del__(self):
		self._ptr = None
		self._pptr = None
		self._params = None

def create_network(args=None):
	_args = () if args is None else ((args,) if np.isscalar(args) else copy.deepcopy(args))
	_pptr = ctypes.cast(ctypes.pointer(ctypes.py_object([None, dict(), dict(), dict(), None, _args])), ctypes.c_void_p)
	return network(lib.ndxlCreateNetwork(_pptr), _pptr)
```

We then define some comparison dunder methods so we can do C-like comparisons between network pointers, and stringify dunder methods for printing the class to an output stream.

```python
	def __bool__(self):
		return bool(self._ptr)
		
	def __eq__(self, nwk):
		if isinstance(nwk, network):
			return self._Int_ptr == nwk._Int_ptr
		return False

	def __ne__(self, nwk):
		if isinstance(nwk, network):
			return self._Int_ptr != nwk._Int_ptr
		return False

	def __hash__(self):
		return hash(self._Int_ptr)

	def __str__(self):
		return "'<ndxl.network object at " + f"{id(self._Int_ptr):#0{int(platform.architecture()[0][:-3]) // 4 + 2}X}" + " {'order': " + str(lib.ndxlGetNetworkOrder(self._ptr)) + ", 'size': " + str(lib.ndxlGetNetworkSize(self._ptr)) + ", 'time': " + str(lib.ndxlGetNetworkTime(self._ptr)) + "}>'"

	def __repr__(self):
		return f"NDXL network\n============\n\nVertices (order): {lib.ndxlGetNetworkOrder(self._Int_ptr)}\nEdges (size): {lib.ndxlGetNetworkSize(self._ptr)}\nTime: {lib.ndxlGetNetworkTime(self._ptr)}\n"
```

We also provide some class management methods for handling the pointers and internal errors stored/raised by NDXL.

\warning These methods should <b>NEVER</b> be called by the user, as they are purely for internal resource management.

```python
	def _Deregister_channel(self, cid):
		del self._cdict[cid]

	def _Deregister_edge(self, eid):
		del self._edict[eid]

	def _Deregister_vertex(self, vid):
		del self._vdict[vid]

	def _Set_error(self, err):
		self._params[-2] = err

	@property
	def _Int_ptr(self):
		return ctypes.cast(self._ptr, ctypes.c_void_p).value
```

## Class Properties and Object Constructors
Most of the getter and setter functions, and the destructor `destroy()`, interface with and return values directly from the C API, while the traversal functions simply wrap the traversed objects in the Python classes.

```python
@property
	def c_ptr(self):
		return self._ptr

	def destroy(self):
		del network.nwkdict[self._Int_ptr]
		_res = lib.ndxlDestroyNetwork(ctypes.pointer(self._ptr))
		del self._pptr
		del self._params
		return _res

	@property
	def first_vertex(self):
		from .vertex import vertex
		_vert = lib.ndxlGetNetworkFirstVertex(self._ptr)
		if _vert:
			return vertex(_vert, lib.ndxlGetVertexParams(_vert))
		return None

	@property
	def last_vertex(self):
		from .vertex import vertex
		_vert = lib.ndxlGetNetworkLastVertex(self._ptr)
		if _vert:
			return vertex(_vert, lib.ndxlGetVertexParams(_vert))
		return None

	@property
	def order(self):
		return lib.ndxlGetNetworkOrder(self._ptr)
	
	@property
	def params(self):
		return self._params[-1]

	@property
	def time(self):
		return lib.ndxlGetNetworkTime(self._ptr)

	@property
	def vertices(self):
		from .vertex import vertex
		_vert = lib.ndxlGetNetworkFirstVertex(self._ptr)
		while _vert:
			yield vertex(_vert, lib.ndxlGetVertexParams(_vert))
			_vert = lib.ndxlGetNetworkNextVertex(_vert)

	@property
	def vertex_list(self):
		return list(self.vertices)
```

The `vertices` and `vertex_list` functions serve similar but distinct purposes: the former is a Python generator that iterates over the vertices in a network, evaluating each "next" vertex only when needed.
This is useful for operating on each vertex in sequence, but <b>should not be used for destroying vertices</b>, as this will invalidate the underlying pointer and cause @ref ndxlGetNetworkNextVertex "`ndxlGetNetworkNextVertex(_vert)`" to fail.
The latter is a fully evaluated Python list containing the vertex objects, useful for grabbing a list once and then performing repeated operations on them.
For destroying vertices in sequence, the list should be used instead of the generator, as the list does not rely on getting the "next" vertex pointer.

Creating vertices is done through the network's `create_vertex()` method, which registers the vertex with its internal `_vdict` dictionary.
The constructor also handles shaped and complex input arrays, and links the `dynamics`, `noise`, and `callback` functions of the vertex respectively to the Python dispatch functions `_vfunc_bridge`, `_vnoise_bridge`, and `_vcb_bridge` (see `vertex.py` in the [source code](https://github.com/NonlinearC/NDXL/tree/main/bindings/python)).

```python
def create_vertex(self, state=None, dynamics=None, noise=None, channels=None, cb=None, args=None, cplx=False):
		from .vertex import vertex, _vfunc_bridge, _vnoise_bridge, _vcb_bridge, NoneVFn, NoneVNz, NoneVCb
		_state = None
		_nstate = 1
		_sstate = ()
		_cplx = cplx
		_args = () if args is None else ((args,) if np.isscalar(args) else copy.deepcopy(args))
		if state is not None:
			_arr = np.array([state]) if np.isscalar(state) else np.asarray(state)
			_sstate = _arr.shape
			if np.issubdtype(_arr.dtype, np.complexfloating) or cplx:
				_st = _arr.flatten().astype(complex)
				_nstate = len(_st) * 2
				_state = np.ctypeslib.as_ctypes(_st.view(float))
				_cplx = True
			else:
				_st = _arr.flatten().astype(float)
				_nstate = len(_st)
				_state = np.ctypeslib.as_ctypes(_st)
		if channels is None:
			if noise is None:
				_nchannels = 0
				_cn = None
			else:
				_nchannels = 0 if _state is None else len(_state)
				_carr = [None for _ in range(_nchannels)]
				_cn = (NDXLChannel_p * _nchannels)(*_carr)
		else:
			_carr = np.array([channels]) if np.isscalar(channels) else np.asarray(channels).flatten()
			_carr = [(None if _x is None else _x.c_ptr) for _x in _carr]
			_nchannels = len(_carr)
			_cn = (NDXLChannel_p * _nchannels)(*_carr)
		_pptr = ctypes.cast(ctypes.pointer(ctypes.py_object([dynamics, noise, cb, _cplx, _sstate, _args])), ctypes.c_void_p)
		_vert = vertex(lib.ndxlCreateVertex(self._ptr, _state, _nstate, NoneVFn if dynamics is None else _vfunc_bridge, NoneVNz if noise is None else _vnoise_bridge, _cn, _nchannels, NoneVCb if cb is None else _vcb_bridge, _pptr), _pptr)
		self._vdict[_vert._Int_ptr] = _vert
		return _vert
```

Of course, error checking and input validation are yet to be added, as this is still the barebones implementation of an object constructor.
Vertices can then be destroyed using their own `destroy()` destructors.

## Simulation
The final method we will show is the network's `simulate()` method, illustrating how method-specific keyword arguments are handled, and how the interface makes usage far simpler and less verbose than in C.

```python
def simulate(self, tspan, method=None, **simkwargs):
		# Ensure tspan is a numeric, non-complex value
		if (not isinstance(tspan, float)) and (not isinstance(tspan, int)):
			raise TypeError(f"Expected numeric value for tspan, got {tspan} of type {type(tspan)}.")

		# Use DOPRI method by default, ensure passed method is one of the NDXL options
		if method is None:
			_method = methods.dopri
		elif isinstance(method, simulationmethod):
			_method = method
		elif isinstance(method, str):
			_method = methods._Get_method_by_name(method.strip().lower())
			if _method is None:
				raise ValueError(f"Unknown method name '{method}'.")
		else:
			raise TypeError(f"Expected simulation method or string for method, got {method} of type {type(method)}.")

		# Extract simkwargs based on whether method is static or adaptive, ensure passed parameters are of correct type
		if _method.static:
			_step = simkwargs.pop("h", simkwargs.pop("step", simkwargs.pop("stepsize", simkwargs.pop("step_size", _method.kwdef[0]))))
			if (not isinstance(_step, float)) and (not isinstance(_step, int)):
				raise TypeError(f"Expected numeric value for step size, got {_span} of type {type(_span)}.")
		else:
			_atol = simkwargs.pop("atol", simkwargs.pop("abstol", _method.kwdef[0]))
			if (not isinstance(_atol, float)) and (not isinstance(_atol, int)):
				raise TypeError(f"Expected numeric value for absolute tolerance, got {_atol} of type {type(_atol)}.")
			_rtol = simkwargs.pop("rtol", simkwargs.pop("reltol", _method.kwdef[1]))
			if (not isinstance(_rtol, float)) and (not isinstance(_rtol, int)):
				raise TypeError(f"Expected numeric value for relative tolerance, got {_rtol} of type {type(_rtol)}.")
			_tstops = simkwargs.pop("tstops", simkwargs.pop("t_stops", simkwargs.pop("timestops", simkwargs.pop("time_stops", None))))
			if _tstops is None:
				_nstops = 0
			else:
				_stops = [_tstops] if np.isscalar(_tstops) else list(_tstops)
				for i, _stop in enumerate(_stops):
					if (not isinstance(_stop, float)) and (not isinstance(_stop, int)):
						raise TypeError(f"Expected numeric values for time stops, got {_stop} of type {type(_stop)} at index {i}.")
					_stops[i] = float(_stop)
				_nstops = len(_stops)
				_tstops = (ctypes.c_double * _nstops)(*_stops)

		# Validate callback
		_cb = simkwargs.pop("cb", simkwargs.pop("callback", None))
		if _cb is not None:
			if not inspect.isfunction(_cb):
				raise TypeError(f"Expected function for callback, got {_cb} of type {type(_cb)}.")
			_sig = inspect.signature(_cb)
			if not _sig.bind(self, 0., *self._params[-1]):
				raise TypeError(f"Callback must have signature cb(nwk, t, *args).")

		# Ensure no remaining keywords after extraction
		if len(simkwargs) > 0:
			raise KeyError(f"Got unexpected or unsupported keyword argument '{next(iter(simkwargs))}'.")

		# Run simulation
		self._params[0] = _cb
		if _method.static:
			_result = _method.func(self._ptr, tspan, _step, NoneNCb if _cb is None else _network_bridge)
		else:
			_result = _method.func(self._ptr, tspan, _atol, _rtol, _tstops, _nstops, NoneNCb if _cb is None else _network_bridge)
		if self._params[-2] is not None:
			_err = self._params[-2]
			self._params[-2] = None
			raise _err
		if _result != NDXL_OK:
			print_error()
			raise ValueError(f"Simulation exited with code {get_last_error_code()}: {get_last_error_msg()}")
		return _result
```

The `_network_bridge` is a dispatcher function similar to what we saw previously with `_vfunc_bridge` and `_vcb_bridge`.
Dispatcher functions analyse the calling object and its `void* params` for the Python-specific code to call (hence the verbose `_args` created in `create_network()`), creating a common function pointer that all the C objects use and avoiding overhead for multiple functions in Python.

```python
@NDXLNetworkCallback
def _network_bridge(nwk, time, args):
	_n = network(nwk, args)
	try:
		_meta = ctypes.cast(args, ctypes.POINTER(ctypes.py_object)).contents.value
		_cb = _meta[0]
		if _cb:
			_userdata = _meta[-1]
			_cb(_n, time, *_userdata)
		return 0
	except BaseException as err:
		_n._Set_error(err)
		return -1
```

We can see here the importance of `void* params` for storing all the object's metadata, as well as the return value for indicating an external failure in the Python function.
Assuming you have created a network in Python, simulating it is now as easy as calling any of:

```python
nwk.simulate(10.)
nwk.simulate(10., cb=cb)
nwk.simulate(10., atol=1e-5)
nwk.simulate(10., atol=1e-7, rtol=1e-2)
nwk.simulate(10., method="ralston")
nwk.simulate(10., method=ndxl.methods.ralston, step=0.05)
```