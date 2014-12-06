/****************************************************************************/
/*  Note: This test is not in the regular SPRNG test format                 */
/*                                                                          */ 
/*                 _________ 2D FFT test _________                          */
/*                                                                          */
/* Type: make fft to compile this program; it is not automatically made     */
/*                                                                          */
/* The FFT routine is based on that on the Power Challenge array at NCSA    */
/* Please make changes on your machine. (routines dzfft*)                   */
/*                                                                          */
/* We fill a random array with 'n' numbers from 'nstreams' streams.         */
/* We then compute the 2D FFT for this array. The expected value for all    */
/* terms except the constant one is 0. The standard deviation too can be    */
/* determined theoretically. We check to see if most of the coefficients    */
/* are acceptable based on how far they are from the expected value. As     */
/* its output, this program prints the coeffieients that fall outside a     */
/* a certain acceptable range. We should alway expect a number of them      */
/* to fall outside the range. However, in repeated runs of this test, we    */
/* would expect that the same coefficients do not keep falling outside      */
/* the range each time.                                                     */
/*                                                                          */
/* Each column of the array consists of numbers from the same stream.       */
/* Note that we use a *** column major *** order, due to the requirements   */
/* of the SGI software. The Fourier coeffients are stored in place. This    */
/* can be done, even though the results are complex because only about half */
/* the coefficients are needed; the other half can be determined from the   */
/* ones computed. We need slightly more than 'n' rows, and the number of    */
/* rows needed for the coefficients is given by 'lda'.                      */
/****************************************************************************/

#include <stdio.h>
/*#if defined(SPRNG_MPI)
#include "mpi.h"
#endif*/
#if defined(SPRNG_MPI)
#undef SPRNG_MPI
#endif
#include "sprng.h"
#include <math.h>
#include <fft.h>


void Analyze(int nstreams, int nruns, int n);
void FFTCalc (int nstreams, int nruns, int n);


int **streams, lda;
double *means, *FFTcoeffs;

void main (int argc, char *argv[])
{

  int i, seed, param, nruns, nstreams, n, *stream, myid=0, nprocs=1;


#ifdef SPRNG_MPI
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &myid);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  if(myid != 0)			/* This is a sequential program currently  */
  {
    MPI_Finalize();
    exit(0);
  }
#endif

  /**************************** Initialization *****************************/

  if(argc != 8 || atoi(argv[2]) != 1 || atoi(argv[6]) != 0)
  {
    fprintf(stderr,"USAGE: %s nstreams 1 seed param nruns 0 n\n",
	    argv[0]);
    exit(-1);
  }
  else if(atoi(argv[1]) < 1 || atoi(argv[5]) < 1 || atoi(argv[7]) < 1 )
  {
    fprintf(stderr,"ERROR: nstreams, nruns, and n must be > 0\n");
    exit(-1);
  }
  
  nstreams = atoi(argv[1]);	/* number of streams                       */
  param = atoi(argv[4]);	/* parameter to the generator              */
  nruns = atoi(argv[5]);	/* number of runs to repeat                */
  n = atoi(argv[7]);		/* number of random numbers per stream     */

  if(n&1)			/* number of rows for Fourier coefficients */
    lda = n+1;
  else
    lda = n+2;
  
  FFTcoeffs = (double *) malloc(lda*nstreams*sizeof(double)); /* FFT coeffs */
  means = (double *) malloc(lda*nstreams*sizeof(double)); /* average coeffs */
  streams = (int **) malloc(nstreams*sizeof(int *)); /*random number streams*/
  
  for(i=0; i<lda*nstreams; i++)
    FFTcoeffs[i] = means[i] = 0.0;
  
  seed = make_sprng_seed();
  
  for (i=0; i<nstreams; i++)	/* initialize random number streams         */
    streams[i] = init_sprng(i,nstreams,seed,param);
  

  /***************************** Calculate FFTs *****************************/

  FFTCalc(nstreams,nruns,n);


  /************************* Analyze coefficients ***************************/

  Analyze(nstreams,nruns,n); 

  free(FFTcoeffs);
  free(means);
  free(streams);

#if defined(SPRNG_MPI)
     MPI_Finalize();
#endif

  
}


/* Analyze the output of the Fourier Transform. Check if coefficients are  */
/* within a "reasonable" distance of the expected values.                  */

