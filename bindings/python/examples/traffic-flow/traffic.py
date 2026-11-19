# Packages
import matplotlib
matplotlib.rcParams['pgf.texsystem'] = 'pdflatex'
matplotlib.rcParams.update({
	'font.family': 'serif', 'font.size': 12,
	'axes.labelsize': 10,
	'axes.titlesize': 12,
	'figure.titlesize': 16
})
matplotlib.rcParams['text.usetex'] = True

import ddeint
import jitcdde
import matplotlib.pyplot as plt
import ndxl
import numpy as np
import symengine
import sympy


# Functions
def acc(car, state, t, a, e_dict):
	vmax = car.network.params[0]["speed-limit"]
	d = car.network.params[0]["safe-distance"]
	x, v = state
	x_ahead = car.neighbour_state(e_dict["ahead"])[0]
	x_delayed = car.neighbour_state(e_dict["delayed"])[0]
	tanhd = np.tanh(d)
	V = vmax * (np.tanh(x_ahead - x_delayed - d) + tanhd) / (1. + tanhd)
	return [v, a * (V - v)]


# Simulation parameters
N = 20											# No. cars
tau = np.random.uniform(0.5, 1.5, size=N)		# Reaction times
a = np.random.uniform(0.1, 0.3, size=N)			# Reaction sensitivity
v_max = 60. * (1000. / 3600.)					# Speed limit (60 kph) in m/s
x0 = np.linspace(0., 100., N, endpoint=False)	# Initial spatial distribution = 1/L
v0 = np.random.uniform(10., 15., size=N)		# Initial velocity distribution
d_safe = 1.5									# Safe bumper-to-bumper distance


# Network
traffic = ndxl.create_network(({"speed-limit": v_max, "safe-distance": d_safe}, [], []))
cars = np.empty(N, dtype=ndxl.vertex)
for i in range(N):
	cars[i] = traffic.create_vertex([x0[i], v0[i]], dynamics=acc, args=(a[i], {"ahead": None, "delayed": None}))
for i in range(N):
	cars[(i - 1) % N].params[1]["ahead"] = cars[i].create_edge(cars[(i - 1) % N], delay=tau[(i - 1) % N], directed=True)
	cars[i].params[1]["delayed"] = cars[i].create_edge(cars[i], delay=tau[i], directed=True)
	
def cb(net, t, road_dict, t_eval, s_eval):
	global N, cars
	t_eval.append(t)
	states = np.empty((N, 2), dtype=float)
	for i in range(N):
		states[i, :] = cars[i].state.copy()
	s_eval.append(states)

traffic.simulate(100., method="nystrom", cb=cb)


# ddeint
def history(t):
	global x0, v0
	return np.concatenate((x0, v0), axis=0)

def car_model(y, t):
	global N, tau, v_max, d_safe, a
	s = y(t)
	x = s[:N]
	v = s[N:]
	dx = v
	dv = np.empty(N, dtype=float)
	for i in range(N):
		x_del = y(t - tau[i])[:N]
		dx_del = x_del[(i + 1) % N] - x_del[i]
		V = v_max * (np.tanh(dx_del - d_safe) + np.tanh(d_safe)) / (1. + np.tanh(d_safe))
		dv[i] = a[i] * (V - v[i])
	return np.concatenate((dx, dv), axis=0)

t_span = np.arange(0., 100.1, 0.1)
sol = ddeint.ddeint(car_model, history, t_span)


# jitcdde
dx, dv = [], []
for i in range(N):
	#cond = sympy.sympify(jitcdde.t < tau[i])
	del_xp1 = jitcdde.y((i + 1) % N, jitcdde.t - tau[i])
	del_x = jitcdde.y(i, jitcdde.t - tau[i])
	V = v_max * (symengine.tanh(del_xp1 - del_x - d_safe) + np.tanh(d_safe)) / (1. + np.tanh(d_safe))
	dx.append(jitcdde.y(i + N, jitcdde.t))
	dv.append(a[i] * (V - jitcdde.y(i + N, jitcdde.t)))

DDE = jitcdde.jitcdde(dx + dv, max_delay=1.5)
DDE.constant_past(list(x0) + list(v0))
DDE.adjust_diff()
sol_jit = np.array([DDE.integrate(tp) for tp in t_span])


# Plotting
fig, ax = plt.subplots(3, 2, figsize=(6.5, 9))

for i in range(5):
	x_vals = traffic.params[2][i * 250][:, 0] * 0.02 * np.pi
	x_coords = 50. * np.cos(x_vals) / np.pi
	y_coords = 50. * np.sin(x_vals) / np.pi
	ax[i // 2][i % 2].scatter(x_coords, y_coords, c='b')
	x_sol = sol[:, :N][i * 250] * 0.02 * np.pi
	x_coords = 50. * np.cos(x_sol) / np.pi
	y_coords = 50. * np.sin(x_sol) / np.pi
	ax[i // 2][i % 2].scatter(x_coords, y_coords, c='g', marker='*')
	x_sol = sol_jit[i * 250][:N] * 0.02 * np.pi
	x_coords = 50. * np.cos(x_sol) / np.pi
	y_coords = 50. * np.sin(x_sol) / np.pi
	ax[i // 2][i % 2].scatter(x_coords, y_coords, c='r', marker='^')
	ax[i // 2][i % 2].text(0., 0., "$t=" + str(i * 25.) + "$", va='center', ha='center', size=12)

rmsd_ndjit = []
rmsd_ddejit = []
rmsd_nddde = []
for i in range(len(traffic.params[1])):
	rmsd_ndjit.append(np.sqrt(np.sum((traffic.params[2][i][:, 0] - sol_jit[i][:N])**2) / N))
	rmsd_ddejit.append(np.sqrt(np.sum((sol[:, :N][i] - sol_jit[i][:N])**2) / N))
	rmsd_nddde.append(np.sqrt(np.sum((sol[:, :N][i] - traffic.params[2][i][:, 0])**2) / N))
ax[2][1].plot(traffic.params[1][::4], rmsd_ndjit[::4], c='k', marker='o', ms=0.5, lw=0.5, label="$JiTCDDE$/$NDXL$")
ax[2][1].plot(traffic.params[1][::4], rmsd_ddejit[::4], c="orange", marker='*', ms=0.5, lw=0.5, ls='-.', label="$ddeint$/$JiTCDDE$")
ax[2][1].plot(traffic.params[1][::4], rmsd_nddde[::4], c="magenta", marker='^', ms=0.5, lw=0.5, ls=':', label="$NDXL$/$ddeint$")
ax[2][1].legend(loc="lower right", fontsize=12)
ax[2][1].set_yscale('log')

fig.tight_layout()
fig.savefig("traffic.pdf", bbox_inches='tight')