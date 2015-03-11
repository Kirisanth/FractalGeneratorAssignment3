#include <assert.h>
#ifdef _OPENACC
#include <openacc.h>
#endif

#include "julia.h"

int julia(const double* restrict x, int xres, const double* restrict y, int yres, const double* restrict c, 
		int flag, int maxIterations, int* restrict iterations)
{
	int maxIterationCount = 0;

	double a, b, xgap, ygap, cr, ci, radius, temp_a;
	int count;
	xgap = (x[1] - x[0]) / xres;
	ygap = (y[1] - y[0]) / yres;
	#pragma acc data copyin(c[0:2]) copy(iterations[0:xres*yres])
	#pragma acc parallel loop reduction(max:maxIterationCount) private(flag, xgap, ygap)
	for (int j = 0; j < yres; j++)
	{
		for (int i = 0; i < xres; i++)    
		{
			/* pixel to coordinates */
			a = x[0] + i * xgap;
			b = y[0] + j * ygap;

			/* initial value for the iteration */
			cr = flag*c[0] + (1-flag)*a;
			ci = flag*c[1] + (1-flag)*b;

			radius = 0.0;
			count = 0;
			//#pragma acc loop seq
			while ( radius <= 4.0 && count < maxIterations )
			{
				temp_a = a;
				a = a * a - b * b + cr;
				b = 2.0f * temp_a * b + ci;
				radius = a * a + b * b;
				//radius = a_square + b_square;
				count++;
			}

			if(count > maxIterationCount )
				maxIterationCount = count;

			int *p = iterations + j*xres+i;

			/* If radius <= 4.0, we have hit maxIterations. The point is
			likely in the set. */
			if (radius <= 4.0)
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
	return maxIterationCount;
}
