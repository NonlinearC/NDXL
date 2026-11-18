import ndxl
import numpy as np

if __name__ == '__main__':
	# Numerical
	nwk = ndxl.create_network()
	v1 = nwk.create_vertex([[3-5j, 1+2j], [2j, 4]], func=lambda v, y, t: y * t)
	print(v1.state)
	nwk.simulate_dopri(5.)
	print(v1.state)

	# Analytical
	t_final = 5.
	A = np.array([[3-5j, 1+2j], [2j, 4]])
	y = A * np.exp(0.5 * t_final * t_final)
	print("Analytical:\n", y)