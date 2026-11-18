import matplotlib.pyplot as plt
import ndxl

# Store computed outputs from simulation
t_arr = []
v_prey = []
v_pred = []
def cb(net, t):
	global t_arr, v_prey, v_pred
	print(f"{t=}")
	t_arr.append(t)
	verts = net.vertices
	v_prey.append(verts[0].state)
	v_pred.append(verts[1].state)

# LV prey model
def lv_prey(vert, x, t, alpha, beta):
	y = list(vert.neighbours)[0].state
	return alpha * x - beta * x * y

# LV predator model
def lv_pred(vert, y, t, delta, gamma):
	x = list(vert.neighbours)[0].state
	return delta * x * y - gamma * y

if __name__ == '__main__':
	nwk = ndxl.create_network()

	# Prey vertex, alpha = 1.1, beta = 0.4
	v1 = nwk.create_vertex(10., func=lv_prey, args=(1.1, 0.4))

	# Predator vertex, delta = 0.1, gamma = 0.4
	v2 = nwk.create_vertex(10., func=lv_pred, args=(0.1, 0.4))

	# Couple the vertices
	e1 = v1.create_edge(v2)

	# Simulate and plot
	nwk.simulate_dopri(300., cb=cb)
	plt.plot(t_arr, v_prey, label="Prey")
	plt.plot(t_arr, v_pred, label="Predator")
	plt.xlabel("Time, $t$")
	plt.ylabel("Population, $N$")
	plt.legend(loc="upper left")
	plt.show()