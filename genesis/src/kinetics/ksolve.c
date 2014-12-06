/**********************************************************************
** This program is part of the kinetics library and is
**           copyright (C) 1995 Upinder S. Bhalla.
** It is made available under the terms of the
**           GNU Library General Public License. 
** See the file COPYRIGHT for the full notice.
**********************************************************************/
static char rcsid[] = "$Id: ksolve.c,v 1.2 2005/10/16 20:48:35 svitak Exp $";
 
/* $Log: ksolve.c,v $
/* Revision 1.2  2005/10/16 20:48:35  svitak
/* Changed hard-coded character arrays from 100 to 256 or malloc'd and freed
/* memory instead. This was only done where directory paths might have easily
/* exceeded 100 characters.
/*
 * Revision 1.1.1.1  2005/06/14 04:38:34  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.2  2001/04/18 22:39:37  mhucka
 * Miscellaneous portability fixes, mainly for SGI IRIX.
 *
 * Revision 1.1  1997/07/24 17:49:40  dhb
 * Initial revision
 *
 * Revision 1.4  1994/09/29  18:24:45  bhalla
 * Much delayed entry. This current version is not working anymore,
 * because of changes to the pool object to do with CONSERVE and
 * SUMTOTAL msgs, and also because of changes with the slave_enable flag.
 *
 * In its last working form, it had handled runge-kutta and also
 * assorted variants of the exponential Euler method. All worked, but
 * none was as stable as the ordinary Genesis exp-Euler. I am
 * suspicious of the stepsize routines, but the underlying problem may
 * simply be that the equations are stiff and need an implicit solver.
 *
 * Revision 1.3  1994/09/06  14:27:54  bhalla
 * Partly debugged version with B-S method. Still need to put in code
 * to correctly handle the YN array for consvs. At this point it works
 * with ordinary reactions plus slave msgins.
 *
 * Revision 1.2  1994/09/05  18:17:35  bhalla
 * Working Forward Euler, complete but buggy Bulirsch-Stoer.
 *
 * Revision 1.1  1994/08/30  21:27:49  bhalla
 * Initial revision
 *
 */

#include "kin_ext.h"
#include "hash.h"

#define KSOLVE_ARRAY_WIDTH 9
#define KSOLVE_CURR_Y 5
#define KSOLVE_CURR_DY 4
#define KSOLVE_LAST_Y 3
#define KSOLVE_YSCAL 2
#define KSOLVE_YSEQ 1
#define KSOLVE_YM 0
#define KSOLVE_YN 7
#define KSOLVE_YT 8
#define KSOLVE_YERR 6
#define KSOLVE_LAST_DY KSOLVE_YERR

#define KSOLVE_IMAX 11
#define KSOLVE_NUSE 7
#define KSOLVE_SHRINK 0.95
#define KSOLVE_GROW 1.2
#define KMAXSTEPS 1000
#define KTINY 1e-30
#define KPOOL	1
#define KENZ	2
#define KREAC	3
#define KCONCCHAN	4
#define KCONSV	5
#define KSUMTOT	6
#define KSUMCONSV	7
#define KNSLAVE	8
#define KCONCSLAVE	9
#define KBUFFER	10
#define KBULIRSCH_STOER	20
#define KRUNGE_KUTTA	19
#define KEULER			-1
#define KEEULER			0


/*
** The ksolve object handles the integration of groups of the
** following kinetics objects:
** pool enz reac concchan
** Later it will also handle diffusion between pools.
**
** A note on implementation of the B-S algorithm. In order
** to minimize cache thrashing and avoid repeated allocs, I
** use a large array of doubles holding the pool values and
** assorted temporary arrays. Referring to the NumRec book, we
** need the following arrays:
** mmid: y dydx yout | ym yn
** bsstep: y dydx yscal |
**	ysav dysav yseq yerr (of these dysav is unnecessary
**	since dydx is not changed by mmid)
** rzextr: yest yz dy |
** odeint: ystart |  yscal y dydx
** Considering calling sequence, the unique vectors we need are:
** 	yold y==ym dydx yscal ysav=KSOLVE_LAST_Y yseq yerr==yn
** for cleanness, we'll use KSOLVE_ARRAY_WIDTH = 8
** rather than seven vectors.
*/

static int ScanKsolvePath();
static int ScanManagedMsgOuts();
static int ScanSlaveMsgIns();
static int DoKsolveInteg();
static int DoKsolveUpdate();
static int DoKsolveGather();
static int DoDerivs();
static int DoSumtotsAndConsvs();
static int DoConsvs();
static void TeenageMutantMidpointMethod();
static void KBSstep();
static void RichExtrap();
static ENTRY *ksolve_hash_find();
static ENTRY *ksolve_hash_enter();
static int ksolve_hash_function();
static int PoolSpecialType();
static MsgIn *GetSlaveMsgIn();
static void DoBSDriver();
static void TestRichExtrap();
static void DoKRunge();
static void DoRKDriver();
static void DoRKStep();


KsolveFunc(ksolve,action)
	struct ksolve_type *ksolve;
	Action		*action;
{
MsgIn	*msg;

    if(debug > 1){
		ActionHeader("ReactionPool",ksolve,action);
    }

    SELECT_ACTION(action){
    case PROCESS:
        		MSGLOOP(ksolve,msg) {
            		case 0:		/* TYPE = DIFFUSION */
        			break;
					case 1:		/* TYPE = VOL */
						/*	ksolve->vol = MSGVALUE(msg,0); */
					break;
        		}
				/* Gather the info from incoming msgs to slaves */
				DoKsolveGather(ksolve);
				/* Do the integration */
				DoKsolveInteg(ksolve);
				/* Update the outgoing msgs */
				DoKsolveUpdate(ksolve);
		break;
    case RESET: {
		double *pools = ksolve->pools + KSOLVE_CURR_Y;
		int i,j; /* j keeps track of pool addresses */
		double pooltot;
		struct conserve_coeff_type *cc;

#ifdef TESTRICHEXTRAP
		TestRichExtrap(ksolve);
#endif
		/* May wish to set all child volumes here */

		/* Start up all pools at their init values */
		for (j=0,i = 0; i < ksolve->npoolcoeffs; i++,
			j+=KSOLVE_ARRAY_WIDTH)
			pools[j] = ksolve->poolcoeffs[i].init;
		/* Start up all enzs at their init values */
		for (i = 0; i < ksolve->nenzs; i++,j+=KSOLVE_ARRAY_WIDTH)
			pools[j] = ksolve->enzs[i].enzcomplexinit;
		/* Start up all consvs and sumtots at their init values */
		for (i = 0; i < (ksolve->nsumtots + ksolve->nconsvs);
			i++,j+=KSOLVE_ARRAY_WIDTH)
			pools[j] = ksolve->consvs[i].init;
		/* Set up the slaves to their initial values too */

		/* Set up all sumtots to the total specified by the
		** sum of the init values */
		for (cc = ksolve->consvs, i = 0; i < ksolve->nsumtots;i++,cc++){
			pooltot = 0;
			for (j = 0; j < cc->num_others; j++)
				pooltot += *(KSOLVE_CURR_Y + cc->others[j]);
			cc->tot = pooltot;
		}
		/* continuing, now setting the consvs. In this case we need
		** to add in the init of the current element as well */
		for (i = 0; i < ksolve->nconsvs;i++,cc++) {
			pooltot = cc->init;
			for (j = 0; j < cc->num_others; j++)
				pooltot += *(KSOLVE_CURR_Y + cc->others[j]);
			cc->tot = pooltot;
		}

		/* Make sure all clocks are starting at zero */
		for (i = 0 ; i < ksolve->nmsgouts; i++)
			ksolve->msgouts[i].nexttime = 0;
		for (i = 0 ; i < ksolve->nmsgins; i++)
			ksolve->msgins[i].nexttime = 0;

		/* Set the integration management stuff */
		ksolve->nextstep = Clockrate(ksolve);
		ksolve->currtime = ksolve->lasttime = 0;

		/* random default */
		if (ksolve->minstep <= 0)
			ksolve->minstep = 1e-9 * Clockrate(ksolve);

		ksolve->nok = ksolve->nbad = 0;
		}
    break;
	case SETUP:
		/* This is where we load in the ksolve children */
		ScanKsolvePath(ksolve);
	break;
	case CREATE:
		ksolve->vol = 1; /* need to initialize this */
		ksolve->npools = 0;
		ksolve->pools = NULL; /* Make sure that this is NULL */
		ksolve->nenzs = 0;
		ksolve->nreacs = 0;
		ksolve->nconcchans = 0;
		ksolve->nconsvs = 0;
		ksolve->nsumtots = 0;
		ksolve->nmsgins = 0;
		ksolve->nmsgouts = 0;
		ksolve->nkentries = 0;
		ksolve->eps = 0.005;	/* sensible starting point */
		ksolve->minstep = -1;	/* The reset sets it to a default */
	break;
    case SET :
		if (action->argc == 2) {
			char *field = action->argv[0];
			double newval;
				/* Do the normal set on all the fields */
				return(0);
		}
        return(0); /* do the normal set */
		break;
	case DELETE: { /* free a lot of stuff here */
			int i;
			ElementList	*elist,*WildcardGetElement();
	
			if (ksolve->path)
				elist = WildcardGetElement(ksolve->path);
			if (!elist) return(0);
			for (i = 0; i < elist->nelements; i++) {
				HsolveEnable(elist->element[i]);
			}
			FreeElementList(elist);
		}
		break;
	}
}

