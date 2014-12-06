/**********************************************************************
** This program is part of the kinetics library and is
**           copyright (C) 1995 Upinder S. Bhalla.
** It is made available under the terms of the
**           GNU Library General Public License. 
** See the file COPYRIGHT for the full notice.
**********************************************************************/

static char rcsid[] = "$Id: enz.c,v 1.6 2005/08/12 09:38:17 svitak Exp $";
 
/*
 *$Log: enz.c,v $
 *Revision 1.6  2005/08/12 09:38:17  svitak
 *Switched comment style from slashslash to slashstar.
 *
 *Revision 1.5  2005/07/20 20:02:00  svitak
 *Added standard header files needed by some architectures.
 *
 *Revision 1.4  2005/07/01 10:03:04  svitak
 *Misc fixes to address compiler warnings, esp. providing explicit types
 *for all functions and cleaning up unused variables.
 *
 *Revision 1.3  2005/06/20 21:08:31  svitak
 *Changed sprng() calls to G_RNG(). This is so the kinetics code can still
 *be compiled even if sprng cannot.
 *
 *Revision 1.2  2005/06/16 23:21:50  svitak
 *Updated June 2004 by Upi Bhalla to correct a bug which caused failure
 *on some non-Linux systems such as IRIX.
 *
 *Revision 1.1.1.1  2005/06/14 04:38:34  svitak
 *Import from snapshot of CalTech CVS tree of June 8, 2005
 *
*/

/*
** Old log.
** Revision 1.5  2003/05/29 22:26:40  gen-dbeeman
** New version of kinetics library from Upi Bhalla, April 2003
**
** Revision 1.3  1998/07/21 19:49:11  dhb
** Fixed ANSI C isms.
**
 * Revision 1.2  1998/07/15 06:45:33  dhb
 * Upi update
 *
 * Revision 1.1  1997/07/24 17:49:40  dhb
 * Initial revision
 *
 * Revision 1.3  1994/11/07  22:04:49  bhalla
 * Fixed bug with calculating nComplex and nComplexInt when Co was changed
 *
 * Revision 1.2  1994/08/05  19:23:06  bhalla
 * Conversion to using n rather than conc.
 *
 * Revision 1.1  1994/06/13  22:55:39  bhalla
 * Initial revision
 * */

#include <math.h>
#include "kin_ext.h"
/* #ifdef CHECK
#undef CHECK
#define SIMPLE_SPRNG
#include "sprng.h"
#endif */

#ifdef __STDC__
static int check_msg_type(struct enz_type *enz, MsgIn *msg);
#else
static int check_msg_type();
#endif

/*
** enz handles 2 kinds of incoming and 3 outgoing msgs: from
**	substrate, product and enzyme pools. nComplex is stored locally
**		ksum = k2 + k3
*/

/* returns nenz */
double EnzOldMsgmode(struct enz_type *enz, double* mm_substrate_prd) {
	MsgIn	*msg;
	double nintramol = 1.0;
	double nenz = 0;

	*mm_substrate_prd = enz->k1;

   	MSGLOOP(enz,msg) {
		case 0:		/* TYPE = ENZYME. */
			/* msg 0 = n */
   			nenz = MSGVALUE(msg,0);
   		break;
		case 1:		/* TYPE = SUBSTRATE. */
			/* msg 0 = S */
			/* dn = -k1.Co.S + (k2+k3).nComplex*/
			/* dnComplex = +k1.Co.S - */
			/* (k2+k3).nComplex */
			/* msg 0 = S */
			*mm_substrate_prd *= MSGVALUE(msg,0);
		break;
		case 2:		/* TYPE = VOL */
			/* msg 0 = vol */
			enz->vol = MSGVALUE(msg,0);
		break;
			case 3:		/* TYPE = INTRAMOL */
					nintramol *= MSGVALUE(msg,0);
			break;
	}
	if (nintramol > 0)
		nenz /= nintramol;

	return nenz;
}

/* returns nenz */
double EnzNewMsgmode(struct enz_type *enz, double* mm_substrate_prd) {
	double** data = enz->msgdata;
	double nintramol = 1.0;
	double nenz = 0;
	int j;

         /* TYPE = ENZYME. */
	for (j = 0; j < enz->msgcount[0]; j++, data++)
		nenz = **data;
         /* TYPE = SUBSTRATE. */
	for (j = 0; j < enz->msgcount[1]; j++, data++)
		*mm_substrate_prd *= **data;
         /* TYPE = VOL. */
	for (j = 0; j < enz->msgcount[2]; j++, data++)
		enz->vol = **data;
	for (j = 0; j < enz->msgcount[3]; j++, data++)
		nintramol *= **data;

	if (nintramol > 0)
		nenz /= nintramol;

	return nenz;
}

