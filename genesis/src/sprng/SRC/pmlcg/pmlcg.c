/*************************************************************************/
/*************************************************************************/
/*           Parallel Prime Modulus Linear Congruential Generator        */
/*                                                                       */ 
/* Author: Ashok Srinivasan,                                             */
/*            NCSA, University of Illinois, Urbana-Champaign             */
/* E-Mail: ashoks@ncsa.uiuc.edu                                          */
/*                                                                       */ 
/* Based on: ???                                                         */
/*                                                                       */
/* Disclaimer: NCSA expressly disclaims any and all warranties, expressed*/
/* or implied, concerning the enclosed software.  The intent in sharing  */
/* this software is to promote the productive interchange of ideas       */
/* throughout the research community. All software is furnished on an    */
/* "as is" basis. No further updates to this software should be          */
/* expected. Although this may occur, no commitment exists. The authors  */
/* certainly invite your comments as well as the reporting of any bugs.  */
/* NCSA cannot commit that any or all bugs will be fixed.                */
/*************************************************************************/
/*************************************************************************/

/*             This is version 0.2, created 13 April 1998                */
 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define NDEBUG
#include <assert.h>
#include <limits.h>
#include "memory.h"
#include "interface.h"
#include "gmp.h"
#include "basic.h"
#include <math.h>

#ifdef CONVEX
#undef _LONG_LONG  /* problems on convex compiler with 64 bit arithmetic */
#endif

#if LONG_MAX > 2147483647L 
#if LONG_MAX > 35184372088831L 
#if LONG_MAX >= 9223372036854775807L 
#define LONG_SPRNG
#define LONG64 long		/* 64 bit long */
#endif
#endif
#endif

#if !defined(LONG_SPRNG) &&  defined(_LONG_LONG)
#define LONG64 long long
#endif

#ifndef LONG64
#include "longlong.h"
#endif

#define VERSION "00"
/*** Name for Generator ***/
#define GENTYPE  VERSION "Prime modulus LCG"


#define NPARAMS 1		/*** number of valid parameters ***/
int MAX_STREAMS = (1<<30); /* Maximum number of streams for initialization */
				/* ... more streams can be spawned, though  */
   

struct rngen
{
  char *gentype;
  int stream_number;
  int nstreams;
  int init_seed;
  int parameter;
  int narrays;
  /*** declare other variables here ***/
#ifdef LONG64
  unsigned LONG64 mult, x;
#else
  unsigned long r[2], a[2];
  int a_size;               /* length of array 'a' */
#endif
  MP_INT k, si;         
};

int NGENS=0;		  /* number of random streams in current process */

  
/* ************************************************************* */
/* *************************   init   ************************** */
/* ************************************************************* */

static int init( aa, x0, k, seed, param)
unsigned long *aa, *x0;
MP_INT *k;
int seed, param;
{
  /*
     called by: initialize_int()
     calls    : GMP routines
                init_rel_prime(), prim_elt()   [ rand_lcg_mu.h ]

     params   : unsigned long aa, x0 = 'a' and 'r' arrays of a generator
                           ( empty when called )
                MP_INT k = k value to use to calculate polynomial
		param determines the power

     returns  : 'a_size' : the length of the multiplier array
	        ( also, params 'aa[]' and 'x0[]' will be filled )

        Sets up the multiplier ('a' array) and initial seed ('r' array)
        for the given value of k.   [ 'a' == 'aa' ,  'r' == 'x0' ] 
	*/

  MP_INT A;
  REL_PRIME_TABLE data;
  long i, a_size;


  /* find multiplier value */
  mpz_init_set_str(&A, MAXVAL, 10);

  init_rel_prime(&data, &A); /* param = 2^61-1 is assumes here */
  prim_elt(&A, k, data);

  for (i=0; i<OP_SIZE; i++)
  {
    aa[i] = mpz_get_ui(&A)&0xffffffff;
    x0[i] = 0;
    mpz_div_2exp( &A, &A, 32);
  }

  /* initialize seed value */
  x0[0] = ((unsigned int)seed)<<1 | 1;

  free_rel_prime(&data);
  mpz_clear(&A);

  /* calculate 'a_size' ( length of the multiplier array ) */
  i = 0;
  while (!(aa[OP_SIZE-i-1]))
    i++;
  a_size = OP_SIZE - i;

  return(a_size);

}  /* end of init() */


