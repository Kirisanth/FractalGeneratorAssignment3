/* julia_omp.c
 * CS 4F03 Assignmnet 3
 * Brandon Byskov, 1068517
 * Kirisanth Subramaniam, 1054839
 * Mitch Errgyrs, 1137537
 * Ramy Abdul Samad, 1152051
 */

//#include <assert.h>
#include <omp.h>
#include <stdio.h>

#include "julia.h"

int julia(const double *x, int xres, const double *y, int yres, const double *c, 
		int flag, int maxIterations, int *iterations)
{
	int maxIterationCount = 0;
	//uses reduction "bitwise or" instead of max becasue because school compilers are too old to support max
	#pragma omp parallel reduction(|:maxIterationCount)
	{
		int nthreads, tid;
		double a, b, xgap, ygap, cr, ci, a_square, b_square; 
		int count,i,j;
		int* p;
		tid = omp_get_thread_num();
		nthreads = omp_get_num_threads();
		if (tid == 0)
			printf("Threads: %d\n", nthreads);
	
		xgap = (x[1] - x[0]) / xres;
		ygap = (y[1] - y[0]) / yres;

		if (flag) {
			cr = c[0];
			ci = c[1];
		} else {
			cr = 0.0;
			ci = 0.0;
		}

		for (j = tid; j < yres; j += nthreads)
		{
			for (i = 0; i < xres; i++)    
			{
				/* pixel to coordinates */
				a = x[0] + i * xgap;
				b = y[0] + j * ygap;

				/* initial value for the iteration */
				if (!flag) {
					cr = a;
					ci = b;
				}

				count = 0;
				a_square = a*a;
				b_square = b*b;
				while ( a_square + b_square <= 4.0 && count < maxIterations )
				{				
					b = 2.0f * a * b + ci;
					a = a_square - b_square + cr;
					a_square = a*a;
					b_square = b*b;
					
					count++;
				}

				if(count > maxIterationCount )
					maxIterationCount = count;

				p = iterations + j*xres+i;

				/* If radius <= 4.0, we have hit maxIterations. The point is
				likely in the set. */
				if (a_square + b_square <= 4.0)
				{
					//assert(count==maxIterations);
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

	// take the minimum of the possible number of iterations, because the school
	// uses an old gcc compiler, we can't use reduction(max), and we might overestimate
	// maxIterationCount with our workaround. See the #pragma omp declaration.
	return (maxIterationCount <= maxIterations)? maxIterationCount : maxIterations;
}
