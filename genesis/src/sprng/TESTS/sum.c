/*  Note: This test is not in the regular SPRNG test format */


#include <stdio.h>
#if defined(SPRNG_MPI)
#include "mpi.h"
#endif
#include "tests.h"
#include <math.h>

#define min(a,b) ((a)<(b))?(a):(b)
#define max(a,b) ((a)>(b))?(a):(b)

double test_normal(double *array, int n);
void set_normal_params(double mu, double sd);
double normalF(double x);
void mean_sd(double *x, int n, double *mean, double *sd);

int group_size;

main(int argc, char *argv[])
{
  int ntests, n, *stream, i, j, k;
  double result, rn, *temparray, *sumarray;


  /****************** Initialization values *******************************/

  if(argc != N_STREAM_PARAM+3 || atoi(argv[2]) != 1)
  {
    fprintf(stderr,"USAGE: %s nstreams 1 seed param 1 0 n group_size\n",
	    argv[0]);
    exit(-1);
  }

  ntests = init_tests(argc,argv);
  
  n = atoi(argv[N_STREAM_PARAM+1]);
  group_size = atoi(argv[N_STREAM_PARAM+2]);
  
  temparray = (double *) mymalloc(n*sizeof(double));
  sumarray = (double *) mymalloc(n*sizeof(double));
  
  for(j=0; j<n; j++)
    temparray[j] = 0.0;
  
  for(i=0; i<ntests; i++)	/* sum elements from each sequence */
  {
    for(j=0; j<n; j++)
      for(k=0; k<group_size; k++)
      {
	temparray[j] += get_rn();
      }
    next_stream();
  }
  
#if defined(SPRNG_MPI)
  MPI_Reduce(temparray,sumarray, n, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
  free(temparray);
  temparray = sumarray;
  
#endif

  if(proc_rank == 0)
  {
    result = test_normal(temparray,n);
    printf("KS Percent =  %f %%\n", result*100);
  }
  
  free(temparray);

#if defined(SPRNG_MPI)
     MPI_Finalize();
#endif

}




double test_normal(double *array, int n)
{
  double mean1, sd1, mean2, sd2, result, percent;
  int i;
  double result_min, result_max, error;
  
  mean2 = NTESTS*(group_size/2.0);
  sd2 = sqrt( (double) NTESTS*(group_size/12.0) );
  for(i=0; i<n; i++)
    array[i] = (array[i] - mean2)/sd2;
  
  mean_sd(array,n,&mean1,&sd1);
  printf("\tMean = %f\tstandard deviation = %f\n", mean1, sd1);
  
  set_normal_params(0.0, 1.0);
  result = KS(array,n,normalF);

  /* State error in using the normal distribution instead of the exact      */
  /* distribution. Note that the formula used has been empirically derived, */
  /* and verified only for NTESTS*groupsize <= 10^5                         */
  error = 0.0275/NTESTS/group_size*sqrt((double) n);
  printf("\tKS range = [%f, %f]%%\n", 100*KSpercent(max(0.0,result-error),n),
	 100*KSpercent(min(sqrt((double) n),result+error),n) );
  
  percent = KSpercent(result,n);

  return percent;
}