/*
**	Operations 					Data struct
**	Basic calculations			pools, *_coeffs arrays
**	Incoming msgs				handle_msgins, pools and coeffs array
**	Outgoing msgs				handle_msgouts
**	Setting fields				elm_to_pointer lookup
**	Getting fields				elm_to_pointer lookup
**	Setting up calculations		elm_to_pointer lookup
**
** I need to maintain a hash table for every pool (including
** the virtual pools in enzymes). This table contains the
** indices for looking up the pools in the pools array.
** - The reacs and concs, while they do not
** have any messages that I need to worry about, have fields
** that may change. So I also need to keep track of where
** they are stored. This means a pointer to the appropriate
** reac_coeff_type structure. 
** - Enzymes are the messiest, since
** there we need to look up both the coeffs and the enzyme pool.
** For this purpose, we first look up the enz_coeff_type
** array, and then seek out the enzcomplex pointer in it.
** - The next problem is: how do we identify what the 
** object is ? We could encode a higher-order bit in the 
** index with this info, or we could simply have an auxiliary
** structure with the index and type info, or just do the ElementIsA.
** - How do we go from the internal array to the object ? Would
** 	we ever need to ?
** - We need to keep info about updating the elements, eg clocks etc.
** 	This includes info about the elements with outgoing messages 
**	outside the purview of the ksolve. Perhaps a list of structs
**	with the clock info followed by a list of all the elements.
** - How about a struct with the info for each element:
**	. pointer in ksolve
**	. Element pointer
**	. type
**	This involves a lot of storage, and is inefficient for the runtime
**	phase. For example, the clock ids should really each be their own
**	list, each of which is called when needed, rather than scanning
**	the whole list each timestep. On the other hand, we would have
**	duplication of info that way too.
*/

static int ScanKsolvePath(ksolve)
	struct ksolve_type *ksolve;
{
	ElementList	*elist,*WildcardGetElement();
	struct pool_type	*pool;
	struct enz_type		*enz;
	struct reac_type	*reac;
	struct concchan_type	*concchan;
	HASH	*elm_to_pointer,*hash_create();
	ENTRY	entry;
	ENTRY	*tempentry;
	struct kentry_type	*kentries;
	MsgIn	*msg;
	MsgOut	*msgout;
	int i,j,k;
	int npools, npoolcoeffs, nenzs, nreacs, nconcchans, nconsvs,
		nsumtots, nslaves, nsumconsvs, nbuffers;
	int start_enzs, start_sumtots, start_consvs, start_slaves;

/*******************************************************************/

	/* set up a temporary elist */
	if (ksolve->path)
		elist = WildcardGetElement(ksolve->path);
	if (!elist) return(0);
	if (elist->nelements == 0) return(0);
	j=0;

/*******************************************************************/

	/* create the hash table */
	if (ksolve->elm_to_pointer) {
		free(ksolve->elm_to_pointer);
		ksolve->elm_to_pointer = NULL;
	}
	elm_to_pointer = hash_create(elist->nelements * 2);
	ksolve->elm_to_pointer = (char *)elm_to_pointer;

/*******************************************************************/

	/* create the entry table */
	if (ksolve->nkentries > 0) {
		free(ksolve->kentries);
		ksolve->kentries = NULL;
	}
	ksolve->kentries = kentries = (struct kentry_type *)
		calloc(elist->nelements, sizeof(struct kentry_type));
	/* Fill up the hash table. It will index the kentries */
	ksolve->nkentries = elist->nelements;
	for (i = 0; i < elist->nelements; i++) {
			entry.data =(char *)i;
			entry.key = (char *)elist->element[i];
			ksolve_hash_enter(&entry,elm_to_pointer);
	}


/*******************************************************************/

	/* count the elements of each type and enter in the kentries */
	/* Disable elements if they are of the approved types */
	npools = npoolcoeffs = nenzs = nreacs =  nconcchans =  nconsvs =
		nsumtots = nslaves = nsumconsvs = nbuffers = 0;
	for (i = 0; i < elist->nelements; i++) {
		if (ElementIsA(elist->element[i],"pool")) {
			HsolveBlock(elist->element[i]);
			npools++;
			kentries[i].type = PoolSpecialType(elist->element[i]);
			switch (kentries[i].type) {
				case KSUMTOT:
					nsumtots++;
					break;
				case KCONSV:
					nconsvs++;
					break;
				case KSUMCONSV:
					nsumconsvs++;
					break;
				case KNSLAVE:
					nslaves++;
					break;
				case KCONCSLAVE:
					nslaves++;
					break;
				case KBUFFER:
					nbuffers++;
					break;
				case KPOOL:
					npoolcoeffs++;
					break;
			}
		} else if (ElementIsA(elist->element[i],"enz")) {
			kentries[i].type = KENZ;
			HsolveBlock(elist->element[i]);
			nenzs++;
			npools++; /* This accounts for the enzyme complexes */
		} else if (ElementIsA(elist->element[i],"reac")) {
			kentries[i].type = KREAC;
			HsolveBlock(elist->element[i]);
			nreacs++;
		} else if (ElementIsA(elist->element[i],"concchan")) {
			kentries[i].type = KCONCCHAN;
			HsolveBlock(elist->element[i]);
			nconcchans++;
		}
	}

/*******************************************************************/


	/* Allocate the arrays */

	if (ksolve->npoolcoeffs > 0)
		free(ksolve->poolcoeffs);
	ksolve->npoolcoeffs = npoolcoeffs;
	if (npoolcoeffs > 0)
		ksolve->poolcoeffs = (struct pool_coeff_type *)
			calloc(npoolcoeffs, sizeof(struct pool_coeff_type));

	if (ksolve->npools > 0)
		free(ksolve->pools);
	ksolve->npools = npools;
	if (ksolve->npools > 0)
		ksolve->pools = (double *) calloc(KSOLVE_ARRAY_WIDTH * npools,
			sizeof(double));

	if (ksolve->nenzs > 0) free(ksolve->enzs);
	ksolve->nenzs = nenzs;
	if (ksolve->nenzs > 0)
		ksolve->enzs = (struct enz_coeff_type *)
			calloc(nenzs, sizeof(struct enz_coeff_type));

	if (ksolve->nreacs > 0) free(ksolve->reacs);
	ksolve->nreacs = nreacs;
	if (ksolve->nreacs > 0)
	ksolve->reacs = (struct reac_coeff_type *)
		calloc(nreacs, sizeof(struct reac_coeff_type));

	if (ksolve->nconcchans > 0) free(ksolve->concchans);
	ksolve->nconcchans = nconcchans;
	if (ksolve->nconcchans > 0)
		ksolve->concchans = (struct concchan_coeff_type *)
		calloc(nconcchans, sizeof(struct concchan_coeff_type));

	if (ksolve->nconsvs + ksolve->nsumtots > 0) free(ksolve->consvs);
	ksolve->nconsvs = nconsvs;
	ksolve->nsumtots = nsumtots;
	if (ksolve->nconsvs + ksolve ->nsumtots > 0)
		ksolve->consvs = (struct conserve_coeff_type *)
			calloc(nconsvs + nsumtots,
				sizeof(struct conserve_coeff_type));

	/* The slaves are handled by the handle_msgins structs later */
	ksolve->nslaves = nslaves;

/*******************************************************************/