/* ************************************************************* */
/* *********************  initialize_int  ********************** */
/* ************************************************************* */

int **initialize(int ngen, MP_INT *old_si, int seed, int param)
{
  /*
     called by: init_rng(), spawn_rng_int
     calls    : init()
     GMP routines
     
     params   : int ngen = number of generators to initialize
     MP_INT old_si = value of k to use for first generator produced
     seed = encoding of starting state of generator
     param = power that determines Merssene prime
     returns  : pointer to pointers to RNGs (rngen structures)
     
     Initializes 'ngen' new generators
     ( allocates memory and gives initial values to the elements of 'rngen' )
     */
  int i,k,l,*order;
  struct rngen **q;	
  static unsigned long a[2], r[2];
  int a_size;
  
  order = (int *) mymalloc(ngen*sizeof(int));
  /* allocate memory for 'ngen' generators */
  q = (struct rngen **) malloc(ngen * sizeof(struct rngen *));
  if (q==NULL || order==NULL) 
    return ((int **)NULL); 

  for (i=0; i<ngen; i++)
  {
    q[i] = (struct rngen *) malloc(sizeof(struct rngen));
    if(q[i] == NULL)
      return NULL;
    
    mpz_init(&(q[i]->si));
    mpz_init(&(q[i]->k));
  }

  /* set up 1st generator */        
  mpz_set(&(q[0]->k),old_si);
#ifdef LONG64
  a_size = init(a, r, &(q[0]->k),seed,param);
  q[0]->mult = (unsigned LONG64)a[1]<<32|a[0];
  q[0]->x = (unsigned LONG64)r[1]<<32|r[0];
#else
  q[0]->a_size = init(q[0]->a, q[0]->r, &(q[0]->k),seed,param);
#endif

  
  mpz_mul_ui(&(q[0]->si), old_si, 2);
  mpz_add_ui(&(q[0]->si), &(q[0]->si), 1);

  /* set up remaining generators */
  i = 1;
  order[0] = 0;
  if (ngen>1) while (1) 
  {
    l = i;
    for (k=0; k<l; k++)
    {
      mpz_set(&(q[i]->k), &(q[order[k]]->si));
#ifdef LONG64
      a_size = init(a,r,&(q[i]->k),seed,param);
      q[i]->mult = (unsigned LONG64)a[1]<<32|a[0];
      q[i]->x = (unsigned LONG64)r[1]<<32|r[0];
#else
      q[i]->a_size = init(q[i]->a,q[i]->r,&(q[i]->k),seed,param);
#endif
      mpz_mul_ui(&(q[order[k]]->si), &(q[order[k]]->si), 2);
      mpz_set(&(q[i]->si), &(q[order[k]]->si));
      mpz_add_ui(&(q[i]->si), &(q[i]->si), 1);
      if (ngen == ++i) 
	break;
    }
    if (ngen == i) 
      break;
    for (k=l-1; k>0; k--)
    {
      order[2*k+1] = l + k;
      order[2*k] = order[k];
    }
    order[1] = l;
  }
			
  free(order);
  
  return( (int **)q );

} /* end of initialize */

/* Initialize random number stream */

