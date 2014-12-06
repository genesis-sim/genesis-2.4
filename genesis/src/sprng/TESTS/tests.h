#ifndef _tests_h
#define _tests_h
#include "util.h"

#define N_STREAM_PARAM 6

#ifndef ANSI_ARGS
#ifdef __STDC__
#define ANSI_ARGS(args) args
#else
#define ANSI_ARGS(args) ()
#endif
#endif

extern long NTESTS;
/*NTESTS is the number of times the tests are performed on the random
 subsequences. Though init_tests also returns the number of tests when
 the tests are done on one processor, the number returned is different
 when multiple processors are used */

extern int proc_rank;
/*Rank of processor in the set of processors */

extern int init_nprocs;
/* Number of processors */

double chipercent ANSI_ARGS((double chiSq, long fr));
/*chiSq is the chisquare value, normally returned by the function chisquare.
fr is the degrees of freedom. The value returned is the percentile level in 
the chisquare test */

double chisquare ANSI_ARGS((long *actual, double *probability, long n, long k, int *nb));
/* actual is an array of integers, that gives the frequency in each class    */
/* (bin). probability is an array of expected probabilities in each bin. k is*/
/* the number of bins. n is the number of trials. So n*probabilit[i] will    */
/* give the expected frequency for bin i. The chisquare 'error' value is     */
/* returned. If the expected value in a bin is too low, then it is combined  */
/* with adjacent bins until this value is sufficiently high; 'nb' is set to  */
/* the effective number of bins, and thus is at most k.*/

double chiF ANSI_ARGS((double chiSq));
/* This is the chisquare cumulative distribution function. It is identical to the function 'chipercent', except that the degrees of freedom is not a parameter. This function is usually not called directly, but is passed as an argument to the KS function. Before this, the segrees of freedom must be set using 'set_d_of_f.*/

void set_d_of_f ANSI_ARGS((long df));
/* This sets the degrres of freedom to 'df' for use by chiF. */

double KS ANSI_ARGS((double *V, long n, double (*F)(double) ));
/* This returns the Kolmogorov-Smirnov 'error' value. V is an array of doubles, which contains the values of the samples. n is the size of the array. F is the pointer to the cumulative distribution function from which it is expected that the samples are drawn. */

double KSpercent ANSI_ARGS((double value, long n));
/* value is the Kolmogorov-Smirnov error value, normally returned by the function KS. n is the number of samples that were used in order to determine this value. This function returns the percentile according to the K-S test. */

double KSF ANSI_ARGS((double value));
/* This is identical to KSpercent, but does not have an argument for n. n must be set using 'set_KS_n' before this function is called. Normally a pointer to this function is passed to the KS algorithms. */

void set_KS_n ANSI_ARGS((long n));
/* n is the sample size for use in the KS algorithms. */

#if defined(SPRNG_MPI)
void getKSdata ANSI_ARGS((double *V, long ntests));
/* When performing the tests on mulitple processors using MPI, a set
   of streams is tested on each processor. The processor numbered '0'
   must collect the data (ex, chisquare value) for each stream before
   it can perform the final KS test. So this function must be called
   before the KS test is done. ntests is the total number of random
   subsequences tested, and should be set to NTESTS, rather than to
   the value returned by init_tests. V is an array of the chisquare
   values returned for each stream. Processor '0' collects this data
   from each processor in this function. The other processors send
   their values in this function. Note that if MPI is being used, then
   SPRNG_MPI must be defined by the user. */ 
 
void reduce_sum ANSI_ARGS((double *array,int n));
/* Perform a reduction (with addition) operation on each of the 'n'
 elements of 'array'*/

void  reduce_sum_long ANSI_ARGS((long *array,long n));
/* Perform a reduction (with addition) operation on each of the 'n'
 elements of 'array'*/
#endif

void next_stream ANSI_ARGS((void));
/*This function frees memory concerned with the current stream, and
  initiallizes a new stream for use by get_rn */

double get_rn ANSI_ARGS((void));
/*get_rn returns the next random number*/

long init_tests ANSI_ARGS((int argc, char *argv[]));
/*The arguments to init_test are the same as for the command line
  arguments. The first command line argument is the number of stream
  sets to be tested. The second argument is the number of streams to
  interleave while creating a enw stream, 'ncombine'.  The next two
  paramemters are the seed and the parameter for the generator. By
  default, they can be set to 0. The next argument is the number of
  subsequences to test in each new stream created. The argument after
  that tell us how many random numbers should be skipped after the
  completion of a test, before testing the next subsequence of the
  stream. The rest of the arguments are based on the tests being
  performed. */

#endif
