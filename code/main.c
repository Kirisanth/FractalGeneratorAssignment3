#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>

#include "julia.h"

int main(int argc, char *argv[])
{
	int width, height, maxiter, flag;
	double x[2], y[2], c[2];
	char *image, *stats;
	clock_t start_time_c;
	time_t start_time_t;
	start_time_c = clock();
	start_time_t = time(NULL);

	getParams(argv, &flag, c, x, y, &width, &height, &maxiter, &image, &stats);

	int *iterations = (int*)malloc( sizeof(int) * width * height );
	assert(iterations);

	/* compute set */
	
	printf("\n");

	int maxCount = julia(x, width, y, height, c, flag, maxiter, iterations);
	printf("~max iterations hit: %d /%d\n", maxCount, maxiter);

	printf("Elapsed Time (clock): %f\n", ((double)(clock()-start_time_c))/CLOCKS_PER_SEC);
	printf("Elapsed Time (time): %d\n\n", (int)(time(NULL)-start_time_t));

	/* save our picture for the viewer */
	saveBMP(image, iterations, width, height);

	free(iterations);
	return 0;
}