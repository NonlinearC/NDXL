from __future__ import annotations

from .core import *

# Callbacks vvvvv

NDXLVertexDynamics = ctypes.CFUNCTYPE(ctypes.c_int, NDXLVertex_p, ctypes.POINTER(ctypes.c_double), ctypes.POINTER(ctypes.c_double), ctypes.c_double, ctypes.c_void_p)
NDXLVertexNoise = ctypes.CFUNCTYPE(ctypes.c_int, NDXLVertex_p, ctypes.POINTER(ctypes.c_double), ctypes.POINTER(ctypes.c_double), ctypes.c_double, ctypes.c_void_p)
NDXLVertexCallback = ctypes.CFUNCTYPE(ctypes.c_int, NDXLVertex_p, ctypes.POINTER(ctypes.c_double), ctypes.c_double, ctypes.c_void_p)
NoneVFn = NDXLVertexDynamics(0)
NoneVNz = NDXLVertexNoise(0)
NoneVCb = NDXLVertexCallback(0)

# ^^^^^ Callbacks | constructor and destructor vvvvv

lib.ndxlCreateVertex.restype = NDXLVertex_p
lib.ndxlCreateVertex.argtypes = [NDXLNetwork_p, ctypes.POINTER(ctypes.c_double), ctypes.c_size_t, NDXLVertexDynamics, NDXLVertexNoise, ctypes.POINTER(NDXLChannel_p), ctypes.c_size_t, NDXLVertexCallback, ctypes.c_void_p]

lib.ndxlDestroyVertex.restype = ctypes.c_int
lib.ndxlDestroyVertex.argtypes = [ctypes.POINTER(NDXLVertex_p)]

# ^^^^^ Constructor and destructor | getters vvvvv

lib.ndxlGetNetworkNextVertex.restype = NDXLVertex_p
lib.ndxlGetNetworkNextVertex.argtypes = [NDXLVertex_p]

lib.ndxlGetNetworkPreviousVertex.restype = NDXLVertex_p
lib.ndxlGetNetworkPreviousVertex.argtypes = [NDXLVertex_p]

lib.ndxlGetVertexCallback.restype = NDXLVertexCallback
lib.ndxlGetVertexCallback.argtypes = [NDXLVertex_p]

lib.ndxlGetVertexChannelCount.restype = ctypes.c_size_t
lib.ndxlGetVertexChannelCount.argtypes = [NDXLVertex_p]

lib.ndxlGetVertexChannels.restype = ctypes.c_int
lib.ndxlGetVertexChannels.argtypes = [NDXLVertex_p, ctypes.POINTER(NDXLChannel_p)]

lib.ndxlGetVertexDegree.restype = ctypes.c_size_t
lib.ndxlGetVertexDegree.argtypes = [NDXLVertex_p]

lib.ndxlGetVertexDegreeIn.restype = ctypes.c_size_t
lib.ndxlGetVertexDegreeIn.argtypes = [NDXLVertex_p]

lib.ndxlGetVertexDegreeOut.restype = ctypes.c_size_t
lib.ndxlGetVertexDegreeOut.argtypes = [NDXLVertex_p]

lib.ndxlGetVertexEdges.restype = ctypes.c_int
lib.ndxlGetVertexEdges.argtypes = [NDXLVertex_p, ctypes.POINTER(NDXLEdge_p)]

lib.ndxlGetVertexFirstEdge.restype = NDXLEdge_p
lib.ndxlGetVertexFirstEdge.argtypes = [NDXLVertex_p]

lib.ndxlGetVertexDynamics.restype = NDXLVertexDynamics
lib.ndxlGetVertexDynamics.argtypes = [NDXLVertex_p]

lib.ndxlGetVertexInitialState.restype = ctypes.POINTER(ctypes.c_double)
lib.ndxlGetVertexInitialState.argtypes = [NDXLVertex_p]

lib.ndxlGetVertexLastEdge.restype = NDXLEdge_p
lib.ndxlGetVertexLastEdge.argtypes = [NDXLVertex_p]

lib.ndxlGetVertexNeighbour.restype = NDXLVertex_p
lib.ndxlGetVertexNeighbour.argtypes = [NDXLVertex_p, NDXLEdge_p]

lib.ndxlGetVertexNeighbours.restype = ctypes.c_int
lib.ndxlGetVertexNeighbours.argtypes = [NDXLVertex_p, ctypes.POINTER(NDXLVertex_p)]

lib.ndxlGetVertexNeighbourState.restype = ctypes.POINTER(ctypes.c_double)
lib.ndxlGetVertexNeighbourState.argtypes = [NDXLVertex_p, NDXLEdge_p]

