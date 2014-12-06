/*********************************************************************
       Permutation Test
*********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "tests.h"

/* # of parameters for the test engin */
#define NUM_TEST_ENGIN_PARAM 2
/* Size of each group */
static int     grpSize;
/* # of groups in each test */
static long    numGrp;
/* # of bins */
static long    numBin;
/* # of tests repeated */
static long    numRepeat;
/* One group of random numbers */
static double  *oneGrp;
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
void initTest(argc, argv) int argc; char *argv[]; {
#endif
   int  numParam=NUM_TEST_ENGIN_PARAM+N_STREAM_PARAM;
   long  index;
   double  temp;

   if (argc<(numParam+1)) {
      printf("Error: %i number of parameters needed\n", numParam);
      FATAL_ABORT;
   }

   grpSize = atoi(argv[N_STREAM_PARAM+1]);
   numGrp = atol(argv[N_STREAM_PARAM+2]);
   if ((grpSize<=0) || (numGrp<=0)) {
      printf("Error: incorrect parameter value(s)\n");
      FATAL_ABORT;
   }

   for (numBin=index=1;index<=grpSize;index++) numBin *= index;
   numRepeat = init_tests(argc, argv);

   oneGrp = mymalloc(sizeof(double)*grpSize);
   bins = mymalloc(sizeof(long)*numBin);
   probs = mymalloc(sizeof(double)*numBin);
   chiSqrs = mymalloc(sizeof(double)*NTESTS);

   temp = 1.0 / numBin;
   for (index=0;index<numBin;index++) probs[index] = temp;
}

/*------------------------------------------------------------------*/
#ifdef __STDC__
void deinitTest(void) {
#else
void deinitTest() {
#endif
   free(oneGrp);
   free(bins);
   free(probs);
   free(chiSqrs);
}

/*------------------------------------------------------------------*/
#define PROC_ONE_GROUP {                                       \
   long    theBin, index, r, maxPos;                           \
   double  tempHold;                                           \
                                                               \
   for (index=0;index<grpSize;index++) {                       \
      oneGrp[index] = get_rn();                                \
   }                                                           \
                                                               \
   for (theBin=0,r=grpSize;r>1;r--) {                          \
      for (index=1,maxPos=0;index<r;index++) {                 \
         if (oneGrp[maxPos]<oneGrp[index]) maxPos = index;     \
      }                                                        \
      if (maxPos!=r-1) {                                       \
        tempHold = oneGrp[r-1];                                \
        oneGrp[r-1] = oneGrp[maxPos];                          \
        oneGrp[maxPos] = tempHold;                             \
      }                                                        \
      theBin = theBin * r + maxPos;                            \
   }                                                           \
   bins[theBin]++;                                             \
}

/********************************************************************/
#ifdef __STDC__
void main(int argc, char *argv[]) {
#else
void main(argc, argv) int argc; char *argv[]; {
#endif
   long    curRound, index;
   double  KSvalue, KSprob;
   int Bins_used;
   
   initTest(argc, argv);
   for (curRound=0;curRound<numRepeat;curRound++) {
      for (index=0;index<numBin;index++) bins[index] = 0;
      for (index=0;index<numGrp;index++) PROC_ONE_GROUP;

      chiSqrs[curRound] = chisquare(bins, probs, numGrp, numBin, &Bins_used);
      /*printf("\tChisquare for stream = %f, %% = %f\n",chiSqrs[curRound] , 
	     chipercent(chiSqrs[curRound],numBin-1));*/
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

    printf("Result: KS value = %f\n", (float) KSvalue);
    printf("        KS value prob = %f %%\n\n", (float) KSprob*100);
    deinitTest();
 }
   

#if defined(SPRNG_MPI)
     MPI_Finalize();
#endif

}
