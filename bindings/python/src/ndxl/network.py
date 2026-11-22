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

lib.ndxlGetNetworkEdges.restype = ctypes.c_int
lib.ndxlGetNetworkEdges.argtypes = [NDXLNetwork_p, ctypes.POINTER(NDXLEdge_p)]

lib.ndxlGetNetworkOrder.restype = ctypes.c_size_t
lib.ndxlGetNetworkOrder.argtypes = [NDXLNetwork_p]

lib.ndxlGetNetworkParams.restype = ctypes.c_void_p
lib.ndxlGetNetworkParams.argtypes = [NDXLNetwork_p]

lib.ndxlGetNetworkSize.restype = ctypes.c_size_t
lib.ndxlGetNetworkSize.argtypes = [NDXLNetwork_p]

lib.ndxlGetNetworkTime.restype = ctypes.c_double
lib.ndxlGetNetworkTime.argtypes = [NDXLNetwork_p]

lib.ndxlGetNetworkVertices.restype = ctypes.c_int
lib.ndxlGetNetworkVertices.argtypes = [NDXLNetwork_p, ctypes.POINTER(NDXLVertex_p)]

# ^^^^^ Getters | setters vvvvv

lib.ndxlSetNetworkParams.restype = ctypes.c_int
lib.ndxlSetNetworkParams.argtypes = [NDXLNetwork_p, ctypes.c_void_p]

# ^^^^^ Setters | traversal vvvvv

lib.ndxlGetNetworkFirstChannel.restype = NDXLChannel_p
lib.ndxlGetNetworkFirstChannel.argtypes = [NDXLNetwork_p]

lib.ndxlGetNetworkFirstEdge.restype = NDXLEdge_p
lib.ndxlGetNetworkFirstEdge.argtypes = [NDXLNetwork_p]

lib.ndxlGetNetworkFirstVertex.restype = NDXLVertex_p
lib.ndxlGetNetworkFirstVertex.argtypes = [NDXLNetwork_p]

lib.ndxlGetNetworkLastChannel.restype = NDXLChannel_p
lib.ndxlGetNetworkLastChannel.argtypes = [NDXLNetwork_p]

lib.ndxlGetNetworkLastEdge.restype = NDXLEdge_p
lib.ndxlGetNetworkLastEdge.argtypes = [NDXLNetwork_p]

lib.ndxlGetNetworkLastVertex.restype = NDXLVertex_p
lib.ndxlGetNetworkLastVertex.argtypes = [NDXLNetwork_p]

lib.ndxlGetNetworkNextChannel.restype = NDXLChannel_p
lib.ndxlGetNetworkNextChannel.argtypes = [NDXLChannel_p]

lib.ndxlGetNetworkNextEdge.restype = NDXLEdge_p
lib.ndxlGetNetworkNextEdge.argtypes = [NDXLEdge_p]

lib.ndxlGetNetworkNextVertex.restype = NDXLVertex_p
lib.ndxlGetNetworkNextVertex.argtypes = [NDXLVertex_p]

lib.ndxlGetNetworkPreviousChannel.restype = NDXLChannel_p
lib.ndxlGetNetworkPreviousChannel.argtypes = [NDXLChannel_p]

lib.ndxlGetNetworkPreviousEdge.restype = NDXLEdge_p
lib.ndxlGetNetworkPreviousEdge.argtypes = [NDXLEdge_p]

lib.ndxlGetNetworkPreviousVertex.restype = NDXLVertex_p
lib.ndxlGetNetworkPreviousVertex.argtypes = [NDXLVertex_p]

# ^^^^^ Traversal | simulation vvvvv

lib.ndxlNetworkSimulateRalston.restype = ctypes.c_int
lib.ndxlNetworkSimulateRalston.argtypes = [NDXLNetwork_p, ctypes.c_double, ctypes.c_double, NDXLNetworkCallback]

lib.ndxlNetworkSimulateNystrom.restype = ctypes.c_int
lib.ndxlNetworkSimulateNystrom.argtypes = [NDXLNetwork_p, ctypes.c_double, ctypes.c_double, NDXLNetworkCallback]

lib.ndxlNetworkSimulateDopri.restype = ctypes.c_int
lib.ndxlNetworkSimulateDopri.argtypes = [NDXLNetwork_p, ctypes.c_double, ctypes.c_double, ctypes.c_double, ctypes.POINTER(ctypes.c_double), ctypes.c_size_t, NDXLNetworkCallback]

lib.ndxlNetworkSimulateRkck.restype = ctypes.c_int
lib.ndxlNetworkSimulateRkck.argtypes = [NDXLNetwork_p, ctypes.c_double, ctypes.c_double, ctypes.c_double, ctypes.POINTER(ctypes.c_double), ctypes.c_size_t, NDXLNetworkCallback]

lib.ndxlNetworkSimulateEuler.restype = ctypes.c_int
lib.ndxlNetworkSimulateEuler.argtypes = [NDXLNetwork_p, ctypes.c_double, ctypes.c_double, NDXLNetworkCallback]

lib.ndxlNetworkSimulateHeun.restype = ctypes.c_int
lib.ndxlNetworkSimulateHeun.argtypes = [NDXLNetwork_p, ctypes.c_double, ctypes.c_double, ctypes.c_double, ctypes.POINTER(ctypes.c_double), ctypes.c_size_t, NDXLNetworkCallback]

lib.ndxlNetworkSimulateNorsett.restype = ctypes.c_int
lib.ndxlNetworkSimulateNorsett.argtypes = [NDXLNetwork_p, ctypes.c_double, ctypes.c_double, NDXLNetworkCallback]

