#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "mpi.h"


void generate_list(int * x, int n) {
   int i;
   srand (time (NULL));
   for (i = 0; i < n; i++)
      x[i] = rand() % n; 
}

void print_list(int * x, int n) {
   int i;
   for (i = 0; i < n; i++) {
      printf("%d ",x[i]);
   } 
}

void merge(int * X, int n, int * tmp) {
   int i = 0;
   int j = n/2;
   int ti = 0;

   while (i<n/2 && j<n) { /* merge */
      if (X[i] < X[j]) {
         tmp[ti] = X[i];
         ti++; i++;
      } else {
         tmp[ti] = X[j];
         ti++; j++;
      }
   }
   
   while (i<n/2) { /* finish up lower half */
      tmp[ti] = X[i];
      ti++; i++;
   }
   
   while (j<n) { /* finish up upper half */
      tmp[ti] = X[j];
      ti++; j++;
   }

   memcpy(X, tmp, n*sizeof(int));
} 

void mergesort(int * X, int n, int * tmp)
{
   if (n < 2) return;

   mergesort(X, n/2, tmp); 
   mergesort(X+(n/2), n-(n/2), tmp);

   merge(X, n, tmp);
}


void main(int argc, char *argv[])
{
   int n;
   int *data, *tmp;
   double starttime, stoptime;
   int rank, size;

   if (argc != 2) {
      printf ("Usage : %s <list size>\n", argv[0]);
      exit(1);
   }
   
   MPI_Init(&argc,&argv);
   MPI_Comm_size(MPI_COMM_WORLD, &size);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   
   n = strtol(argv[1], NULL, 10);
   data = (int *) malloc (sizeof(int)*n);

   generate_list(data, n);

   if (rank == 0)
   {
      starttime = MPI_Wtime();
   }
   

   int chunk = n / size;
   int *local_array = malloc(chunk * sizeof(int));
   tmp = (int *) malloc (sizeof(int)*chunk);
   
   MPI_Scatter(data, chunk, MPI_INT, local_array, chunk, MPI_INT, 0, MPI_COMM_WORLD);

   mergesort(local_array, chunk, tmp);
   
   int* sorted;
   
   if (rank == 0)
   {
      sorted = (int *) malloc (sizeof(int)*n);
   }
   
   MPI_Gather(local_array, chunk, MPI_INT, sorted, chunk, MPI_INT, 0, MPI_COMM_WORLD);
   
   if (rank == 0)
   {  
      free(tmp);
      tmp = (int *) malloc (sizeof(int)*n);

      mergesort(sorted, n, tmp);
      stoptime = MPI_Wtime();
      print_list(sorted, n);
      printf("\ntime elapsed %f\n", ((double)stoptime-starttime)); 

      free(sorted);
      free(tmp);
   }
   
   free(local_array);
   free(data);

   MPI_Finalize();
}

