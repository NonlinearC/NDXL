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
def swing(node, y, t, P, I):
	du = y[1]
	dv = P - I * y[1] + np.sum([e.state[0] * np.sin(node.neighbour(e).state[0] - y[0]) for e in node.edges])
	return [du, dv]

def power_line(line, y, t, idx, lim):
	if np.isclose(t, 1.) and (idx == 4):
		y[1] = 2.
	else:
		y[1] = y[0] * np.sin(line.vertex2.state[0] - line.vertex1.state[0])


# Grid and initial conditions obtained by running `julia tmp.julia`
grid = [
	[0, 1, 1, 0, 1],
	[1, 0, 1, 1, 0],
	[1, 1, 0, 1, 0],
	[0, 1, 1, 0, 1],
	[1, 0, 0, 1, 0]
]

theta0 = [0.8031776440871194, 1.076515586764153, 0.6863392710509373, 0.8031776440871194, 1.28131103515625]
p = [-1., 1.5, -1., -1., 1.5]


# Network
power_grid = ndxl.create_network(([], [[], [], [], [], [], [], []]))
nodes = [power_grid.create_vertex([theta0[i], 0.0], dynamics=swing, args=(p[i], 0.1)) for i in range(5)]

l_idx = 0
for i in range(5):
	for j in range(i, 5):
		if grid[i][j] == 1:
			K0 = 1.63 * np.sin(nodes[i].state[0] - nodes[j].state[0])
			nodes[j].create_edge(nodes[i], [1.63, K0], cb=power_line, args=(l_idx, 1.))
			l_idx += 1

def cb(net, t, t_eval, l_eval):
	t_eval.append(t)
	for line in net.edge_list:
		if np.abs(line.state[1]) >= line.params[1]:
			print(f"Line {line.params[0] + 1} tripped near {t=}")
			line.destroy()
		else:
			l_eval[line.params[0]].append(line.state[1])

power_grid.simulate(6., method=ndxl.methods.nystrom, h=1e-3, cb=cb)


# Plotting
len_t = len(power_grid.params[0])
for i in range(7):
	plt.plot(power_grid.params[0], power_grid.params[1][i] + [0] * (len_t - len(power_grid.params[1][i])), label="Line $" + str(i + 1) + "$")

plt.xlabel("Time, $t$")
plt.ylabel("Power flow, $A$")
plt.legend()
plt.savefig("cascfail.pdf", bbox_inches='tight')