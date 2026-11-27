from __future__ import annotations

from .core import *

# Callbacks vvvvv

NDXLNetworkCallback = ctypes.CFUNCTYPE(ctypes.c_int, NDXLNetwork_p, ctypes.c_double, ctypes.c_void_p)
NoneNCb = NDXLNetworkCallback(0)

# ^^^^^ Callbacks | constructor and destructor vvvvv

lib.ndxlCreateNetwork.restype = NDXLNetwork_p
lib.ndxlCreateNetwork.argtypes = [ctypes.c_void_p]
ndxlCreateNetwork = lib.ndxlCreateNetwork

lib.ndxlDestroyNetwork.restype = ctypes.c_int
lib.ndxlDestroyNetwork.argtypes = [ctypes.POINTER(NDXLNetwork_p)]
ndxlDestroyNetwork = lib.ndxlDestroyNetwork

# ^^^^^ Constructor and destructor | getters vvvvv

lib.ndxlGetNetworkNoiseDimension.restype = ctypes.c_size_t
lib.ndxlGetNetworkNoiseDimension.argtypes = [NDXLNetwork_p]
ndxlGetNetworkNoiseDimension = lib.ndxlGetNetworkNoiseDimension

lib.ndxlGetNetworkOrder.restype = ctypes.c_size_t
lib.ndxlGetNetworkOrder.argtypes = [NDXLNetwork_p]
ndxlGetNetworkOrder = lib.ndxlGetNetworkOrder

lib.ndxlGetNetworkParams.restype = ctypes.c_void_p
lib.ndxlGetNetworkParams.argtypes = [NDXLNetwork_p]
ndxlGetNetworkParams = lib.ndxlGetNetworkParams

lib.ndxlGetNetworkSize.restype = ctypes.c_size_t
lib.ndxlGetNetworkSize.argtypes = [NDXLNetwork_p]
ndxlGetNetworkSize = lib.ndxlGetNetworkSize

lib.ndxlGetNetworkTime.restype = ctypes.c_double
lib.ndxlGetNetworkTime.argtypes = [NDXLNetwork_p]
ndxlGetNetworkTime = lib.ndxlGetNetworkTime

# ^^^^^ Getters | setters vvvvv

lib.ndxlSetNetworkParams.restype = ctypes.c_int
lib.ndxlSetNetworkParams.argtypes = [NDXLNetwork_p, ctypes.c_void_p]
ndxlSetNetworkParams = lib.ndxlSetNetworkParams

# ^^^^^ Setters | functions vvvvv

lib.ndxlNetworkChannel.restype = NDXLChannel_p
lib.ndxlNetworkChannel.argtypes = [NDXLNetwork_p, ctypes.c_size_t]
ndxlNetworkChannel = lib.ndxlNetworkChannel

lib.ndxlNetworkChannelByIndex.restype = NDXLChannel_p
lib.ndxlNetworkChannelByIndex.argtypes = [NDXLNetwork_p, ctypes.c_size_t]
ndxlNetworkChannelByIndex = lib.ndxlNetworkChannelByIndex

lib.ndxlNetworkEdge.restype = NDXLEdge_p
lib.ndxlNetworkEdge.argtypes = [NDXLNetwork_p, ctypes.c_size_t]
ndxlNetworkEdge = lib.ndxlNetworkEdge

lib.ndxlNetworkEdgeByIndex.restype = NDXLEdge_p
lib.ndxlNetworkEdgeByIndex.argtypes = [NDXLNetwork_p, ctypes.c_size_t]
ndxlNetworkEdgeByIndex = lib.ndxlNetworkEdgeByIndex

lib.ndxlNetworkVertex.restype = NDXLVertex_p
lib.ndxlNetworkVertex.argtypes = [NDXLNetwork_p, ctypes.c_size_t]
ndxlNetworkVertex = lib.ndxlNetworkVertex

lib.ndxlNetworkVertexByIndex.restype = NDXLVertex_p
lib.ndxlNetworkVertexByIndex.argtypes = [NDXLNetwork_p, ctypes.c_size_t]
ndxlNetworkVertexByIndex = lib.ndxlNetworkVertexByIndex

# ^^^^^ Functions | simulation vvvvv

