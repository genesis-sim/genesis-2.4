/*********************************************************************
       Gap Test
*********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "tests.h"


#define NUM_TEST_ENGIN_PARAM 4

static int     maxGapLen;
static double  lowerBound, upperBound;
static long    numTotGap;
static int     numBin;
static long    numRepeat;
static long    *bins;
static double  *probs;
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

   maxGapLen = atoi(argv[N_STREAM_PARAM+1]);
   lowerBound = atof(argv[N_STREAM_PARAM+2]);
   upperBound = atof(argv[N_STREAM_PARAM+3]);
   numTotGap = atol(argv[N_STREAM_PARAM+4]);

   if ((maxGapLen<0) || (lowerBound<0.0) ||
       (upperBound<=lowerBound) || (1.0<upperBound) ||
       (numTotGap<=0)) {
      printf("Error: incorrect parameter value(s)\n");
      FATAL_ABORT;
   }

   /* +2 to include "0" & "> max" cases */
   numBin = maxGapLen + 2;
   numRepeat = init_tests(argc, argv);

   bins = mymalloc(sizeof(long)*numBin);
   probs = mymalloc(sizeof(double)*numBin);
   chiSqrs = mymalloc(sizeof(double)*NTESTS);

   probs[0] = upperBound - lowerBound;
   for (index=1;index<numBin-1;index++) {
      probs[index] = probs[index-1] * (1-upperBound+lowerBound);
   }

   probs[numBin-1] = 1.0;
   for (index=0;index<maxGapLen+1;index++) {
      probs[numBin-1] *= (1 - upperBound + lowerBound);
   }
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
#define PROC_ONE_GAP {                                           \
   long  gapLen;                                                 \
   double  tempX;                                                \
                                                                 \
   gapLen = -1;                                                  \
   do {                                                          \
      gapLen++;                                                  \
      tempX = get_rn();                                          \
   } while ((tempX<lowerBound) || (tempX>=upperBound));          \
                                                                 \
   /* Add (gapLen<0) clause to catch overflow */                 \
   if ((gapLen>maxGapLen) || (gapLen<0)) bins[numBin-1]++;       \
   else bins[gapLen]++;                                          \
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
      for (index=0;index<numTotGap;index++) PROC_ONE_GAP;

      chiSqrs[curRound] = chisquare(bins, probs, numTotGap, numBin,&Bins_used);
      /*printf("\tChisquare for stream = %f, %% = %f\n", chiSqrs[curRound], 
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
