from __future__ import annotations

from .core import *

# Callbacks vvvvv

NDXLEdgeDynamics = ctypes.CFUNCTYPE(ctypes.c_int, NDXLEdge_p, ctypes.POINTER(ctypes.c_double), ctypes.POINTER(ctypes.c_double), ctypes.c_double, ctypes.c_void_p)
NDXLEdgeNoise = ctypes.CFUNCTYPE(ctypes.c_int, NDXLEdge_p, ctypes.POINTER(ctypes.c_double), ctypes.POINTER(ctypes.c_double), ctypes.c_double, ctypes.c_void_p)
NDXLEdgeCallback = ctypes.CFUNCTYPE(ctypes.c_int, NDXLEdge_p, ctypes.POINTER(ctypes.c_double), ctypes.c_double, ctypes.c_void_p)
NoneEFn = NDXLEdgeDynamics(0)
NoneENz = NDXLEdgeNoise(0)
NoneECb = NDXLEdgeCallback(0)

# ^^^^^ Callbacks | constructor and destructor vvvvv

lib.ndxlCreateEdge.restype = NDXLEdge_p
lib.ndxlCreateEdge.argtypes = [NDXLVertex_p, NDXLVertex_p, ctypes.POINTER(ctypes.c_double), ctypes.c_size_t, ctypes.c_double, ctypes.c_char, NDXLEdgeDynamics, NDXLEdgeNoise, ctypes.POINTER(NDXLChannel_p), ctypes.c_size_t, NDXLEdgeCallback, ctypes.c_void_p]

lib.ndxlDestroyEdge.restype = ctypes.c_int
lib.ndxlDestroyEdge.argtypes = [ctypes.POINTER(NDXLEdge_p)]

# ^^^^^ Constructor and destructor | getters vvvvv

lib.ndxlGetEdgeCallback.restype = NDXLEdgeCallback
lib.ndxlGetEdgeCallback.argtypes = [NDXLEdge_p]

lib.ndxlGetEdgeChannelCount.restype = ctypes.c_size_t
lib.ndxlGetEdgeChannelCount.argtypes = [NDXLEdge_p]

lib.ndxlGetEdgeChannels.restype = ctypes.c_int
lib.ndxlGetEdgeChannels.argtypes = [NDXLEdge_p, ctypes.POINTER(NDXLChannel_p)]

lib.ndxlGetEdgeDelay.restype = ctypes.c_double
lib.ndxlGetEdgeDelay.argtypes = [NDXLEdge_p]

lib.ndxlGetEdgeDirected.restype = ctypes.c_char
lib.ndxlGetEdgeDirected.argtypes = [NDXLEdge_p]

lib.ndxlGetEdgeFirstVertex.restype = NDXLVertex_p
lib.ndxlGetEdgeFirstVertex.argtypes = [NDXLEdge_p]

lib.ndxlGetEdgeDynamics.restype = NDXLEdgeDynamics
lib.ndxlGetEdgeDynamics.argtypes = [NDXLEdge_p]

lib.ndxlGetEdgeInitialState.restype = ctypes.POINTER(ctypes.c_double)
lib.ndxlGetEdgeInitialState.argtypes = [NDXLEdge_p]

lib.ndxlGetEdgeNetwork.restype = NDXLNetwork_p
lib.ndxlGetEdgeNetwork.argtypes = [NDXLEdge_p]

lib.ndxlGetEdgeNoise.restype = NDXLEdgeNoise
lib.ndxlGetEdgeNoise.argtypes = [NDXLEdge_p]

lib.ndxlGetEdgeOrder.restype = ctypes.c_size_t
lib.ndxlGetEdgeOrder.argtypes = [NDXLEdge_p]

lib.ndxlGetEdgeParams.restype = ctypes.c_void_p
lib.ndxlGetEdgeParams.argtypes = [NDXLEdge_p]

lib.ndxlGetEdgeRelativeState.restype = ctypes.POINTER(ctypes.c_double)
lib.ndxlGetEdgeRelativeState.argtypes = [NDXLEdge_p]