lib.ndxlNetworkSimulateRalston.restype = ctypes.c_int
lib.ndxlNetworkSimulateRalston.argtypes = [NDXLNetwork_p, ctypes.c_double, ctypes.c_double, NDXLNetworkCallback]
ndxlNetworkSimulateRalston = lib.ndxlNetworkSimulateRalston

lib.ndxlNetworkSimulateNystrom.restype = ctypes.c_int
lib.ndxlNetworkSimulateNystrom.argtypes = [NDXLNetwork_p, ctypes.c_double, ctypes.c_double, NDXLNetworkCallback]
ndxlNetworkSimulateNystrom = lib.ndxlNetworkSimulateNystrom

lib.ndxlNetworkSimulateDopri.restype = ctypes.c_int
lib.ndxlNetworkSimulateDopri.argtypes = [NDXLNetwork_p, ctypes.c_double, ctypes.c_double, ctypes.c_double, ctypes.POINTER(ctypes.c_double), ctypes.c_size_t, NDXLNetworkCallback]
ndxlNetworkSimulateDopri = lib.ndxlNetworkSimulateDopri

lib.ndxlNetworkSimulateRkck.restype = ctypes.c_int
lib.ndxlNetworkSimulateRkck.argtypes = [NDXLNetwork_p, ctypes.c_double, ctypes.c_double, ctypes.c_double, ctypes.POINTER(ctypes.c_double), ctypes.c_size_t, NDXLNetworkCallback]
ndxlNetworkSimulateRkck = lib.ndxlNetworkSimulateRkck

lib.ndxlNetworkSimulateEuler.restype = ctypes.c_int
lib.ndxlNetworkSimulateEuler.argtypes = [NDXLNetwork_p, ctypes.c_double, ctypes.c_double, NDXLNetworkCallback]
ndxlNetworkSimulateEuler = lib.ndxlNetworkSimulateEuler

lib.ndxlNetworkSimulateHeun.restype = ctypes.c_int
lib.ndxlNetworkSimulateHeun.argtypes = [NDXLNetwork_p, ctypes.c_double, ctypes.c_double, ctypes.c_double, ctypes.POINTER(ctypes.c_double), ctypes.c_size_t, NDXLNetworkCallback]
ndxlNetworkSimulateHeun = lib.ndxlNetworkSimulateHeun

