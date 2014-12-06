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

void init_collision ANSI_ARGS((long n, int logmd, int logd));
long collision ANSI_ARGS((long n, int logmd, int logd));
double identity ANSI_ARGS((double d));
void compute_probability ANSI_ARGS((double *A, int n, int m));

#define MIN_CHISQ_NTESTS 50

int *array, *mask;
int arraysize, intsize;
double *probability;


#ifdef __STDC__
int main(int argc, char *argv[])
#else
int main(argc, argv)
int argc;
char *argv[];
#endif
{
  long ntests, n, i, *bins, ncollisions;
  double *V, result;
  int logmd, logd, size, Bins_used;
  
  if(argc != N_STREAM_PARAM + 4)
  {
    fprintf(stderr,"USAGE: %s (... %d arguments)\n",argv[0], N_STREAM_PARAM+3);
    exit(1);
  }

  ntests = init_tests(argc,argv);
  
  n = atol(argv[N_STREAM_PARAM+1]);
  logmd = atoi(argv[N_STREAM_PARAM+2]);
  logd = atoi(argv[N_STREAM_PARAM+3]);

  if(logmd*logd > 31)
  {
    fprintf(stderr,"ERROR: log(m-d)*log(d) = %d must be less than 32\n", 
	    logmd*logd);
    exit(1);
  }
  if( (1<<(logmd*logd)) < n)
  {
    fprintf(stderr,"ERROR: m = %d must be at least as high as n = %ld\n", 
	    (1<<(logmd*logd)), n);
    exit(1);
  }

  if( NTESTS < MIN_CHISQ_NTESTS)
    V = (double *) mymalloc(NTESTS*sizeof(double));
  else
  {
    bins = (long *) mymalloc((n+1)*sizeof(long));
    memset(bins,0,(n+1)*sizeof(long));
  }
  

  init_collision(n,logmd,logd);

  for(i=0; i<ntests; i++)
  {
    ncollisions = collision(n,logmd,logd);
    if( NTESTS < MIN_CHISQ_NTESTS)
      V[i] = probability[ncollisions];
    else
    {
      bins[ncollisions]++;
    }
    
    next_stream();
  }
  
#if defined(SPRNG_MPI)
  if( NTESTS < MIN_CHISQ_NTESTS) 
    getKSdata(V,NTESTS);
  else
    reduce_sum_long(bins,n+1);
#endif


  if(proc_rank == 0 && NTESTS < MIN_CHISQ_NTESTS )
  {
    result = KS(V,NTESTS,identity);
    printf("\nResult: KS value = %f", result);

    /*for(i=0; i<NTESTS; i++)
      printf("\tstream = %d, prob = %f\n", i, V[i]);*/

    result = KSpercent(result,NTESTS);
    printf("\t %% = %.2f\n\n", result*100.0);
  }
  else if(proc_rank == 0) 
  {
    printf("\n Please ignore any warning message about the effect of combining bins\n\n");
    
    result = chisquare(bins, probability, NTESTS, n+1, &Bins_used);
    printf("\nResult: Chi Square value = %f", result);

    /*for(i=0; i<n; i++)
      printf("\t# of collisions = %d, frequency = %ld\n", i, bins[i]);*/

    result = chipercent(result,Bins_used-1);
    printf("\t %% = %.2f\n\n", result*100.0);
  }
    
  free(mask);
  free(probability);
  free(array);
  if( NTESTS < MIN_CHISQ_NTESTS)
    free(V);
  else
    free(bins);

#if defined(SPRNG_MPI)
  MPI_Finalize();
#endif
  return 0;
}



#ifdef __STDC__
double identity(double d)
#else
double identity(d)
double d;
#endif
{
  return d;
}



#ifdef __STDC__
void init_collision(long n, int logmd, int logd)
#else
void init_collision(n,logmd,logd)
long n;
int logmd, logd;
#endif
{
  long m, size;
  int tempmask, i;
  
  intsize = sizeof(int)*8;	/*** Assumes 8 bit characters ****/
  m = (1<<(logmd*logd));
  size = m/intsize;
  if(m%intsize > 0) 
    size++;
  
  arraysize = size;
  array = (int *) mymalloc(arraysize*sizeof(int));
  mask = (int *) mymalloc(intsize*sizeof(int));
  
  tempmask = 1;
  for(i=0; i<intsize; i++)
  {
    mask[i] = tempmask;
    tempmask <<=1;
  }
  
  probability = (double *) mymalloc((n+1)*sizeof(double));
  
  compute_probability(probability, (int) n, m);
}



#ifdef __STDC__
void compute_probability(double *A, int n, int m)
#else
void compute_probability(A, n, m)
int n, m;
double *A;
#endif
{
  int i, j, j0, j1;
  
  for(j=0; j<=n; j++)
    A[j] = 0.0;
  
  A[1] = j0 = j1 = 1;
  
  for(i=0; i<n-1; i++)
  {
    j1++;
    for(j=j1; j>=j0; j--)
    {
      A[j] = (double) j/(double) m *A[j] + 
	( (1.0+1.0/(double)m) - (double)j/(double)m )*A[j-1];
      
      if( A[j] < 1.0e-20 )
      {
	A[j] = 0.0;
	if(j == j1)
	  j1--;
	else if(j == j0)
	  j0++;
      }
    }
  }
  
  
  if(NTESTS < MIN_CHISQ_NTESTS && sqrt((double)NTESTS/(double)(j1-j0+1)) > 0.5)
  {
    fprintf(stderr,"WARNING: Error in KS estimate may be ~ %f\n", 
	    sqrt((double)NTESTS/(double)(j1-j0+1)));
    if(NTESTS*n > 1.0e10) /* We cannot afford to waste large amounts ... */
    {			  /* ... of computer time */
      fprintf(stderr,"Exiting ... \n");
      exit(-1);
    }
  }
  
  
  if(NTESTS < MIN_CHISQ_NTESTS)
  {
    for(j=j1-1; j>=j0; j--)
      A[j] += A[j+1];

    if(A[j0] >= 1.0)
      A[j0] = 1.0 - 1.0e-7;	/* returning 1.0 confuses the KS test! */
  }
  
}



#ifdef __STDC__
long collision(long n, int logmd, int logd)
#else
long collision(n, logmd, logd)
int logmd, logd;
long n;
#endif
{
  long m, i, ncollisions;
  int d, j, index, bit;
  unsigned int temp, num;
  
  d = 1<<logd;
  m = 1<<(logmd*logd);
  
  ncollisions = 0;
  memset(array,0,arraysize*sizeof(int));
  
  for(i=0; i<n; i++)
  {
    temp = 0;
    for(j=0; j<logmd; j++)
    {
      num = d*get_rn();
      temp <<= logd;
      temp |= num;
    }
    
    index = temp/intsize;
    bit = temp%intsize;
    
    if(array[index]&mask[bit])
      ncollisions++;
    else
      array[index] |= mask[bit];
  }
  
  /*printf("ncollisions = %ld, probability = %f\n", ncollisions, probability[n-ncollisions]);*/
  
  return n-ncollisions;
}
