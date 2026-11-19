# Packages
import ndxl
import numpy as np


# Network
nwk = ndxl.create_network()
for i in range(10):
	destruct_state = np.random.uniform(0.5, 7.)
	nwk.create_vertex(1., dynamics=lambda v, y, t, *args: y * t, args=(i, destruct_state))


# Callback and simulation
def cb(net, t):
	# Note the use of the _list iterator!
	for v in net.vertex_list:
		if v.state[0] >= v.params[1]:
			print(f"Vertex {v.params[0]} failed at {t=}")
			v.destroy()

nwk.simulate(2., method=ndxl.methods.ralston, step=1e-3, cb=cb)