lib.ndxlGetEdgeSecondVertex.restype = NDXLVertex_p
lib.ndxlGetEdgeSecondVertex.argtypes = [NDXLEdge_p]

lib.ndxlGetEdgeTime.restype = ctypes.c_double
lib.ndxlGetEdgeTime.argtypes = [NDXLEdge_p]

lib.ndxlGetNetworkNextEdge.restype = NDXLEdge_p
lib.ndxlGetNetworkNextEdge.argtypes = [NDXLEdge_p]

lib.ndxlGetNetworkPreviousEdge.restype = NDXLEdge_p
lib.ndxlGetNetworkPreviousEdge.argtypes = [NDXLEdge_p]

# ^^^^^ Getters | setters vvvvv

lib.ndxlSetEdgeCallback.restype = ctypes.c_int
lib.ndxlSetEdgeCallback.argtypes = [NDXLEdge_p, NDXLEdgeCallback]

lib.ndxlSetEdgeDynamics.restype = ctypes.c_int
lib.ndxlSetEdgeDynamics.argtypes = [NDXLEdge_p, NDXLEdgeDynamics]

lib.ndxlSetEdgeNoise.restype = ctypes.c_int
lib.ndxlSetEdgeNoise.argtypes = [NDXLEdge_p, NDXLEdgeNoise]

# ^^^^^ Setters | functions vvvvv

lib.ndxlEdgeConnectsVertex.restype = ctypes.c_char
lib.ndxlEdgeConnectsVertex.argtypes = [NDXLEdge_p, NDXLVertex_p]

lib.ndxlEdgeState.restype = ctypes.POINTER(ctypes.c_double)
lib.ndxlEdgeState.argtypes = [NDXLEdge_p]

# ^^^^^ Functions | Python wrappers vvvvv