#ifdef __STDC__
int *init_rng( int gennum, int total_gen,  int seed, int param)
#else
int *init_rng(gennum,total_gen,seed,param)
int gennum,param,seed,total_gen;
#endif
{
/*      gives back one stream (node gennum) with updated spawning         */
/*      info; should be called total_gen times, with different value      */
/*      of gennum in [0,total_gen) each call                              */
  struct rngen *genptr;
  int i, **p;
  MP_INT k;
  if (total_gen <= 0) /* Is total_gen valid ? */
  {
    total_gen = 1;
    fprintf(stderr,"WARNING - init_rng: Total_gen <= 0. Default value of 1 used for total_gen\n");
  }

  if (gennum >= MAX_STREAMS) /* check if gen_num is valid    */
    fprintf(stderr,"WARNING - init_rng: gennum: %d > maximum number of independent streams: %d\n\tIndependence of streams cannot be guranteed.\n",
	    gennum, MAX_STREAMS); 

  if (gennum < 0 || gennum >= total_gen) /* check if gen_num is valid    */
  {
    fprintf(stderr,"ERROR - init_rng: gennum %d out of range [%d,%d).\n",
	    gennum, 0, total_gen); 
    return (int *) NULL;
  }

  if (param < 0 || param >= NPARAMS)     /* check if parameter is valid */
  {
    fprintf(stderr,"WARNING - init_rng: parameter not valid. Using Default parameter.\n");
    param = 0;
  }
  
  seed &= 0x7fffffff;   /* Only 31 LSB of seed considered */
  
  mpz_init_set_ui(&k, gennum); /*final seed != 0 */
  p = initialize(1, &k, seed, param);
  if(p==NULL)
    return NULL;
  else
    genptr = (struct rngen *) p[0]; 
  free(p);
  
  /* Initiallize data structure variables */
  genptr->gentype = GENTYPE;
  genptr->stream_number = gennum;
  genptr->nstreams = total_gen;
  genptr->init_seed = seed; 
  genptr->parameter = param;
  
  genptr->narrays = 0;	/* number of arrays needed by your generator */

  while ( mpz_cmp_ui(&(genptr->si), total_gen) < 0 )
    mpz_mul_ui(&(genptr->si), &(genptr->si), 2);		
  mpz_clear(&k);
  
  NGENS++;			/* NGENS = # of streams */
  return (int *) genptr;
} 



/* ************************************************************* */
/* *************************  iterate  ************************* */
/* ************************************************************* */
 
