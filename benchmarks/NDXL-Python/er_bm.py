import ndxl
import numpy as np
import time

def kuramoto(vert, y, t, w):
	return w + np.sum([e.state * np.sin(vert.neighbour_state(e) - y) for e in vert.edges]) / (1. + vert.degree)

simtimes = (10., 20., 30.)
f_bm = open("ndxlpy.csv", 'w')

for j, n in enumerate((20, 200, 2000)):
	print(f"Simulating {n}-node networks")
	for s in np.arange(0.1, 1.1, 0.1):
		print(f"{s=}")
		t_set = []
		t_sim = []

		for rep in range(30):
			t_set0 = time.perf_counter_ns()
			theta0 = np.random.uniform(-np.pi, np.pi, n)
			omega0 = np.random.uniform(9.9, 10.1, n)
			verts = np.empty(n, dtype=ndxl.vertex)
			nwk = ndxl.create_network()
			for i in range(n):
				verts[i] = nwk.create_vertex(theta0[i], dynamics=kuramoto, args=omega0[i])
			for i in range(n):
				for k in range(i + 1, n):
					if np.random.uniform(0., 1.) < s:
						verts[i].create_edge(verts[k], 1.)
			t_set1 = time.perf_counter_ns()
			t_set.append(t_set1 - t_set0)
			t_sim0 = time.perf_counter_ns()
			nwk.simulate(simtimes[j])
			t_sim1 = time.perf_counter_ns()
			t_sim.append(t_sim1 - t_sim0)
			nwk.destroy()

		f_bm.write(f"{np.mean(t_set) * 1e-9},{np.std(t_set) * 1e-9},{np.mean(t_sim) * 1e-9},{np.std(t_sim) * 1e-9}\n")
	print()
f_bm.close()