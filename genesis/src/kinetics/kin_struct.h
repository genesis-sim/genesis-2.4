/**********************************************************************
** This program is part of the kinetics library and is
**           copyright (C) 1995 Upinder S. Bhalla.
** It is made available under the terms of the
**           GNU Library General Public License. 
** See the file COPYRIGHT for the full notice.
**********************************************************************/
/* $Id: kin_struct.h,v 1.3 2005/08/12 09:38:17 svitak Exp $ */
 
/*
 * $Log: kin_struct.h,v $
 * Revision 1.3  2005/08/12 09:38:17  svitak
 * Switched comment style from slashslash to slashstar.
 *
 * Revision 1.2  2005/07/01 10:03:04  svitak
 * Misc fixes to address compiler warnings, esp. providing explicit types
 * for all functions and cleaning up unused variables.
 *
 * Revision 1.1.1.1  2005/06/14 04:38:34  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.5  2003/05/29 22:26:40  gen-dbeeman
 * New version of kinetics library from Upi Bhalla, April 2003
 *
 * Revision 1.4  2000/06/12 04:44:47  mhucka
 * Removed nested comments to quiet some compilers.
 *
 * Revision 1.3  1998/07/15 06:45:33  dhb
 * Upi update
 *
 * Revision 1.2  1997/07/24 18:26:26  dhb
 * Library modified to live in the GENESIS distribution source heirarchy
 * and to exclude the 2D interpolation and table modules which have been
 * integrated into the distribution elsewhere.
 *
 * Also excluded ksolve object as it no longer works (per the author).
 *
 * Revision 1.1  1997/07/24 17:49:40  dhb
 * Initial revision
 *
 * Revision 1.3  1994/08/30  21:28:35  bhalla
 * Many changes related to the new ksolve object
 *
 * Revision 1.2  1994/08/05  19:25:24  bhalla
 * Changed many structs so that we can use n rather than conc.
 * Also added conserve flag to the pool for handling conservation eqns.
 *
 * Revision 1.1  1994/06/13  22:55:39  bhalla
 * Initial revision
 * */


#include "struct_defs.h"

/*
** The 2D interpolation and table modules have been integrated into
** the GENESIS distribution elsewhere.  If this library is used in
** some other code and the 2D table code is desired, define
** INCLUDE_INTERP.
*/

#ifdef INCLUDE_INTERP

#ifndef _kin_interp_h
#define _kin_interp_h
#ifndef TABCREATE
#define TABCREATE 200
#endif

/* 2-d interpolation can use either a uniform grid or a nonuniform
** grid. The former is more efficient but not good for handling 
** wide-ranging values. The latter is slower both since a search is
** needed to find the correct grid points, and since the calculations
** are a little longer, but good 
** for grids with varying coarseness. For now only the first is used
*/

struct interpol2d_struct {
    double  xmin;
    double  xmax;
    int     xdivs;
    double  dx;
    double  invdx;
	double	*xgrid;
	short	xgrid_allocated;

    double  ymin;
    double  ymax;
    int     ydivs;
    double  dy;
    double  invdy;
	double	*ygrid;
	short	ygrid_allocated;

	double	sx,sy,sz;
	double	ox,oy,oz;

    double  **table;
    short   calc_mode;	/* This can be one of : NO_INTERP, LIN_INTERP,
						** FIXED */
    short   allocated;
};

typedef struct interpol2d_struct Interpol2D;

#define TAB_IO 0
#define TAB_LOOP 1
#define TAB_ONCE 2
#define TAB_BUF 3

struct table2d_type {
	ELEMENT_TYPE
	double	input;
	double	input2;
	double	output;
	double	negoutput;
	struct	interpol2d_struct *table;
	short	alloced;
	short	step_mode;
	double	stepsize;
};

#endif /* _kin_interp_h */

#endif /* INCLUDE_INTERP */

/* 
**Sequence of processing must be:
** 1. PROCESS of enz and reac. Collects all the concens from the pools
** 		and figures out changes.
** 2. PROCESS of pools. Updates concens using calculations of enz and 
**		reac.
*/

#define KIN_POOL_MSGCOUNT 8

struct pool_type {
	ELEMENT_TYPE
	double	CoTotal;	/* Total concen of C. Needed when C exists in
						** 2 states, and the other should be slaved to
						** this one */
	double	CoInit;		/* Initial value for Co after RESET */
	double	Co;			/* Current concen of C */
	double	CoRemaining;	/* Cototal - Co */
	double	CoMin;		/* minimum value (usually 0) that Co can take*/
	double	n;			/* Current # of pool molecules */
	double	nInit;		/* Initial value for n after RESET */
	double 	nTotal;		/* Total number of pool molecules */
	double	nRemaining;	/* nTotal - n */
	double 	nMin;		/* Minimum allowed value for n */
	double	vol;		/* Volume of pool */
	int		slave_enable;
	int		keepconc;	/* flag for whether to change concs or n when
						** vol changes */
	int		consv_flag;	/* Internal flag keeping track of presence
						** of CONSERVE and SUMTOTAL msgs */
	int		stoch_flag;	/* Flag for doing stochastic calculations */
	int		mirror_src;	/* Flag for mirror sources */
	/* Private fields */
	double**	msgdata;
	int		mirrormsgoffset; /* Saves start of mirror msgs */
	int		msgcount[KIN_POOL_MSGCOUNT];
	int		was_slaved;
	int		oldmsgmode;
};