#ifdef ANSI_C
void iterate( int *genptr ) 
#else
void iterate( genptr )
int *genptr;
#endif
{

#ifdef LONG64
  struct rngen *gen = (struct rngen *) genptr;
  unsigned LONG64 x0, x1, x3, ul, uh, vl, vh;
#define MULT_MASK1  0x7fffffffU
#define MULT_MASK2 0x3fffffffU

#ifdef LONG_SPRNG
#define MULT_MASK3 0x1fffffffffffffffUL
#define MULT_MASK4 0x2000000000000000UL
#else
#define MULT_MASK3 0x1fffffffffffffffULL
#define MULT_MASK4 0x2000000000000000ULL
#endif

  ul = gen->mult&MULT_MASK1;
  uh = (gen->mult>>31)&MULT_MASK2;
  vl = gen->x&MULT_MASK1;
  vh = (gen->x>>31)&MULT_MASK2;
  
  x0 = ul*vl;
  x1 = ul*vh + uh*vl + (x0>>31);
  x0 &= MULT_MASK1;
  x3 = ((uh*vh)<<1) + (x1>>30);
  x0 |= (x1&MULT_MASK2)<<31;
  
  gen->x = (x0+x3);
  if(gen->x&MULT_MASK4) /*Note: x != ..MASK3 since x!=0 mod prime for pmlcg */
  {
    gen->x &= MULT_MASK3;
    gen->x += 1;
    if(gen->x == MULT_MASK4)
      gen->x = 1;
  }
#else /* end LONG64, start 32 bit arithmetic */
/*

called by: get_rn_dbl(int)
calls    : add_ssaaaa(), umul_ppmm()  [ longlong.h ]

params   : int *genptr = generator to iterate
returns  : void

Performs the modular multiplication needed to iterate generator 
Xn+1 = (Xn * a) mod (2^N - 1)                                   

*/

	/*
	           aa[]    
		*  Xn[]
	       --------
               result[] -> result[] is split into 2 parts : kk[]rr[] 

	       the new Xn[] = ( kk[] + rr[] ) mod ( 2^n - 1)
	*/
  unsigned long *aa, *Xn, *rr, *kk;
  static unsigned long result[4]; /* should be atleast 2*OP_SIZE */
  static char overflow; /* should be atleast 2*OP_SIZE */
  unsigned long a0, b0, of, temp, temp2;  /* temporary storage variables */
  unsigned long prod_lo, prod_hi, res_lo, res_hi;
  long i,j;  /* counter variables           */
  int param = 0;
  
  aa = ((struct rngen *)genptr)->a;  
  Xn = ((struct rngen *)genptr)->r;

  memset(result,0,4*sizeof(unsigned long)); /* initialize to 0 */
  overflow = 0;
  

  /* result[] = aa[] * Xn[] */
  a0 = aa[0];
  b0 = Xn[0];
  umul_ppmm(prod_hi,prod_lo, a0,b0); 
  result[0] = prod_lo;
  result[1] = prod_hi;
    
  b0 = Xn[1];
  umul_ppmm(prod_hi,prod_lo, a0,b0); 
  res_lo = result[1];
  add_ssaaaa(of,temp2, 0,prod_lo, 0,res_lo);
  result[1] = temp2;
  add_ssaaaa(res_hi,res_lo,0,prod_hi, 0,of);
  result[2] = res_lo;
  overflow = res_hi;

  if(((struct rngen *)genptr)->a_size == 2)
  {
    a0 = aa[1];
    b0 = Xn[0];
    res_lo = result[1];
    res_hi = result[2];
    umul_ppmm(prod_hi,prod_lo, a0,b0); 
    add_ssaaaa(of,temp2, 0,prod_lo, 0,res_lo);
    result[1] = temp2;
    add_ssaaaa(temp,temp2,  0, prod_hi,  0,res_hi);
    add_ssaaaa(res_hi,res_lo,  temp,temp2, 0,of);
    result[2] = res_lo;
    overflow += res_hi;

    b0 = Xn[1];
    res_lo = result[2];
    res_hi = result[3];
    umul_ppmm(prod_hi,prod_lo, a0,b0); 
    add_ssaaaa(of,temp2, 0,prod_lo, 0,res_lo);
    result[2] = temp2;
    add_ssaaaa(temp,of, 0,of, 0,overflow);
    add_ssaaaa(temp,temp2,  0, prod_hi,  0,res_hi);
    add_ssaaaa(res_hi,res_lo,  temp,temp2, 0,of);
    result[3] = res_lo;
    }
    
  /*  rr = low(result) (R)    kk = hi(result) (K)  */
  rr = result;
  kk = result + OP_SIZE;

  /* shift 'kk' left */
  temp2 = 0;
  for (i=0; i<OP_SIZE; i++)
  {
    temp = kk[i];
    kk[i] = ((temp<<RNGBITS)&0xffffffff) + temp2;
    temp2 = temp >> SHIFT;
  }

  /* move extra bits at top of rr[] into start of kk[] */
  temp = rr[OP_SIZE-1];
  temp2 = temp >> SHIFT;
  rr[OP_SIZE-1] = ((temp<<RNGBITS)&0xffffffff) >> RNGBITS;
  kk[0] += temp2;

  /*  Xn+1 = rr + kk   */
  a0 = 0;
  for (i=0; i<OP_SIZE; i++)
  {
    temp = rr[i];
    temp2 = kk[i];
    add_ssaaaa(of,b0, 0,temp, 0,temp2);
    add_ssaaaa(temp2,temp, of,b0,  0,a0);
    Xn[i] = temp;
    a0 = temp2;
  }

  /*  perform mod operation  Xn+1 = Xn+1 mod 2^n - 1 */
  /*  Xn+1 = ( r & (2^n-1) ) + ( r >> n )  */
  temp2 = Xn[OP_SIZE - 1] >> SHIFT;
  Xn[OP_SIZE - 1] &= MASK; 
  for (i=0; i<OP_SIZE; i++)
  {
    temp = Xn[i]; 
    add_ssaaaa(temp2,res_lo, 0,temp, 0,temp2);
    Xn[i] = res_lo;
  }
#endif
  
} /* end of iterate() */


/* Returns a double precision random number */

