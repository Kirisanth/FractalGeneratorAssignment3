#include <assert.h>
#include <omp.h>
#include <stdio.h>

#include "julia.h"

int julia(const double *x, int xres, const double *y, int yres, const double *c, 
		int flag, int maxIterations, int *iterations)
{
	int maxIterationCount = 0;

	omp_set_num_threads(4);
	#pragma omp parallel reduction(max:maxIterationCount)
	{
		int nthreads, tid;
		tid = omp_get_thread_num();
		nthreads = omp_get_num_threads();
		printf("thread: %d, Max: %d\n", tid, nthreads);
	

		double a, b, xgap, ygap, cr, ci, radius;
		int count;

		//thread private variables
		int *p_iterations = iterations + ((yres/nthreads)* xres * tid);
		int t_yres = yres/nthreads + ((tid == nthreads - 1)? yres % nthreads : 0);
		double t_y0 = y[0] + (y[1] - y[0])*tid/nthreads;
		xgap = (x[1] - x[0]) / xres;
		ygap = (y[1] - y[0]) / yres;

		int i,j;
		for (j = 0; j < t_yres; j++)
		{
			for (i = 0; i < xres; i++)    
			{
				/* pixel to coordinates */
				a = x[0] + i * xgap;
				b = t_y0 + j * ygap;

				/* initial value for the iteration */
				cr = flag*c[0] + (1-flag)*a;
				ci = flag*c[1] + (1-flag)*b;

				radius = 0.0;
				count = 0;
				while ( radius <= 4.0 && count < maxIterations )
				{
					double temp_a = a;
					a = a * a - b * b + cr;
					b = 2.0f * temp_a * b + ci;
					radius = a * a + b * b;
					//radius = a_square + b_square;
					count++;

				}

				if(count > maxIterationCount )
					maxIterationCount = count;

				int *p = p_iterations + j*xres+i;

				/* If radius <= 4.0, we have hit maxIterations. The point is
				likely in the set. */
				if (radius <= 4.0)
				{
					assert(count==maxIterations);
					*p = 0;
				}
				else
				/* radius > 4.0. The point tends to infinity. We recognize
				this at iteration number count */
				{
					*p = count;
				}
			}
		}

	}

	return maxIterationCount;
}