/* 
** These numbers apply when stochastic integration is in progress.
** They limit the value that the probability of a transition may
** reach. If the probability is greater than KIN_STOCH_UPPER_PROB_LIMIT
** the system switches into continuous integration mode. If it falls
** below KIN_STOCH_LOWER_PROB_LIMIT it goes back into stoch mode.
** There are two limits so as to introduce some hysteresis and
** prevent 'surfing' oscillatory behaviour.
*/
extern double KIN_STOCH_PROB_LIMIT;
/* This sets a minimum on the number of molecules that must be
 * present before continuous integration is allowed
 *
 */
#define KIN_STOCH_EPSILON 1e-9
extern double KIN_STOCH_MOL_LIMIT;

struct reac_type {
	ELEMENT_TYPE
	double	A,B;		/* state variables used in communicating 
						** with pools */
	float	kf,kb;		/* forward and backward rate consts */
	int		stoch_flag;	/* Flag for doing stochastic calculations */
	int		sub_stoch_flag; /* Flag for forward part of reacn */
	int		prd_stoch_flag; /* Flag for backward part of reacn */
  int dep_graph_index;
	/* private */
	double	**msgdata;
	int		*se[50]; /* Hack. I don't expect more than 20 messages
			from pools, hence 20 slave_enable flags. */
	int		msgcount[2];
	int		oldmsgmode;
};

struct enz_type {
	ELEMENT_TYPE
	double	CoComplexInit;	/* Initial value for CoComplex */
	double	CoComplex;	/* enzyme complex */
	double	nComplexInit;	/* Initial value for nComplex */
	double	nComplex;	/* enzyme complex */
	double	vol;		/* enzyme vol: normally slaved to pool vol */
	double	sA,pA,eA,B;	/* values for returning msgs to pools */ 
	float	k1,k2,k3;	/* Michaelis-Menten parameters */
	int		keepconc;	/* flag for whether to change concs or n when
						** vol changes */
	int		usecomplex;	/* Flag for whether the enz complex can be
						** used in other reactions */
	int		stoch_flag;	/* Flag for doing stochastic calculations */
	int		sub_stoch_flag;
	int		complex_stoch_flag;
	int		propensity_problem;
  int dep_graph_index;
	/* private fields */
	float	ksum;		/* ksum = k2+k3, set automatically */
	double	**msgdata;
	int		*se[50]; /* Hack. I don't expect more than 20 messages
			from pools, hence 20 slave_enable flags. */
	int		msgcount[5];
	int		oldmsgmode;
};

struct concchan_type {
	ELEMENT_TYPE
	double	A,B;		/* state variables used in communicating 
						** with pools */
	float	perm;		/* Permeability */
	float	gmax;		/* conductance */
	double	n;			/* number of concchans */
	double	Vm;			/* potential across channel */
	/* private */
	int	is_active;
	int	use_nernst;
};

#if 0
/* For each coeff, the product of the pool 'n's with the coeff is
** summed back onto the destpool. This arrangement handles all
** enz and reacs. 
*/
struct reac_coeff_type {
	double coeff;	/* the rate const */
	int	npools;		/* the number of pools involved */
	double	**n;	/* pointers to the pools; */
	double	*destpool;	/* pointer to the pool whose rate consts are
						** being calculated */
};

/* Calculates all terms for a simple first-order reacn */
/* ddest = -dsrc = kf * src - kb * dest */
struct reac_1_1_type {
	double *src;
	double *dest;
	double kf;
	double kb;
};

/* Calculates all terms for a simple 2nd-order reacn */
/* ddest = -dsrc = kf * src1 * src2 - kb * dest */
struct reac_2_1_type {
	double *src1;
	double *src2;
	double *dest;
	double kf;
	double kb;
};

/* Calculates all terms for a simple 3rd-order reacn */
/* ddest = -dsrc = kf * src1 * src2 * src3 - kb * dest */
struct reac_3_1_type {
	double *src1;
	double *src2;
	double *src3;
	double *dest;
	double kf;
	double kb;
};
#endif

/* This isn't used in the integration steps, but for
** housekeeping and other not-too-expensive operations
*/
struct pool_coeff_type {
	double *pool;
	double init;
};

/* Calculates all terms for a simple nth-order reacn */
/* ddest = -dsrc = kf * prd(srci) - kb * prd(desti) */
struct reac_coeff_type {
	double **pools; /* first the src, then NULL, then prd, then NULL*/
	double kf;
	double kb;
};