#ifdef __STDC__
double get_rn_dbl(int *igenptr)
#else
double get_rn_dbl(igenptr)
int *igenptr;
#endif
{
  struct rngen *genptr = (struct rngen *) igenptr;

#ifdef LONG64
  static double dtemp[1] = {0.0};

#ifdef LONG_SPRNG
#define EXPO 0x3ff0000000000000UL
#else
#define EXPO 0x3ff0000000000000ULL
#endif

  iterate(igenptr);

#if defined(CONVEX) || defined(O2K) || defined(SGI) || defined(GENERIC)
  *((unsigned LONG64 *) dtemp) = (genptr->x>>9) | EXPO;
  return *dtemp - (double) 1.0;
#else
  return (genptr->x>>9)*2.2204460492503131e-16;
#endif

#else  /* 32 bit arithmetic */
  double num1,num2;
  long i;

  iterate(igenptr);

  num1 = (double) genptr->r[0];
  num2 = (double) genptr->r[1];
  num2 *= (double) 0XFFFFFFFF + 1.0;
  num1 += num2;	
	
  num2 = (double) 0XFFFFFFFF + 1.0;
  num2 *= (double) 0X1FFFFFFF + 1.0;
  num2 -= 1.0;
  num1 /= num2;        
  
  return (num1);
#endif
  
} 



/* Return a random integer */

#ifdef __STDC__
int get_rn_int(int *igenptr)
#else
int get_rn_int(igenptr)
int *igenptr;
#endif
{
  struct rngen *genptr = (struct rngen *) igenptr;
  
#ifdef LONG64
  iterate(igenptr);
  return (int) (genptr->x>>30);
#else
  unsigned long irn;
  
  iterate(igenptr);

  irn = (genptr->r[1]<<2) | ((genptr->r[0]&0xc0000000)>>30);
  
  return (int) (irn&0x7fffffff); 	
#endif
  
} 



/* Return a single precision random number */

#ifdef __STDC__
float get_rn_flt(int *igenptr)
#else
float get_rn_flt(igenptr)
int *igenptr;
#endif
{
    return (float) get_rn_dbl(igenptr);
}






/*************************************************************************/
/*************************************************************************/
/*                  SPAWN_RNG: spawns new generators                     */
/*************************************************************************/
/*************************************************************************/

#ifdef __STDC__
int spawn_rng(int *igenptr, int nspawned, int ***newgens, int checkid)
#else
int spawn_rng(igenptr,nspawned, newgens, checkid)
int *igenptr,nspawned, ***newgens, checkid;
#endif
{
  struct rngen **genptr, *tempptr = (struct rngen *) igenptr;
  int i;
  
  if (nspawned <= 0) /* is nspawned valid ? */
  {
    nspawned = 1;
    fprintf(stderr,"WARNING - spawn_rng: nspawned <= 0. Default value of 1 used for nspawned\n");
  }
  
  
  genptr = (struct rngen **) 
    initialize(nspawned, &tempptr->si,tempptr->init_seed,
		      tempptr->parameter);
  if(genptr == NULL)	   /* allocate memory for pointers to structures */
  {
    *newgens = NULL;
    return 0;
  }
  else
  {
    *newgens = (int **) genptr;
    for(i=0; i<nspawned; i++)
    {
      genptr[i]->gentype = GENTYPE;
      genptr[i]->stream_number = tempptr->stream_number;
      genptr[i]->nstreams = tempptr->nstreams;
      genptr[i]->init_seed = tempptr->init_seed; 
      genptr[i]->parameter = tempptr->parameter;
  
      genptr[i]->narrays = 0;		/* number of arrays needed by your generator */

      NGENS++;
    }
  }
  
  if(checkid != 0)
    for(i=0; i<nspawned; i++)
      if(addID(( int *) genptr[i]) == NULL)
	return i;
  
  return nspawned;
}


/* Free memory allocated for data structure associated with stream */