void DoEnzMMStochCalc(struct enz_type *enz) {
	/* Apply classical M-M calculation
	 * rate = Sub * K3 * E / (Km + Sub)
	 */
	double dt = Clockrate(enz);
	double psub = 1.0;
	double minsub = KIN_STOCH_MOL_LIMIT;
	double n = 0.0;
	double rate;
	double** data = enz->msgdata + enz->msgcount[0];
	int** se = enz->se + enz->msgcount[0];
	int j;
	double *lastptr = NULL;

	for (j = 0; j < enz->msgcount[1]; j++, data++, se++) {
		if (lastptr == *data) {
			if (n <= 0)
				break;
			psub *= (--n);
		} else {
			lastptr = *data;
			n = *lastptr;
			psub *= n;
		}
/*
		n = **data;
		psub *= n;
*/
		if (minsub > n && **se == 0)
			minsub = n;
	}
	rate = psub * enz->k3 * dt * (**enz->msgdata) / 
			(psub + (enz->k2 + enz->k3)/enz->k1);
	if (minsub < KIN_STOCH_MOL_LIMIT ||
					psub * enz->k3 * dt < KIN_STOCH_PROB_LIMIT)
	{
		rate = floor(rate) + (G_RNG()  < (rate - floor(rate)));

		if (minsub < rate)
			rate = floor(minsub);
	} else if (minsub < rate) {
			rate = minsub;
	}
	enz->eA = 0;
	enz->pA = 0;
	enz->sA = 0;
	enz->B = 0;

	/* Now update the values */
	data = enz->msgdata + enz->msgcount[0];
	se = enz->se + enz->msgcount[0];
    /* TYPE = SUBSTRATE. */
	for (j = 0; j < enz->msgcount[1]; j++, data++, se++) {
	/* Euler method, but anyone using classical MM
							   ** enzymes deserves it */
		if (**se == 0)
			**data -= rate;
	}

	/* This is actually an outgoing message, but we have hacked
	* it into msgcount[4]. Note that se skips msg types 2 and 3.
	* Note also that this is just added in so we don't have to
	* deal with the exponential form. 
	* */
	data += enz->msgcount[2] + enz->msgcount[3];
	for (j = 0; j < enz->msgcount[4]; j++, data++, se++)
		if (**se == 0)
				**data += rate;
}

/* This version explicitly separates each of the enzyme calculation
 * steps and treats each independently for stochastic/deterministic
 * calculation.
 */
void DoEnzStochCalc(enz)
struct enz_type *enz;
{
	int j;
	double dt = Clockrate(enz);
	double nenz = **(enz->msgdata); /* Intramol reactions have
											** a scaled nenz */
	/* Use the first entry in the se table which will be the parent
	 * enzyme molecule */
	double minsub = (**(enz->se) == 0) ? nenz : KIN_STOCH_MOL_LIMIT;
	int enz_is_buffered = **(enz->se);
	double **data = enz->msgdata + enz->msgcount[0];
	int **se = enz->se + enz->msgcount[0];
	double n = 0;
	double B = nenz * enz->k1 * dt;
	double nComplex = enz->nComplex;
	double sA;
	double pA;
	double eA;
	double denz;
	double dsub;
	double *lastptr;

	/* Ensure that nComplex is rounded up or down if it goes below
	 * stoch limit */
	if (nComplex < KIN_STOCH_MOL_LIMIT && 
			(nComplex - floor(nComplex)) > 0) {
		nComplex = floor(nComplex) +
			(G_RNG() < (nComplex - floor(nComplex)));
	}
	if (nComplex < 0)
		nComplex = 0;

	sA = enz->k2 * nComplex * dt;
	pA = enz->k3 * nComplex * dt;
	eA = sA + pA;

	lastptr = NULL;
	for (j = 0; j < enz->msgcount[1]; j++, data++, se++) {
		if (lastptr == *data) {
			if (n <= 0)
				break;
			B *= (--n);
		} else {
			lastptr = *data;
			n = *lastptr;
			B *= n;
		}
/*
		n = **data;
		B *= n;
*/
		if (minsub > n && **se == 0)
			minsub = n;
	}

