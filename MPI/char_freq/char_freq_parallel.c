#include <stdio.h> 
#include <stdlib.h> 
#include "mpi.h"

#define N 128
#define base 0

int main (int argc, char *argv[]) {
	
	FILE *pFile;
	long file_size;
	char * buffer;
	char * filename;
	size_t result;
	int freq[N];
    int size, rank;
    double starttime, stoptime;


	if (argc != 2) {
		printf ("Usage : %s <file_name>\n", argv[0]);
		return 1;
	}

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    for (int j=0; j<N; j++){
		freq[j]=0;
	}

	filename = argv[1];
	pFile = fopen ( filename , "rb" );

	if (pFile==NULL) {
		printf ("File error\n");
	    MPI_Finalize();
		return 2;
	}

	// obtain file size:
	fseek (pFile , 0 , SEEK_END);
	file_size = ftell (pFile);
	rewind (pFile);

    if (rank == 0){
    	starttime = MPI_Wtime();
    }

	int local_freq[N];

    for (int j=0; j<N; j++){
		local_freq[j]=0;
	}
    
    int chunk = file_size / size;
    int extra = file_size % size;
    int start = rank * chunk;
    int stop = start + chunk;
    
    // last one
    if (rank == size - 1) stop += extra;

    int count = stop - start;

    // allocate memory to contain the file:
	buffer = (char*) malloc (sizeof(char)*count);
	
    fseek(pFile, start, SEEK_SET);	
    result = fread(buffer, 1, count, pFile);
	
	if (result != count)
	{
		printf ("File error\n");
	    MPI_Finalize();
		return 2;
	}
	
	for (int i=0; i<count; i++){
		local_freq[buffer[i] - base]++;
	}

    if (rank == 0) {
        stoptime = MPI_Wtime();
    }

    MPI_Reduce(local_freq, freq, N, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

	if (rank == 0){
        for (int j=0; j<N; j++){
		    printf("%d = %d\n", j+base, freq[j]);
	    }    

	    printf("time elapsed %f\n", ((double)stoptime-starttime)); 
    }
    
	fclose (pFile);
	free (buffer);

    MPI_Finalize();

	return 0;
}