class edge:
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
		
	def __eq__(self, ed):
		if isinstance(ed, edge):
			return self._Int_ptr == ed._Int_ptr
		return False

	def __ne__(self, ed):
		if isinstance(ed, edge):
			return self._Int_ptr != ed._Int_ptr
		return False

	def __hash__(self):
		return hash(self._Int_ptr)

	def __len__(self):
		_ord = lib.ndxlGetEdgeOrder(self._ptr)
		return (_ord // 2) if self._cplx else _ord

	def __str__(self):
		return "'<ndxl.edge object at " + f"{self._Int_ptr:#0{int(platform.architecture()[0][:-3]) // 4 + 2}X}" + " {'network': " + f"<ndxl.network object at {self.network._Int_ptr:#0{int(platform.architecture()[0][:-3]) // 4 + 2}X}>" + ", 'vertex 1': " + str(self.first_vertex) + ", 'vertex 2': " + str(self.second_vertex) + ", 'state': " + str(self.state) + ", 'initial-state': " + str(self.initial_state) + ", 'time': " + str(lib.ndxlGetEdgeTime(self._ptr)) + ", 'function': " + str(self._params[0]) + ", 'noise': " + str(self._params[1]) + ", 'callback': " + str(self._params[2]) + "}>'"

	def __repr__(self):
		return f"NDXL edge\n===========\n\nNetwork: <ndxl.network object at {self.network._Int_ptr:#0{int(platform.architecture()[0][:-3]) // 4 + 2}X}>\nVertex 1: {self.first_vertex}\nVertex 2: {self.second_vertex}\nState: {self.state}\nInitial state: {self.initial_state}\nFunction: {self._params[0]}\nNoise: {self._params[1]}\nCallback: {self._params[2]}\nTime: {lib.ndxlGetEdgeTime(self._ptr)}\n"

	@property
	def _Int_ptr(self):
		return ctypes.cast(self._ptr, ctypes.c_void_p).value

	@property
	def c_ptr(self):
		return self._ptr

	def destroy(self):
		self.network._Deregister_edge(self._Int_ptr)
		_res = lib.ndxlDestroyEdge(ctypes.pointer(self._ptr))
		del self._pptr
		del self._params
		return _res

	@property
	def callback(self):
		return self._params[2]

	@callback.setter
	def callback(self, cb):
		self._params[2] = cb
		return lib.ndxlSetEdgeCallback(self._ptr, NoneECb if cb is None else _ecb_bridge)

	@property
	def channel_count(self):
		return lib.ndxlGetEdgeChannelCount(self._ptr)

	@property
	def channels(self):
		from. channel import channel
		_nchannels = lib.ndxlGetEdgeChannelCount(self._ptr)
		_carr = (NDXLChannel_p * _nchannels)()
		if lib.ndxlGetEdgeChannels(self._ptr, _carr) != NDXL_OK:
			raise ValueError(ndxl.get_last_error_msg())
		for i in range(_nchannels):
			yield channel(_carr[i], lib.ndxlGetChannelParams(_carr[i]))

	@property
	def channel_list(self):
		from. channel import channel
		_nchannels = lib.ndxlGetEdgeChannelCount(self._ptr)
		_carr = (NDXLChannel_p * _nchannels)()
		if lib.ndxlGetEdgeChannels(self._ptr, _carr) != NDXL_OK:
			raise ValueError(ndxl.get_last_error_msg())
		return list(channel(_carr[i], lib.ndxlGetChannelParams(_carr[i])) for i in range(_nchannels))

	@property
	def cplx(self):
		return self._cplx

	@property
	def delay(self):
		return lib.ndxlGetEdgeDelay(self._ptr)

	@property
	def directed(self):
		return True if lib.ndxlGetEdgeDirected(self._ptr) == 1 else False

	@property
	def first_vertex(self):
		from .vertex import vertex
		_vert = lib.ndxlGetEdgeFirstVertex(self._ptr)
		return vertex(_vert, lib.ndxlGetVertexParams(_vert))

	@property
	def func(self):
		return self._params[0]

	@func.setter
	def func(self, fn):
		self._params[0] = fn
		return lib.ndxlSetEdgeFunction(self._ptr, NoneEFn if fn is None else _efunc_bridge)

	@property
	def initial_state(self):
		_ord = self.order
		_state = np.ctypeslib.as_array(lib.ndxlGetEdgeInitialState(self._ptr), shape=(_ord,)).copy()
		if self._cplx:
			return _state.view(complex).reshape(self._params[-2])
		return _state.reshape(self._params[-2])

	@property
	def network(self):
		from .network import network as nwk
		_nwk = lib.ndxlGetEdgeNetwork(self._ptr)
		return nwk(_nwk, lib.ndxlGetNetworkParams(_nwk))

	@property
	def next(self):
		_edge = lib.ndxlGetNetworkNextEdge(self._ptr)
		if _edge:
			return edge(_edge, lib.ndxlGetEdgeParams(_edge))
		return None

	@property
	def noise(self):
		return self._params[1]

	@noise.setter
	def noise(self, zeta):
		self._params[1] = zeta
		return lib.ndxlSetEdgeNoise(self._ptr, NoneEFn if zeta is None else _enoise_bridge)

	@property
	def order(self):
		return lib.ndxlGetEdgeOrder(self._ptr)
	
	@property
	def params(self):
		return self._params[-1]

	@property
	def relative_state(self):
		_ord = self.order
		_state = np.ctypeslib.as_array(lib.ndxlGetEdgeRelativeState(self._ptr), shape=(_ord,)).copy()
		if self._cplx:
			return _state.view(complex).reshape(self._params[-2])
		return _state.reshape(self._params[-2])

	@property
	def previous(self):
		_edge = lib.ndxlGetNetworkPreviousEdge(self._ptr)
		if _edge:
			return edge(_edge, lib.ndxlGetEdgeParams(_edge))
		return None

	@property
	def second_vertex(self):
		from .vertex import vertex
		_vert = lib.ndxlGetEdgeSecondVertex(self._ptr)
		return vertex(_vert, lib.ndxlGetVertexParams(_vert))

	@property
	def shape(self):
		return self._params[-2]

	@property
	def state(self):
		_ord = self.order
		_state = np.ctypeslib.as_array(lib.ndxlEdgeState(self._ptr), shape=(_ord,))
		if self._cplx:
			return _state.view(complex).reshape(self._params[-2])
		return _state.reshape(self._params[-2])

	@property
	def time(self):
		return lib.ndxlGetEdgeTime(self._ptr)

	@property
	def vertex1(self):
		return self.first_vertex

	@property
	def vertex2(self):
		return self.second_vertex

	@property
	def vertices(self):
		return [self.vertex1, self.vertex2]

	def connects_vertex(self, vert):
		return lib.ndxlEdgeConnectsVertex(self._ptr, vert.c_ptr)

@NDXLEdgeDynamics
def _efunc_bridge(ed, dstate, state, time, args):
	_e = edge(ed, args)
	try:
		_meta = ctypes.cast(args, ctypes.POINTER(ctypes.py_object)).contents.value
		_func = _meta[0]
		if _func:
			_userdata = _meta[-1]
			_ord = _e.order
			_m = _ord * _e.channel_count
			_inview = np.ctypeslib.as_array(state, shape=(_ord,)).copy()
			_outview = np.ctypeslib.as_array(dstate, shape=(_ord,))
			if _e.cplx:
				_len = len(_e)
				_feval = _func(_e, _inview.view(complex).reshape(_e.shape), time, *_userdata)
				_result = np.full(_len, _feval, dtype=complex) if np.isscalar(_feval) else np.asarray(_feval, dtype=complex).flatten()
				_outview[:] = _result.view(float)
			else:
				_feval = _func(_e, _inview.reshape(_e.shape), time, *_userdata)
				_outview[:] = np.repeat(_feval, _ord) if np.isscalar(_feval) else np.asarray(_feval).flatten()
		return 0
	except BaseException as err:
		_e.network._Set_error(err)
		return -1

@NDXLEdgeNoise
def _enoise_bridge(ed, dstate, state, time, args):
	_e = edge(ed, args)
	try:
		_meta = ctypes.cast(args, ctypes.POINTER(ctypes.py_object)).contents.value
		_noise = _meta[1]
		if _noise:
			_userdata = _meta[-1]
			_ord = _e.order
			_m = _e.channel_count
			_inview = np.ctypeslib.as_array(state, shape=(_ord,)).copy()
			_outview = np.ctypeslib.as_array(dstate, shape=(_ord * _m,))
			if _e.cplx:
				_len = len(_e)
				_feval = _noise(_e, _inview.view(complex).reshape(_e.shape), time, *_userdata)
				_result = np.full(_len * _m, _feval, dtype=complex) if np.isscalar(_feval) else np.asarray(_feval, dtype=complex).flatten()
				_outview[:] = _result.view(float)
			else:
				_feval = _noise(_e, _inview.reshape(_e.shape), time, *_userdata)
				_outview[:] = np.repeat(_feval, _ord * _m) if np.isscalar(_feval) else np.asarray(_feval).flatten()
		return 0
	except BaseException as err:
		_e.network._Set_error(err)
		return -1

@NDXLEdgeCallback
def _ecb_bridge(ed, state, time, args):
	_e = edge(ed, args)
	try:
		_meta = ctypes.cast(args, ctypes.POINTER(ctypes.py_object)).contents.value
		_cb = _meta[2]
		if _cb:
			_userdata = _meta[-1]
			_ord = _e.order
			_view = np.ctypeslib.as_array(state, shape=(_ord,))
			if _e.cplx:
				_len = len(_e)
				_tview = _view.view(complex).reshape(_e.shape)
				_cb(_e, _tview, time, *_userdata)
			else:
				_cb(_e, _view.reshape(_e.shape), time, *_userdata)
		return 0
	except BaseException as err:
		_e.network._Set_error(err)
		return -1

# ^^^^^ Python wrappers