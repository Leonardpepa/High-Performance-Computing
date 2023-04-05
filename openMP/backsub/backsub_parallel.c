#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <omp.h>

void main ( int argc, char *argv[] )  {
	int   i, j, N;
	double *x, *b, **a, sum;
	char any;

	if (argc != 2) {
		printf ("Usage : %s <matrix size>\n", argv[0]);
				exit(1);
	}
	N = strtol(argv[1], NULL, 10);

	/* Allocate space for matrices */
	a = (double **) malloc ( N * sizeof ( double *) );
	for ( i = 0; i < N; i++) 
		a[i] = ( double * ) malloc ( (i+1) * sizeof ( double ) );
	b = ( double * ) malloc ( N * sizeof ( double ) );
	x = ( double * ) malloc ( N * sizeof ( double ) );

	/* Create doubles between 0 and 1. Diagonal elents between 2 and 3. */
	srand ( time ( NULL));
	for (i = 0; i < N; i++) {
		x[i] = 0.0;
		b[i] = (double)rand()/(RAND_MAX*2.0-1.0);
		a[i][i] = 2.0+(double)rand()/(RAND_MAX*2.0-1.0);
		for (j = 0; j < i; j++) 
			a[i][j] = (double)rand()/(RAND_MAX*2.0-1.0);;
	} 

	double starttime = omp_get_wtime();
	for (i = 0; i < N; i++) {
		sum = 0.0;
		#pragma omp parallel for num_threads(16) reduction(+:sum) shared(a, x, i)
		for (j = 0; j < i; j++) {
			sum = sum + (x[j] * a[i][j]);
		}
		x[i] = (b[i] - sum) / a[i][i];
	}
	double stoptime = omp_get_wtime();

	/* Print result */
	// for (i = 0; i < N; i++) {
	// 	for (j = 0; j <= i; j++)
	// 		printf ("%lf \t", a[i][j]);	
	// 	printf ("%lf \t%lf\n", x[i], b[i]);
	// }

	/* Check result */
	for (i = 0; i < N; i++) {
		sum = 0.0;
		for (j = 0; j <= i; j++) 
			sum = sum + (x[j]*a[i][j]);	
		if (fabsf(sum - b[i]) > 0.00001) {
			printf("%lf != %lf\n", sum, b[i]);
			printf("Validation Failed...\n");
		}
	}

	printf("time elapsed %lf\n", stoptime-starttime); 

}