	/* Fill in the kentries list and assign locations for each pool */
	/* Order in the pools array is: pools, enzs, sumtots, consvs,
	** slaves, dummys */
	npools = npoolcoeffs = nenzs = nreacs =  nconcchans = 
		nconsvs = nsumtots = nslaves = 0;
	start_enzs = ksolve->npoolcoeffs;
	start_sumtots = start_enzs + ksolve->nenzs;
	start_consvs = start_sumtots + ksolve->nsumtots;
	start_slaves = start_consvs + ksolve->nconsvs;
	for (i = 0; i < elist->nelements; i++) {
		switch (kentries[i].type) {
			case KPOOL:
				kentries[i].coeff =
					(char *)(ksolve->poolcoeffs + npoolcoeffs);
				kentries[i].pool = 
				ksolve->poolcoeffs[npoolcoeffs].pool =
					ksolve->pools + npoolcoeffs * KSOLVE_ARRAY_WIDTH;
				npoolcoeffs++;
				break;
			case KCONSV:
				/* The consvs come in after the sumtots */
				kentries[i].coeff = (char *)
					(ksolve->consvs + ksolve->nsumtots + nconsvs);
				kentries[i].pool = 
				ksolve->consvs[ksolve->nsumtots + nconsvs].pool =
					ksolve->pools + (start_consvs + nconsvs) *
					KSOLVE_ARRAY_WIDTH;
				nconsvs++;
				break;
			case KSUMTOT:
				kentries[i].coeff =(char *)(ksolve->consvs + nsumtots);
				kentries[i].pool = 
				ksolve->consvs[nsumtots].pool =
					ksolve->pools + (start_sumtots + nsumtots) *
					KSOLVE_ARRAY_WIDTH;
				nsumtots++;
				break;
			case KENZ:
				kentries[i].coeff = (char *)(ksolve->enzs + nenzs);
				kentries[i].pool = 
				ksolve->enzs[nenzs].enzcomplex =
					ksolve->pools + (start_enzs + nenzs) *
					KSOLVE_ARRAY_WIDTH;
				nenzs++;
				break;
			case KREAC:
				kentries[i].coeff = (char *)(ksolve->reacs + nreacs);
				kentries[i].pool = NULL;
				nreacs++;
				break;
			case KCONCCHAN:
				kentries[i].coeff =
					(char *)(ksolve->concchans + nconcchans);
				kentries[i].pool = NULL;
				nconcchans++;
				break;
			case KNSLAVE:
				kentries[i].coeff = NULL;/*this is handled by msgins*/ 
				kentries[i].pool = 
					ksolve->pools + (start_slaves + nslaves) *
					KSOLVE_ARRAY_WIDTH;
				nslaves++;
				break;
		}
	}

/*******************************************************************/


/* Step through elist scanning each entry and load in
** suitable fields for internal calculations.  */
	for (i = 0; i < elist->nelements; i++) {
		switch (kentries[i].type) {
			case KPOOL: {
				struct pool_coeff_type *poolcoeffs;
				pool = (struct pool_type *) elist->element[i];
				poolcoeffs = (struct pool_coeff_type *)
					ksolve->kentries[i].coeff;
			/*	poolcoeffs->vol = pool->vol; */
				poolcoeffs->init = pool->nInit;
				}
				break;
			case KCONSV: 
			case KSUMTOT: {
				double *others[100]; /* an absurd upper limit */
				int	num_others = 0;
				Element *elm;
				struct conserve_coeff_type *consvs;

				pool = (struct pool_type *) elist->element[i];
				MSGLOOP(pool,msg) {
					case 4:	/* type = SUMTOTAL */
						elm = msg->src;
						if (tempentry = ksolve_hash_find(
							(char *)elm, elm_to_pointer)) {
								others[num_others] = ksolve->kentries[
									(int)tempentry->data].pool;
								num_others++;
						} else {
						/* Foreign sumtotals will be handled by
						** msgins */
						}
					break;
				}
				/* Need to fix this to deal with new approach to consv
				** and sumtots */
				/*
				consvs = (struct conserve_coeff_type *)
					ksolve->kentries[i].coeff;
				consvs->tot = (pool->conserve) ? -pool->nTotal : 
					pool->nTotal;
				consvs->init = pool->nInit;
				consvs->num_others = num_others;
				consvs->others = (double **) calloc(num_others, 
					sizeof(double *));
				for(j = 0; j < num_others; j++)
					consvs->others[j] = others[j];
				*/
				}
				break;
			case KENZ: {
					double *tempsub[10]; /* absurdly high */
					int	nsub = 0;
					double *tempprd[10]; /* absurdly high */
					int	nprd = 0;
					struct enz_coeff_type *enzcoeff = 
						(struct enz_coeff_type *)
						ksolve->kentries[i].coeff;
		
					enz = (struct enz_type *) elist->element[i];
					enzcoeff->k1 = enz->k1;
					enzcoeff->k2 = enz->k2;
					enzcoeff->k3 = enz->k3;
					enzcoeff->enzcomplexinit = enz->nComplexInit;
					MSGLOOP(enz,msg) {
						case 0:	/* type = ENZYME */
						if (tempentry = ksolve_hash_find((char *)msg->src,
							elm_to_pointer)) {
							enzcoeff->enz =
							ksolve->kentries[(int)tempentry->data].pool;
						} else {
							/* this should never happen */
							return;
						}
						break;
						case 1:	/* type = SUBSTRATE */
						if (tempentry = ksolve_hash_find((char *)msg->src,
							elm_to_pointer)) {
							tempsub[nsub] =
							ksolve->kentries[(int)tempentry->data].pool;
							nsub++;
						} else {
							/* this should never happen */
							return;
						}
						break;
						case 2:	/* type = VOL */
						break;
					}
					MSGOUTLOOP(enz,msgout) {
						Element *elm = msgout->dst;
						msg = GetMsgInByMsgOut(elm,msgout);
						if (msg) {
							/* msg = msgout->msgin; */
							if (tempentry =
								ksolve_hash_find((char *)elm,
								elm_to_pointer)) {
								j = (int)tempentry->data;
								/* check that elm is a pool */
								if (ksolve->kentries[j].pool != NULL) {
									if (msg->type == 1) { /* MM_PRD */
										tempprd[nprd] = 
											ksolve->kentries[j].pool;
										nprd++;
									}
								}
							} else {
								/* this is handled by the HandleMsgout
								** function */
							}
						} else {
							printf("Warning: missing msgin for enz %s while setting up %s\n",
							Pathname(enz),Pathname(ksolve));
						}
					}
					enzcoeff->sub = (double **) calloc(1 + nsub, 
						sizeof(double *));
						for(j = 0; j < nsub; j++)
							enzcoeff->sub[j] = tempsub[j];
					enzcoeff->sub[nsub] = NULL;
					enzcoeff->prd = (double **) calloc(1 + nprd, 
						sizeof(double *));
						for(j = 0; j < nprd; j++)
							enzcoeff->prd[j] = tempprd[j];
					enzcoeff->prd[nprd] = NULL;
					if (nsub == 0) enzcoeff->k1 = 0;
				}
				break;
			case KREAC: {
					double *tempsub[10]; /* absurdly high */
					int	nsub = 0;
					double *tempprd[10]; /* absurdly high */
					int	nprd = 0;
					struct reac_coeff_type *reaccoeff =
						(struct reac_coeff_type *)
						ksolve->kentries[i].coeff;
		
					reac = (struct reac_type *) elist->element[i];
					reaccoeff->kf = reac->kf;
					reaccoeff->kb = reac->kb;
					MSGLOOP(reac,msg) {
						case 0:	/* type = SUBSTRATE */
						if (tempentry =
							ksolve_hash_find((char *)msg->src,
							elm_to_pointer)) {
							tempsub[nsub] = 
							ksolve->kentries[(int)tempentry->data].pool;
							nsub++;
						} else {
							/* this should never happen */
							return;
						}
						break;
						case 1:	/* type = PRODUCT */
						if (tempentry = ksolve_hash_find((char *)msg->src,
							elm_to_pointer)) {
							tempprd[nprd] =
							ksolve->kentries[(int)tempentry->data].pool;
							nprd++;
						} else {
							/* this should never happen */
							return;
						}
						break;
					}
					reaccoeff->pools =
						(double **) calloc(2 + nsub + nprd, 
						sizeof(double *));
					for(k = 0; k < nsub; k++)
						reaccoeff->pools[k] = tempsub[k];
					reaccoeff->pools[k] = NULL;
					k++;
					for(j = 0; j < nprd; j++,k++)
						reaccoeff->pools[k] = tempprd[j];
					reaccoeff->pools[k] = NULL;
					if (nsub == 0) reaccoeff->kf = 0;
					if (nprd == 0) reaccoeff->kb = 0;
				}
				break;
			case KCONCCHAN: {
					struct concchan_coeff_type *concchancoeff =
						(struct concchan_coeff_type *)
						ksolve->kentries[i].coeff;
		
					concchan =
						(struct concchan_type *) elist->element[i];
					MSGLOOP(concchan,msg) {
						case 0:	/* type = SUBSTRATE */
						pool = (struct pool_type *)msg->src;
						concchancoeff->perm_by_vol1 =
							concchan->perm/pool->vol;
						if (tempentry =
							ksolve_hash_find((char *)msg->src,
							elm_to_pointer)) {
							concchancoeff->src =
							ksolve->kentries[(int)tempentry->data].pool;
						} else {
							/* this should never happen */
							return;
						}
						break;
						case 1:	/* type = PRODUCT */
						pool = (struct pool_type *)msg->src;
						concchancoeff->perm_by_vol2 =
							concchan->perm/pool->vol;
						if (tempentry =
							ksolve_hash_find((char *)msg->src,
							elm_to_pointer)) {
							concchancoeff->dest =
							ksolve->kentries[(int)tempentry->data].pool;
						} else {
							/* this should never happen */
							return;
						}
						break;
						case 2:	/* type = NUMCHAN */
						if (tempentry =
							ksolve_hash_find((char *)msg->src,
							elm_to_pointer)) {
							concchancoeff->chan = 
							ksolve->kentries[(int)tempentry->data].pool;
						} else {
							/* this should never happen */
							return;
						}
						break;
						case 3:	/* type = Vm */
						break;
					}
				}
				break;
		}
	}

/*******************************************************************/
/* Step through elist scanning each entry for the outgoing
** msgs. The clock of each dest is noted and used to determine how
** often the msg field needs updating */

	ScanManagedMsgOuts(ksolve,elist);

/*******************************************************************/

/* Step through hash table scanning each entry for the incoming
** msgs. If it is one that the ksolve understands, set up a msg 
** directly to the ksolve from the source element. */
	ScanSlaveMsgIns(ksolve,elist);

/*******************************************************************/

/* Set up the special matrices for the B_S method */
	ksolve->xtrapvector = (double *)calloc(KSOLVE_IMAX,sizeof(double));
	ksolve->xtrapmatrix = (double *)calloc(
		KSOLVE_ARRAY_WIDTH *
		(ksolve->npoolcoeffs + ksolve->nenzs + ksolve->nsumtots),
		sizeof(double));

/*******************************************************************/


/* Free temporary arrays like elist */
	FreeElementList(elist);
}