void Analyze(int nstreams, int nruns, int n)
{
  int i, j, count;
  double StdDev1, StdDev2, mean1, mean2, StdDev, mult, percentile;
  
  /* The "reasonable" permitted distance from the expected value is some   */
  /* multiple of the standard deviation. We choose different multiples     */
  /* for different sample sizes; otherwise the output could be too large   */
  /* or too small. The percentile range corresponds to this multiple;      */
  /* but this value is approximate, especially for small values of 'nruns' */
  /* since the normality assumption may not hold otherwise.                */
  count = 0;
  if(nstreams*n <100)		/* percentile at which test fails.         */
  {				/* Different sizes have different          */
    mult = 1.0;			/* percentiles; otherwise the number       */
    percentile = 68.0;		/* of coefficients printed will be too     */
  }                             /* large. */
  else if(nstreams*n < 5000)
  {
    mult = 2.0;
    percentile = 95.0;
  }
  else if(nstreams*n < 20000)
  {
    mult = 3.0;
    percentile = 99.7;
  }
  else
  {
    mult = 3.9;
    percentile = 99.99;
  }
  
  mean1 = (double)nstreams*(double)n/2.0; /* Expected value of constant term*/
  mean2 = 0.0;			/* Expected values of all other terms       */
  StdDev1 = sqrt((double)nstreams*(double)n/(double)nruns/12.0);
  StdDev2 = sqrt((double)nstreams*(double)n/(double)nruns/24.0);

  if(fabs(means[0]-mean1) > mult*StdDev1) /* Check constant term            */
  {
    count++;
    printf("%d. (0,0) th coefficient exceeds ~ %g th percentile: Expected = %g, observed = %g, StdDev = %g\n\n", count, percentile, mean1, means[0], StdDev1);
  }
  
  for (j=0; j<nstreams; j++)  /*  Check each coefficient                    */
    for (i=0; i<lda/2; i++) 
    {
      if(i==0 && j==0)	      /* The constant term has already been checked */
	continue;
      else
      {
	if(nstreams%2 == 1 || j%(nstreams/2) != 0 || i%(n/2) != 0)
	  StdDev = StdDev2;
	else
	  StdDev = StdDev1;
      }
      
      if(fabs(means[2*i+j*lda]-mean2) > mult*StdDev) /* Real part of coeff. */
      {
	count++;
	printf("%d. Real part of (%d,%d) th coefficient exceeds ~ %g th percentile: Expected = %g, observed = %g, StdDev = %g\n\n", count, i, j, percentile, mean2, means[2*i+j*lda], StdDev);
      }
      
      if(fabs(means[2*i+j*lda+1]-mean2) > mult*StdDev) /* Imaginary part    */
      {
	count++;
	printf("%d. Complex part of (%d,%d) th coefficient exceeds ~ %g th percentile: Expected = %g, observed = %g, StdDev = %g\n\n", count, i, j, percentile, mean2, means[2*i+j*lda+1], StdDev);
      }
    }
  
  printf("No other coefficient exceeds %g th percentile\n\n", percentile);
  
  return;
}


/* Perform FFT on random arrays. Each column is from the same stream.       */
/* There are n rows per column, and nstream rows                            */

void FFTCalc(int nstreams, int nruns, int n) 
{
  int i,j,k;
  double *coeff;
  
  for(k=0; k<nruns; k++)	/* Repeat calculation 'nruns' times         */
  {
    for (j=0; j<nstreams; j++)  /*  fill array with random numbers          */
      for (i=0; i<n; i++) 
	FFTcoeffs[i+j*lda] = sprng(streams[j]); 
    
    coeff = dzfft2dui (n, nstreams, NULL); /*initialize FFT modules         */
    dzfft2du(1,n,nstreams,FFTcoeffs,lda, coeff); /* Compute FFT in place    */

    for (i=0; i<lda*nstreams; i++)  /*  Compute mean for each coefficient   */
	means[i] += FFTcoeffs[i]; 

#ifdef DEBUG	    /* Check if inverse transform yields original data      */
    zdfft2du (-1, n, nstreams, FFTcoeffs, lda, coeff); 
    dscal2d(n,nstreams,1.0/(double)(n*nstreams),FFTCoeffs,lda);
    for(i=0; i<(lda*nstreams); i++) 
      printf("array[%d] = %g\n", i, FFTcoeffs[i]); 
#endif
  } /*end loop for nruns                                                    */

  for (i=0; i<lda*nstreams; i++)  /*  Compute mean for each coefficient     */
    means[i] /= nruns; 
  
  return;
}



