#include <stdio.h>
#include "tests.h"

double mytest (long n);

main(int argc, char *argv[])
{
  long ntests, n, i;
  double *V, result;
  
  ntests = init_tests(argc,argv);
  
  V = (double *) mymalloc(NTESTS*sizeof(double));
  n = atol(argv[N_STREAM_PARAM+1]);
  
  for(i=0; i<ntests; i++)
  {
    V[i] = mytest(n);     /* test and find chisquare value */
    next_stream();       
  }
  
#if defined(SPRNG_MPI)
  /* Get chisquare values from other processes in order to perform the
     Kolmogorov-Smirnov test                            */
  getKSdata(V,NTESTS);  
#endif
  
  if(proc_rank == 0)      /* Perform KS test on data from individual tests */
  {
    set_d_of_f(1);
    result = KS(V,NTESTS,chiF);
    result = KSpercent(result,NTESTS);
    printf("\t KS %% = %.2f\n\n", result*100.0);
  }
  
  free(V);
}



double mytest(long n)
{
  double temp;
  long actual[2]={0,0};
  double expected[2]={0.5, 0.5};
  long i;
  int temp2;
  
  for(i=0; i<n; i++)
  {
    temp = get_rn();
    if(temp > 0.5)
      actual[0]++;
    else
      actual[1]++;
  }
  
  return chisquare(actual,expected,n,2,&temp2);
}