/*******************************************************************
**
**
**		Solver Setup routines for outgoing messages
**
**
*******************************************************************/


#define KSOLVE_MAX_MSGOUTS 1000
static int ScanManagedMsgOuts(ksolve,elist)
	struct ksolve_type *ksolve;
	ElementList	*elist;
{
/* Step through elist scanning each entry for the outgoing
** msgs. The clock of each dest is noted and used to determine how
** often the msg field needs updating */
	struct temp_msgout_type {
		int clockno;	/* clock for this msg */
		Element	*msgelm; /* element to be updated */
		int		isenz;	/* 0 if pool, 1 if enz */
		int		fieldtype;	/* type info */
		double *srcpool; /* source of info for update */
	};

	int i,j,k;
	Element *elm;
	MsgOut *msgout;
	/* Temporary array of outgoing msgs */
	struct temp_msgout_type *tempmsgouts =
		(struct temp_msgout_type *) calloc (
		KSOLVE_MAX_MSGOUTS,sizeof (struct temp_msgout_type));
	double dt,destdt;
	int nclocks[NCLOCKS];
	int	idclocks[NCLOCKS];
	int ntempmsgouts;
	ENTRY	*tempentry;
	int	elmclock;

	/* Zap existing msgouts */
	if (ksolve->nmsgouts > 0 && ksolve->msgouts != NULL) {
		for (i = 0; i < ksolve->nmsgouts; i++) {
			j = ksolve->msgouts[i].nmsgouts;
			if (j > 0) {
				if (ksolve->msgouts[i].msgelm != NULL)
					free(ksolve->msgouts[i].msgelm);
				if (ksolve->msgouts[i].srcpool != NULL)
					free(ksolve->msgouts[i].srcpool);
				if (ksolve->msgouts[i].isenz != NULL)
					free(ksolve->msgouts[i].isenz);
			}
		}
		free(ksolve->msgouts);
		ksolve->msgouts = NULL;
	}

	j = 0; /* count the tempmsgouts */
	for (i = 0; i < elist->nelements; i++) {
		elm = elist->element[i];

		dt = Clockrate(elm);
		elmclock = Clock(elm);
		switch(ksolve->kentries[i].type) {
			case KPOOL:
			case KCONSV:
			case KSUMTOT:
			case KNSLAVE:
			case KENZ:
				MSGOUTLOOP(elm,msgout) {
					/* look for outgoing messages whose dest is not on
					** the list */
					if (!(tempentry =
						ksolve_hash_find((char *)msgout->dst,
						ksolve->elm_to_pointer))) {
						/* Assume that the only output lines will be
						** Co and n, and update both. It isn't worth
						** the overhead in trying to narrow it down
						** any further.
						*/
						destdt = Clockrate(msgout->dst);
						tempmsgouts[j].clockno = (dt > destdt) ?
							elmclock : Clock(msgout->dst);
						tempmsgouts[j].srcpool =
							ksolve->kentries[i].pool;
						tempmsgouts[j].msgelm = elm;
						tempmsgouts[j].isenz = 
							(ksolve->kentries[i].type == KENZ) ? 1 : 0;
						j++;
						if (j >= KSOLVE_MAX_MSGOUTS) {
							printf("Error in ScanManagedMsgOuts: %s\n",
								Pathname(ksolve));
							return(-1);
						}
						break; /* each elm must be updated just once */
					}
				}
			break;
			default: /* we do not care about these cases */
			break;
		}
	}
	/* Sort the tempmsgs in terms of clocks */
	ntempmsgouts = j;
	for (i = 0; i < NCLOCKS; i++) 
		nclocks[i] = idclocks[i] = 0;
	/* count the # of cases for each clock */
	for (i = 0; i < ntempmsgouts; i++)
		nclocks[tempmsgouts[i].clockno]++;
	k =0; /* count the number of handle_msgins */
	for (i = 0; i < NCLOCKS; i++) {
		if (nclocks[i] > 0) {
			idclocks[k] = i;
			k++;
		}
	}
	/* Set up the msgouts */
	ksolve->nmsgouts = k;
	ksolve->msgouts = (struct handle_msgouts *)
		calloc(k,sizeof(struct handle_msgouts));
	/* Fill up the msgouts */
	for (k = 0; k < ksolve->nmsgouts; k++) {
		int clockno = idclocks[k];
		ksolve->msgouts[k].nmsgouts = nclocks[clockno];
		ksolve->msgouts[k].msgelm = (Element **) calloc(
			nclocks[clockno],sizeof(Element *));
		ksolve->msgouts[k].srcpool = (double **) calloc(
			nclocks[clockno],sizeof(double *));
		ksolve->msgouts[k].isenz = (int *) calloc(
			nclocks[clockno],sizeof(int));
		ksolve->msgouts[k].clockno =  clockno;
		ksolve->msgouts[k].nexttime =  0;
		/* here j counts the number of elms filled in so far */
		j = 0;
		for(i = 0; i < ntempmsgouts; i++) {
			if (clockno == tempmsgouts[i].clockno) {
				ksolve->msgouts[k].msgelm[j] =
					tempmsgouts[i].msgelm;
				ksolve->msgouts[k].srcpool[j] =
					tempmsgouts[i].srcpool;
				ksolve->msgouts[k].isenz[j] =
					tempmsgouts[i].isenz;
				j++;
			}
		}
	}
	
	free(tempmsgouts); /* Free tempmsgs */
}

/*******************************************************************
**
**
**		Solver Setup routines for slave messages
**
**
*******************************************************************/


#define KSOLVE_MAX_MSGINS 1000
static int ScanSlaveMsgIns(ksolve,elist)
	struct ksolve_type *ksolve;
	ElementList	*elist;
{
/* Step through elist scanning each entry for the incoming SLAVE
** msgs. The clock of each dest is noted and used to determine how
** often the msg field needs updating */
	struct temp_msgin_type {
		int clockno;	/* clock for this msg */
		Element	*msgelm; /* enslaved element */
		MsgIn	*msgin;	/* message */
		double *destpool; /* destination of info for update */
	};

	int i,j,k;
	Element *elm;
	MsgIn *msgin;
	/* Temporary array of outgoing msgs */
	struct temp_msgin_type *tempmsgins =
		(struct temp_msgin_type *) calloc (
		ksolve->nslaves,sizeof (struct temp_msgin_type));
	int nclocks[NCLOCKS];
	int	idclocks[NCLOCKS];
	int ntempmsgins;
	ENTRY	*tempentry;
	int	elmclock;

	/* Zap existing msgins */
	if (ksolve->nmsgins > 0 && ksolve->msgins != NULL) {
		for (i = 0; i < ksolve->nmsgins; i++) {
			j = ksolve->msgins[i].nmsgins;
			if (j > 0) {
				if (ksolve->msgins[i].msgelm != NULL)
					free(ksolve->msgins[i].msgelm);
				if (ksolve->msgins[i].destpool != NULL)
					free(ksolve->msgins[i].destpool);
				if (ksolve->msgins[i].msgin != NULL)
					free(ksolve->msgins[i].msgin);
			}
		}
		free(ksolve->msgins);
		ksolve->msgins = NULL;
	}

	ntempmsgins = 0; /* count the tempmsgins */
	for (i = 0; i < elist->nelements; i++) {
		elm = elist->element[i];
		elmclock = Clock(elm);
		switch(ksolve->kentries[i].type) {
			case KNSLAVE:
				if (msgin = GetSlaveMsgIn(elm,msgin)) {
					if (ntempmsgins >= ksolve->nslaves) {
						printf("Error in ScanSlaveMsgIns: %s\n",
							Pathname(ksolve));
						return(-1);
					}
					tempmsgins[ntempmsgins].clockno = elmclock;
					tempmsgins[ntempmsgins].destpool =
						ksolve->kentries[i].pool;
					tempmsgins[ntempmsgins].msgelm = elm;
					tempmsgins[ntempmsgins].msgin = msgin;
					ntempmsgins++;
					break; /* each elm must have only 1 slave msg */
				}
			break;
			default: /* we do not care about these cases */
			break;
		}
	}
	/* Sort the tempmsgs in terms of clocks */
	for (i = 0; i < NCLOCKS; i++) 
		nclocks[i] = idclocks[i] = 0;
	/* count the # of cases for each clock */
	for (i = 0; i < ntempmsgins; i++)
		nclocks[tempmsgins[i].clockno]++;
	k =0; /* count the number of handle_msgins */
	for (i = 0; i < NCLOCKS; i++) {
		if (nclocks[i] > 0) {
			idclocks[k] = i;
			k++;
		}
	}
	/* Set up the msgins */
	ksolve->nmsgins = k;
	ksolve->msgins = (struct handle_msgins *)
		calloc(k,sizeof(struct handle_msgins));
	/* Fill up the msgins */
	for (k = 0; k < ksolve->nmsgins; k++) {
		int clockno = idclocks[k];
		ksolve->msgins[k].nmsgins = nclocks[clockno];
		ksolve->msgins[k].msgelm = (Element **) calloc(
			nclocks[clockno],sizeof(Element *));
		ksolve->msgins[k].destpool = (double **) calloc(
			nclocks[clockno],sizeof(double *));
		ksolve->msgins[k].msgin = (MsgIn **) calloc(
			nclocks[clockno],sizeof(MsgIn *));
		ksolve->msgins[k].clockno =  clockno;
		ksolve->msgins[k].nexttime =  0;
		/* here j counts the number of elms filled in so far */
		j = 0;
		for(i = 0; i < ntempmsgins; i++) {
			if (clockno == tempmsgins[i].clockno) {
				ksolve->msgins[k].msgelm[j] =
					tempmsgins[i].msgelm;
				ksolve->msgins[k].destpool[j] =
					tempmsgins[i].destpool;
				ksolve->msgins[k].msgin[j] =
					tempmsgins[i].msgin;
				j++;
			}
		}
	}
	
	free(tempmsgins); /* Free tempmsgs */
}

