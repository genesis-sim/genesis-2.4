/*********************************************************************
       Run Test
*********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "tests.h"

/* # of parameters for the test engin */
#define NUM_TEST_ENGIN_PARAM 2
/* Maximum run length */
static int     maxRunLen;
/* # of runs in each test */
static long    numRun;
/* # of bins */
static int     numBin;
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
void initTest(argc, argv) int argc; char *argv[]; {
#endif
   int  numParam=NUM_TEST_ENGIN_PARAM+N_STREAM_PARAM;
   int  index;

   if (argc<(numParam+1)) {
      printf("Error: %i number of parameters needed\n", numParam);
      FATAL_ABORT;
   }

   maxRunLen = atoi(argv[N_STREAM_PARAM+1]);
   numRun = atol(argv[N_STREAM_PARAM+2]);
   if ((maxRunLen<=0) || (numRun<=0)) {
      printf("Error: incorrect parameter value(s)\n");
      FATAL_ABORT;
   }

   /* +1 to include ">maxRunLen" case */
   numBin = maxRunLen + 1;
   numRepeat = init_tests(argc, argv);

   bins = mymalloc(sizeof(long)*numBin);
   probs = mymalloc(sizeof(double)*numBin);
   chiSqrs = mymalloc(sizeof(double)*NTESTS);

   probs[0] = 0.5;
   for (index=2;index<numBin;index++) {
      probs[index-1] = probs[index-2] * index /
                       (index+1) / (index-1);
   }
   probs[numBin-1] = probs[numBin-2] / maxRunLen;
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
#define PROC_ONE_RUN {                                           \
   int  theLen;                                                  \
   double  tempX, tempY;                                         \
                                                                 \
   theLen = 0;                                                   \
   tempY = get_rn();                                             \
   do {                                                          \
      theLen++;                                                  \
      tempX = tempY;                                             \
      tempY = get_rn();                                          \
   } while (tempX<tempY);                                        \
                                                                 \
   /* Add (theLen<1) clause to catch overflow */                 \
   if ((theLen>maxRunLen) || (theLen<1)) bins[numBin-1]++;       \
   else bins[theLen-1]++;                                        \
}

/********************************************************************/
#ifdef __STDC__
void main(int argc, char *argv[]) {
#else
void main(argc, argv) int argc; char *argv[]; {
#endif
   long  curRound, index;
   double  KSvalue, KSprob;
   int i, Bins_used;
   
   initTest(argc, argv);
   for (curRound=0;curRound<numRepeat;curRound++) {
      for (index=0;index<numBin;index++) bins[index] = 0;
      for (index=0;index<numRun;index++) PROC_ONE_RUN;

      chiSqrs[curRound] = chisquare(bins, probs, numRun, numBin, &Bins_used);
      /*printf("\tChisquare for stream = %f, %% = %f\n",chiSqrs[curRound] , 
	 chipercent(chiSqrs[curRound],numBin-1)*100);*/
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