/* Calculates all terms for an enz reaction */
/* dsub =  -k1 * enz * PRD(sub) + k2 * enzcomplex
	dprd = k3 * enzcomplex
	denz = dsub + dprd
*/
struct enz_coeff_type {
	double k1,k2,k3;
	double enzcomplexinit;
	double *enzcomplex;
	double *enz;
	double **prd;
	double **sub;
};
/* Calculates terms for concchan */
/* Calculates dnsrc = -dndest = 
	-nchan * (perm_by_vol1 * nsrc - perm_by_vol2 * ndest)
*/
struct concchan_coeff_type {
	double *chan;	/* pointer to the channel n */
	double perm_by_vol1;	/* coeff for source pool */
	double *src;	/* n of source */
	double perm_by_vol2;	/* coeff for dest pool */
	double *dest;	/* n of dest */
};

/* Calculates npool = nTot - sum(n) to handle conservation reactions.
** This is done after all other calculations in case any of the
** other eager calculations changed npool. In a reflection of
** a similar hack with the pool, if nTot is -ve then it uses
** npool = sum(nothers) */
struct conserve_coeff_type {
	double	*pool;
	double	tot;
	double	init;
	int		num_others;
	double	**others;
};

struct handle_msgins {
	int clockno; /* this is the largest dt of the src and dest
			** It is used to decide whether to sample the msg */
	double	nexttime;
	int nmsgins; /* Number of incoming msgs handled at this dt */
	Element **msgelm; /* Array of pointers to the elms getting msg */
	MsgIn **msgin; /* array of pointers to the msgs itself */
	double **destpool; /* destination pools */
};

struct gsolve_type
{
  ELEMENT_TYPE
    /*
     * gsolve fields 
     */
  /*
   * int stoch_flag;
 */
/*
   * Flag for doing stochastic calculations 
 */
  /*
   * Private fields 
   */
  double **msgdata;
  int mirrormsgoffset;		/* Saves start of mirror msgs */
  int msgcount[KIN_POOL_MSGCOUNT];
  int was_slaved;
  int oldmsgmode;
  ElementList *reac_list;
  int reac_list_populated;
  ElementList *enz_list;
  int enz_list_populated;
  ElementList *pool_list;
  int pool_list_populated;
  ElementList *sumtotal_list;  /* List of pools handling sumtotals */
  double propensity_reac[2][500];	/* stores propensity values, one
				 * dimension for forward/backward,
				 * other for different reactions */
  double propensity_enz[3][300];	/* stores propensity values for
									 enzymes */ 
  double propensity_total;
  double internal_time;
  int method;
};

struct handle_msgouts {
	int clockno; /* this is clockno of the largest dt of the
			** src and dest.
			** It is used to decide whether to update the field */
	double	nexttime;
	int		nmsgouts; /* number of fields needing update at this dt*/
	Element	**msgelm; /* array of pointers to the elms with msgouts */
	double	**srcpool; /* array of source pools */
	int		*isenz;
};

struct kentry_type {
	int type;
	char	*coeff;
	double	*pool;
};

struct ksolve_type {
	ELEMENT_TYPE
	double	currtime;/* current time to which integration has been done.
					** Note that this may exceed Simtime because 
					** ksolve uses adaptive timesteps */
	double	lasttime; /* previous time found */
	double	nextstep; /* next time step to use */
	double	minstep;	/* minimum allowed time step */
	int		nok;		/* count the good and bad steps */
	int		nbad;
	double	vol;
	double	*xtrapvector;
	double	*xtrapmatrix;
	char	*path;
	int		method;
	double	eps;
	char	*elm_to_pointer; /* Hash table for finding pointers */
	int		npools;
	double	*pools; /* successive entries are explained in the
					** ksolve.c. There are 8 entries for each pool */
					/* The array is filled in order, by pools, enzs,
					** sumtots, consvs, slaves, and dummys. We assume
					** that the correct state is known at setup time
					** so that the pools can be ordered efficiently,
					** with the pools, enzs and sumtots first, since
					** they are the only ones that need to be
					** integrated. */
	int		npoolcoeffs;
	struct	pool_coeff_type *poolcoeffs;
	int		nreacs;
	struct	reac_coeff_type *reacs;
	int		nenzs;
	struct	enz_coeff_type *enzs;
	int		nconcchans;
	struct	concchan_coeff_type *concchans;
	int		nconsvs; /* consvs and sumtots share a common data struct */
	int		nsumtots;
	struct	conserve_coeff_type *consvs;
	int		nslaves;
	int		nmsgins;
	struct	handle_msgins	*msgins;
	int		nmsgouts;
	struct	handle_msgouts	*msgouts;
	int		nkentries;
	struct	kentry_type *kentries;
};

#define TEXT_START_SIZE 1000

struct text_type {
	ELEMENT_TYPE
	char *str;
	int len;
};