static int DoKsolveInteg(ksolve)
	struct ksolve_type *ksolve;
{
	int nv = KSOLVE_ARRAY_WIDTH * 
		(ksolve->npoolcoeffs + ksolve->nenzs + ksolve->nsumtots);

	if (ksolve->method == KEULER) { /* forward Euler */
		int i,j;
		double dt = Clockrate(ksolve);
		double *pools = ksolve->pools + KSOLVE_CURR_Y;
		double *dpools = ksolve->pools + KSOLVE_CURR_DY;

		for(j=0; j < nv;j+=KSOLVE_ARRAY_WIDTH)
			dpools[j]  = 0;
		DoDerivs(ksolve,KSOLVE_CURR_Y,KSOLVE_CURR_DY);
		for(j=0; j < nv; j+=KSOLVE_ARRAY_WIDTH)
			pools[j] += dpools[j] * dt;
		/* Do the updates for the sumtotals and consvs */
		DoSumtotsAndConsvs(ksolve,KSOLVE_CURR_Y);
	} else if (ksolve->method == KEEULER) { /* Exponential Euler */
		int i,j;
		double dt = Clockrate(ksolve);
		double *pools = ksolve->pools + KSOLVE_CURR_Y;
		double *dpools = ksolve->pools + KSOLVE_CURR_DY;
		double A,B,C,D;
		for(j=0; j < nv; j+=KSOLVE_ARRAY_WIDTH)
			dpools[j]  = 0;
		DoDerivs(ksolve,KSOLVE_CURR_Y,KSOLVE_CURR_DY);
		for(j=0; j < nv; j+=KSOLVE_ARRAY_WIDTH) {
			C = pools[j];
			A = dpools[j];
			if (C > fabs(A) && C > KTINY) {
				D = exp(dt * A/(C*2));
				pools[j] = (C + C) * D  - C;
			} else {
				pools[j] += A * dt;
			}
		}
		DoSumtotsAndConsvs(ksolve,KSOLVE_CURR_Y);
	} else if (ksolve->method == 1) { /* Exponential Euler, vs 2 */
		int i,j;
		double dt = Clockrate(ksolve);
		double *pools = ksolve->pools + KSOLVE_CURR_Y;
		double *dpools = ksolve->pools + KSOLVE_CURR_DY;
		double A,B,C,D;
		for(j=0; j < nv; j+=KSOLVE_ARRAY_WIDTH)
			dpools[j]  = 0;
		DoDerivs(ksolve,KSOLVE_CURR_Y,KSOLVE_CURR_DY);
		for(j=0; j < nv; j+=KSOLVE_ARRAY_WIDTH) {
			C = pools[j];
			A = dpools[j];
			if (A > 0 || C <= 0) {
				pools[j] += A * dt;
			} else if (A < 0) {
				D = exp(dt * A/C);
				pools[j] = C * D;
			}
		}
		DoSumtotsAndConsvs(ksolve,KSOLVE_CURR_Y);
	} else if (ksolve->method == 2) { /* Exponential Euler, vs 3 */
		int i,j;
		double dt = Clockrate(ksolve);
		double *pools = ksolve->pools + KSOLVE_CURR_Y;
		double *dpools = ksolve->pools + KSOLVE_CURR_DY;
		double A,B,C,D;
		for(j=0; j < nv; j+=KSOLVE_ARRAY_WIDTH)
			dpools[j]  = 0;
		DoDerivs(ksolve,KSOLVE_CURR_Y,KSOLVE_CURR_DY);
		for(j=0; j < nv; j+=KSOLVE_ARRAY_WIDTH) {
			C = pools[j];
			A = dpools[j];
			if (C <= 0) {
				pools[j] += A * dt;
			} else if (A > 0) {
				B = -A * 0.5 * dt / C;
				A *= 1.5;
				D = exp(B);
				pools[j] = C * D + 3 * (1 - D);
			} else if (A < 0) {
				B = A * 1.5 * dt / C;
				A *= -0.5;
				D = exp(B);
				pools[j] = C * D + 0.333333333333 * (1 - D);
			}
		}
		DoSumtotsAndConsvs(ksolve,KSOLVE_CURR_Y);
	} else if (ksolve->method == KBULIRSCH_STOER) { /* Bulirsch-Stoer */
		/* Check if simtime > internal time.
		** Later we might want to do interpolation based on this. 
		** For now, we'll just integrate through */
		DoBSDriver(ksolve);
	} else if (ksolve->method == KRUNGE_KUTTA) { /* Runge_Kutta */
		/* Check if simtime > internal time.
		** Later we might want to do interpolation based on this. 
		** For now, we'll just integrate through */
		DoRKDriver(ksolve);
	}
}

/*******************************************************************
**
**
**		Message handling routines
**
**
*******************************************************************/


/* This routine updates the values in the pools to allow
** incoming msgs to be handled. If the msg is a slave, then
** we just have to set the value of the corresponding pool.
** The case of handling inputs to reacs, enzs and concs is
** too messy to handle at this point, and we may just want to
** restrict all foreign msgs to SLAVE msgs.
*/
static int DoKsolveGather(ksolve)
	struct ksolve_type *ksolve;
{
	double simtime = SimulationTime();
	int 	nupdates;
	Element **elm;
	double **destpool;
	struct pool_type *pool;
	MsgIn	**msg;
	int i,j;

	for (i = 0; i < ksolve->nmsgins; i++) {
		if (simtime >= ksolve->msgins[i].nexttime) {
			elm = ksolve->msgins[i].msgelm;
			msg = ksolve->msgins[i].msgin;
			destpool = ksolve->msgins[i].destpool;
			for(j = 0; j < ksolve->msgins[i].nmsgins; j++) {
				pool = (struct pool_type *)elm[j];
				if (pool->slave_enable == 1) /* slave to n */
					*(destpool[j] + KSOLVE_CURR_Y) = MSGVALUE(msg[j],0);
				else /* slave to Co */
					*(destpool[j] + KSOLVE_CURR_Y) =
						pool->vol * MSGVALUE(msg[j],0);
			}
			ksolve->msgins[i].nexttime +=
				ClockValue(ksolve->msgins[i].clockno);
		}
	}
}

/* This routine updates the values in the elements so as to
** allow messages to continue to flow smoothly.
*/
static int DoKsolveUpdate(ksolve)
	struct ksolve_type *ksolve;
{
	double simtime = SimulationTime();
	int 	nupdates;
	Element **elm;
	double **srcpool;
	int	*isenz;
	struct pool_type *pool;
	struct enz_type *enz;
	int i,j;


	for (i = 0; i < ksolve->nmsgouts; i++) {
		if (simtime >= ksolve->msgouts[i].nexttime) {
			elm = ksolve->msgouts[i].msgelm;
			srcpool = ksolve->msgouts[i].srcpool;
			isenz = ksolve->msgouts[i].isenz;
			for(j = 0; j < ksolve->msgouts[i].nmsgouts; j++) {
				if (isenz[j]) {
					enz = (struct enz_type *)elm[j];
					enz->CoComplex = (enz->nComplex =
						*(KSOLVE_CURR_Y + srcpool[j])) /
						enz->vol;
				} else {
					pool = (struct pool_type *)elm[j];
					pool->Co = (pool->n =
						*(KSOLVE_CURR_Y + srcpool[j])) / pool->vol;
				}
			}
			ksolve->msgouts[i].nexttime +=
				ClockValue(ksolve->msgouts[i].clockno);
		}
	}
}

/*******************************************************************
**
**
**		Routines for the Runge-Kutta method
**
**
*******************************************************************/

static void DoRKDriver(ksolve)
	struct ksolve_type *ksolve;
{
	int	nsteps;
	int	nvar = 
		KSOLVE_ARRAY_WIDTH * 
		(ksolve->npoolcoeffs + ksolve->nenzs + ksolve->nsumtots);
	double *yscal = ksolve->pools + KSOLVE_YSCAL;
	double *y = ksolve->pools + KSOLVE_CURR_Y;
	double *dy = ksolve->pools + KSOLVE_CURR_DY;
	double dt = Clockrate(ksolve);
	double h = ksolve->nextstep;
	double hdid;
	double endstep = SimulationTime() + dt;
	int	i,j,k;

