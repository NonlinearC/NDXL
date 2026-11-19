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

import matplotlib.pyplot as plt
import ndxl
import numpy as np


# Functions
def drdt(body, state, t, m):
	# Extract parameters
	G = body.network.params[0]
	u = state[0, :]
	v = state[1, :]

	# Compute force sum in both dimensions
	F_sum = np.zeros(2, dtype=float)
	for bod in body.network.vertices:
		if bod != body:
			r_diff = u - bod.state[0, :]
			F_sum += (bod.params[0] * r_diff / (np.linalg.norm(r_diff)**3))

	# Return derivatives, preserve state shape
	du = v
	dv = -G * F_sum
	return np.array([du, dv])


# Network
N = 10
t_stops = [1.5, 3.]
nwk = ndxl.create_network((1., [], []))
for i in range(N):
	dist = np.random.uniform(8., 10.)
	ang = 0.2 * i * np.pi
	pos = [dist * np.cos(ang), dist * np.sin(ang)]
	vel = [0.5 * dist * np.cos(ang + 0.5 * np.pi), 0.5 * dist * np.sin(ang + 0.5 * np.pi)]
	nwk.create_vertex([pos, vel], dynamics=drdt, args=1.)

def cb(net, t, G, t_eval, r_eval):
	global N, t_stops
	if np.isclose(t % 1.5, 0.):
		t_eval.append(t)
		r_eval.append([v.state.copy() for v in net.vertices])

nwk.simulate(4.5, tstops=t_stops, cb=cb)
r_eval = np.array(nwk.params[2])


# Plotting
cols = ["blue", "orange", "magenta", "green", "black", "red", "grey", "brown", "navy", "olivedrab"]
fig, ax = plt.subplots(2, 2, figsize=(5, 5))
for i in range(4):
	x = r_eval[i, :, 0, 0]
	y = r_eval[i, :, 0, 1]
	xv = r_eval[i, :, 1, 0]
	yv = r_eval[i, :, 1, 1]
	for j in range(N):
		ax[i // 2][i % 2].scatter(x[j], y[j], c=cols[j])
	xl = ax[i // 2][i % 2].get_xlim()
	yl = ax[i // 2][i % 2].get_ylim()
	for j in range(N):
		ax[i // 2][i % 2].arrow(x[j], y[j], 0.1 * (xl[1] - xl[0]) * xv[j], 0.1 * (yl[1] - yl[0]) * yv[j], head_width=0.05 * (xl[1] - xl[0]), color=cols[j])
	ax[i // 2][i % 2].text(0., 0., "$t=" + str(i * 1.5) + "$", ha='center', va='center')

bbox_coords = [a.get_position() for a in ax.flat]
x0 = min(b.x0 for b in bbox_coords)
x1 = max(b.x1 for b in bbox_coords)
y0 = min(b.y0 for b in bbox_coords)
y1 = max(b.y1 for b in bbox_coords)

fig.tight_layout(rect=[0.03, 0.03, 0.999, 0.999])
fig.text(0.5 * (x0 + x1) + 0.03, y0 - 0.08, "$x$", ha='center', va='top', fontsize=12)
fig.text(x0 - 0.08, 0.5 * (y0 + y1) + 0.03, "$y$", ha='right', va='center', rotation='vertical', fontsize=12)
fig.savefig("nbody.pdf", bbox_inches='tight')