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

double xt ANSI_ARGS((double x));
double maxt ANSI_ARGS((long n, int t));
void set_t ANSI_ARGS((int t));
double *V2;

int xt_t = 1;


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
  int t;
  
  if(argc != N_STREAM_PARAM + 3)
  {
    fprintf(stderr,"USAGE: %s (... %d arguments)\n",argv[0], N_STREAM_PARAM+2);
    exit(1);
  }
  
  ntests = init_tests(argc,argv);
  
  n = atol(argv[N_STREAM_PARAM+1]);
  t = atoi(argv[N_STREAM_PARAM+2]);
  
  V = (double *) mymalloc(NTESTS*sizeof(double));
  V2 = (double *) mymalloc(n*sizeof(double));
  
  for(i=0; i<ntests; i++)
  {
    V[i] = maxt(n,t);
    
    next_stream();
  }
  
  set_KS_n(NTESTS);

#if defined(SPRNG_MPI)
  getKSdata(V,NTESTS);
#endif
  
  if(proc_rank == 0)
  {
    result = KS(V,NTESTS,KSF);
    printf("\nResult: KS value = %f", result);
    result = KSpercent(result,NTESTS);
    printf("\t %% = %.2f\n\n", result*100.0);
  }
  

#if defined(SPRNG_MPI)
     MPI_Finalize();
#endif

}


#ifdef __STDC__
double xt(double x)
#else
double xt(x)
double x;
#endif
{
  return pow(x,(double) xt_t);
}

#ifdef __STDC__
void set_t(int t)
#else
void set_t(t)
int t;
#endif
{
  xt_t = t;
}

#ifdef __STDC__
double maxt(long n, int t)
#else
double maxt(n, t)
int t;
long n;
#endif
{
  double *V=V2, temp;
  int j;
  long i;
  
  for(i=0; i<n; i++)
  {
    V[i] = 0.0;
    for(j=0; j<t; j++)
    {
      temp=get_rn();
      
      if( temp > V[i])
	V[i] = temp;
    }
  }
  
  set_t(t);
  temp = KS(V,n,xt);
  /*printf("\tKS for stream = %f, %% = %f\n", temp, KSpercent(temp,n));*/
  
  /*free(V);*/
  
  return temp;
}
