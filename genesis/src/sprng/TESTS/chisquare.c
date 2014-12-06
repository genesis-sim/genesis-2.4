/**********************************************************
     Chi-Square Probability Function and Kolmogorv-Smirnov 
**********************************************************/

#include <stdio.h>
#include <math.h>
#include "util.h"

#define ITMAX 10000000        /* maximum allowed number of iterations */
#define EPS 3.0e-7            /* relative accuracy                    */
#define FPMIN 1.0e-30         /* number near the smallest             */
                              /* representable floating-point number  */

#ifndef max
#define max(a,b) (a<b?b:a)
#endif
#ifndef min
#define min(a,b) (a>b?b:a)
#endif

static char *errorMessage;
long degrees_of_freedom=1;
long KS_n = 1;


#ifndef ANSI_ARGS
#ifdef __STDC__
#define ANSI_ARGS(args) args
#else
#define ANSI_ARGS(args) ()
#endif
#endif

void set_d_of_f ANSI_ARGS((long df));
double chiF ANSI_ARGS((double chiSq));
double chisquare ANSI_ARGS((long *actual,double *probability,long n,long k, int *nb));
double chipercent ANSI_ARGS((double chiSq, long fr));
double gammp ANSI_ARGS((double a, double x));
double KSpercent ANSI_ARGS((double value, long n));
double KS ANSI_ARGS((double *V, long n, double (*F)(double) ));

void  gser ANSI_ARGS((double *gamser, double a, double x, double *gln));
void  gcf ANSI_ARGS((double *gammcf, double a, double x, double *gln));
double gammln ANSI_ARGS((double xx));

void  chiGammaClearErrMess ANSI_ARGS((void));
char  *chiGammaReadErrMess ANSI_ARGS((void));
void  chiGammaFlagError ANSI_ARGS((char *errorText));

double cum_normal(double sample, double mean, double stddev);
void set_normal_params(double mu, double sd);
double normalF(double x);

void mean_sd(double *x, int n, double *mean, double *sd);


#ifdef __STDC__
double KS(double *X, long n, double (*F)(double) )
#else
double KS(X, n, F)
double *X,(*F)();
long n;
#endif
{
  double *a, *b, Y, rminus, rplus, value;
  long *c, m, k, j;
  
  m = n+1;
  
  a = (double *) mymalloc((m+1)*sizeof(double));
  b = (double *) mymalloc((m+1)*sizeof(double));
  c = (long *) mymalloc((m+1)*sizeof(long));
  
  memset(c,0,(m+1)*sizeof(long));
  for(k=0; k<m+1; k++)
  {
    a[k] = 1.0;
    b[k] = 0.0;
  }
  
  for(j=0; j<n; j++)
  {
    Y = F(X[j]);
    
    k = m*Y;
    a[k] = min(a[k],Y);
    b[k] = max(b[k],Y);
    c[k]++;
  }
  
  for(k=j=0, rplus=rminus=0.0; k<m; k++)
  {
    if(c[k] <= 0)
      continue;
    
    rminus = max(rminus,a[k] - (double) j/n);
    j += c[k];
    rplus = max(rplus,(double) j/n - b[k]);
  }
  
  value = sqrt((double) n)*max(rplus,rminus);
#ifdef ONE_SIDED
  value = sqrt((double) n)*rplus;
#endif
  free(a);
  free(b);
  free(c);
  
  return value;
}


#ifdef __STDC__
void set_KS_n(long n)
#else
void set_KS_n(n)
long n;
#endif
{
  KS_n = n;
}


#ifdef __STDC__
double KSF(double value)
#else
double KSF(value)
double value;
#endif
{
  return KSpercent(value, KS_n);
}

#ifndef ONE_SIDED
#ifdef __STDC__
double KSpercent(double value, long n)
#else
double KSpercent(value, n)
double value;
long n;
#endif
{
  int i;
  double mult, temp, term, previous_term, percent;
  
  value /= sqrt((double) n);
  
  value *= 0.12 + sqrt((double) n) + 0.11/sqrt((double) n);
  
  mult = 2.0;
  percent = 0.0;
  temp = -2.0*value*value;
  previous_term = 0.0;
  
  for(i=1; i<150; i++)
  {
    term = mult*exp(temp*i*i);
    percent += term;
    if( fabs(term)<0.005*fabs(previous_term) || fabs(term)<0.0000001*percent)
      return 1.0 - percent;
    
    mult = -mult;
    previous_term = term;
  }
  
  fprintf(stderr,"WARNING: KSpercent failed to converge\n");
  
  return max(percent,0.0);
}
#endif


