/**********************************************************************
** This program is part of the kinetics library and is
**           copyright (C) 1995 Upinder S. Bhalla.
** It is made available under the terms of the
**           GNU Library General Public License. 
** See the file COPYRIGHT for the full notice.
**********************************************************************/

static char rcsid[] = "$Id: reac.c,v 1.6 2005/07/29 16:13:15 svitak Exp $";
 
/*
 *$Log: reac.c,v $
 *Revision 1.6  2005/07/29 16:13:15  svitak
 *Various changes for MIPSpro compiler warnings.
 *
 *Revision 1.5  2005/07/20 20:02:01  svitak
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
 *Revision 1.2  2005/06/16 23:23:49  svitak
 *Updated 8 June 2004, Upi Bhalla. Put in fix for the case of
 *higher-order reactions. If 2A -> B then the transition
 *prob goes as nA * (nA - 1) rather than nA * nA. The current
 *fix assumes that lookups for the same substrate will occur
 *consecutively, which is normally the case in kinetikit-defined
 *reaction schemes.
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
 * Revision 1.2  1994/08/05  19:24:43  bhalla
 * Minor changes due to conversion to using n rather than conc
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
#endif
*/

/*
** A reaction handling element. does the basic forward/backward
** rate const management, must be used in conjunction with pools.
** Can handle any number of substrate or product components, but
** the reaction is only a single stage
*/

#ifdef __STDC__
static int do_oldmsgmode(struct reac_type *reac);
static void check_msg_type(struct reac_type *reac, MsgIn *msg);
#else
static int do_oldmsgmode();
static void check_msg_type();
#endif

double KIN_STOCH_MOL_LIMIT = 100;
double KIN_STOCH_PROB_LIMIT = 0.1;

void ReacContProcess(reac)
struct reac_type* reac;
{
	double substrate_prd = reac->kf;
	double product_prd = reac->kb;
	double **data = reac->msgdata;
	int j;

	/* TYPE = SUBSTRATE. */
	for (j = 0; j < reac->msgcount[0]; j++, data++)
		substrate_prd *= **data;
	/* TYPE = PRODUCT. */
	for (j = 0; j < reac->msgcount[1]; j++, data++)
		product_prd *= **data;

	reac->A = product_prd;
	reac->B = substrate_prd;
}

void ReacStochProcess(reac)
struct reac_type* reac;
{
	double dt = Clockrate(reac);
	double substrate_rate = reac->kf * dt;
	double product_rate = reac->kb * dt;
	int rm0 = reac->msgcount[0];
	double **data = reac->msgdata;
	int **se = reac->se;
	int j;
	double submin = KIN_STOCH_MOL_LIMIT;
	double prdmin = KIN_STOCH_MOL_LIMIT;
	double n = 0.0;
	int dn;
	int sf = 0;

	if (substrate_rate > 0) {
		double* lastptr = NULL;
		for (j = 0; j < rm0; j++, data++, se++) {
			if (lastptr == *data) {
				substrate_rate *= (--n) ;
			} else {
				lastptr = *data;
				n = *lastptr;
				substrate_rate *= n;
			}
/* Pre 2004 version: neater but misses out higher-order issues.
				n = **data;
				substrate_rate *= n;
*/
			if (submin > n && **se == 0)
				submin = n;
		}
		sf = (submin < KIN_STOCH_MOL_LIMIT ||
						substrate_rate < KIN_STOCH_PROB_LIMIT);
	}
	if (product_rate > 0) {
		double* lastptr = NULL;
		for (j = 0; j < reac->msgcount[1]; j++, data++, se++) {
			if (lastptr == *data) {
				product_rate *= (--n) ;
			} else {
				lastptr = *data;
				n = *lastptr;
				product_rate *= n;
			}
/*
				n = **data;
				product_rate *= n;
*/
				if (prdmin > n && **se == 0)
					prdmin = n;
		}
		sf |= (prdmin < KIN_STOCH_MOL_LIMIT ||
						product_rate < KIN_STOCH_PROB_LIMIT);
	}

	if (sf)
	{ /* Use stoch calculations */
		dn = ((int)substrate_rate) + 
					(G_RNG() < (substrate_rate - (int)substrate_rate));
		dn -= ((int)product_rate) +
					(G_RNG()  < (product_rate - (int)product_rate));

		/* Ensure we don't have negative molecule numbers */
		if (floor(submin) < dn)
			dn = floor(submin);
		if (floor(prdmin) < -dn)
			dn = -floor(prdmin);
		/* TYPE = SUBSTRATE. */
		data = reac->msgdata;
		se = reac->se;
		for (j = 0; j < rm0; j++, data++, se++) {
			if (**se == 0) {
				**data -= dn;
			}
		}
		/* TYPE = PRODUCT. */
		for (j = 0; j < reac->msgcount[1]; j++, data++, se++) {
			if (**se == 0) {
				**data += dn;
			}
		}

		reac->A = reac->B = 0;
	} else {
		reac->B = substrate_rate / dt;
		reac->A = product_rate / dt;
	}
}


void ReacStochCheck(reac)
struct reac_type* reac;
{
	double substrate_prd = reac->kf;
	double product_prd = reac->kb;
	double **data = reac->msgdata;
	int j;
	double dt = Clockrate(reac);