	if (minsub < KIN_STOCH_MOL_LIMIT || 
					B < KIN_STOCH_PROB_LIMIT ||
					nComplex < KIN_STOCH_MOL_LIMIT ||
					eA < KIN_STOCH_PROB_LIMIT)
	{
		B = floor(B) + (G_RNG()  < (B - floor(B)));
		sA = floor(sA) + (G_RNG()  < (sA - floor(sA)));
		pA = floor(pA) + (G_RNG()  < (pA - floor(pA)));

		denz = B - sA - pA;

		/* Check for negative numbers of molecules */
		if (!enz_is_buffered && nenz < denz)
			denz = floor(nenz);
		else if (nComplex < -denz)
			denz = -floor(nComplex);

		dsub = B - sA;
		if (minsub < dsub)
			dsub = floor(minsub);

		pA = dsub - denz;
		if (pA < 0) {
				denz = dsub;
				pA = 0;
		}

		nenz -= denz;
		/* Now update the values */
		/* enz->nComplex += denz; */
		enz->nComplex = nComplex + denz;
		data = enz->msgdata;
		se = enz->se;
		for (j = 0; j < enz->msgcount[0]; j++, data++, se++) {
			if (**se == 0) {
				**data = nenz;
			}
		}
       	/* TYPE = SUBSTRATE. */
		for (j = 0; j < enz->msgcount[1]; j++, data++, se++) {
			n = **data;
			if (**se == 0) {
				**data -= dsub;
			}
		}

		/* This is actually an outgoing message, but we have hacked
	 	* it into msgcount[4]. Note that se skips msg types 2 and 3.
	 	* Note also that this is just added in so we don't have to
	 	* deal with the exponential form. 
	 	* */
		data += enz->msgcount[2] + enz->msgcount[3];
		for (j = 0; j < enz->msgcount[4]; j++, data++, se++)
			if (**se == 0)
					**data += pA;

		enz->eA = 0;
		enz->sA = 0;
		enz->pA = 0;
		enz->B = 0;
	} else {
		/* Do updating for the nComplex, the rest is done by pools */
		if (enz->nComplex > 1e-10 && eA > 1e-20) { /* Exp Euler */
			double D = exp(-eA / enz->nComplex);
			enz->nComplex *= D + (B/eA) * (1 - D);
		} else {
			enz->nComplex += B - eA;
		}
		enz->eA = eA/dt;
		enz->sA = sA/dt;
		enz->pA = pA/dt;
		enz->B = B/dt;
	}
}

void DoEnzContCalc(struct enz_type *enz) {
	double dt = Clockrate(enz);
	double orig_nenz = 0.0;
	double mm_substrate_prd = 1.0;
	double nenz;

	if (enz->oldmsgmode) {
		nenz = EnzOldMsgmode(enz, &mm_substrate_prd);
	} else {
		if (enz->msgcount[0] == 0 ||
			enz->msgcount[1] == 0 ||
			enz->msgcount[4] == 0)
			return;
		nenz = EnzNewMsgmode(enz, &mm_substrate_prd);
	}

	if (enz->usecomplex == 1) { /* Classical M-M form */
		double rate = mm_substrate_prd * nenz * enz->k3 /
				(mm_substrate_prd + (enz->k2 + enz->k3) / enz->k1);
		enz->sA = 0;
		enz->B = rate; /* This is disconnected from the enz */
		enz->pA = rate;
		enz->eA = 0; /* This is disconnected from the enz */
		enz->nComplex = rate / enz->k3;
		enz->CoComplex = enz->nComplex / enz->vol;
		return;
	}
	if (enz->usecomplex == 2) { /* Legacy method, deprecated */
		orig_nenz = nenz;
		nenz -= enz->nComplex;
		if (nenz < 0)
			nenz = 0;
	}
	enz->sA = enz->k2 * enz->nComplex;
	enz->B = mm_substrate_prd * nenz * enz->k1;
	enz->pA = enz->k3 * enz->nComplex;
	enz->eA = enz->pA + enz->sA;

	if (enz->nComplex > 1e-10 && enz->eA > 1e-20) { /* Exp Euler */
		double D = exp(-enz->eA * dt / enz->nComplex);
		enz->nComplex *= D + (enz->B/enz->eA) * (1 - D);
	} else {
		enz->nComplex += (enz->B - enz->eA) * dt;
	}

	if (enz->nComplex < 0)
		enz->nComplex = 0;
	if (enz->usecomplex == 2 && enz->nComplex > orig_nenz) {
		enz->nComplex = orig_nenz;
	}
	enz->CoComplex = enz->nComplex / enz->vol;
}