lib.ndxlNetworkSimulateRadau.restype = ctypes.c_int
lib.ndxlNetworkSimulateRadau.argtypes = [NDXLNetwork_p, ctypes.c_double, ctypes.c_double, ctypes.c_double, ctypes.POINTER(ctypes.c_double), ctypes.c_size_t, NDXLNetworkCallback]

# ^^^^^ Simulation | Python wrappers vvvvv

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
	def nystrom(self):
		return simulationmethod(lib.ndxlNetworkSimulateNystrom, True, (0.1,))

	@property
	def dopri(self):
		return simulationmethod(lib.ndxlNetworkSimulateDopri, False, (1e-6, 1e-3))

	@property
	def rkck(self):
		return simulationmethod(lib.ndxlNetworkSimulateRkck, False, (1e-6, 1e-3))

	@property
	def cashkarp(self):
		return simulationmethod(lib.ndxlNetworkSimulateRkck, False, (1e-6, 1e-3))

	@property
	def euler(self):
		return simulationmethod(lib.ndxlNetworkSimulateEuler, True, (0.05,))

	@property
	def heun(self):
		return simulationmethod(lib.ndxlNetworkSimulateHeun, False, (1e-3, 1e-3))

	@property
	def radau(self):
		return simulationmethod(lib.ndxlNetworkSimulateRadau, False, (1e-8, 1e-6))

	def _Get_method_by_name(self, name):
		if re.match(r"(?i)ralston", name):
			return self.ralston
		if re.match(r"(?i)nystrom", name):
			return self.nystrom
		if re.match(r"(?i)(dopri)|(dormand(-| )?prince)", name):
			return self.dopri
		if re.match(r"(?i)(rkck)|(cash(-| )?karp)", name):
			return self.rkck
		if re.match(r"(?i)euler((-| )?maruyama)?", name):
			return self.euler
		if re.match(r"(?i)heun((-| )?euler)?", name):
			return self.heun
		if re.match(r"(?i)radau((ii)|2a)?", name):
			return self.radau
		return None

class methods(metaclass=methodnames):
	pass

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
	def channels(self):
		from .channel import channel
		_cnl = lib.ndxlGetNetworkFirstChannel(self._ptr)
		while _cnl:
			yield channel(_cnl, lib.ndxlGetChannelParams(_cnl))
			_cnl = lib.ndxlGetNetworkNextChannel(_cnl)

	@property
	def channel_list(self):
		return list(self.channels)

	@property
	def edges(self):
		from .edge import edge
		_edge = lib.ndxlGetNetworkFirstEdge(self._ptr)
		while _edge:
			yield edge(_edge, lib.ndxlGetEdgeParams(_edge))
			_edge = lib.ndxlGetNetworkNextEdge(_edge)

	@property
	def edge_list(self):
		return list(self.edges)

	@property
	def first_channel(self):
		from .channel import channel
		_cnl = lib.ndxlGetNetworkFirstChannel(self._ptr)
		if _cnl:
			return channel(_cnl, lib.ndxlGetChannelParams(_cnl))
		return None

	@property
	def first_edge(self):
		from .edge import edge
		_edge = lib.ndxlGetNetworkFirstEdge(self._ptr)
		if _edge:
			return edge(_edge, lib.ndxlGetEdgeParams(_edge))
		return None

	@property
	def first_vertex(self):
		from .vertex import vertex
		_vert = lib.ndxlGetNetworkFirstVertex(self._ptr)
		if _vert:
			return vertex(_vert, lib.ndxlGetVertexParams(_vert))
		return None

	@property
	def last_channel(self):
		from .channel import channel
		_cnl = lib.ndxlGetNetworkLastChannel(self._ptr)
		if _cnl:
			return channel(_cnl, lib.ndxlGetChannelParams(_cnl))
		return None

	@property
	def last_edge(self):
		from .edge import edge
		_edge = lib.ndxlGetNetworkLastEdge(self._ptr)
		if _edge:
			return edge(_edge, lib.ndxlGetEdgeParams(_edge))
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
	def size(self):
		return lib.ndxlGetNetworkSize(self._ptr)

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

	def create_channel(self, args=None):
		from .channel import channel
		_args = () if args is None else ((args,) if np.isscalar(args) else copy.deepcopy(args))
		_pptr = ctypes.cast(ctypes.pointer(ctypes.py_object([_args])), ctypes.c_void_p)
		_channel = channel(lib.ndxlCreateChannel(self._ptr, _pptr), _pptr)
		self._cdict[_channel._Int_ptr] = _channel
		return _channel

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

	def create_edge(self, v1, v2, state=None, delay=0., directed=False, dynamics=None, noise=None, channels=None, cb=None, args=None, cplx=False):
		from .edge import edge, _efunc_bridge, _enoise_bridge, _ecb_bridge, NoneEFn, NoneENz, NoneECb
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
		_edge = edge(lib.ndxlCreateEdge(v1 if isinstance(v1, NDXLVertex_p) else v1.c_ptr, v2 if isinstance(v2, NDXLVertex_p) else v2.c_ptr, _state, _nstate, delay, 1 if directed else 0, NoneEFn if dynamics is None else _efunc_bridge, NoneENz if noise is None else _enoise_bridge, _cn, _nchannels, NoneECb if cb is None else _ecb_bridge, _pptr), _pptr)
		self._edict[_edge._Int_ptr] = _edge
		return _edge

def create_network(args=None):
	_args = () if args is None else ((args,) if np.isscalar(args) else copy.deepcopy(args))
	_pptr = ctypes.cast(ctypes.pointer(ctypes.py_object([None, dict(), dict(), dict(), None, _args])), ctypes.c_void_p)
	return network(lib.ndxlCreateNetwork(_pptr), _pptr)

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

# ^^^^^ Python wrappers