/*********************************************************************
       Equidistribution Test
*********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "tests.h"

/* # of parameters for the test engin */
#define NUM_TEST_ENGIN_PARAM 2
/* # of divisions between [0,1) */
static long    numDiv;
/* # of random numbers being tested */
static long    numRanNum;
/* # of tests repeated */
static long    numRepeat;
/* Array of bins */
static long    *bins;
/* Array of corresponding probabilities */
static double  *probs;
/* Array of chi-squares */
static double  *chiSqrs;

/********************************************************************/
#define FATAL_ABORT printf("Program terminated.\n"); exit(0)

/*------------------------------------------------------------------*/
#ifdef __STDC__
void initTest(int argc, char *argv[]) {
#else
void initTest(argc, *argv[]) int argc; char *argv[]; {
#endif
   int     numParam=NUM_TEST_ENGIN_PARAM+N_STREAM_PARAM;
   long    index;
   double  temp;

   if (argc<(numParam+1)) {
      printf("Error: %i number of parameters needed\n", numParam);
      FATAL_ABORT;
   }

   numDiv = atol(argv[N_STREAM_PARAM+1]);
   numRanNum = atol(argv[N_STREAM_PARAM+2]);
   if ((numDiv<=0) || (numRanNum<=0)) {
      printf("Error: incorrect parameter value(s)\n");
      FATAL_ABORT;
   }
   numRepeat = init_tests(argc, argv);

   bins = mymalloc(sizeof(long)*numDiv);
   probs = mymalloc(sizeof(double)*numDiv);
   chiSqrs = mymalloc(sizeof(double)*NTESTS);

   temp = 1.0 / numDiv;
   for (index=0;index<numDiv;index++) probs[index] = temp;
}

/*------------------------------------------------------------------*/
#ifdef __STDC__
void deinitTest(void) {
#else
void deinitTest() {
#endif
   free(bins);
   free(probs);
   free(chiSqrs);
}

/*------------------------------------------------------------------*/
#define PROC_ONE_NUMBER {                                       \
   long  binIndex;                                              \
                                                                \
   binIndex = get_rn() * numDiv;                                \
   bins[binIndex]++;                                            \
}

/********************************************************************/
#ifdef __STDC__
void main(int argc, char *argv[]) {
#else
void main(argc, argv) int argc; char *argv[]; {
#endif
   long  curRound, index;
   double  KSvalue, KSprob;
   int temp, Bins_used;
   
   initTest(argc, argv);
   for (curRound=0;curRound<numRepeat;curRound++) {
      for (index=0;index<numDiv;index++) bins[index] = 0;
      for (index=0;index<numRanNum;index++) PROC_ONE_NUMBER;

      chiSqrs[curRound] = chisquare(bins, probs, numRanNum, numDiv, &Bins_used);
      /*printf("\tChisquare for stream = %f, %% = %f\n",chiSqrs[curRound] , 
	 chipercent(chiSqrs[curRound],numDiv-1));*/
      next_stream();
   }

  
#if defined(SPRNG_MPI)
   getKSdata(chiSqrs,NTESTS);
#endif
  
  if(proc_rank == 0)
  {
    set_d_of_f(Bins_used-1);
    KSvalue = KS(chiSqrs, NTESTS, chiF);
    KSprob = KSpercent(KSvalue, NTESTS);

    printf("Result: KS value = %f\n", KSvalue);
    printf("        KS value prob = %.2f %%\n\n", KSprob*100.0);
 }
   
   deinitTest();

#if defined(SPRNG_MPI)
     MPI_Finalize();
#endif

}
