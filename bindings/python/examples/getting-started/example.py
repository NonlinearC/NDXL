import matplotlib.pyplot as plt
import ndxl
import numpy as np

nwk = ndxl.create_network(([], [[], [], [], [], []]))

def kuramoto(vert, y, t, w):
    return w + np.sum([e.state * np.sin(vert.neighbour_state(e) - y) for e in vert.edges])

omega = np.linspace(9.9, 10.1, 5)
theta = np.linspace(-np.pi, np.pi, 5)
oscillators = np.empty(5, dtype=ndxl.vertex)
for i in range(5):
    oscillators[i] = nwk.create_vertex(theta[i], dynamics=kuramoto, args=omega[i])

K = 1.0
for i in range(5):
    oscillators[i].create_edge(oscillators[(i + 1) % 5], K)

def cb(net, t, t_eval, p_eval):
	t_eval.append(t)
	for i, vert in enumerate(net.vertices):
		p_eval[i].append(vert.state[0])

nwk.simulate(10.0, cb=cb)

for i in range(5):
	plt.plot(nwk.params[0], nwk.params[1][i], label=f"Oscillator {i+1}")
plt.xlabel("Time, $t$")
plt.ylabel("Phase, $\\theta$")
plt.legend()
plt.savefig("example.png", bbox_inches='tight')