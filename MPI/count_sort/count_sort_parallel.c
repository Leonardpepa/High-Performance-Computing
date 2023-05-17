#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

#define UPPER 1000
#define LOWER 0

int main(int argc, char *argv[])
{
    int *x, *y;
    int* indexes;
    int rank, size;
    double starttime;
    double stoptime;

    if (argc != 2) {
        printf ("Usage : %s <array_size>\n", argv[0]);
        return 1;
    }

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    int n = strtol(argv[1], NULL, 10);

    x = ( int * ) malloc ( n * sizeof ( int ) );
    y = ( int * ) malloc ( n * sizeof ( int ) );

    if (rank == 0){
        for (int i=0; i<n; i++){
            // x[i] = n - i;
            x[i] = (rand() % (UPPER - LOWER + 1)) + LOWER;
        
        }
        starttime = MPI_Wtime();   
    }
    
    int chunk = n / size;
    int start = rank * chunk;
    int stop = start + chunk;
    int recv_count = stop-start;
    
    int* local_indexes = (int*)malloc(sizeof(int)*(stop-start));

    if (local_indexes == NULL) {
        printf ("Something went wrong\n");
        MPI_Finalize();
        exit(1);
    }

    MPI_Bcast(x, n, MPI_INT, 0, MPI_COMM_WORLD);
    
    for (int j=start; j<stop; j++) {

        int my_num = x[j];
        int my_place = 0;
        
        for (int i=0; i<n; i++){
            if ((my_num > x[i]) || ((my_num == x[i]) && (j < i))){
                my_place++;
            } 
        }
        local_indexes[j - start] = my_place;
    }

    if (rank == 0){
        indexes = (int*)malloc(sizeof(int)*n);
        if (indexes == NULL) {
            printf ("Something went wrong\n");
            MPI_Finalize();
            exit(1);
        }

        MPI_Gather(local_indexes, recv_count, MPI_INT, indexes, recv_count, MPI_INT, 0, MPI_COMM_WORLD);
        
        for(int i=0; i<n; i++){
            y[indexes[i]] = x[i];
        }

        stoptime = MPI_Wtime();
       
        for (int i=0; i<n; i++){
            printf("%d\n", y[i]);
        }
        printf("time elapsed %f\n", ((double)stoptime-starttime)); 

    }else{
        MPI_Gather(local_indexes, recv_count, MPI_INT, indexes, recv_count, MPI_INT, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();


    return 0;
}
