#include <assert.h>

#include "julia.h"

int julia(const double *x, int xres, const double *y, int yres, const double *c, 
		int flag, int maxIterations, int *iterations)
{
	int maxIterationCount = 0, i,j;

	double a, b, xgap, ygap, cr, ci, radius;
	int count;
	xgap = (x[1] - x[0]) / xres;
	ygap = (y[1] - y[0]) / yres;

	for (j = 0; j < yres; j++)
	{
		for (i = 0; i < xres; i++)    
		{
			/* pixel to coordinates */
			a = x[0] + i * xgap;
			b = y[0] + j * ygap;

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

			int *p = iterations + j*xres+i;

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

	return maxIterationCount;
}
