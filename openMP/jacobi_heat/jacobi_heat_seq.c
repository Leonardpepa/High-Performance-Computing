#include <stdio.h>
#include <math.h>
#include <omp.h>

#define maxsize 700
#define iterations 100
#define row 20
#define col 20
#define start 100
#define accuracy 50

int main(int argc, char* argv[])
{
int i, j, k;
double table1[maxsize][maxsize], table2[maxsize][maxsize];
double diff;

/* initialize both tables*/

for(i=0;i<maxsize;i++)
  for(j=0;j<maxsize;j++)
  {
    table1[i][j]=0;
    table2[i][j]=0;
  }

double starttime = omp_get_wtime();

/* repeate for each iteration */
for(k = 0; k < iterations; k++) 
{
  
  /* create a heat source */
  table1[row][col] = start;
  
  /* difference initialization */
   diff = 0.0;
   
  /* perform the calculations */
  for(i=1;i<maxsize-1;i++)
    for(j=1;j<maxsize-1;j++) {
      table2[i][j] = 0.25 *(table1[i-1][j] + table1[i+1][j] + table1[i][j-1] + table1[i][j+1]);
      diff += (table2[i][j]-table1[i][j])*(table2[i][j]-table1[i][j]);
    }

  /* print result */
  // for(i=0;i<maxsize;i++)
  // {
  //   for(j=0;j<maxsize;j++)
  //     printf("%5.0f ",table2[i][j]);
  //   printf("\n");
  // }
  // printf("\n");
 
  /* print difference and check convergence */
  diff = sqrt(diff);
  printf("diff = %3.25f\n\n", diff);

  if (diff < accuracy) {
	printf ("\n\nConvergence in %d iterations\n\n", k);
        break;
 }	
 
 /* copy new table to old table */ 
  for(i=0;i<maxsize;i++)
    for(j=0;j<maxsize;j++)
      table1[i][j]=table2[i][j];
}
double stoptime = omp_get_wtime();


printf("time elapsed %f\n", stoptime-starttime); 


return 0;
}
