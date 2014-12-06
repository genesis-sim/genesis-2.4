#include <stdio.h>
#include "tests.h"
#include <math.h>

#ifndef ANSI_ARGS
#ifdef __STDC__
#define ANSI_ARGS(args) args
#else
#define ANSI_ARGS(args) ()
#endif
#endif

void init_poker ANSI_ARGS((long n, int k, int d));
double poker ANSI_ARGS((long n, int k, int d));
double stirling ANSI_ARGS((int n, int m));

static int ncatagories = 0, *bins, *index, Bins_used=0;
long *actual;
double *probability;


#ifdef __STDC__
main(int argc, char *argv[])
#else
main(argc, argv)
int argc;
char *argv[];
#endif
{
  long ntests, n, i;
  double *V, result;
  int k, d;
  
  if(argc != N_STREAM_PARAM + 4)
  {
    fprintf(stderr,"USAGE: %s (... %d arguments)\n",argv[0], N_STREAM_PARAM+3);
    exit(1);
  }
  
  ntests = init_tests(argc,argv);
  
  V = (double *) mymalloc(NTESTS*sizeof(double));
 
  n = atol(argv[N_STREAM_PARAM+1]);
  k = atoi(argv[N_STREAM_PARAM+2]);
  d = atoi(argv[N_STREAM_PARAM+3]);
  
  init_poker(n,k,d);
  
  for(i=0; i<ntests; i++)
  {
    V[i] = poker(n,k,d);
    
    next_stream();
  }
  
#if defined(SPRNG_MPI)
  getKSdata(V,NTESTS);
#endif
  
  if(proc_rank == 0)
  {
    set_d_of_f(Bins_used-1);
    result = KS(V,NTESTS,chiF);
    printf("\nResult: KS value = %f", result);
    result = KSpercent(result,NTESTS);
    printf("\t %% = %.2f\n\n", result*100.0);
  }
  
  free(V);
  free(bins);
  free(actual);
  free(probability);
  free(index);

#if defined(SPRNG_MPI)
     MPI_Finalize();
#endif

}


#ifdef __STDC__
void init_poker(long n, int k, int d)
#else
void init_poker(n,k,d)
long n;
int k, d;
#endif
{
  int i;
  double *pr, temp;
  long sum;
  
  bins = (int *) mymalloc(d*sizeof(int));
  index = (int *) mymalloc((k+1)*sizeof(int));
  pr = (double *) mymalloc((k+1)*sizeof(double));
  temp = pow((double) d, - (double) k);
  
  for(i=1; i<=k; i++)
  {
    temp *= d-i+1;
    pr[i] = temp*stirling(k,i);
  }
  
  ncatagories = 0;
  sum = 0;
  for(i=1; i<=k; i++)
  {
    index[i] = ncatagories;
    sum += n*pr[i];
    
    if(sum > 5 && i < k)
    {
      sum = 0;
      ncatagories++;
    }
  }
  
  ncatagories++;
  
  actual = (long *) mymalloc(ncatagories*sizeof(long));
  probability = (double *) mymalloc(ncatagories*sizeof(double));
  for(i=0; i< ncatagories; i++)
    probability[i] = 0.0;
  
  for(i=1; i<=k; i++)
    probability[index[i]] += pr[i];
  
  free(pr);
}

#ifdef __STDC__
double poker(long n, int k, int d)
#else
double poker(n, k, d)
int k, d;
long n;
#endif
{
  double temp;
  int j, sum, temp2;
  long i;
  
  memset(actual, 0, ncatagories*sizeof(long));
  
  for(i=0; i<n; i++)
  {
    memset(bins,0,d*sizeof(int));
    
    for(j=0; j<k; j++)
    {
      bins[(int) (d*get_rn())] = 1;
    }
    
    sum = 0;
    for(j=0; j<d; j++)
      sum += bins[j];
    
    actual[index[sum]]++;
  }
  
  temp = chisquare(actual,probability,n, ncatagories, &Bins_used);
  /*printf("\tChisquare for stream = %f, %% = %f\n", temp, 
	 chipercent(temp,ncatagories-1));*/
  
  return temp;
}