#ifdef __STDC__
int free_rng(int *genptr)
#else
int free_rng(genptr)
int *genptr;
#endif
{
  struct rngen *q;
  int i;
  
  q = (struct rngen *) genptr;
  assert(q != NULL);
  
  
  mpz_clear(&(q->k));
  mpz_clear(&(q->si));

  free(q);

  NGENS--;
  return NGENS;
}


#ifdef __STDC__
int pack_rng( int *genptr, char **buffer)
#else
int pack_rng(genptr,buffer)
int *genptr;
char **buffer;
#endif
{
  char *temp_buffer;
  int size, i;
  struct rngen *q;
  int pos=0;

  q = (struct rngen *) genptr;
  size = sizeof(struct rngen) + q->narrays*sizeof(int) + strlen(q->gentype)+1;
  size += q->k._mp_alloc*sizeof(mp_limb_t);
  size += q->si._mp_alloc*sizeof(mp_limb_t);
  
  temp_buffer = (char *) mymalloc(size); /* allocate memory */
  if(temp_buffer == NULL)
  {
    *buffer = NULL;
    return 0;
  }
  
  strcpy(temp_buffer+pos,q->gentype);
  pos += strlen(q->gentype)+1;
  memcpy(temp_buffer+pos,q,sizeof(struct rngen));
  pos += sizeof(struct rngen);
  
  
  memcpy(temp_buffer+pos,q->k._mp_d,q->k._mp_alloc*sizeof(mp_limb_t));
  pos += q->k._mp_alloc*sizeof(mp_limb_t);
  memcpy(temp_buffer+pos,q->si._mp_d,q->si._mp_alloc*sizeof(mp_limb_t));
  pos += q->si._mp_alloc*sizeof(mp_limb_t);

  assert(pos == size);
  
  *buffer = temp_buffer;
  return size;
}



#ifdef __STDC__
int *unpack_rng( char *packed)
#else
int *unpack_rng(packed)
char *packed;
#endif
{
  struct rngen *q;
  int i;
  int pos=0;

  q = (struct rngen *) mymalloc(sizeof(struct rngen));
  if(q == NULL)
    return NULL;

  if(strcmp(packed+pos,GENTYPE) != 0)
  {
    fprintf(stderr,"ERROR: Unpacked ' %.24s ' instead of ' %s '\n",  
	    packed+pos, GENTYPE); 
    return NULL; 
  }
  else
    q->gentype = GENTYPE;
  pos += strlen(q->gentype)+1;
    
  memcpy(q,packed+pos,sizeof(struct rngen));
  pos += sizeof(struct rngen);

    
  q->k._mp_d = (mp_limb_t *) mymalloc(q->k._mp_alloc*sizeof(mp_limb_t));
  q->si._mp_d = (mp_limb_t *) mymalloc(q->si._mp_alloc*sizeof(mp_limb_t));
  if(q->k._mp_d == NULL || q->si._mp_d == NULL)
    return NULL;
  memcpy(q->k._mp_d,packed+pos,q->k._mp_alloc*sizeof(mp_limb_t));
  pos += q->k._mp_alloc*sizeof(mp_limb_t);
  memcpy(q->si._mp_d,packed+pos,q->si._mp_alloc*sizeof(mp_limb_t));
  pos += q->si._mp_alloc*sizeof(mp_limb_t);
    
  NGENS++;
  
  return (int *) q;
}

      

#ifdef __STDC__
int get_seed_rng(int *gen)
#else
int get_seed_rng(gen)
int *gen;
#endif
{
  return ((struct rngen *) gen)->init_seed;
}



#ifdef __STDC__
int print_rng( int *igen)
#else
int print_rng(igen)
int *igen;
#endif
{
  struct rngen *gen;
  
  printf("\n%s\n", GENTYPE+2);
  
  gen = (struct rngen *) igen;
  printf("\n \tseed = %d, stream_number = %d\tparameter = %d\n\n", gen->init_seed, gen->stream_number, gen->parameter);

  /*#ifdef LONG64
  printf("multiplier = %llu, seed = %llu\n", gen->mult, gen->x);
  #endif*/
  return 1;
}


#include "../simple_.h"
#include "../fwrap_.h"
