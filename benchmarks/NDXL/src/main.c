#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define NDXL_STATIC

#include "../include/ndxl.h"

const double randdbl(const double low, const double high)
{
	double time = uniTime();
	size_t sec = (size_t)time;
	size_t nsec = (size_t)(1e9 * (time - sec));
	srand(sec ^ nsec);
	return low + (high - low) * ((double)rand()) / ((double)RAND_MAX);
}

int kuramoto(NDXLVertex* vert, double* dtheta, const double* theta, double t, void* p)
{
	double w = *(double*)p;
	*dtheta = 0.0;
	NDXLEdge* edge;
	for (size_t i = 0; i < ndxlGetVertexDegree(vert); i++)
	{
		edge = ndxlVertexEdge(vert, i);
		*dtheta += *ndxlEdgeState(edge) * sin(*ndxlGetVertexNeighbourState(vert, edge) - *theta);
	}
	*dtheta = w + (*dtheta / (1.0 + ndxlGetVertexDegree(vert)));
	return 0;
}

int main(int argc, char** argv)
{
	double t_set0, t_set1, t_sim0, t_sim1, set_mean, set_std, sim_mean, sim_std;
	FILE* f_bm = fopen("ndxl.csv", "w");
	const double simtimes[3] = { 10.0, 20.0, 30.0 };

	for (long n = 20, j = 0; n <= 200; n *= 10, j++)
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
				t_set0 = uniTime();
				NDXLNetwork* nwk = ndxlCreateNetwork(0);
				for (long i = 0; i < n; i++)
				{
					const double theta0 = randdbl(-PI, PI);
					omega[i] = randdbl(9.9, 10.1);
					verts[i] = ndxlCreateVertex(nwk, &theta0, 1, kuramoto, NULL, NULL, 0, NULL, 0.0, 0.0, omega + i);
				}
				
				const double K = 1.0;
				for (long i = 0; i < n; i++)
					for (long k = i + 1; k < n; k++)
						if (randdbl(0.0, 1.0) < s)
							ndxlCreateEdge(verts[i], verts[k], &K, 1, 0.0, 0, NULL, NULL, NULL, 0, NULL, 1e-6, 1e-3, NULL);

				t_set1 = uniTime();
				t_set[rep] = t_set1 - t_set0;
				set_mean += t_set[rep];

				t_sim0 = uniTime();
				ndxlNetworkSimulateDopri(nwk, simtimes[j], 1e-6, 1e-3, NULL, 0, NULL);
				t_sim1 = uniTime();
				t_sim[rep] = t_sim1 - t_sim0;
				sim_mean += t_sim[rep];
				ndxlDestroyNetwork(nwk);
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