	for(nsteps = 0; nsteps < KMAXSTEPS; nsteps++) {
		if (ksolve->currtime + h > endstep)
			h = endstep - ksolve->currtime;
		if (h <= 0) return;
		DoDerivs(ksolve,KSOLVE_CURR_Y,KSOLVE_CURR_DY);
		for(i=0 ; i < nvar; i+= KSOLVE_ARRAY_WIDTH) {
			yscal[i] = fabs(y[i]) + fabs(dy[i] * h) + KTINY;
		}
		DoRKStep(ksolve,h,&hdid);
		if (hdid == h)
			ksolve->nok++;
		else 
			ksolve->nbad++;
		ksolve->lasttime = ksolve->currtime;
		ksolve->currtime += hdid;
		if (ksolve->currtime >= endstep) {
						/* end of this step */
			return;
		}
		if (ksolve->nextstep <= ksolve->minstep) {
			printf("Stepsize %g too small in RKDriver in %s\n",
				ksolve->nextstep,Pathname(ksolve));
		}
		h = ksolve->nextstep;
	}
	Warning();
	printf("Too many steps in RKDriver in %s\n", Pathname(ksolve));
}

static void DoKRunge(ksolve,y, dydx,yout,h)
	struct ksolve_type *ksolve;
	double	*y, *dydx, *yout;
	double h;
{
	int i;

	int nv = KSOLVE_ARRAY_WIDTH *
		(ksolve->npoolcoeffs + ksolve->nenzs + ksolve->nsumtots);

	double *dym = ksolve->pools + KSOLVE_YM;
	double *dyt = ksolve->pools + KSOLVE_YSEQ;
	double *yt = ksolve->pools + KSOLVE_YT;

	double hh = h * 0.5;
	double h6  = h/6.0;

	for (i = 0; i < nv ; i+= KSOLVE_ARRAY_WIDTH)
		yt[i] = y[i] + hh * dydx[i];
	DoSumtotsAndConsvs(ksolve,KSOLVE_YT);
	DoDerivs(ksolve,KSOLVE_YT,KSOLVE_YSEQ);
	for (i = 0; i < nv ; i+= KSOLVE_ARRAY_WIDTH) 
		yt[i] = y[i] + hh * dyt[i];
	DoSumtotsAndConsvs(ksolve,KSOLVE_YT);
	DoDerivs(ksolve,KSOLVE_YT,KSOLVE_YM);
	for (i = 0; i < nv ; i+= KSOLVE_ARRAY_WIDTH)  {
		yt[i] = y[i] + h * dym[i];
		dym[i] += dyt[i];
	}
	DoSumtotsAndConsvs(ksolve,KSOLVE_YT);
	DoDerivs(ksolve,KSOLVE_YT,KSOLVE_YSEQ);
	for (i = 0; i < nv ; i+= KSOLVE_ARRAY_WIDTH) 
		yout[i] = y[i] + h6 * (dydx[i] + dyt[i] + 2.0 * dym[i]);
}


#define K_ERRCON	6e-4
#define K_PGROW		-0.2
#define K_PSHRINK	-0.25
#define K_SAFETY	0.9
#define K_FCOR		0.06666666666666

static void DoRKStep(ksolve,htry,hdid)
	struct ksolve_type *ksolve;
	double htry;
	double *hdid;
{
	int nv = KSOLVE_ARRAY_WIDTH *
		(ksolve->npoolcoeffs + ksolve->nenzs + ksolve->nsumtots);
	int	i,j,k;
	double *ysav = ksolve->pools + KSOLVE_LAST_Y;
	double *dysav = ksolve->pools + KSOLVE_LAST_DY;
	double *y = ksolve->pools + KSOLVE_CURR_Y;
	double *dy = ksolve->pools + KSOLVE_CURR_DY;
	double *ytemp = ksolve->pools + KSOLVE_YN;
	double *yscal = ksolve->pools + KSOLVE_YSCAL;

	double h=htry;
	double hh;
	double temp,xest;
	double errmax;

	/* This loop needs to include the consvs. We do not need
	** to recalculate the consvs, because it is just copying
	** from the old values. In later steps in the method
	** we will need to run the DoConsvs routine to update
	** the consvs and the sumtots */
	k = KSOLVE_ARRAY_WIDTH *
		(ksolve->npoolcoeffs + ksolve->nenzs + ksolve->nsumtots + 
		ksolve->nconsvs);
	for (i = 0; i < k; i+=KSOLVE_ARRAY_WIDTH) {
		ysav[i] = y[i];
		dysav[i] = dy[i];
	}

	if (ksolve->nslaves > 0) {
		/* We need to set the YT and LAST_Y entries for slaves, because
		** these are used in the DoKRunge method */
		int startslave = nv + KSOLVE_ARRAY_WIDTH * ksolve->nconsvs;
		int endslave = startslave +
			KSOLVE_ARRAY_WIDTH * ksolve->nslaves;
		double *yt = ksolve->pools + KSOLVE_YT;
		for (i = startslave; i < endslave; i+=KSOLVE_ARRAY_WIDTH) {
			yt[i] = ytemp[i] = ysav[i] = y[i];
		}
	}

	for (;;) {
		hh = 0.5 * h;
		DoKRunge(ksolve,ysav,dysav,ytemp,hh);
		DoDerivs(ksolve,KSOLVE_YN,KSOLVE_CURR_DY);
		DoKRunge(ksolve,ytemp,dy,y,hh);
		if (ksolve->currtime + h == ksolve->currtime) {
			printf("Step size %g too small in DoKRungeStep in %s\n",
				h,Pathname(ksolve));
			return;
		}
		DoKRunge(ksolve,ysav,dysav,ytemp,h);
		errmax  = 0.0;
		for(i = 0; i < nv; i += KSOLVE_ARRAY_WIDTH)  {
			ytemp[i] = y[i] - ytemp[i];
			temp = fabs(ytemp[i]/yscal[i]);
			if (errmax < temp)
				errmax = temp;
		}
		errmax /= ksolve->eps;
		if (errmax  <= 1.0) {
			*hdid = h;
			ksolve->nextstep = (errmax > K_ERRCON) ? 
				K_SAFETY * h * exp(K_PGROW * log(errmax)) : 4.0 * h;
			break;
		}
		h *= K_SAFETY * exp(K_PSHRINK * log(errmax));
	}
	for (i = 0; i < nv; i+=KSOLVE_ARRAY_WIDTH) {
		y[i] += ytemp[i] * K_FCOR;
	}
}

#undef K_ERRCON
#undef K_PGROW
#undef K_PSHRINK
#undef K_SAFETY
#undef K_FCOR

/*******************************************************************
**
**
**		Routines for the Bulirsch-Stoer method
**
**
*******************************************************************/

/* This function is loosely based on the NumRecC routine mmid.
** It incorporates several cache-optimizing changes, and also
** avoids repeated allocs and frees
** It assumes that the pools array contains values in the sequence
** defined at the start of the file.
*/
static void TeenageMutantMidpointMethod(ksolve, htot, nstep)
	struct ksolve_type *ksolve;
	double htot;
	int	nstep;
{
	double h=htot/nstep;
	double h2=2*h;
	int i,n;
	double	z;
	int	nvar = 
		(ksolve->npoolcoeffs + ksolve->nenzs + ksolve->nsumtots) *
		KSOLVE_ARRAY_WIDTH;

	double	*pools = ksolve->pools + KSOLVE_LAST_Y;
	double	*ym = ksolve->pools + KSOLVE_YM;
	double	*yn = ksolve->pools + KSOLVE_YN;
	double	*dy = ksolve->pools + KSOLVE_CURR_DY;
	double	*yout = ksolve->pools + KSOLVE_YSEQ;

	/* First step */
	for (i = 0; i < nvar; i+=KSOLVE_ARRAY_WIDTH) {
		ym[i] = pools[i];
		yn[i] = pools[i] + h * dy[i];
	}
	DoSumtotsAndConsvs(ksolve,KSOLVE_YN);
	DoDerivs(ksolve,KSOLVE_YN,KSOLVE_YSEQ);
	/* General steps */
	for (n = 1; n < nstep; n++) {
		for (i = 0; i < nvar; i+=KSOLVE_ARRAY_WIDTH) {
			z = ym[i] + h2 * yout[i];
			ym[i] = yn[i];
			yn[i] = z;
		}
		DoSumtotsAndConsvs(ksolve,KSOLVE_YN);
		DoDerivs(ksolve,KSOLVE_YN,KSOLVE_YSEQ);
	}
	/* Last step */
	for (i = 0; i < nvar; i+=KSOLVE_ARRAY_WIDTH)
		yout[i] = 0.5 * (ym[i] + yn[i] + h * yout[i]);
	DoSumtotsAndConsvs(ksolve,KSOLVE_YSEQ);
}

/* #define H_IS_DT 1 */
static void DoBSDriver(ksolve)
	struct ksolve_type *ksolve;
{
	int	nsteps;
	int	nvar = 
		KSOLVE_ARRAY_WIDTH * 
		(ksolve->npoolcoeffs + ksolve->nenzs + ksolve->nsumtots);
	double *yscal = ksolve->pools + KSOLVE_YSCAL;
	double *y = ksolve->pools + KSOLVE_CURR_Y;
	double *dy = ksolve->pools + KSOLVE_CURR_DY;
	double dt = Clockrate(ksolve);
	/* temporary hack */
#ifdef H_IS_DT
	double h = dt;
#else
	double h = ksolve->nextstep;
#endif
	double hdid;
	double endstep = SimulationTime() + dt;
	int	i,j,k;