int EnzFunc(enz, action)
struct enz_type *enz;
Action* action;
{
MsgIn	*msg;
double	**data;
static int dummy_int = 0;
int i,j;

    if(debug > 1){
		ActionHeader("Enzyme",enz,action);
    }


    SELECT_ACTION(action){
    case PROCESS:
			if (enz->stoch_flag == 1) {
					if (enz->usecomplex)
						DoEnzMMStochCalc(enz);
					else
						DoEnzStochCalc(enz);
			} else {
				DoEnzContCalc(enz);
			}
		break;
    case RESET: {
		double *tempdata[200];
		int **se = enz->se;

		if (enz->vol <= 0)
			enz->vol = 1;
		data = tempdata;
		for (i = 0; i < 5; i++)
			enz->msgcount[i] = 0;

		enz->ksum = enz->k3 + enz->k2;
		enz->nComplex = enz->nComplexInit;
		enz->sA = enz->pA = enz->eA = enz->B = 0;
       	MSGLOOP(enz,msg) {
			case 0: /* TYPE = ENZYME */
				*data++ = (double*)(MSGPTR(msg,0));
				if (ElementIsA(msg->src, "pool"))
					*se++ =
						&((struct pool_type *)(msg->src))->slave_enable;
				else
					*se++ = &dummy_int;
			 	enz->msgcount[0]++;
			break;
			default:
				check_msg_type(enz,msg);
			break;
		}
       	MSGLOOP(enz,msg) {
			case 1: /* TYPE = SUBSTRATE */
				*data++ = (double*)(MSGPTR(msg,0));
				if (ElementIsA(msg->src, "pool"))
					*se++ =
						&((struct pool_type *)(msg->src))->slave_enable;
				else
					*se++ = &dummy_int;
			 	enz->msgcount[1]++;
			break;
		}
       	MSGLOOP(enz,msg) {
			case 2:		/* TYPE = VOL */
				*data++ = (double*)(MSGPTR(msg,0));
			 	enz->msgcount[2]++;
				enz->vol = MSGVALUE(msg,0);
			break;
		}
       	MSGLOOP(enz,msg) {
			case 3: /* TYPE = INTRAMOL */
				*data++ = (double*)(MSGPTR(msg,0));
			 	enz->msgcount[3]++;
			break;
		}
		MSGOUTLOOP(enz, msg) {
			if (ElementIsA(msg->dst, "pool") && msg->type == 1) {
					/* I tried using the MSGPTR, but it points back
					 * to the enz, not to the pool */
				*data++ = &(( (struct pool_type *)(msg->dst) )->n);
				*se++ = &(( (struct pool_type *)(msg->dst) )->slave_enable);
			 	enz->msgcount[4]++;
			}
		}
		for (i = 0, j = 0; i < 5; i++)
			j += enz->msgcount[i];
		enz->msgdata = alloc_msgdata(j, tempdata);

		if (enz->vol > 0)
			enz->CoComplexInit = enz->CoComplex =
				enz->nComplex / enz->vol;
		}
		
		enz->sub_stoch_flag = enz->complex_stoch_flag = 
				enz->stoch_flag;
        break;
    case SET :
        if (action->argc == 2) {
			char *field = action->argv[0];
			if (strcmp(field,"vol") == 0 ||
				strcmp(field,"nComplex") == 0 ||
				strcmp(field,"nComplexInit") == 0 ||
				strcmp(field,"CoComplex") == 0 ||
				strcmp(field,"CoComplexInit") == 0) {
				double newval = Atof(action->argv[1]);
				if (newval < 0) {
					Error();
					printf("Cannot set field=%s of enz=%s to -ve value\n",
							field,Pathname(enz));
					return(1);
				}
        		if (strcmp(field,"vol") == 0 && newval > 0) {
					enz->vol = newval;
					if (enz->keepconc) { /* keep conc constant */
						enz->nComplex =enz->CoComplex*enz->vol;
						enz->nComplexInit =enz->CoComplexInit*enz->vol;
					} else {
						enz->CoComplex =enz->nComplex/enz->vol;
						enz->CoComplexInit =enz->nComplexInit/enz->vol;
					}
					return(1);
				}
				if (enz->vol > 0) {
        			if (strcmp(field,"nComplex") == 0)
						enz->CoComplex = newval / enz->vol;
        			if (strcmp(field,"nComplexInit") == 0)
						enz->CoComplexInit = newval / enz->vol;

        			if (strcmp(field,"CoComplex") == 0)
						enz->nComplex = newval * enz->vol;
        			if (strcmp(field,"CoComplexInit") == 0)
						enz->nComplexInit = newval * enz->vol;
				}
			}
        	if (strcmp(field,"k2") == 0)
				enz->ksum = enz->k3 + Atof(action->argv[1]);
        	if (strcmp(field,"k3") == 0)
				enz->ksum = enz->k2 + Atof(action->argv[1]);
			if (strcmp(field,"stoch_flag") == 0)
					enz->complex_stoch_flag = enz->sub_stoch_flag =
						atoi(action->argv[1]);
		}
        return(0); /* do the normal set */
		break;
	case CREATE: /* initializing the vol */
			enz->vol = 1;
			enz->oldmsgmode = 0;
			enz->complex_stoch_flag = enz->sub_stoch_flag = 0;
			return(1);
		break;
	}
	return 0;
}


static int check_msg_type(enz, msg)
struct enz_type *enz;
MsgIn *msg;
{
	if (MSGSLOT(msg)[0].func != DoubleMessageData) {
		Warning();
		/* Complain */
		printf("Message source for REAC msg to %s is not a double\n",
		Pathname(enz));
		printf("Reverting to slow old messaging\n");
		enz->oldmsgmode = 1;
	}
	return 0;
}
