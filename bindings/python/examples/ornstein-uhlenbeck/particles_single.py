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

import matplotlib.pyplot as plt
import ndxl
import numpy as np


# Functions
def drift(part, x, t, G, s):
	return -G * x

def diff(part, x, t, G, s):
	return s


# Parameters
N = 1000
x0 = -1.
Gamma = 1.5
sigma = 1.
ts = 2.


# Network simulation
nwk = ndxl.create_network(([], []))
c1 = nwk.create_channel()

for i in range(N):
	nwk.create_vertex(x0, dynamics=drift, noise=diff, channels=c1, args=(Gamma, sigma))

def cb(net, t, t_eval, x_eval):
	global ts
	if np.isclose(t % ts, 0.):
		t_eval.append(t)
		x_vals = []
		for particle in net.vertices:
			x_vals.append(particle.state[0])
		x_eval.append(x_vals)

nwk.simulate(ts * 7, method="heun", tstops=[i * ts for i in range(1, 7)], cb=cb)


# Plotting
fig, ax = plt.subplots(4, 2, figsize=(7, 7))
for i in range(8):
	t = ts * i
	x = np.sort(np.asarray(nwk.params[1][i]))
	u = x0 * np.exp(-Gamma * t)
	var = sigma**2 / (2. * Gamma) * (1. - np.exp(-2. * Gamma * t))
	P = np.exp(-(x - u)**2 / (2. * var)) / np.sqrt(2. * np.pi * var)
	ax[i // 2][i % 2].hist(x, bins=50, density=True, stacked=True)
	x_pos = -0.5325
	ax[i // 2][i % 2].text(x_pos, 0.8 * ax[i // 2][i % 2].get_ylim()[1], "$t=" + str(t) + "$", ha='right', va='top')
plt.show()