	for(nsteps = 0; nsteps < KMAXSTEPS; nsteps++) {
		if (ksolve->currtime + h > endstep)
			h = endstep - ksolve->currtime;
		if (h <= 0) return;
		DoDerivs(ksolve,KSOLVE_CURR_Y,KSOLVE_CURR_DY);
		for(i=0 ; i < nvar; i+= KSOLVE_ARRAY_WIDTH) {
			yscal[i] = fabs(y[i]) + fabs(dy[i] * h) + KTINY;
		}
		KBSstep(ksolve,h,&hdid);
		if (hdid == h)
			ksolve->nok++;
		else 
			ksolve->nbad++;
		ksolve->lasttime = ksolve->currtime;
		ksolve->currtime += hdid;
		if (ksolve->currtime >= endstep) {
						/* end of this step */
			return;
		}
		if (ksolve->nextstep <= ksolve->minstep) {
			/* Warning(); */
			printf("Stepsize %g too small in BSDriver in %s\n",
				ksolve->nextstep,Pathname(ksolve));
		}
#ifdef H_IS_DT
		h = dt;
		h = ksolve->nextstep;
#else
		h = ksolve->nextstep;
#endif
	}
	Warning();
	printf("Too many steps in BSDriver in %s\n", Pathname(ksolve));
}
#undef H_IS_DT

/* This function is loosely based on the NumRec bsstep func.
** Again, we avoid most of the allocs and try to be cache
** conscious 
*/
static void KBSstep(ksolve,htry,hdid)
	struct ksolve_type *ksolve;
	double htry;
	double *hdid;
{
	int nv = KSOLVE_ARRAY_WIDTH *
		(ksolve->npoolcoeffs + ksolve->nenzs + ksolve->nsumtots);
	int	i,j,k;
	static int nseq[] = {2,4,6,8,12,16,24,32,48,64,96};
	double *ysav = ksolve->pools + KSOLVE_LAST_Y;
	double *yerr = ksolve->pools + KSOLVE_YERR;
	double *y = ksolve->pools + KSOLVE_CURR_Y;
	double *yscal = ksolve->pools + KSOLVE_YSCAL;

	double h=htry;
	double temp,xest;
	double errmax;

	/* This loop needs to include the consvs. We do not need
	** to recalculate the consvs, because it is just copying
	** from the old values. In later steps in the method
	** we will need to run the DoConsvs routine to update
	** the consvs and the sumtots */
	k = KSOLVE_ARRAY_WIDTH *
		(ksolve->npoolcoeffs + ksolve->nenzs + ksolve->nsumtots + 
		ksolve->nconsvs);
	for (i = 0; i < k; i+=KSOLVE_ARRAY_WIDTH) {
		ysav[i] = y[i];
	}

	if (ksolve->nslaves > 0) {
		/* We need to set the YN and LAST_Y entries for slaves, because
		** these are used in the midpoint method */
		double *yn = ksolve->pools + KSOLVE_YN;
		int startslave = nv + KSOLVE_ARRAY_WIDTH * ksolve->nconsvs;
		int endslave = startslave +
			KSOLVE_ARRAY_WIDTH * ksolve->nslaves;
		for (i = startslave; i < endslave; i+=KSOLVE_ARRAY_WIDTH) {
			yn[i] = ysav[i] = y[i];
		}
	}

	for (;;) {
		for(i = 0;i < KSOLVE_IMAX; i++) {
			TeenageMutantMidpointMethod(ksolve, h, nseq[i]);
			temp = h/nseq[i];
			xest = temp*temp;
			RichExtrap(ksolve,i,xest,nv);
			/* This time the consvs are the only thing that should
			** be updated, since the RichExtrap should put the 
			** correct extrapolated value for the sumtots in CURR_Y
			** from the intermediate
			** calculations of the sumtots.
			*/
			DoConsvs(ksolve,KSOLVE_CURR_Y);
			errmax = 0.0;
			for (j=0; j < nv; j+= KSOLVE_ARRAY_WIDTH) {
				if (errmax < (temp = fabs(yerr[j]/yscal[j])))
					errmax = temp;
			}
			if (errmax < ksolve->eps) { /* see if step converged */
			/* The currtime is advanced in the BSDriver routine */
			/* 	ksolve->currtime += h; */
				*hdid = h;
				ksolve->nextstep = (i == KSOLVE_NUSE - 1) ?
					h * KSOLVE_SHRINK : 
						(i == KSOLVE_NUSE - 2) ?
							h * KSOLVE_GROW :
							(h * nseq[KSOLVE_NUSE - 2])/nseq[i];
				return;
			}
		}
		h *= 0.25;
		for (i = 0; i < (KSOLVE_IMAX - KSOLVE_NUSE)/2; i++)
			h *= 0.5;
		if ((ksolve->currtime + h) == ksolve->currtime) {
			Error();
			printf("step size underflow in ksolver %s\n",
				Pathname(ksolve));
		}
	}
}

/* This is just a utility to test whether the RichExtrap function
** works as advertised. It is purely for debugging purposes.
*/
static void TestRichExtrap(ksolve)
	struct ksolve_type *ksolve;
{
	int nv = KSOLVE_ARRAY_WIDTH *
		(ksolve->npoolcoeffs + ksolve->nenzs + ksolve->nsumtots);
	int i,j;
	double *yz = ksolve->pools + KSOLVE_CURR_Y;
	double *yest = ksolve->pools + KSOLVE_YSEQ;
	double *dy = ksolve->pools + KSOLVE_YERR;
	double xest;

	for (i = 0 ; i < KSOLVE_NUSE; i++) {
		for(j = 0; j < nv; j += KSOLVE_ARRAY_WIDTH) {
			yest[j] = j + 1/(1 + i * i);
		}
		xest = KSOLVE_NUSE - i;
		RichExtrap(ksolve,i,xest,nv);
		printf("\n\n yvals:");
		for(j = 0; j < nv; j += KSOLVE_ARRAY_WIDTH) {
			printf("%g,",yz[j]);
		}
		printf("\n\n errs:");
		for(j = 0; j < nv; j += KSOLVE_ARRAY_WIDTH) {
			printf("%g,",dy[j]);
		}
		printf("\n");
	}
}

/* This function is loosely based on the NumRec rzextr. Here the
** changes due to different indexing schemes and cache management
** are quite drastic.
*/
static void RichExtrap(ksolve,iest,xest,nv)
	struct ksolve_type *ksolve;
	int iest;
	double xest;
	int nv; /* This is the number of variables times KSOLVE_ARRAY_W */
{
	int m1,k,j;
	double	yy,v,ddy,c,b1,b;
	double fx[KSOLVE_NUSE];
	double *x = ksolve->xtrapvector;
	double *d = ksolve->xtrapmatrix;
	double *yz = ksolve->pools + KSOLVE_CURR_Y;
	double *yest = ksolve->pools + KSOLVE_YSEQ;
	double *dy = ksolve->pools + KSOLVE_YERR;
	/* this is defined as a matrix of dim nv by 7, so I just use 
	** the pools indexing with width 8. */

	x[iest] = xest;

	if (iest == 0) {
		for(j = 0; j < nv; j+=KSOLVE_ARRAY_WIDTH) {
			dy[j] = d[j] = yz[j] = yest[j];
		}
	} else {
		m1 = (iest + 1 < KSOLVE_NUSE) ? iest : KSOLVE_NUSE -1;
		for(k = 0; k < m1; k++)
			fx[k+1] = x[iest - k - 1]/xest;
		for(j = 0; j < nv; j+= KSOLVE_ARRAY_WIDTH) {
			v = d[j];
			d[j] = c = yy = yest[j];
			for (k = 1; k <= m1; k++) {
				b1 = fx[k]*v;
				b = b1 - c;
				if (fabs(b) < KTINY)
					printf("Warning: RichExtrap failing at simtime %f on %s\n",SimulationTime(),Pathname(ksolve));
				if (b != 0) { /* avoid division by zero */
					b = (c-v)/b;
					ddy = c*b;
					c = b1 * b;
				} else {
					ddy = v;
				}
				if (k != m1)
					v = d[j+k];
				d[j+k] = ddy;
				yy += ddy;
			}
			dy[j] = ddy;
			yz[j] = yy;
		}
	}
}

/*******************************************************************
**
**
**		General Integration routines
**
**
*******************************************************************/

