/**********************************************************************
** This program is part of the kinetics library and is
**           copyright (C) 1995 Upinder S. Bhalla.
** It is made available under the terms of the
**           GNU Library General Public License. 
** See the file COPYRIGHT for the full notice.
**********************************************************************/
static char rcsid[] = "$Id: oenz.c,v 1.3 2005/08/12 09:38:17 svitak Exp $";

/*
** $Log: oenz.c,v $
** Revision 1.3  2005/08/12 09:38:17  svitak
** Switched comment style from slashslash to slashstar.
**
** Revision 1.2  2005/06/20 21:08:31  svitak
** Changed sprng() calls to G_RNG(). This is so the kinetics code can still
** be compiled even if sprng cannot.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.1  2003/05/30 18:07:56  gen-dbeeman
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

#include "kin_ext.h"
/* #ifdef CHECK
#undef CHECK
#define SIMPLE_SPRNG
#include "sprng.h"
#endif */

#ifdef __STDC__
static int check_msg_type(struct enz_type *enz, MsgIn *msg);
static int do_old_enzmsgmode(struct enz_type *enz);
#else
static int check_msg_type();
static int do_old_enzmsgmode();
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
			/* dnComplex = +k1.Co.S -
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

/* This version explicitly separates each of the enzyme calculation
 * steps and treats each independently for stochastic/deterministic
 * calculation.
 */
void DoEnzStochCalc(struct enz_type *enz, double dt, double nenz) {
	int i;
	int j;
	double true_nenz = **(enz->msgdata); /* Intramol reactions have
											** a scaled nenz */
	/* Use the first entry in the se table which will be the parent
	 * enzyme molecule */
	double minsub = (**(enz->se) == 0) ?
			nenz : KIN_STOCH_UPPER_MOL_LIMIT;
	double ustoch_limit = KIN_STOCH_UPPER_PROB_LIMIT / dt;
	double lstoch_limit = KIN_STOCH_LOWER_PROB_LIMIT / dt;
	double **data = enz->msgdata + enz->msgcount[0];
	int **se = enz->se + enz->msgcount[0];
	double n;
	double B = nenz * enz->k1 * dt;
	int B_floor = B;
	double nComplex = enz->nComplex;
	double sA = enz->k2 * nComplex * dt;
	double pA = enz->k3 * nComplex * dt;
	double eA = sA + pA;
	double substrate_rate = 0; /* From substrate or enz to complex */
	double complex_rate = 0; /* From complex to substrate or product */
	double backsub_rate = 0; /* From complex to substrate */
	double product_rate = 0; /* From complex to product */

	for (j = 0; j < enz->msgcount[1]; j++, data++, se++) {
		n = **data;
		B *= n;
		if (minsub < n && **se == 0)
			minsub = n;
	}

	if (enz->sub_stoch_flag > 1) {
			if (minsub < KIN_STOCH_LOWER_MOL_LIMIT)
					enz->sub_stoch_flag = 1;
			else if (B < KIN_STOCH_LOWER_PROB_LIMIT)
					enz->sub_stoch_flag--;
	} else if (minsub >= KIN_STOCH_UPPER_MOL_LIMIT &&
			B > KIN_STOCH_UPPER_PROB_LIMIT) {
			enz->sub_stoch_flag = KIN_STOCH_COUNTDOWN;
	}

	if (enz->sub_stoch_flag == 1) {
			B = B_floor + (sprng() < (B - B_floor));
	}

	if (enz->complex_stoch_flag > 1) {
			if (nComplex < KIN_STOCH_LOWER_MOL_LIMIT)
					enz->complex_stoch_flag = 1;
			else if (eA < KIN_STOCH_LOWER_PROB_LIMIT)
					enz->complex_stoch_flag--;
	} else if (nComplex >= KIN_STOCH_UPPER_MOL_LIMIT &&
			eA > KIN_STOCH_UPPER_PROB_LIMIT) {
			enz->complex_stoch_flag = KIN_STOCH_COUNTDOWN;
	}

	if (enz->complex_stoch_flag == 1) {
			if (nComplex < 1) {
					sA = pA = 0;
			} else {
					sA = (int)sA + (sprng() < (sA - (int)sA));
					nComplex += B - sA;
					if (nComplex < 1) {
						pA = 0;
					} else {
						pA = enz->k3 * nComplex * dt;
						pA = (int)pA + (sprng() < (pA - (int)pA));
						nComplex -= pA;
					}
			}
			eA = sA + pA;
	}
	if (enz->complex_stoch_flag != 1 && enz->sub_stoch_flag != 1) {
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
	} else {
		if (enz->usecomplex) { /* Here we do NOT want to update nenz of
								* parent enzyme */
			data = enz->msgdata + enz->msgcount[0];
			se = enz->se + enz->msgcount[0];
		} else {
			data = enz->msgdata;
			se = enz->se;
         	/* TYPE = ENZYME. */
			for (j = 0; j < enz->msgcount[0]; j++, data++, se++) {
				if (**se == 0) {
					**data += eA - B;
					if (**data < 0) **data = 0;
				}
			}
		}
         	/* TYPE = SUBSTRATE. */
		for (j = 0; j < enz->msgcount[1]; j++, data++, se++) {
			n = **data;
			if (**se == 0) {
				**data += sA - B;
				if (**data < 0) **data = 0;
			}
		}

		/* This is actually an outgoing message, but we have hacked
	 	* it into msgcount[4]. Note that se skips msg types 2 and 3.
	 	* Note also that this is just added in so we don't have to
	 	* deal with the exponential form. 
	 	* */
		data += enz->msgcount[2] + enz->msgcount[3];
		for (j = 0; j < enz->msgcount[4]; j++, data++, se++)
			if (**se == 0) **data += pA;
	
			/* Here we deal with the complex itself */
		enz->nComplex += B - eA;
		if (enz->nComplex < 0) enz->nComplex = 0;
		enz->sA = enz->B = enz->pA = enz->eA = 0;
	}
}

