static char rcsid[] = "$Id: sim_random.c,v 1.2 2005/06/27 19:01:08 svitak Exp $";

/*
** $Log: sim_random.c,v $
** Revision 1.2  2005/06/27 19:01:08  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.7  2001/04/25 17:17:02  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.6  2000/05/23 20:07:29  mhucka
** Removed the init and print statements from setrand.  The init statement in
** particular was bad because it caused the seed to be reset each time.  The
** print statement was unnecessary, and if the seed was never initialized, it
** also printed an unnecessary warning about that.  Things seem to work fine
** without the init statement.
**
** Revision 1.5  2000/04/24 06:11:55  mhucka
** Small fixes for making this work with SPRNG.
**
** Revision 1.4  1998/04/21 22:14:49  dhb
** Added conditional compile for SPRNG code
**
** Revision 1.3  1998/01/08 23:51:11  dhb
** Changes to support additional random number generators.  Added
** do_setRNG() which allows user to switch dynamically between
** the NR and SPRNG generators.
**
** Revision 1.2  1993/07/21 21:32:47  dhb
** fixed rcsid variable type
**
 * Revision 1.1  1992/10/27  20:24:43  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include <time.h>
#include "shell_func_ext.h"
#include "sim_ext.h"

/*
** Now we support more than one RNG system.  Two basic functions are
** to initialize the RNG with a seed and to get the next random number.
** All RNGs will be initialized and seeded when a seed is given.  Only
** one RNG is used to provide numbers, though the user might switch
** between them.
*/

/*
** Random number routines from "Numerical Recipes in C", Chapter 7
** Entered by Michael D. Speight, 10th September 1991
** All references to '*idum' changed to 'idum'
*/
#define M1 259200
#define IA1 7141
#define IC1 54773
#define RM1 (1.0)/M1
#define M2 134456
#define IA2 8121
#define IC2 28411
#define RM2 (1.0)/M2
#define M3 243000
#define IA3 4561
#define IC3 51349

float ran1(idum)
     time_t idum;
{
  static long ix1,ix2,ix3;
  static float r[98];
  float temp;
  static int iff=0;
  int j;
  if (idum < 0 || iff == 0) {
    iff=1;
    ix1=(IC1-(idum)) % M1;
    ix1=(IA1*ix1+IC1) % M1;
    ix2=ix1 % M2;
    ix1=(IA1*ix1+IC1) % M1;
    ix3=ix1 % M3;
    for (j=1;j<=97;j++) {
      ix1=(IA1*ix1+IC1) % M1;
      ix2=(IA2*ix2+IC2) % M2;
      r[j]=(ix1+ix2*RM2)*RM1;
    }
    idum=1;
  }
  ix1=(IA1*ix1+IC1) % M1;
  ix2=(IA2*ix2+IC2) % M2;
  ix3=(IA3*ix3+IC3) % M3;
  j=1 + ((97*ix3)/M3);
  if (j>97||j<1) printf("RAN1: This cannot happen.\n");
  temp=r[j];
  r[j]=(ix1+ix2*RM2)*RM1;
  return temp;
}

#define MBIG 1000000000
#define MSEED 161803398
#define MZ 0
#define FAC (1.0/MBIG)
float ran3(idum)
     time_t idum;
{
  static int inext,inextp;
  static long ma[56];
  static int iff=0;
  long mj,mk;
  int i,ii,k;
  int temp;
  if (idum < 0 || iff == 0) {
    iff=1;
    mj=MSEED-(idum < 0 ? -idum : idum);
    mj %= MBIG;
    ma[55]=mj;
    mk=1;
    for (i=1;i<=54;i++) {
      ii=(21*i) % 55;
      ma[ii]=mk;
      mk=mj-mk;
      if (mk < MZ) mk += MBIG;
      mj=ma[ii];
    }
    for (k=1;k<=4;k++)
      for (i=1;i<=55;i++) {
	/* The original program has the line 
	**	ma[i] -= ma[1+(i+30) % 55];
	** which causes the i860 compiler to croak.
	** Therefore, this work-around.
	** Upinder S. Bhalla, Caltech, 1992
	*/
		temp=(i+30) % 55;
		temp++;
		ma[i] -= ma[temp];
		if (ma[i] < MZ) ma[i] += MBIG;
      }
    inext=0;
    inextp=31;
    idum=1;
  }
  if (++inext == 56) inext=1;
  if (++inextp == 56) inextp=1;
  mj=ma[inext]-ma[inextp];
  if (mj < MZ) mj += MBIG;
  ma[inext]=mj;
  return mj*FAC;
}

/*
** undef the GENESIS CHECK action macro as sprng uses CHECK for
** something else.
*/

#undef CHECK

/*
** SPRNG generator.  Which of the SPRNG supported RNGs is determined
** by which SPRNG library is linked.
*/

#ifdef INCSPRNG
#define	SIMPLE_SPRNG
/*
** For some reason, using FLOAT_GEN with the lfg causes numbers to
** be < .01.  Using double generator I get 0, 1 range as expected.
**
** #define FLOAT_GEN
*/
#include "sprng.h"
#endif


/*
** general interface to GENESIS random numbers
*/

#define	G_RNG_NR	0
#define	G_RNG_SPRNG	1

/*
** default to old standby, the NR RNG
*/

static int	Which_RNG = G_RNG_NR;

/*
** Get net GENESIS random number.  Returns a float between 0 and 1.
*/

float G_RNG()

{
	switch(Which_RNG)
	  {

	  case G_RNG_NR:
	    return ran1(0);

#ifdef INCSPRNG
	  case G_RNG_SPRNG:
	    return (float) sprng();
#endif
	  }

	return 0.0;
}


/*
** Seed the GENESIS random number gneerator(s)
*/

void G_SEEDRNG(seed)

time_t	seed;

{
	ran1(-seed);
#ifdef INCSPRNG
	init_sprng((int)seed, SPRNG_DEFAULT);
#endif
}

void do_setRNG(argc, argv)

int	argc;
char*	argv[];

{

	int	status;

	initopt(argc, argv, "-nr -sprng");
	while ((status = G_getopt(argc, argv)) == 1)
	    if (strcmp(G_optopt, "-nr") == 0)
		Which_RNG = G_RNG_NR;
	    else if (strcmp(G_optopt, "-sprng") == 0)
#ifdef INCSPRNG
		Which_RNG = G_RNG_SPRNG;
#else
		printf("SPRNG has not been compiled into GENESIS\n");
#endif

	if (status < 0)
	  {
	    printoptusage(argc,argv);
	    return;
	  }

	if (IsSilent() < 1)
	    switch (Which_RNG)
	      {
	      case G_RNG_NR:
		printf("Using Numerical Recipes random number generator\n");
		break;

#ifdef INCSPRNG
	      case G_RNG_SPRNG:
		printf("Using SPRNG random number generator\n");
		break;
#endif
	      }
}