static int DoSumtotsAndConsvs(ksolve,pooloffset)
	struct ksolve_type *ksolve;
	int	pooloffset;
{
	int i,j;
	int nsumtots = ksolve->nsumtots;
	int	nconsvs = ksolve->nconsvs;
	struct conserve_coeff_type *cc;
	double pooltot,rem;

	for (cc = ksolve->consvs, i = 0; i < nsumtots; cc++, i++) {
		pooltot = 0;
		/* At this point the pool has been integrated to a new value.
		** So we need to apply the total operation to figure out
		** what is really left */

		/* find the total amount of the pool from its contributors */
		for (j = 0; j < cc->num_others; j++)
			pooltot += *(pooloffset + cc->others[j]);
		/* Find out what is left from last times total and this times
		** integrated value */
		rem = cc->tot - *(pooloffset + cc->pool);
		/* The total contributed pool must not be smaller than this */
		if (pooltot < rem)
			pooltot = rem;
		/* The new total contributed pool is saved */
		cc->tot = pooltot;
		/* We work out what is left for the free part of the pool */
		*(cc->pool + pooloffset) = pooltot - rem;
	}
	/* Continuing with the consvs array, now the entries are really
	** consvs, not sumtots */
	for (i = 0; i < nconsvs; cc++, i++) {
	/* Note that in this case we need to compute cc->tot from all
	** the Init values on RESET */
		pooltot = cc->tot;
		for (j = 0; j < cc->num_others; j++)
			pooltot -= *(pooloffset + cc->others[j]);
		*(cc->pool + pooloffset) = (pooltot > 0) ? pooltot : 0;
	}
}

static int DoConsvs(ksolve,pooloffset)
	struct ksolve_type *ksolve;
	int	pooloffset;
{
	int i,j;
	int nsumtots = ksolve->nsumtots;
	int	nconsvs = ksolve->nconsvs;
	struct conserve_coeff_type *cc;
	double pooltot;

	for (cc = ksolve->consvs + nsumtots, i = 0; i < nconsvs; cc++, i++){
	/* Note that in this case we need to compute cc->tot from all
	** the Init values on RESET */
		pooltot = cc->tot;
		for (j = 0; j < cc->num_others; j++)
			pooltot -= *(pooloffset + cc->others[j]);
		*(cc->pool + pooloffset) = (pooltot > 0) ? pooltot : 0;
	}
}

/* This function calculates the derivatives */
/* The pooloffset contains the offset of the pool info wr to
** the pointer in the reacs, enz or conc coeff struct.
** Likewise, the dpooloffset has the offset for the desired
** place to put the differential.
** note that pooloffset must not be the same as dpooloffset
*/
static int DoDerivs(ksolve,pooloffset,dpooloffset)
	struct ksolve_type *ksolve;
	int	pooloffset;
	int	dpooloffset;
{
	double dsub;
	double dprd;
	double denz;
	double **ppool;
	struct reac_coeff_type *rc;
	struct enz_coeff_type *ec;
	struct concchan_coeff_type *cc;
	int i,j = ksolve->npoolcoeffs + ksolve->nenzs;
	int nlivepools = ksolve->npoolcoeffs + ksolve->nenzs + ksolve->nsumtots;
	double *poolptr;
	double	psrc,pprd,dpool;



	/* Clear out the dpools */
	for (i = 0, poolptr = ksolve->pools + dpooloffset;
		i < nlivepools; i++ ,poolptr +=KSOLVE_ARRAY_WIDTH)
		*poolptr = 0.0;
		
	/* Do the reacs */
	for (rc = ksolve->reacs, i = 0 ; i < ksolve->nreacs; rc++, i++) {
		psrc=rc->kf; /* note that this is zeroed if no srcs */
		pprd=rc->kb; /* note that this is zeroed if no prds */
		/* Getting the pool n's */
		for(ppool = rc->pools;*ppool;ppool++)
			psrc *= *(*ppool + pooloffset);
		for(ppool++;*ppool;ppool++)
			pprd *= *(*ppool + pooloffset);

		dpool = psrc - pprd;
		/* Setting the src dpools */
		for(ppool = rc->pools;*ppool;ppool++)
			*(*ppool + dpooloffset) -= dpool;
		/* Setting the dest dpools */
		for(++ppool;*ppool;ppool++)
			*(*ppool + dpooloffset) += dpool;
	}

	/* Do the enzs */
	for (ec = ksolve->enzs, i = 0 ; i < ksolve->nenzs; ec++, i++) {
		psrc=ec->k1 * *(ec->enz + pooloffset);
			/*note that this is zeroed if no srcs */
		pprd=ec->k2 * *(ec->enzcomplex + pooloffset);

		/* Getting the pool n's */
		for(ppool = ec->sub;*ppool;ppool++)
			psrc *= *(*ppool + pooloffset);

		dsub = pprd - psrc;
		dprd = ec->k3 * *(ec->enzcomplex + pooloffset);
		denz = dsub + dprd;
		/* Setting the src dpools */
		for(ppool = ec->sub ;*ppool;ppool++)
			*(*ppool + dpooloffset) += dsub;
		/* Setting the prd dpools */
		for(ppool = ec->prd ;*ppool;ppool++)
			*(*ppool + dpooloffset) += dprd;
		/* Setting the enz dpool */
		*(ec->enz+dpooloffset) += dsub + dprd;
		/* Setting the enzcomplex dpool */
		*(ec->enzcomplex+dpooloffset) -= dsub + dprd;
	}
	/* Do the concchans */
	for (cc = ksolve->concchans, i = 0 ;
		i < ksolve->nconcchans; cc++, i++) {
		dprd = *(cc->chan + pooloffset) *
			(cc->perm_by_vol1 * *(cc->src + pooloffset) -
			cc->perm_by_vol2 * *(cc->dest + pooloffset));
		/* set dsrc */
		*(cc->src + dpooloffset) -= dprd;
		*(cc->dest + dpooloffset) += dprd;
	}
}

/*******************************************************************
**
**
**		Utility routines for setup phase
**
**
*******************************************************************/

static int PoolSpecialType(pool)
	struct pool_type *pool;
{
	MsgIn	*msg;
	int		do_slave = 0;
	int		do_sumtot = 0;
	int		do_consv = 0;

	if (pool->slave_enable == 3)
		return(KBUFFER);
	MSGLOOP(pool,msg) {
		case 2: /* SLAVE */
			if (pool->slave_enable == 1)
				return(KNSLAVE);
			if (pool->slave_enable == 2)
				return(KCONCSLAVE);
		break;
		case 4: /* SUMTOTAL */
			do_sumtot = 1;
		break;
		case 6: /* CONSERVE */
			do_consv = 1;
		break;
	}

	if (do_sumtot && do_consv)
		return(KSUMCONSV);

	if (do_sumtot)
		return(KSUMTOT);

	if (do_consv)
		return(KCONSV);

	return(KPOOL);
}

static MsgIn *GetSlaveMsgIn(elm,msgin)
	Element 	*elm;
	MsgIn		*msgin;
{
	MSGLOOP(elm,msgin) {
		case 2 : /* type SLAVE */
			return(msgin);
		break;
	}
	return(NULL);
}

/*******************************************************************
**
**
**		Hash table routines
**
**
*******************************************************************/

/* This is a duplication of a function in xo. I think we should
** add this more sensible set of hash utilities to the sys library */
static int ksolve_hash_function(key,table)
	char	*key;
	HASH	*table;
{
	int val = (int)key;

	return(((val & 0xff) + (val >> 8)) % table->size);
}

static ENTRY *ksolve_hash_find(key,table)
	char	*key;
	HASH	*table;
{
int size;
ENTRY *hash_ptr;
ENTRY *start_ptr;
 
    if(key == NULL || table == NULL) return(NULL);
    start_ptr = hash_ptr = table->entry + ksolve_hash_function(key,table);
    size = table->size;
    while(hash_ptr->key){
        if(key == hash_ptr->key){
            return(hash_ptr);
        }
        if(++hash_ptr >= table->entry + size) hash_ptr = table->entry;
        if(hash_ptr == start_ptr){
            return(NULL);
        }
    }
    return(NULL);
}

static ENTRY *ksolve_hash_enter(item,table)
ENTRY *item;
HASH *table;
{
int size;
ENTRY *hash_ptr;
ENTRY *start_ptr;
char *key;
 
    if(item == NULL || table == NULL) return(NULL);
    /*
    ** look for the next available spot
    */
    key = item->key;
    start_ptr = hash_ptr = table->entry + ksolve_hash_function(key,table);
    size = table->size;
    while(hash_ptr->key){
        /*
        ** copy over existing keys if they exist
        */
        if(key==hash_ptr->key){
            break;
        }
        if(++hash_ptr >= table->entry + size) hash_ptr = table->entry;
        if(hash_ptr == start_ptr){
            return(NULL);
        }
    }
    /*
    ** leave the last entry empty to identify the end of the table
    */
    BCOPY(item,hash_ptr,sizeof(ENTRY));
    return(hash_ptr);
}

#undef KSOLVE_ARRAY_WIDTH
#undef KSOLVE_CURR_Y
#undef KSOLVE_CURR_DY
#undef KSOLVE_LAST_Y
#undef KSOLVE_YSCAL
#undef KSOLVE_YSEQ
#undef KSOLVE_YM
#undef KSOLVE_YN
#undef KSOLVE_YERR
#undef KSOLVE_IMAX
#undef KSOLVE_NUSE
#undef KSOLVE_SHRINK
#undef KSOLVE_GROW
#undef KPOOL
#undef KENZ
#undef KREAC
#undef KCONCCHAN
#undef KCONSV
#undef KSOLVE_MAX_MSGOUTS
#undef KMAXSTEPS
#undef KTINY
#undef KBULIRSCH_STOER
#undef KRUNGE_KUTTA
#undef KEULER