int EnzIntegrate(struct enz_type *enz, double mm_substrate_prd, double nenz) {
	double dt = Clockrate(enz);
	double orig_nenz;

	if (enz->usecomplex == 1) {
		orig_nenz = nenz;
		nenz -= enz->nComplex;
		if (nenz < 0)
			nenz = 0;
	}

	enz->sA = enz->k2 * enz->nComplex;
	enz->B = mm_substrate_prd * nenz;
	enz->pA = enz->k3 * enz->nComplex;

	/* msgcount[3] checks for intramol msgs */
	if (enz->stoch_flag && enz->msgcount[3] == 0) {
		DoEnzStochCalc(enz, dt, nenz);
	} else {
		enz->eA = enz->pA + enz->sA;
		if (enz->nComplex > 1e-10 && enz->eA > 1e-20) { /* Exp Euler */
			double D = exp(-enz->eA * dt / enz->nComplex);
			enz->nComplex *= D + (enz->B/enz->eA) * (1 - D);
		} else {
			enz->nComplex += (enz->B - enz->eA) * dt;
		}
	}

	if (enz->nComplex < 0)
		enz->nComplex = 0;
	if (enz->usecomplex == 1 && enz->nComplex > orig_nenz) {
		enz->nComplex = orig_nenz;
	}

	enz->CoComplex = enz->nComplex / enz->vol;
}

EnzFunc(enz,action)
register struct enz_type *enz;
Action		*action;
{
MsgIn	*msg;
double	mm_substrate_prd;
double	CA,CB;
double  nenz;
double	dt;
double	**data;
static int dummy_int = 0;
int i,j;

    if(debug > 1){
		ActionHeader("Enzyme",enz,action);
    }


    SELECT_ACTION(action){
    case PROCESS:
			mm_substrate_prd = enz->k1;
			if (enz->oldmsgmode) {
				nenz = EnzOldMsgmode(enz, &mm_substrate_prd);
			} else {
				if (enz->msgcount[1] == 0 && enz->msgcount[2] == 0)
					return;
				nenz = EnzNewMsgmode(enz, &mm_substrate_prd);
			}

			/* handles stoch as well as cont cases */
			EnzIntegrate(enz, mm_substrate_prd, nenz);
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
}


#ifdef __STDC__
static int check_msg_type(struct enz_type *enz, MsgIn *msg) {
#else
static int check_msg_type(enz, msg)
struct enz_type *enz;
MsgIn *msg;
{
#endif
	if (MSGSLOT(msg)[0].func != DoubleMessageData) {
		Warning();
		/* Complain */
		printf("Message source for REAC msg to %s is not a double\n",
		Pathname(enz));
		printf("Reverting to slow old messaging\n");
		enz->oldmsgmode = 1;
	}
}
