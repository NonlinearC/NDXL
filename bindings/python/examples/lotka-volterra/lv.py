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


# Function definitions
def lv_prey(prey, N_prey, t, alpha, beta):
	N_pred = prey.neighbour_state(prey.first_edge)
	return alpha * N_prey - beta * N_prey * N_pred

def lv_pred(pred, N_pred, t, delta, gamma):
	N_prey = pred.neighbour_state(pred.first_edge)
	return delta * N_prey * N_pred - gamma * N_pred


# Create and simulate network
nwk = ndxl.create_network(([], [], []))
v_prey = nwk.create_vertex(40, dynamics=lv_prey, args=(1., 0.1))
v_pred = nwk.create_vertex(9, dynamics=lv_pred, args=(0.075, 1.5))
v_prey.create_edge(v_pred)

def cb(net, t, t_eval, prey_pop, pred_pop):
	t_eval.append(t)
	prey_pop.append(net.vertex_list[0].state[0])
	pred_pop.append(net.vertex_list[1].state[0])
nwk.simulate(20., method="rkck", cb=cb)


# Plotting
plt.figure(figsize=(6, 3))
plt.plot(nwk.params[0], nwk.params[1], marker='o', ms=5, lw=0.5, c='b', label="Prey")
plt.plot(nwk.params[0], nwk.params[2], marker='*', ms=5, lw=0.5, c="dodgerblue", label="Predator")
plt.xlabel("Time, $t$")
plt.ylabel("Population, $N$")
plt.legend(loc="upper right")
plt.tight_layout()
plt.savefig("lotka-volterra.pdf", bbox_inches='tight')