#ifdef ONE_SIDED
#ifdef __STDC__
double KSpercent(double value, long n)
#else
double KSpercent(value, n)
double value;
long n;
#endif
{
  int Nmax;
  
  value *= sqrt((double) n);
  
  if(value <= 0.0)
    return 0.0;
  
  Nmax = 30;			/* between 30 and 50 */

  if(n > Nmax)
  {
    double yp, p;
    
    yp = value + 1.0/6.0/sqrt((double) n);
    p = 1 - exp(-2.0*yp*yp);
    return 1.0 - 2.0*(1.0-p);
    /*    return p;*/
  }
  else
  {
    static double K[16][9] = {{0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
		       {0.0,0.01,0.05,0.25,0.5,0.75,0.95,0.99,1.001},
		       {0.0,0.014,0.06749,0.2929,0.5176,0.7071,1.098,1.2728,
			  1.415},
		       {0.0,0.01699,0.07919,0.3112,0.5147,0.7539,1.1017,1.3589,
			  1.733},
		       {0.0,0.01943,0.08789,0.3202,0.5110,0.7642,1.1304,1.3777,
			  2.01},
		       {0.0,0.02152,0.09471,0.3249,0.5245,0.7674,1.1392,1.4024,
			  2.237},
		       {0.0,0.02336,0.1002,0.3272,0.5319,0.7703,1.1463,1.4144,
			  2.45},
		       {0.0,0.02501,0.1048,0.328,0.5364,0.7755,1.1537,1.4246,
			  2.647},
		       {0.0,0.0265,0.1086,0.328,0.5392,0.7797,1.1586,1.4327,
			  2.829},
		       {0.0,0.02786,0.1119,0.3274,0.5411,0.7825,1.1624,1.4388,
			  3.01},
		       {0.0,0.02912,0.1147,0.3297,0.5426,0.7845,1.1658,1.444,
			  3.163},
		       {0.0,0.03028,0.1172,0.333,0.5439,0.7863,1.1688,1.4484,
			  3.318},
		       {0.0,0.03137,0.1193,0.3357,0.5453,0.788,1.1714,1.4521,
			  3.465},
		       {0.0,0.03424,0.1244,0.3412,0.55,0.7926,1.1773,1.4606,
			  3.874},
		       {0.0,0.03807,0.1298,0.3461,0.5547,0.7975,1.1839,1.4698,
			  4.473},
		       {0.0,0.04354,0.1351,0.3509,0.5605,0.8036,1.1916,1.4801,
			  5.478}};
    static int index[16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,15,20,30};
    static double p[9] = {0.0,0.01,0.05,0.25,0.5,0.75,0.95,0.99, 1.0};
    int i, j;
    double percent1, percent2, dist1, dist2, percentage;
    
    i = 1;
    while(index[i] < n)		/* look up correct row for 'n' in table */
      i++;
    
    j = 1;
    while(K[i][j] < value)	/* look up percentage in table */
      j++;
    
    dist1 = K[i][j]-K[i][j-1];
    dist2 = K[i][j]-value;
    percent1 = p[j]*(dist1-dist2)/dist1 + p[j-1]*dist2/dist1;
    
    if(index[i]==n)
      return percent1;
    else
    {
      j = 1;
      while(K[i-1][j] < value)	/* look up percentage in table */
	j++;
    
      dist1 = K[i-1][j]-K[i-1][j-1];
      dist2 = K[i-1][j]-value;
      percent2 = p[j]*(dist1-dist2)/dist1 + p[j-1]*dist2/dist1;
      
      dist1 = (double) (index[i]-index[i-1]);
      dist2 = (double) (index[i]-n);
      percentage = percent1*(dist1-dist2)/dist1+ percent2*dist2/dist1;
      /*      return percentage;*/
      return 1.0-2.0*(1.0-percentage);
    }
  }
}
#endif

#ifdef __STDC__
double chisquare(long *actual, double *probability, long n, long k, int *nb)
#else
double chisquare(actual, probability, n, k,nb)
long *actual, n, k;
double *probability;
int *nb;
#endif
{
  double V, prob;
  long i, n_actual, nbins;
  int ch;
  
  V = 0.0;
  for(i=n_actual=nbins=0, prob=0.0; i<k; i++)
  {
    n_actual += actual[i];
    prob += probability[i];
    /*printf("\t %ld. %ld %ld %f %f %ld %ld\n", i, n_actual, actual[i], prob, probability[i], n, nbins);
      ch = getchar();*/
    
    if(n*prob >=5 || i==k-1)
    {
      V += (n_actual-n*prob)*(n_actual-n*prob)/
	(n*prob);
      nbins++;
      n_actual = 0;
      prob = 0.0;
    }
  }
  
  *nb = nbins;
  
  if(nbins == 1)		
    fprintf(stderr,"WARNING: Only one bin was used in Chisquare\n");
  else if(nbins==0)
    fprintf(stderr,"WARNING: No bin was used in Chisquare => n was <= 5.\n");
  else if(nbins != k)
    fprintf(stderr,"WARNING: The effective number of bins in Chisquare: %ld, was less than the given number of bins: %ld, due to the effect of combining bins in order to make the expected number of hits per bin sufficiently large.\n", nbins, k);
   
  return V;
}


#ifdef __STDC__
void set_d_of_f(long df)
#else
void set_d_of_f(df)
long df;
#endif
{
  degrees_of_freedom = df;
}


#ifdef __STDC__
double chiF(double chiSq) /* returns the chi-square prob.   */
#else
double chiF(chiSq)
double chiSq;
#endif
{                                         
  return chipercent(chiSq,degrees_of_freedom);
}


#ifdef __STDC__
double chipercent(double chiSq, long fr) /* returns the chi-square prob.   */
                                           /* function with chi-square chiSq */
#else
double chipercent(chiSq, fr) /* returns the chi-square prob.   */
double chiSq;         /* function with chi-square chiSq */
long fr;
#endif
{                                          /* & degrees of freedom fr        */
  return (gammp(fr/2.0, chiSq/2.0));
}


#ifdef __STDC__
double gammp(double a, double x)           /* returns the incomplete gamma  */
#else                                          /* function P(a,x)      */
double gammp(a, x)           /* returns the incomplete gamma  */
double a, x;
#endif
{
  
  double gamser, gammcf, gln;

  if (x < 0.0 || a <= 0.0) 
  {
    chiGammaFlagError("Invalid arguments in routine gammp");
    return (0.0);
  }

  if (x < (a + 1.0)) 
  {                   /* use the series representation */
    gser(&gamser,a,x,&gln);
    return (gamser);
  } 
  else 
  {                               /* use the continued fraction    */
    gcf(&gammcf,a,x,&gln);            /* representation */
    return (1.0 - gammcf);            /* and take its complement       */
  }
}

#ifdef __STDC__
void gser(double *gamser, double a, double x, /* Returns the incomplete gamma*/
          double *gln)                    /* function P(a,x) evaluated by  */
#else
void gser(gamser, a, x, gln)  /* Returns the incomplete gamma*/
                              /* function P(a,x) evaluated by  */
double *gamser, a, x, *gln;
#endif
{                                         /* its series representation as  */
                                          /* gamser.  Also returns         */
                                          /* ln(gamma(a)) as gln.          */
  int n;
  double sum, del, ap;

  *gln=gammln(a);
  if (x <= 0.0) 
  {
    if (x < 0.0)
      chiGammaFlagError("x less than 0 in routine gser");
    *gamser = 0.0;
    return;
  } 
  else
  {
    ap = a;
    del = sum = 1.0/a;
    for (n=1; n<=ITMAX; n++) 
    {
      ++ap;
      del *= x/ap;
      sum += del;
      if (fabs(del) < fabs(sum)*EPS)
      {
	*gamser = sum * exp(-x + a * log(x) - (*gln));
	return;
      }
    }
    chiGammaFlagError("a too large, ITMAX too small in routine gser");
    return;
  }
}

#ifdef __STDC__
void gcf(double *gammcf, double a, double x, /* Returns the complement  */
         double *gln)                        /* imcomplete gamma function  */
#else
void gcf(gammcf, a, x, gln)      /* Returns the complement  */
                                 /* imcomplete gamma function  */
double *gammcf, a, x, *gln;
#endif
{                                            /* Q(a,x) evaluated by its  */
                                             /* continued fraction rep. as  */
                                             /* gammcf.  Also returns  */
                                             /* ln(gamma(a)) as gln   */
  int i;
  double an, b, c, d, del, h;

  *gln = gammln(a);

  b = x + 1.0 - a;                   /* set up for evaluating   */
  c = 1.0 / FPMIN;                   /* continued fraction by modified*/
  d = 1.0 / b;                       /* Lentz' method with b0=0       */
  h = d;

  for (i=1;i<=ITMAX;i++) 
  {            /* iterate to convergence        */
    an = -i * (i - a);
    b += 2.0;
    d = an * d + b;
    if (fabs(d) < FPMIN) d = FPMIN;
    c = b + an / c;
    if (fabs(c) < FPMIN) c = FPMIN;
    d = 1.0 / d;
    del = d * c;
    h *= del;
    if (fabs(del-1.0) < EPS) 
      break;
  }

  if (i > ITMAX) 
  {
    chiGammaFlagError("a too large, ITMAX too small in gcf");
    return;
  }

  *gammcf = exp(-x + a * log(x) - (*gln)) * h; /* put factors in front  */
}


#ifdef __STDC__
double gammln(double xx)                          /* returns the value       */
#else
double gammln(xx)                          /* returns the value       */
double xx;
#endif
{                                                 /* ln(gamma(xx)) for xx > 0*/
  double x, y, tmp, ser;
  static double cof[6] = { 76.18009172947146,
                             -86.50532032941677,
			     24.01409824083091,
			     -1.231739572450155,
			     0.1208650973866179e-2,
			     -0.5395239384953e-5};
  int j;

  y = x = xx;
  tmp = x + 5.5;
  tmp -= (x + 0.5) * log(tmp);
  ser = 1.000000000190015;

  for (j=0; j<=5; j++) 
    ser += cof[j]/++y;

  return (-tmp + log(2.5066282746310005 * ser / x));
}


#ifdef __STDC__
void chiGammaClearErrMess(void)  /* clears error message buffer   */
#else
void chiGammaClearErrMess()
#endif
{
  errorMessage = NULL;
}

#ifdef __STDC__
char *chiGammaReadErrMess(void)  /* returns the error message     */
#else
char *chiGammaReadErrMess() 
#endif
{                                /* from the buffer               */
  return (errorMessage);
}

#ifdef __STDC__
void chiGammaFlagError(char *errorText) /* writes to the error message   */
#else
void chiGammaFlagError(errorText)
char *errorText;
#endif
{                                       /* buffer                        */
  errorMessage = errorText;
}

#if 0
#ifdef __STDC__
main(int argc, char *argv[])
#else
main(argc,argv)
int argc;
char *argv[];
#endif
{

  double p[1000000], value;
  long obs[1000000], n;
  double X[10];
  int i, nb;
  
   set_d_of_f(99999);
  
  /*printf("\nChi square probability for V = %f, degrees of freedom = %d is: %f\n\n", 34.76,50,chiF(34.76));*/

  /*for(i=0; i<1000000; i++)
    {
    p[i] = (double) 1/1000000;
    obs[i] = 10;
    }*/
  
    p[0] = 0.2;
  p[1] = 0.5; 
  p[2] = 0.3; 
  obs[0] = 30;  
  obs[1] = 30; 
  obs[2] = 40; 
  
  /*printf("\n chisquare value = %f\n\n", chisquare(obs,p,10000000L,1000000L, &nb));*/
  printf("\n chisquare value = %f, nbins = %d\n\n", chisquare(obs,p,100,3, &nb), nb); /*** wrong values of nb may be obtained because function argument evaluation order is not defined. Beware! ***/
  /*value = atof(argv[1]);
  n = atoi(argv[2]); 
  
  printf("\nd = %f,KS = %f\n\n", value, KSpercent(value,n));*/
	
  set_d_of_f(1000);
  /*X[0] = 10.0;
  X[1] = 13.0;
  X[2] = 5.0;
  X[3] = 15.0;
  X[4] = 11.5;
  X[5] = 4.0;
  X[6] = 8.0;
  X[7] = 9.0;
  X[8] = 6.0;
  X[9] = 7.0;*/
  
  /*printf("KS = %.14f\n", KS(p,1000000,chiF));*/
  
} 
#endif


/* Reference: NIST -- NORCDF available on netlib */

double cum_normal(double sample, double mean, double stddev)
{
  double a, b, c, d, e, f, g, h, temp;
  
  a = 0.319381530 ;
  b = -0.356563782; 
  c = 1.781477937; 
  d = -1.821255978;
  e = 1.330274429;
  f = .2316419;
  
  if(stddev <= 0.0)
    return -1.0;
  
  sample = (sample - mean)/stddev;
  
  if(sample < 0.0)
    temp = -sample;
  else
    temp = sample;
  
  g = 1.0/(1.0+f*temp);
  h = 1.0 - (0.39894228040143*exp(-0.5*temp*temp))
    *(a*g + b*g*g + c*g*g*g + d*g*g*g*g + e*g*g*g*g*g);
  
  if(sample < 0.0)
    h=1.0-h; 

  return h;
}


static double normal_mu;
static double normal_sd;

void set_normal_params(double mu, double sd)
{
  normal_mu = mu;
  normal_sd = sd;
}


double normalF(double x)
{
  return cum_normal(x,normal_mu,normal_sd);
}


void mean_sd(double *x, int n, double *mean, double *sd)
{
  double ave, var, diff, error;
  int i;
  
  ave = 0.0;
  for(i=0; i<n; i++)
    ave += x[i];
  ave /= n;
  
  diff = var = 0.0;
  
  for(i=0; i<n; i++)
  {
    diff = x[i] - ave;
    error += diff;
    var +=diff*diff;
  }
  
  var = (var-error*error/n)/(n-1);
  
  *mean = ave;
  *sd = sqrt(var);
}

