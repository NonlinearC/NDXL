from scipy.integrate import solve_ivp

import ndxl
import numpy as np
import time

def kuramoto(t, y, w, K):
    return w + np.sum(K * np.sin(y[None, :] - y[:, None]), axis=1) / (1. + np.count_nonzero(K, axis=1))

simtimes = (10., 20., 30.)
f_bm = open("scipy.csv", 'w')

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
			coupling = np.zeros((n, n), dtype=float)
			for i in range(n):
				for k in range(i + 1, n):
					if np.random.uniform(0., 1.) < s:
						coupling[i, k] = 1.
						coupling[k, i] = 1.
			t_set1 = time.perf_counter_ns()
			t_set.append(t_set1 - t_set0)
			t_sim0 = time.perf_counter_ns()
			solve_ivp(kuramoto, (0., simtimes[j]), theta0, args=(omega0, coupling))
			t_sim1 = time.perf_counter_ns()
			t_sim.append(t_sim1 - t_sim0)

		f_bm.write(f"{np.mean(t_set) * 1e-9},{np.std(t_set) * 1e-9},{np.mean(t_sim) * 1e-9},{np.std(t_sim) * 1e-9}\n")
	print()
f_bm.close()