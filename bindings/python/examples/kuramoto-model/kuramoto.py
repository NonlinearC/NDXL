# Package imports
import matplotlib
matplotlib.rcParams['pgf.texsystem'] = 'pdflatex'
matplotlib.rcParams.update({
	'font.family': 'serif', 'font.size': 12,
	'axes.labelsize': 10,
	'axes.titlesize': 12,
	'figure.titlesize': 16
})
matplotlib.rcParams['text.usetex'] = True

from scipy.integrate import solve_ivp

import matplotlib.pyplot as plt
import ndxl
import numpy as np


# Function definitions
def kuramoto_scipy(t, y, w, K):
	# Since N_i = 2 for all i, we can hardcode 1/N_i = 0.5
	return w + 0.5 * np.sum(K * np.sin(y[None, :] - y[:, None]), axis=1)

def kuramoto_ndxl(vert, y, t, w):
	N_i = vert.degree
	return w + np.sum([e.state * np.sin(vert.neighbour_state(e) - y) for e in vert.edges]) / N_i


# Constants and initial conditions
N = 4
coupling = np.array([
	[0, 1, 1, 0],
	[1, 0, 0, 1],
	[1, 0, 0, 1],
	[0, 1, 1, 0]
], dtype=float)
omega = np.random.uniform(0.9, 1.1, size=N)
theta0 = np.random.uniform(-np.pi, np.pi, size=N)
verts = np.empty(4, dtype=ndxl.vertex)


# Numerical solutions
sol = solve_ivp(kuramoto_scipy, (0., 5.), theta0, args=(omega, coupling))

nwk = ndxl.create_network(([], [], [], [], []))
for i in range(N):
	verts[i] = nwk.create_vertex(theta0[i], dynamics=kuramoto_ndxl, args=omega[i])

for i in range(N):
	for j in range(i, N):
		if coupling[i, j] == 1:
			verts[i].create_edge(verts[j], 1.)

def cb(net, t, t_eval, *v_states):
	global N
	t_eval.append(t)
	for i in range(N):
		v_states[i].append(net.vertex_list[i].state[0])

nwk.simulate(5., method="rkck", cb=cb)


# Perturbation callback
def vcb(v, y, t, w):
	if t == 2.5:
		y[0] = 0.0

nwk2 = ndxl.create_network(([], [], [], [], []))
for i in range(N):
	verts[i] = nwk2.create_vertex(theta0[i], dynamics=kuramoto_ndxl, args=omega[i], cb=vcb if not i else None)

for i in range(N):
	for j in range(i, N):
		if coupling[i, j] == 1:
			verts[i].create_edge(verts[j], 1.)

nwk2.simulate(5., tstops=2.5, cb=cb)



# Plotting
fig, ax = plt.subplots(1, 2, figsize=(6, 3))
for i in range(N):
	ax[0].plot(sol.t, sol.y[i, :], ls='--', c='g', lw=2., label="$SciPy$" if not i else None)
	ax[0].plot(nwk.params[0], nwk.params[i + 1], ms=2, lw=0.5, marker='o', c='b', label="$NDXL$" if not i else None)
	ax[1].plot(sol.t, sol.y[i, :], ls='--', c='g', lw=2., label="$SciPy$" if not i else None)
	ax[1].plot(nwk2.params[0], nwk2.params[i + 1], ms=2, lw=0.5, marker='o', c='b' if i else 'r', label="Perturbed" if not i else ("$NDXL$" if i == 1 else None))
ax[0].set_xlabel("a)")
ax[0].set_ylabel("Phase, $\\theta$")
ax[0].legend(loc="upper left")
ax[1].set_xlabel("b)")
ax[1].legend(loc="upper left")
fig.supxlabel("Time, $t$")
fig.tight_layout()
fig.savefig("kuramoto.pdf", bbox_inches='tight')