	/* TYPE = SUBSTRATE. */
	for (j = 0; j < reac->msgcount[0]; j++, data++)
		substrate_prd *= **data;

	/* TYPE = PRODUCT. */
	for (j = 0; j < reac->msgcount[1]; j++, data++)
		product_prd *= **data;

	reac->A = product_prd * dt;
	reac->B = substrate_prd * dt;
}

int ReacFunc(reac,action)
register struct reac_type *reac;
Action		*action;
{
static int dummy_int = 0;
MsgIn	*msg;
double **data;
int j;

    if(debug > 1){
		ActionHeader("Reaction",reac,action);
    }

    SELECT_ACTION(action){
		case INIT:
			break;
    	case PROCESS:
				if (reac->oldmsgmode)
					return do_oldmsgmode(reac);
				if (reac->stoch_flag)
					ReacStochProcess(reac);
				else
					ReacContProcess(reac);
			break;
    	case RESET: {
				double *tempdata[200];
				int **se = reac->se;
				data = tempdata;
				reac->msgcount[0] = reac->msgcount[1] = 0;
        		MSGLOOP(reac,msg) {
						case 0: /* TYPE = SUBSTRATE */
							*data++ = (double*)(MSGPTR(msg,0));
							if (ElementIsA(msg->src, "pool"))
									*se++ = &((struct pool_type *)
											(msg->src))->slave_enable;
							else
									*se++ = &dummy_int;
							reac->msgcount[0]++;
							break;
						default:
							check_msg_type(reac,msg);
						break;
				}
        		MSGLOOP(reac,msg) {
						case 1: /* TYPE = PRODUCT */
							*data++ = (double*)(MSGPTR(msg,0));
							if (ElementIsA(msg->src, "pool"))
									*se++ = &((struct pool_type *)
											(msg->src))->slave_enable;
							else
									*se++ = &dummy_int;
							reac->msgcount[1]++;
							break;
				}
				j = reac->msgcount[0] + reac->msgcount[1];
		        reac->msgdata = alloc_msgdata(j, tempdata);


        		MSGLOOP(reac,msg) {
						case 0:
							break;
						case 1:
							break;
						case 2:		/* TYPE = KF */
									/* msg 0 = kf */
							reac->kf = MSGVALUE(msg,0);
						break;
						case 3:		/* TYPE = KB */
									/* msg 0 = kb */
							reac->kb = MSGVALUE(msg,0);
						break;
        		}
				reac->sub_stoch_flag = reac->prd_stoch_flag = 
						reac->stoch_flag;
				if (reac->stoch_flag)
					ReacStochCheck(reac);
			}
        	break;
    	case SET :
			if (action->argc != 2)
					return 0;
			if (strcmp(action->argv[0], "stoch_number") == 0) {
				double val = atof(action->argv[1]);
				if (val < 1) {
					printf("Warning: Stoch molecule count %g is < 1. Using default=100.\n", val);
					val = 100;
				}
				KIN_STOCH_MOL_LIMIT = val;
				return 1;
			}
			if (strcmp(action->argv[0], "stoch_propensity") == 0) {
				double val = atof(action->argv[1]);
				if (val <= 0 || val >= 1) {
					printf("Warning: Stoch propensity limit %g out of range 0 to 1. Using default=0.1.\n", val);
					val = 0.1;
				}
				KIN_STOCH_PROB_LIMIT = val;
				return 1;
			}
        	return(0); /* do the normal set */
			break;
		case SHOW :
			if (strcmp(action->data, "stoch_number") == 0) {
				static char ret[30];
				sprintf(ret, "%g", KIN_STOCH_MOL_LIMIT);
				action->passback = ret;
				return(1);
			}
			if (strcmp(action->data, "stoch_propensity") == 0) {
				static char ret[30];
				sprintf(ret, "%g", KIN_STOCH_PROB_LIMIT);
				action->passback = ret;
				return(1);
			}
			return(0); /* do the normal show */
			break;
		case CREATE:
			reac->oldmsgmode = 0;
			return(1);
			break;
	}
	return(0);
}

static int do_oldmsgmode(reac)
struct reac_type *reac;
{
	MsgIn	*msg;
	double	substrate_prd = reac->kf;
	double	product_prd = reac->kb;
   	MSGLOOP(reac,msg) {
   		case 0:		/* TYPE = SUBSTRATE. */
			/* msg 0 = # of sub. Used to be Co */
			substrate_prd *= MSGVALUE(msg,0);
   		break;
		case 1:		/* TYPE = PRODUCT */
				/* msg 0 = # of prd. Used to be Co */
			product_prd *= MSGVALUE(msg,0);
		break;
		case 2:		/* TYPE = KF */
					/* msg 0 = kf */
			reac->kf = MSGVALUE(msg,0);
		break;
		case 3:		/* TYPE = KB */
					/* msg 0 = kb */
			reac->kb = MSGVALUE(msg,0);
		break;
   	}
	reac->A = product_prd;
	reac->B = substrate_prd;
	return 1;
}

static void check_msg_type(reac, msg)
struct reac_type *reac;
MsgIn *msg;
{ 
    if (MSGSLOT(msg)[0].func != DoubleMessageData) {
        Warning();
        /* Complain */
        printf("Message source for REAC msg to %s is not a double\n",
        Pathname(reac));
        printf("Reverting to slow old messaging\n");
        reac->oldmsgmode = 1;
    }
}

