from __future__ import annotations

from .core import *

# Constructor and destructor vvvvv

lib.ndxlCreateChannel.restype = NDXLChannel_p
lib.ndxlCreateChannel.argtypes = [NDXLNetwork_p, ctypes.c_void_p]

lib.ndxlDestroyChannel.restype = ctypes.c_int
lib.ndxlDestroyChannel.argtypes = [ctypes.POINTER(NDXLChannel_p)]

# ^^^^^ Constructor and destructor | getters vvvvv

lib.ndxlGetChannelEdgeCount.restype = ctypes.c_size_t
lib.ndxlGetChannelEdgeCount.argtypes = [NDXLChannel_p]

lib.ndxlGetChannelEdges.restype = ctypes.c_int
lib.ndxlGetChannelEdges.argtypes = [NDXLChannel_p, ctypes.POINTER(NDXLEdge_p)]

lib.ndxlGetChannelNetwork.restype = NDXLNetwork_p
lib.ndxlGetChannelNetwork.argtypes = [NDXLChannel_p]

lib.ndxlGetChannelParams.restype = ctypes.c_void_p
lib.ndxlGetChannelParams.argtypes = [NDXLChannel_p]

lib.ndxlGetChannelVertexCount.restype = ctypes.c_size_t
lib.ndxlGetChannelVertexCount.argtypes = [NDXLChannel_p]

lib.ndxlGetChannelVertices.restype = ctypes.c_int
lib.ndxlGetChannelVertices.argtypes = [NDXLChannel_p, ctypes.POINTER(NDXLVertex_p)]

lib.ndxlGetNetworkNextChannel.restype = NDXLChannel_p
lib.ndxlGetNetworkNextChannel.argtypes = [NDXLChannel_p]

lib.ndxlGetNetworkPreviousChannel.restype = NDXLChannel_p
lib.ndxlGetNetworkPreviousChannel.argtypes = [NDXLChannel_p]

# ^^^^^ Getters | setters vvvvv

lib.ndxlSetChannelParams.restype = ctypes.c_int
lib.ndxlSetChannelParams.argtypes = [NDXLChannel_p, ctypes.c_void_p]

# ^^^^^ Setters | Python wrappers vvvvv

class channel:
	def __init__(self, ptr, args):
		self._ptr = ptr
		self._pptr = args
		self._params = ctypes.cast(self._pptr, ctypes.POINTER(ctypes.py_object))
		if not self._params:
			lib.ndxlSetChannelParams(self._ptr, ctypes.cast(ctypes.pointer(ctypes.py_object([()])), ctypes.c_void_p))
		self._params = self._params.contents.value

	def __del__(self):
		self._ptr = None
		self._pptr = None
		self._params = None

	def __bool__(self):
		return bool(self._ptr)

	def __eq__(self, cnl):
		if isinstance(cnl, channel):
			return self._Int_ptr == cnl._Int_ptr
		return False

	def __ne__(self, cnl):
		if isinstance(cnl, channel):
			return self._Int_ptr != cnl._Int_ptr
		return False

	def __hash__(self):
		return hash(self._Int_ptr)

	def __len__(self):
		return lib.ndxlGetChannelVertexCount(self._ptr) + lib.ndxlGetChannelEdgeCount(self._ptr)

	def __str__(self):
		return "'<ndxl.channel object at " + f"{self._Int_ptr:#0{int(platform.architecture()[0][:-3]) // 4 + 2}X}" + " {'network': " + f"<ndxl.network object at {self.network._Int_ptr:#0{int(platform.architecture()[0][:-3]) // 4 + 2}X}>" + ", 'vertices': " + str(self.vertex_count) + ", 'edges': " + str(self.edge_count) + "}>'"

	def __repr__(self):
		return f"NDXL channel\n===========\n\nNetwork: <ndxl.network object at {self.network._Int_ptr:#0{int(platform.architecture()[0][:-3]) // 4 + 2}X}>\nVertices: {self.vertex_count}\nEdges: {self.edge_count}\n"

	@property
	def _Int_ptr(self):
		return ctypes.cast(self._ptr, ctypes.c_void_p).value

	@property
	def c_ptr(self):
		return self._ptr

	def destroy(self):
		self.network._Deregister_channel(self._Int_ptr)
		_res = lib.ndxlDestroyChannel(ctypes.pointer(self._ptr))
		del self._pptr
		del self._params
		return _res

	@property
	def edge_count(self):
		return lib.ndxlGetChannelEdgeCount(self._ptr)

	@property
	def edges(self):
		from .edge import edge
		_nedges = lib.ndxlGetChannelEdgeCount(self._ptr)
		_earr = (NDXLEdge_p * _nedges)()
		if lib.ndxlGetChannelEdges(self._ptr, _earr) != NDXL_OK:
			raise ValueError(ndxl.get_last_error_msg())
		for _edge in _earr:
			yield edge(_edge, lib.ndxlGetEdgeParams(_edge))

	@property
	def edge_list(self):
		from .edge import edge
		_nedges = lib.ndxlGetChannelEdgeCount(self._ptr)
		_earr = (NDXLEdge_p * _nedges)()
		if lib.ndxlGetChannelEdges(self._ptr, _earr) != NDXL_OK:
			raise ValueError(ndxl.get_last_error_msg())
		return list(edge(_edge, lib.ndxlGetEdgeParams(_edge)) for _edge in _earr)

	@property
	def network(self):
		from .network import network
		_nwk = lib.ndxlGetChannelNetwork(self._ptr)
		return network(_nwk, lib.ndxlGetNetworkParams(_nwk))

	@property
	def next(self):
		_cnl = lib.ndxlGetNetworkNextChannel(self._ptr)
		if _cnl:
			return channel(_cnl, lib.ndxlGetChannelParams(_cnl))
		return None

	@property
	def params(self):
		return self._params[-1]

	@property
	def previous(self):
		_cnl = lib.ndxlGetNetworkPreviousChannel(self._ptr)
		if _cnl:
			return channel(_cnl, lib.ndxlGetChannelParams(_cnl))
		return None

	@property
	def vertex_count(self):
		return lib.ndxlGetChannelVertexCount(self._ptr)

	@property
	def vertices(self):
		from .vertex import vertex
		_nverts = lib.ndxlGetChannelVertexCount(self._ptr)
		_varr = (NDXLVertex_p * _nverts)()
		if lib.ndxlGetChannelVertices(self._ptr, _varr) != NDXL_OK:
			raise ValueError(ndxl.get_last_error_msg())
		for _vert in _varr:
			yield vertex(_vert, lib.ndxlGetVertexParams(_vert))

	@property
	def vertex_list(self):
		from .vertex import vertex
		_nverts = lib.ndxlGetChannelVertexCount(self._ptr)
		_varr = (NDXLVertex_p * _nverts)()
		if lib.ndxlGetChannelVertices(self._ptr, _varr) != NDXL_OK:
			raise ValueError(ndxl.get_last_error_msg())
		return list(vertex(_vert, lib.ndxlGetVertexParams(_vert)) for _vert in _varr)

# ^^^^^ Python wrappers