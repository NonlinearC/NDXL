#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../include/ndxl.h"

const double get_ns()
{
	static struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	return ts.tv_sec + 1e-9 * ts.tv_nsec;
}

const double randdbl(const double low, const double high)
{
	static struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	srand(ts.tv_nsec ^ ts.tv_sec);
	return low + (high - low) * ((double)rand()) / ((double)RAND_MAX);
}

int kuramoto(NDXLVertex* vert, double* dtheta, const double* theta, double t, void* p)
{
	double w = *(double*)p;
	*dtheta = 0.0;
	for (NDXLEdge* edge = ndxlGetVertexFirstEdge(vert); edge; edge = ndxlGetVertexNextEdge(vert, edge))
		*dtheta += *ndxlEdgeState(edge) * sin(*ndxlGetVertexNeighbourState(vert, edge) - *theta);
	*dtheta = w + (*dtheta / (1.0 + ndxlGetVertexDegree(vert)));
	return 0;
}

int main(int argc, char** argv)
{
	ndxlInit();
	double t_set0, t_set1, t_sim0, t_sim1, set_mean, set_std, sim_mean, sim_std;
	FILE* f_bm = fopen("ndxl.csv", "w");
	const double simtimes[3] = { 10.0, 20.0, 30.0 };

	for (long n = 20, j = 0; n <= 2000; n *= 10, j++)
	{
		printf("Running %d-node networks\n", n);
		NDXLVertex** verts = (NDXLVertex**)malloc(n * sizeof(NDXLVertex*));
		double* omega = (double*)malloc(n * sizeof(double));

		for (long S = 1; S <= 10; S++)
		{
			const double s = 0.1 * S;
			printf("s=%f\n", s);
			double t_set[30], t_sim[30];
			set_mean = 0.0;
			sim_mean = 0.0;

			for (long rep = 0; rep < 30; rep++)
			{
				t_set0 = get_ns();
				NDXLNetwork* nwk = ndxlCreateNetwork(0);
				for (long i = 0; i < n; i++)
				{
					const double theta0 = randdbl(-PI, PI);
					omega[i] = randdbl(9.9, 10.1);
					verts[i] = ndxlCreateVertex(nwk, &theta0, 1, kuramoto, NULL, NULL, 0, NULL, omega + i);
				}
				
				const double K = 1.0;
				for (long i = 0; i < n; i++)
					for (long k = i + 1; k < n; k++)
						if (randdbl(0.0, 1.0) < s)
							ndxlCreateEdge(verts[i], verts[k], &K, 1, 0.0, false, NULL, NULL, NULL, 0, NULL, NULL);

				t_set1 = get_ns();
				t_set[rep] = t_set1 - t_set0;
				set_mean += t_set[rep];

				t_sim0 = get_ns();
				ndxlNetworkSimulateDopri(nwk, simtimes[j], 1e-6, 1e-3, NULL, 0, NULL);
				t_sim1 = get_ns();
				t_sim[rep] = t_sim1 - t_sim0;
				sim_mean += t_sim[rep];
				ndxlDestroyNetwork(&nwk);
			}
			
			set_std = 0.0;
			sim_std = 0.0;
			set_mean /= 30.0;
			sim_mean /= 30.0;
			double diff;
			for (long rep = 0; rep < 30; rep++)
			{
				diff = t_set[rep] - set_mean;
				set_std += (diff * diff);
				diff = t_sim[rep] - sim_mean;
				sim_std += (diff * diff);
			}
			set_std = sqrt(set_std / 30.0);
			sim_std = sqrt(sim_std / 30.0);
			
			fprintf(f_bm, "%f,%f,%f,%f\n", set_mean, set_std, sim_mean, sim_std);
		}
		free(verts);
		free(omega);
		printf("\n");
	}
	
	fclose(f_bm);
	return 0;
}