lib.ndxlGetVertexNetwork.restype = NDXLNetwork_p
lib.ndxlGetVertexNetwork.argtypes = [NDXLVertex_p]

lib.ndxlGetVertexNextEdge.restype = NDXLEdge_p
lib.ndxlGetVertexNextEdge.argtypes = [NDXLVertex_p, NDXLEdge_p]

lib.ndxlGetVertexNoise.restype = NDXLVertexNoise
lib.ndxlGetVertexNoise.argtypes = [NDXLVertex_p]

lib.ndxlGetVertexOrder.restype = ctypes.c_size_t
lib.ndxlGetVertexOrder.argtypes = [NDXLVertex_p]

lib.ndxlGetVertexParams.restype = ctypes.c_void_p
lib.ndxlGetVertexParams.argtypes = [NDXLVertex_p]

lib.ndxlGetVertexPreviousEdge.restype = NDXLEdge_p
lib.ndxlGetVertexPreviousEdge.argtypes = [NDXLVertex_p, NDXLEdge_p]

lib.ndxlGetVertexTime.restype = ctypes.c_double
lib.ndxlGetVertexTime.argtypes = [NDXLVertex_p]

# ^^^^^ Getters | setters vvvvv

lib.ndxlSetVertexCallback.restype = ctypes.c_int
lib.ndxlSetVertexCallback.argtypes = [NDXLVertex_p, NDXLVertexCallback]

lib.ndxlSetVertexDynamics.restype = ctypes.c_int
lib.ndxlSetVertexDynamics.argtypes = [NDXLVertex_p, NDXLVertexDynamics]

lib.ndxlSetVertexNoise.restype = ctypes.c_int
lib.ndxlSetVertexNoise.argtypes = [NDXLVertex_p, NDXLVertexNoise]

# ^^^^^ Setters | functions vvvvv

lib.ndxlVertexHasNeighbour.restype = ctypes.c_char
lib.ndxlVertexHasNeighbour.argtypes = [NDXLVertex_p, NDXLVertex_p]

lib.ndxlVertexState.restype = ctypes.POINTER(ctypes.c_double)
lib.ndxlVertexState.argtypes = [NDXLVertex_p]

# ^^^^^ Functions | Python wrappers vvvvv

