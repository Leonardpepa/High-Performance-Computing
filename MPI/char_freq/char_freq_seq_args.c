#include <stdio.h> 
#include <stdlib.h> 
#include <time.h>

#define N 128
#define base 0

int main (int argc, char *argv[]) {
	
	FILE *pFile;
	long file_size;
	char * buffer;
	char * filename;
	size_t result;
	int i, j, freq[N];

	if (argc != 2) {
		printf ("Usage : %s <file_name>\n", argv[0]);
		return 1;
	}

	filename = argv[1];
	pFile = fopen ( filename , "rb" );

	if (pFile==NULL) {
		printf ("File error\n");
		return 2;
	}

	// obtain file size:
	fseek (pFile , 0 , SEEK_END);
	file_size = ftell (pFile);
	rewind (pFile);
	printf("file size is %ld\n", file_size);

	// allocate memory to contain the file:
	buffer = (char*) malloc (sizeof(char)*file_size);
	if (buffer == NULL) {
		printf ("Memory error\n");
	 	return 3;
	}

	// copy the file into the buffer:
	result = fread (buffer,1,file_size,pFile);
	if (result != file_size) {
		printf ("Reading error\n");
		return 4;
	} 

	for (j=0; j<N; j++){
		freq[j]=0;
	}

	double starttime = clock();

	for (i=0; i<file_size; i++){
		freq[buffer[i] - base]++;
	}		
	
	double stoptime = clock();  

	for (j=0; j<N; j++){
		printf("%d = %d\n", j+base, freq[j]);
	}	
	
	printf("time elapsed %f\n", ((double)stoptime-starttime) / CLOCKS_PER_SEC); 

	fclose (pFile);
	free (buffer);

	return 0;
}