lib.ndxlNetworkSimulateRadau.restype = ctypes.c_int
lib.ndxlNetworkSimulateRadau.argtypes = [NDXLNetwork_p, ctypes.c_double, ctypes.c_double, ctypes.c_double, ctypes.POINTER(ctypes.c_double), ctypes.c_size_t, NDXLNetworkCallback]
ndxlNetworkSimulateRadau = lib.ndxlNetworkSimulateRadau

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
	"""The core object in NDXL: a network is a host object representing a graph where vertices, edges, and noise channels can be added, and also provides the network simulation interface.

	Parameters
	----------
	ptr : NDXLNetwork_p
		Pointer to NDXLNetwork object.
	args : ctypes.c_void_p
		User-defined `void* params` associated with the network.

	Attributes
	----------
	_ptr : NDXLNetwork_p
		The underlying `C` NDXLNetwork pointer.
	_pptr : ctypes.c_void_p
		`C void*` pointing to the network's metadata.
	_params : list
		`Python list` containing the network's metadata.
	_vdict : dict
		Hashmap containing all vertices currently residing within the network.
	_edict : dict
		Hashmap containing all edges currently residing within the network.
	_cdict : dict
		Hashmap containing all noise channels currently residing within the network.

	Examples
	--------
	>>> nwk = ndxl.create_network([], [])
	>>> v = nwk.create_vertex(1., dynamics=lambda v, y, t: 2. * y * t)
	>>> def cb(net, t, t_eval, y_eval):
	>>>		t_eval.append(t)
	>>>		y_eval.append(v.state[0])
	>>> nwk.simulate(4., cb=cb)
	>>> print(f"Times = {nwk.params[0]}")
	>>> print(f"States = {nwk.params[1]}")
	"""
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
		"""Destroys the network wrapper without releasing the actual network memory.

		Returns
		-------
		None

		Notes
		-----
		This method should be used for destroying a network without affecting any of its associated vertices, edges, noise channels, or underlying memory associated with `self._ptr`.
		If the goal is to destroy the objects and their underlying memory buffers, use `destroy()` instead.

		See Also
		--------
		[`destroy`][ndxl.network.destroy] : Destroy the memory associated with this network, as well as all of its vertices, edges, and noise channels.
		"""
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
		return "'<ndxl.network object at " + f"{id(self._Int_ptr):#0{int(platform.architecture()[0][:-3]) // 4 + 2}X}" + " {'order': " + str(ndxlGetNetworkOrder(self._ptr)) + ", 'size': " + str(ndxlGetNetworkSize(self._ptr)) + ", 'time': " + str(ndxlGetNetworkTime(self._ptr)) + "}>'"

	def __repr__(self):
		return f"NDXL network\n============\n\nVertices (order): {ndxlGetNetworkOrder(self._Int_ptr)}\nEdges (size): {ndxlGetNetworkSize(self._ptr)}\nTime: {ndxlGetNetworkTime(self._ptr)}\n"

	def _Deregister_channel(self, cid):
		"""Deregisters a noise channel from this network.

		Parameters
		----------
		cid : int
			Integer representation of the NDXLChannel pointer to be removed.

		Returns
		-------
		None

		Notes
		-----
		This function is for internal use only, and should not be called by the user!
		"""
		del self._cdict[cid]

	def _Deregister_edge(self, eid):
		"""Deregisters an edge from this network.

		Parameters
		----------
		eid : int
			Integer representation of the NDXLEdge pointer to be removed.

		Returns
		-------
		None

		Notes
		-----
		This function is for internal use only, and should not be called by the user!
		"""
		del self._edict[eid]

	def _Deregister_vertex(self, vid):
		"""Deregisters a vertex from this network.

		Parameters
		----------
		vid : int
			Integer representation of the NDXLVertex pointer to be removed.

		Returns
		-------
		None

		Notes
		-----
		This function is for internal use only, and should not be called by the user!
		"""
		del self._vdict[vid]

	def _Set_error(self, err):
		"""Sets the internal error state of this network.

		Parameters
		----------
		err : BaseException
			The exception that should next be raised.

		Returns
		-------
		None

		Notes
		-----
		This function is for internal use only, and should not be called by the user!
		"""
		self._params[-2] = err

	@property
	def _Int_ptr(self)->int:
		"""Gets the raw `C` pointer associated with this network.

		Returns
		-------
		int
			Integer representation of the underlying `C` NDXLNetwork pointer wrapped by this network.

		Notes
		-----
		This function is for internal use only, and should not be called by the user!
		"""
		return ctypes.cast(self._ptr, ctypes.c_void_p).value

	@property
	def c_ptr(self)->LP_NDXLNetwork:
		"""Gets the ctypes object pointing to the underlying C object pointer.

		Returns
		-------
		LP_NDXLNetwork
			The NDXLNetwork pointer managed by this network.
		"""
		return self._ptr

	def destroy(self)->int:
		"""Destroys the network and its wrapped NDXLNetwork pointer.

		Returns
		-------
		int
			NDXL_OK if the network is successfully destroyed, or NDXL_ERR on failure.

		Notes
		-----
		This method should be used for destroying a network and all its associated vertices, edges, and noise channels, as it calls the underlying `C` destructor and frees up the alloacted memory.
		If the goal is to release the handle on the memory rather than outright destroy it, use `__del__` instead.

		See Also
		--------
		[`__del__`][ndxl.network.__del__] : Destroy the `Python` wrapper without affecting the underlying memory allocations.
		"""
		del network.nwkdict[self._Int_ptr]
		_res = ndxlDestroyNetwork(ctypes.pointer(self._ptr))
		del self._pptr
		del self._params
		return _res

	@property
	def channels(self):
		"""Gets a list of all the noise channels in this network.

		Returns
		-------
		NumPy.ndarray
			1D-NumPy array containing all the noise channels in the network.

		Notes
		-----
		The returned array can be safely iterated over, its elements modified and/or destroyed, without affecting the array indexing.
		"""
		from .channel import channel
		_nc = ndxlGetNetworkNoiseDimension(self._ptr)
		_cnls = np.empty(_nc, dtype=channel)
		for i in range(_nc):
			_cnl = ndxlNetworkChannel(self._ptr, i)
			_cnls[i] = channel(_cnl, ndxlGetChannelParams(_cnl))
		return _cnls

	@property
	def edges(self):
		"""Gets a list of all the edges in this network.

		Returns
		-------
		NumPy.ndarray
			1D-NumPy array containing all the edges in the network.

		Notes
		-----
		The returned array can be safely iterated over, its elements modified and/or destroyed, without affecting the array indexing.
		"""
		from .edge import edge
		_ne = ndxlGetNetworkSize(self._ptr)
		_edges = np.empty(_ne, dtype=edge)
		for i in range(_ne)
			_edge = ndxlNetworkEdge(self._ptr, i)
			_edges[i] = edge(_edge, ndxlGetEdgeParams(_edge))
		return _edges;

	@property
	def noise_dimension(self):
		"""Gets the number of noise channels in this network.

		Returns
		-------
		int
			The number of noise channels, used and unused, in the network.
		"""
		return ndxlGetNetworkOrder(self._ptr)

	@property
	def order(self):
		"""Gets the number of vertices in this network.

		Returns
		-------
		int
			The number of vertices, dynamic and static, in the network.
		"""
		return ndxlGetNetworkOrder(self._ptr)
	
	@property
	def params(self):
		"""Gets the extra parameters used by this network.

		Returns
		-------
		tuple
			Parameters tuple assigned to this network during creation.

		See Also
		--------
		[`create_network`][ndxl.create_network] : Create a network.
		"""
		return self._params[-1]

	@property
	def size(self):
		"""Gets the number of edges in this network.

		Returns
		-------
		int
			The number of edges, directed, undirected, dynamic, and static, in the network.
		"""
		return ndxlGetNetworkSize(self._ptr)

	@property
	def time(self):
		"""Gets the local simulation time of this network.

		Returns
		-------
		float
			The current simulation time of this network, defined as the time after the most recently accepted simulation step.

		Notes
		-----
		This time is initialised to zero when the network is created. It only updates on successful simulation steps, and does not update during step evaluations.
		"""
		return ndxlGetNetworkTime(self._ptr)

	@property
	def vertices(self):
		"""Gets a list of all the vertices in this network.

		Returns
		-------
		NumPy.ndarray
			1D-NumPy array containing all the vertices in the network.

		Notes
		-----
		The returned array can be safely iterated over, its elements modified and/or destroyed, without affecting the array indexing.
		"""
		from .vertex import vertex
		_nv = ndxlGetNetworkOrder(self._ptr)
		_verts = np.empty(_nv, dtype=vertex)
		for i in range(_nv):
			_vert = ndxlNetworkVertex(self._ptr, i)
			_verts[i] = vertex(_vert, ndxlGetVertexParams(_vert))
		return _verts

	def simulate(self, tspan, method=None, **simkwargs):
		"""Simulates a network over a given time span using a specified numerical method.

		Parameters
		----------
		tspan : float
			Duration of the simulation, starting at `self.time` (which is zero if no simulations have been run yet). Must be greater than zero.
		method : str or simulationmethod, optional
			Which numerical method to use. Default is 'dopri' (ndxl.methods.dopri).
		**simkwargs : dict, optional
			Method-specific arguments passed to the underlying solver; see 'Other Parameters'.

		Other Parameters
		----------------
		h, step, stepsize, step_size : None or float
			Fixed step-size used by static stepping algorithms like 'ralston' (ndxl.methods.ralston). If `None`, uses the default of `0.1`.
		atol, abstol : None or float
			Absolute tolerance (no. correct decimal places) used by adaptive algorithms like 'dopri' (ndxl.methods.dopri). If `None`, uses the default of `1e-6` for explicit methods, `1e-8` for implicit methods, and `1e-3` for stochastic methods.
		rtol, reltol : None or float
			Relative tolerance (no. correct digits) used by adaptive algorithms. If `None`, uses the default of `1e-3` for explicit methods, `1e-6` for implicit methods, and `1e-3` for stochastic methods.
		tstops, t_stops, timestops, time_stops : None or float or iterable of float
			Time points adaptive algorithms must evaluate, in addition to that which is automatically determined by the solver. Must be sorted and lie within `[self.time, self.time + tspan]`. Default is `None`.
		cb, callback : None or callable
			If provided, must have the signature `cb(net, t, *args)`, which is called once at `t=0`, then again after each successful time step. Repeated simulations on the same network do not perform the initial call again. Default is `None`.

		Returns
		-------
		int
			NDXL_OK if the simulation completed successfully, or NDXL_ERR if the simulation failed prematurely.

		Raises
		------
		TypeError
			If any of the parameters are not of the correct type (or of a type convertible to the required type).
		ValueError
			If an unkown method name is provided, or the simulation fails due to an external error.
		KeyError
			If an unknown or repeated keyword is passed to `**simkwargs`.
		"""
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
		"""Creates a new noise channel on this network, representing an independent Wiener process that can affect vertices and edges via a noise function.

		Parameters
		----------
		args : None or scalar or iterable, optional
			Extra parameters used by this channel, converted into a tuple before use. Default is `None`.
		"""
		from .channel import channel
		_args = () if args is None else ((args,) if np.isscalar(args) else copy.deepcopy(args))
		_pptr = ctypes.cast(ctypes.pointer(ctypes.py_object([_args])), ctypes.c_void_p)
		_channel = channel(ndxlCreateChannel(self._ptr, _pptr), _pptr)
		self._cdict[_channel._Int_ptr] = _channel
		return _channel

	def create_vertex(self, state=None, dynamics=None, noise=None, channels=None, cb=None, atol=0., rtol=0., args=None, cplx=False):
		"""Creates a new vertex on this network, representing a node in a graph.

		Parameters
		----------
		state : None or scalar or iterable, optional
			Initial conditions of this vertex, converted to a NumPy.ndarray before use. Default is `None`, which assigns an initial value of `0` to the vertex.
		dynamics : None or callable, optional
			How the state vector of the agent evolves over time, subject to the initial conditions defined by `state`. If provided, must have the signature `dynamics(vert, y, t, *args)`, and return a scalar or iterable with the same shape as `state`. Default is `None`, meaning the state vector does not evolve over time (static vertex).
		noise : None or callable, optional
			Stochastic process driving the state vector of the vertex. If provided, must have the signature `noise(vert, y, t, *args)`, and return a scalar or iterable with shape `(len(state), len(channels))` corresponding to the local diffusion matrix. Default is `None`, meaning the state vector evolution is purely deterministic.
		channels: None or channel or iterable of channel, optional
			Noise channels driving the stochastic process defined by `noise`. If both `channels` and `noise` are `None`, then a new, independent channel assigned only to this vertex is created. If provided as an iterable, `None` elements are allowed, where each `None` entry also creates to a new, independent channel assigned to only this vertex. Default is `None`.
		cb : None or callable, optional
			If provided, must have the signature `cb(vert, y, t, *args)`, which is called once after each successful time step during simulation. Note that unlike network callbacks passed to `simulate()`, this callback is NOT called at `t=0`.
		atol : float
			Absolute tolerance (no. correct decimal places) used during simulation. If `0`, the absolute tolerance passed to the `simulate()` function is used. Default is `0`.
		rtol : float
			Relative tolerance (no. correct digits) used during simulation. If `0`, the relative tolerance passed to the `simulate()` function is used. Default is `0`.
		args : None or scalar or iterable, optional
			Extra parameters used by this vertex, converted to a tuple before use. Default is `None`.
		cplx : Bool, optional
			Whether the state vector of this vertex is (or ever will be) complex-valued. If `state` contains complex values, this argument is forced to `True`. If `state` does not contain complex values but the dynamics or noise of this vertex might result in complex values, set this to `True`. Default is `False`.

		Returns
		-------
		vertex
			A new vertex if creation thereof is successful, or `None` otherwise.
		"""
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
				_state = np.ctypesas_ctypes(_st.view(float))
				_cplx = True
			else:
				_st = _arr.flatten().astype(float)
				_nstate = len(_st)
				_state = np.ctypesas_ctypes(_st)
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
		_vert = vertex(ndxlCreateVertex(self._ptr, _state, _nstate, NoneVFn if dynamics is None else _vfunc_bridge, NoneVNz if noise is None else _vnoise_bridge, _cn, _nchannels, NoneVCb if cb is None else _vcb_bridge, _pptr), _pptr)
		self._vdict[_vert._Int_ptr] = _vert
		return _vert

	def create_edge(self, src, dest, state=None, delay=0., directed=False, dynamics=None, noise=None, channels=None, cb=None, atol=0., rtol=0., args=None, cplx=False):
		"""Creates a new edge on this network, representing a coupling between two nodes in a graph.

		Parameters
		----------
		src : vertex or NDXLVertex_p
			The source vertex.
		dest : vertex or NDXLVertex_p
			The destination vertex.
		state : None or scalar or iterable, optional
			Initial conditions of this edge, converted to a NumPy.ndarray before use. Default is `None`, which assigns an initial value of `0` to the edge.
		delay : float, optional
			How long information takes to travel from `src` to `dest`. If `directed` is `False`, this is also the time taken for information to travel the opposite direction. Historical state information for `src` and `dest` is stored up to `t - delay`. Default is `0`.
		directed : Bool, optional
			If True, information only flows from `src` to `dest` (directed edge), and in both directions if `False` (undirected edge). Default is `False`.
		dynamics : None or callable, optional
			How the state vector of the agent evolves over time, subject to the initial conditions defined by `state`. If provided, must have the signature `dynamics(edge, y, t, *args)`, and return a scalar or iterable with the same shape as `state`. Default is `None`, meaning the state vector does not evolve over time (static edge).
		noise : None or callable, optional
			Stochastic process driving the state vector of the edge. If provided, must have the signature `noise(edge, y, t, *args)`, and return a scalar or iterable with shape `(len(state), len(channels))` corresponding to the local diffusion matrix. Default is `None`, meaning the state vector evolution is purely deterministic.
		channels: None or channel or iterable of channel, optional
			Noise channels driving the stochastic process defined by `noise`. If both `channels` and `noise` are `None`, then a new, independent channel assigned only to this edge is created. If provided as an iterable, `None` elements are allowed, where each `None` entry also creates to a new, independent channel assigned to only this edge. Default is `None`.
		cb : None or callable, optional
			If provided, must have the signature `cb(edge, y, t, *args)`, which is called once after each successful time step during simulation. Note that unlike network callbacks passed to `simulate()`, this callback is NOT called at `t=0`.
		atol : float
			Absolute tolerance (no. correct decimal places) used during simulation. If `0`, the absolute tolerance passed to the `simulate()` function is used. Default is `0`.
		rtol : float
			Relative tolerance (no. correct digits) used during simulation. If `0`, the relative tolerance passed to the `simulate()` function is used. Default is `0`.
		args : None or scalar or iterable, optional
			Extra parameters used by this edge, converted to a tuple before use. Default is `None`.
		cplx : Bool, optional
			Whether the state vector of this edge is (or ever will be) complex-valued. If `state` contains complex values, this argument is forced to `True`. If `state` does not contain complex values but the dynamics or noise of this edge might result in complex values, set this to `True`. Default is `False`.

		Returns
		-------
		edge
			A new edge if creation thereof is successful, or `None` otherwise.

		Notes
		-----
		Edges can also be created directly on the source vertex using `src.create_edge(dest, ...)`.

		See Also
		[vertex.create_edge][vertex.create_edge] : Create an edge from the source vertex.
		"""
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
				_state = np.ctypesas_ctypes(_st.view(float))
				_cplx = True
			else:
				_st = _arr.flatten().astype(float)
				_nstate = len(_st)
				_state = np.ctypesas_ctypes(_st)
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
		_edge = edge(ndxlCreateEdge(src if isinstance(src, NDXLVertex_p) else src.c_ptr, dest if isinstance(dest, NDXLVertex_p) else dest.c_ptr, _state, _nstate, delay, 1 if directed else 0, NoneEFn if dynamics is None else _efunc_bridge, NoneENz if noise is None else _enoise_bridge, _cn, _nchannels, NoneECb if cb is None else _ecb_bridge, _pptr), _pptr)
		self._edict[_edge._Int_ptr] = _edge
		return _edge

def create_network(*args):
	"""Creates an empty network (no vertices, edges, or noise channels).

	Parameters
	----------
	*args : tuple
		Extra information used by the network.

	Returns
	-------
	network
		A new, empty network.
	"""
	_args = tuple([arg for arg in args])
	_pptr = ctypes.cast(ctypes.pointer(ctypes.py_object([None, dict(), dict(), dict(), None, _args])), ctypes.c_void_p)
	return network(ndxlCreateNetwork(_pptr), _pptr)

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