class vertex:
	def __init__(self, ptr, args):
		self._ptr = ptr
		self._pptr = args
		self._params = ctypes.cast(self._pptr, ctypes.POINTER(ctypes.py_object)).contents.value
		self._cplx = self._params[-3]

	def __del__(self):
		self._ptr = None
		self._pptr = None
		self._params = None

	def __bool__(self):
		return bool(self._ptr)
	
	def __eq__(self, vert):
		if isinstance(vert, vertex):
			return self._Int_ptr == vert._Int_ptr
		return False

	def __ne__(self, vert):
		if isinstance(vert, vertex):
			return self._Int_ptr != vert._Int_ptr
		return False

	def __hash__(self):
		return hash(self._Int_ptr)

	def __len__(self):
		_ord = lib.ndxlGetVertexOrder(self._ptr)
		return (_ord // 2) if self._cplx else _ord

	def __str__(self):
		return f"'<ndxl.vertex object at " + f"{self._Int_ptr:#0{int(platform.architecture()[0][:-3]) // 4 + 2}X}" + " {'network': " + f"<ndxl.network object at {self.network._Int_ptr:#0{int(platform.architecture()[0][:-3]) // 4 + 2}X}>" + ", 'degree': " + str(lib.ndxlGetVertexDegree(self._ptr)) + ", 'in-degree': " + str(lib.ndxlGetVertexDegreeIn(self._ptr)) + ", 'out-degree': " + str(lib.ndxlGetVertexDegreeOut(self._ptr)) + ", 'state': " + str(self.state) + ", 'initial-state': " + str(self.initial_state) + ", 'time': " + str(lib.ndxlGetVertexTime(self._ptr)) + ", 'function': " + str(self._params[0]) + ", 'noise': " + str(self._params[1]) + ", 'callback': " + str(self._params[2]) + "}>'"

	def __repr__(self):
		return f"NDXL vertex\n===========\n\nNetwork: <ndxl.network object at {self.network._Int_ptr:#0{int(platform.architecture()[0][:-3]) // 4 + 2}X}>\nState: {self.state}\nInitial state: {self.initial_state}\nDegree: {lib.ndxlGetVertexDegree(self._ptr)}\nIn-degree: {lib.ndxlGetVertexDegreeIn(self._ptr)}\nOut-degree: {lib.ndxlGetVertexDegreeOut(self._ptr)}\nFunction: {self._params[0]}\nNoise: {self._params[1]}\nCallback: {self._params[2]}\nTime: {lib.ndxlGetVertexTime(self._ptr)}\n"

	@property
	def _Int_ptr(self):
		return ctypes.cast(self._ptr, ctypes.c_void_p).value

	@property
	def c_ptr(self):
		return self._ptr

	def destroy(self):
		for e in list(self.edges):
			e.destroy()
		self.network._Deregister_vertex(self._Int_ptr)
		_res = lib.ndxlDestroyVertex(ctypes.pointer(self._ptr))
		del self._pptr
		del self._params
		return _res

	@property
	def callback(self):
		return self._params[2]

	@callback.setter
	def callback(self, cb):
		self._params[2] = cb
		return lib.ndxlSetVertexCallback(self._ptr, NoneVCb if cb is None else _vcb_bridge)

	@property
	def channel_count(self):
		return lib.ndxlGetVertexChannelCount(self._ptr)

	@property
	def channels(self):
		from .channel import channel
		_nchannels = lib.ndxlGetVertexChannelCount(self._ptr)
		_carr = (NDXLChannel_p * _nchannels)()
		if lib.ndxlGetVertexChannels(self._ptr, _carr) != NDXL_OK:
			raise ValueError(ndxl.get_last_error_msg())
		for i in range(_nchannels):
			yield channel(_carr[i], lib.ndxlGetChannelParams(_carr[i]))

	@property
	def channel_list(self):
		from .channel import channel
		_nchannels = lib.ndxlGetVertexChannelCount(self._ptr)
		_carr = (NDXLChannel_p * _nchannels)()
		if lib.ndxlGetVertexChannels(self._ptr, _carr) != NDXL_OK:
			raise ValueError(ndxl.get_last_error_msg())
		return list(channel(_carr[i], lib.ndxlGetChannelParams(_carr[i])) for i in range(_nchannels))

	@property
	def cplx(self):
		return self._cplx

	@property
	def degree(self):
		return lib.ndxlGetVertexDegree(self._ptr)

	@property
	def degree_in(self):
		return lib.ndxlGetVertexDegreeIn(self._ptr)

	@property
	def degree_out(self):
		return lib.ndxlGetVertexDegreeOut(self._ptr)

	@property
	def edges(self):
		from .edge import edge
		_edge = lib.ndxlGetVertexFirstEdge(self._ptr)
		while _edge:
			yield edge(_edge, lib.ndxlGetEdgeParams(_edge))
			_edge = lib.ndxlGetVertexNextEdge(self._ptr, _edge)

	@property
	def edge_list(self):
		return list(self.edges)

	@property
	def first_edge(self):
		from .edge import edge
		_edge = lib.ndxlGetVertexFirstEdge(self._ptr)
		if _edge:
			return edge(_edge, lib.ndxlGetEdgeParams(_edge))
		return None

	@property
	def func(self):
		return self._params[0]

	@func.setter
	def func(self, fn):
		self._params[0] = fn
		return lib.ndxlSetVertexFunction(self._ptr, NoneVFn if fn is None else _vfunc_bridge)

	@property
	def initial_state(self):
		_ord = self.order
		_state = np.ctypeslib.as_array(lib.ndxlGetVertexInitialState(self._ptr), shape=(_ord,)).copy()
		if self._cplx:
			return _state.view(complex).reshape(self._params[-2])
		return _state.reshape(self._params[-2])

	@property
	def last_edge(self):
		from .edge import edge
		_edge = lib.ndxlGetVertexLastEdge(self._ptr)
		if _edge:
			return edge(_edge, lib.ndxlGetEdgeParams(_edge))
		return None

	@property
	def neighbours(self):
		for _edge in self.edges:
			yield self.neighbour(_edge)

	@property
	def neighbour_list(self):
		return list(self.neighbours)

	@property
	def network(self):
		from .network import network as nwk
		_nwk = lib.ndxlGetVertexNetwork(self._ptr)
		return nwk(_nwk, lib.ndxlGetNetworkParams(_nwk))

	@property
	def next(self):
		_vert = lib.ndxlGetNetworkNextVertex(self._ptr)
		if _vert:
			return vertex(_vert, lib.ndxlGetVertexParams(_vert))
		return None

	@property
	def noise(self):
		return self._params[1]

	@noise.setter
	def noise(self, zeta):
		self._params[1] = zeta
		return lib.ndxlSetVertexNoise(self._ptr, NoneVFn if zeta is None else _vnoise_bridge)

	@property
	def order(self):
		return lib.ndxlGetVertexOrder(self._ptr)
	
	@property
	def params(self):
		return self._params[-1]

	@property
	def previous(self):
		_vert = lib.ndxlGetNetworkPreviousVertex(self._ptr)
		if _vert:
			return vertex(_vert, lib.ndxlGetVertexParams(_vert))
		return None

	@property
	def shape(self):
		return self._params[-2]

	@property
	def state(self):
		_ord = self.order
		_state = np.ctypeslib.as_array(lib.ndxlVertexState(self._ptr), shape=(_ord,))
		if self._cplx:
			return _state.view(complex).reshape(self._params[-2])
		return _state.reshape(self._params[-2])

	@property
	def time(self):
		return lib.ndxlGetVertexTime(self._ptr)

	def create_edge(self, vert, state=None, delay=0., directed=False, func=None, noise=None, channels=None, cb=None, args=None, cplx=False):
		return self.network.create_edge(self, vert, state, delay, directed, func, noise, channels, cb, args, cplx)

	def has_neighbour(self, vert):
		return lib.ndxlVertexHasNeighbour(self._ptr, vert.c_ptr)

	def neighbour(self, ed):
		_vert = lib.ndxlGetVertexNeighbour(self._ptr, ed.c_ptr)
		if _vert:
			return vertex(_vert, lib.ndxlGetVertexParams(_vert))
		return None

	def neighbour_state(self, ed):
		_vert = self.neighbour(ed)
		if _vert:
			_ord = _vert.order
			return np.ctypeslib.as_array(lib.ndxlGetVertexNeighbourState(self._ptr, ed.c_ptr), shape=(_ord,)).view(complex if _vert.cplx else float)
		return None

@NDXLVertexDynamics
def _vfunc_bridge(vert, dstate, state, time, args):
	_v = vertex(vert, args)
	try:
		_meta = ctypes.cast(args, ctypes.POINTER(ctypes.py_object)).contents.value
		_func = _meta[0]
		if _func:
			_userdata = _meta[-1]
			_ord = _v.order
			_inview = np.ctypeslib.as_array(state, shape=(_ord,)).copy()
			_outview = np.ctypeslib.as_array(dstate, shape=(_ord,))
			if _v.cplx:
				_len = len(_v)
				_feval = _func(_v, _inview.view(complex).reshape(_v.shape), time, *_userdata)
				_result = np.full(_len, _feval, dtype=complex) if np.isscalar(_feval) else np.asarray(_feval, dtype=complex).flatten()
				_outview[:] = _result.view(float)
			else:
				_feval = _func(_v, _inview.reshape(_v.shape), time, *_userdata)
				_outview[:] = np.repeat(_feval, _ord) if np.isscalar(_feval) else np.asarray(_feval).flatten()
		return 0
	except BaseException as err:
		_v.network._Set_error(err)
		return -1

@NDXLVertexNoise
def _vnoise_bridge(vert, dstate, state, time, args):
	_v = vertex(vert, args)
	try:
		_meta = ctypes.cast(args, ctypes.POINTER(ctypes.py_object)).contents.value
		_noise = _meta[1]
		if _noise:
			_userdata = _meta[-1]
			_ord = _v.order
			_m = _v.channel_count
			_inview = np.ctypeslib.as_array(state, shape=(_ord,)).copy()
			_outview = np.ctypeslib.as_array(dstate, shape=(_ord * _m,))
			if _v.cplx:
				_len = len(_v)
				_feval = _noise(_v, _inview.view(complex).reshape(_v.shape), time, *_userdata)
				_result = np.full(_feval, _len * _m) if np.isscalar(_feval) else np.asarray(_feval, dtype=complex).flatten()
				_outview[:] = _result.view(float)
			else:
				_feval = _noise(_v, _inview.reshape(_v.shape), time, *_userdata)
				_outview[:] = np.repeat(_feval, _ord * _m) if np.isscalar(_feval) else np.asarray(_feval).flatten()
		return 0
	except BaseException as err:
		_v.network._Set_error(err)
		return -1

@NDXLVertexCallback
def _vcb_bridge(vert, state, time, args):
	_v = vertex(vert, args)
	try:
		_meta = ctypes.cast(args, ctypes.POINTER(ctypes.py_object)).contents.value
		_cb = _meta[2]
		if _cb:
			_userdata = _meta[-1]
			_ord = _v.order
			_view = np.ctypeslib.as_array(state, shape=(_ord,))
			if _v.cplx:
				_len = len(_v)
				_tview = _view.view(complex).reshape(_v.shape)
				_cb(_v, _tview, time, *_userdata)
			else:
				_cb(_v, _view.reshape(_v.shape), time, *_userdata)
		return 0
	except BaseException as err:
		_v.network._Set_error(err)
		return -1

# ^^^^^ Python wrappers