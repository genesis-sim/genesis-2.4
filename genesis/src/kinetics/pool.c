/**********************************************************************
** This program is part of the kinetics library and is
**           copyright (C) 1995 Upinder S. Bhalla.
** It is made available under the terms of the
**           GNU Library General Public License. 
** See the file COPYRIGHT for the full notice.
**********************************************************************/

static char rcsid[] = "$Id: pool.c,v 1.6 2005/07/29 16:13:15 svitak Exp $";
 
/*
 *$Log: pool.c,v $
 *Revision 1.6  2005/07/29 16:13:15  svitak
 *Various changes for MIPSpro compiler warnings.
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
 *Revision 1.2  2005/06/16 23:23:00  svitak
 *Updated June 2004 by Upi Bhalla to correct a bug which caused failure
 *on some non-Linux systems such as IRIX.
 *
 *Revision 1.1.1.1  2005/06/14 04:38:34  svitak
 *Import from snapshot of CalTech CVS tree of June 8, 2005
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
 * Revision 1.5  1994/10/26  20:46:09  bhalla
 * Fixed bug with conserve option, where the nTotal got set to twice nInit
 *
 * Revision 1.4  1994/09/29  18:20:21  bhalla
 * -- Split up the SUMTOTAL msg into separate SUMTOTAL and CONSERVE
 * msgs, so that these two cases become more distinct and can coexist.
 * This vastly cleans up the calculation for SUMTOTAL, which usually
 * needs some form of CONSERVE as well.
 *
 * -- Changed the format for slave_enable. It now handles 3 flags:
 * NSLAVE = 0x01, CONCSLAVE = 0x02 and BUFFER = 0x04. The BUFFER
 * option is new, and simplifies the frequent process of using a pool
 * as a buffer by bypassing the whole calculations and just setting
 * n to nInit and Co to CoInit.
 *
 * Revision 1.3  1994/08/15  18:31:55  bhalla
 * Added option to use SLAVE msg to control concen rather than n,
 * using the slave_enable flag set to 2.
 *
 * Revision 1.2  1994/08/05  19:23:48  bhalla
 * conversion to using n rather than conc.
 * Also added new option for using SUMTOTAL msg to handle conservation
 * relationships
 *
 * Revision 1.1  1994/06/13  22:55:39  bhalla
 * Initial revision
 * */

#include <math.h>
#include <sys/time.h>
#include "kin_ext.h"
/* #ifdef CHECK
#undef CHECK
#define SIMPLE_SPRNG
#include "sprng.h"
#endif */

void PoolDistribute();
void PoolProcess();
void PoolIntegrate();

#ifdef __STDC__
static void do_reset(struct pool_type *pool);
static void check_msg_type(struct pool_type *pool, MsgIn *msg);
static void do_oldmsgmode(struct pool_type *pool, double* A, double* B,
	double* conserve, double* sumtotal);
#else
static void do_reset();
static void check_msg_type();
static void do_oldmsgmode();
#endif
static double **msgdata;
static int alloced_nmsgs = 0;
static int used_nmsgs = 0;
#define ALLOCED_NMSGS_INCREMENT 20000

/*
** A pool of a reagent. This version does not carry out any
** reactions, but interfaces to the kin and enz elements which do.
** This way there are no limitations on the number of reactions a
** pool can be involved in
*/

#define POOL_SUMTOTAL	0x01
#define POOL_CONSERVE	0x02
#define POOL_NSLAVE		0x01
#define POOL_CONCSLAVE	0x02
#define POOL_BUFFER		0x04

/* Does the processing for each pool */
void PoolBaseProcess(pool)
struct pool_type *pool;
{
	double A = 0;
	double B = 0;
	double conserve = 0;
	double sumtotal = 0;

	if (pool->stoch_flag && (pool->slave_enable & POOL_BUFFER)) {
			pool->n = floor(pool->nInit) +
					(G_RNG()  < (pool->nInit - floor(pool->nInit)));
			pool->Co = (pool->vol > 0) ? pool->n/pool->vol : pool->n;
	} else {
		PoolProcess(pool, pool, &A, &B, &conserve, &sumtotal);
	}
	if ((pool->slave_enable & POOL_BUFFER) == 0)
		PoolIntegrate(pool, A, B, conserve, sumtotal);
	PoolDistribute(pool);
}

/* Distributes out the results from the master pool to mirrors,
** which are the source of the MIRROR message */
void PoolDistribute(pool)
struct pool_type* pool;
{
	int j;
	struct pool_type *p;
	double **data = pool->msgdata + pool->mirrormsgoffset;
	/* TYPE = MIRROR */
	for (j = 0; j < pool->msgcount[7]; j++, data++) {
		p = (struct pool_type *)(*data);
		p->n = pool->n;
		p->nRemaining = pool->nRemaining;
		p->Co = pool->Co;
		PoolDistribute(p);
	}
}

/* Performs the message reading and summation of results for pools */
void PoolProcess(pool, master, A, B, conserve, sumtotal)
struct pool_type *pool;
struct pool_type *master;
double *A;
double *B;
double *conserve;
double *sumtotal;
{
	/* slave_enable of 3 is the same as buffering,
	** so we bypass everything else */
	if (master->slave_enable & POOL_BUFFER) {
		pool->n = master->nInit;
		pool->Co = master->CoInit;
		return;
	}
	if (pool->oldmsgmode) {
		do_oldmsgmode(pool, A, B, conserve, sumtotal);
	} else {
		int j;

		/* TYPE = REAC */
		/* Keep things together to help cache coherence */
		double **data = pool->msgdata;
		for (j = 0; j < pool->msgcount[0]; j+=2, data++) {
			*A += **data++; /* data is already a double* */
			*B += **data; /* data is already a double* */
		}
	
		/* TYPE = MM_PRD */
		for (j = 0; j < pool->msgcount[1]; j++, data++)
			*A += **data; /* data is already a double* */
	
		/* TYPE = SLAVE */
		if (master->was_slaved) {
			for (j = 0; j < pool->msgcount[2]; j++, data++)
				master->n = **data;
			if (pool->slave_enable == POOL_CONCSLAVE)
				master->n *= pool->vol;
			/* Convert the n's to Co's */
			master->nRemaining = pool->nTotal - pool->n;
			if (pool->vol > 0) {
				master->Co = pool->n / pool->vol;
				master->CoRemaining =
					pool->nRemaining / pool->vol;
			}
			return;
		}
	
		/* TYPE = REMAINING */
		for (j = 0; j < pool->msgcount[3]; j++, data++)
			master->nRemaining -= **data;
	
		/* TYPE = CONSERVE */
		for (j = 0; j < pool->msgcount[4]; j++, data++)
			*conserve += **data;
	
		/* TYPE = VOL */
		for (j = 0; j < pool->msgcount[5]; j++, data++) {
			pool->vol = **data;
			if (pool->vol <= 0) 
				pool->vol = 1;
		}
	
		/* TYPE = CONSERVE */
		for (j = 0; j < pool->msgcount[6]; j++, data++)
			*sumtotal += **data;

		/* TYPE = MIRROR */
		for (j = 0; j < pool->msgcount[7]; j++, data++) {
			PoolProcess((struct pool_type *)(*data), master,
				A, B, conserve, sumtotal);
		}
	}
}

/* Does numerical integration for the pool */
/* The stochastic method uses a little hack so that it melds nicely
** with the A and B format used in the rest of the system. The
** incoming A and B terms from stochastic reacs and enzs are 
** equal to the increase and decrease in # of molecules, divided
** by dt at the source. They get scaled back by dt in the
** integration step here in the pool.
*/

void PoolIntegrate(pool, A, B, conserve, sumtotal)
struct pool_type* pool;
double A;
double B;
double conserve;
double sumtotal;
{
	if (pool->consv_flag) {
		if (pool->consv_flag & POOL_SUMTOTAL)  {
			pool->nTotal = sumtotal;
			if (pool->consv_flag & ~POOL_CONSERVE)  {
				pool->n = sumtotal;
			}
		}
		if (pool->consv_flag & POOL_CONSERVE)  {
			pool->n = pool->nTotal - conserve;
			if (pool->n < 0)
				pool->n = 0;
		}
	} else {
		double dt = Clockrate(pool);
		if (pool->n > 1.0e-10 && B > 1e-20) {
			/* Exp Euler */
			double D = exp(-B * dt / pool->n);
			pool->n *= D + (A/B) * (1 - D);
		} else {
			/* Euler, which also is used for the stochastic method. */
			pool->n += (A - B) * dt;
		}
		if (pool->n < pool->nMin) 
			pool->n = pool->nMin;
	}
	if (pool->stoch_flag && pool->n < KIN_STOCH_MOL_LIMIT &&
					(pool->n - floor(pool->n)) > 0)
			pool->n = floor(pool->n) +
					(G_RNG()  < (pool->n - floor(pool->n)));
	/* Convert the n's to Co's */
	pool->nRemaining = pool->nTotal - pool->n;
	pool->Co = pool->n / pool->vol;
	pool->CoRemaining = pool->CoTotal - pool->Co;
}

void GsolveUpdateSumtotal(pool)
struct pool_type *pool;
{
	double **data = pool->msgdata;
	double sumtotal = 0;
	int j;

	data += pool->msgcount[0] + pool->msgcount[1] + pool->msgcount[2] +
		pool->msgcount[3] + pool->msgcount[4] + pool->msgcount[5];

	for (j = 0; j < pool->msgcount[6]; j++, data++)
		sumtotal += **data;
	if (!(pool->slave_enable & POOL_BUFFER))
		pool->n = sumtotal;
}

void GsolveUpdatePool(pool)
struct pool_type *pool;
{
	if (pool->msgcount[6] > 0)
			GsolveUpdateSumtotal(pool);

	if (pool->slave_enable & POOL_BUFFER)
		pool->n = floor(pool->nInit) +
					(G_RNG()  < (pool->nInit - floor(pool->nInit)));

	pool->nRemaining = pool->nTotal - pool->n;
	if (pool->vol > 0) {
		pool->Co = pool->n / pool->vol;
		pool->CoRemaining = pool->nRemaining / pool->vol;
	}
}

int PoolFunc(pool,action)
register struct pool_type *pool;
Action		*action;
{
	double newval = 0.0;
 
    if(debug > 1){
		ActionHeader("ReactionPool",pool,action);
    }

    SELECT_ACTION(action){
	/* the CoRemaining stuff should actually be calculated
	** in a separate phase, at INIT, but it would be a very
	** expensive undertaking in terms of CPU cycles.
	*/
    case PROCESS:
		if (pool->mirror_src == 0) {
			PoolBaseProcess(pool);
		}
	break;
    case RESET:
		do_reset(pool);
	break;
	case CREATE: {
		int i;

		pool->vol = 1; /* need to initialize this */
		pool->nMin = pool->CoMin = 0;
		pool->oldmsgmode = 0;
		for (i = 0; i < KIN_POOL_MSGCOUNT; i++)
			pool->msgcount[i] = 0;
		return(1);
	}
	break;
    case SET :
		if (action->argc == 2) {
			char *field = action->argv[0];
			if (strcmp(field,"vol") == 0 ||
				strcmp(field,"n") == 0 ||
				strcmp(field,"nInit") == 0 ||
				strcmp(field,"nRemaining") == 0 ||
				strcmp(field,"nTotal") == 0 ||
				strcmp(field,"nMin") == 0 ||
				strcmp(field,"Co") == 0 ||
				strcmp(field,"CoInit") == 0 ||
				strcmp(field,"CoRemaining") == 0 ||
				strcmp(field,"CoTotal") == 0 ||
				strcmp(field,"CoMin") == 0) {
					newval = atof(action->argv[1]);
					if (newval < 0) {
						/* Error(); */
						printf("Cannot set field=%s of pool=%s \n to negative value, using 0.0\n",
						field,Pathname(pool));
						/* return(1); */
						newval = 0.0;
					}
				}
				if (strcmp(field,"vol") == 0 && newval > 0) {
					pool->vol = newval;
					if (pool->keepconc) {
						pool->n = pool->Co * pool->vol;
						pool->nRemaining = pool->CoRemaining* pool->vol;
						pool->nTotal = pool->CoTotal * pool->vol;
						pool->nInit = pool->CoInit * pool->vol;
						pool->nMin = pool->CoMin * pool->vol;
					} else {
						pool->Co = pool->n / pool->vol;
						pool->CoRemaining = pool->nRemaining/pool->vol;
						pool->CoTotal = pool->nTotal / pool->vol;
						pool->CoInit = pool->nInit / pool->vol;
						pool->CoMin = pool->nMin / pool->vol;
					}
					return(1);
				}
				if (pool->vol > 0) {
					if (strcmp(field,"n") == 0)
						pool->Co = newval / pool->vol;
					if (strcmp(action->argv[0],"nInit") == 0)
						pool->CoInit = newval / pool->vol;
					if (strcmp(action->argv[0],"nRemaining") == 0)
						pool->CoRemaining = newval / pool->vol;
					if (strcmp(action->argv[0],"nMin") == 0)
						pool->CoMin = newval / pool->vol;
					if (strcmp(action->argv[0],"nTotal") == 0)
						pool->CoTotal = newval / pool->vol;

					if (strcmp(field,"Co") == 0)
						pool->n = newval * pool->vol;
					if (strcmp(action->argv[0],"CoInit") == 0)
						pool->nInit = newval * pool->vol;
					if (strcmp(action->argv[0],"CoRemaining") == 0)
						pool->nRemaining = newval * pool->vol;
					if (strcmp(action->argv[0],"CoMin") == 0)
						pool->nMin = newval * pool->vol;
					if (strcmp(action->argv[0],"CoTotal") == 0)
						pool->nTotal = newval * pool->vol;
				}
				/* Do the normal set on all the fields */
				return(0);
		}
        return(0); /* do the normal set */
	}
	return(0);
}

static void do_oldmsgmode(pool, A, B, conserve, sumtotal)
struct pool_type *pool;
double* A;
double* B;
double* conserve;
double* sumtotal;
{
	MsgIn	*msg;
        MSGLOOP(pool,msg) {
        case 0:		/* TYPE = REAC */
   			*A += MSGVALUE(msg,0);
   		   	*B += MSGVALUE(msg,1);
        	break;
		case 1:		/* TYPE = MM_PRD. */
   		   	*A += MSGVALUE(msg,0);
		break;
		case 2:		/* TYPE = SLAVE */
					/* n = value to follow */
					/* msg 0 = new value of n */
					if (pool->slave_enable) {
						pool->was_slaved = 1;
						/* hack to use concen for SLAVE
						** rather than n */
						if (pool->slave_enable == 
							POOL_CONCSLAVE)
							pool->n =
								pool->vol * MSGVALUE(msg,0);
						else
							pool->n = MSGVALUE(msg,0);
					}
		break;
		case 3:		/* TYPE = REMAINING */
				pool->nRemaining -= MSGVALUE(msg,0);
		break;
		case 4:		/* TYPE = CONSERVE */
				*conserve += MSGVALUE(msg,0);
		break;
		case 5:		/* TYPE = VOL */
				pool->vol = MSGVALUE(msg,0);
		break;
		case 6:		/* TYPE = SUMTOTAL */
				*sumtotal += MSGVALUE(msg,0);
		break;
   	}
}

static void check_msg_type(pool, msg)
struct pool_type *pool;
MsgIn *msg;
{
	if (MSGSLOT(msg)[0].func != DoubleMessageData) {
		Warning();
		/* Complain */
		printf("Message source for REAC msg to %s is not a double\n",
		Pathname(pool));
		printf("Reverting to slow old messaging\n");
		pool->oldmsgmode = 1;
	}
}

void do_reset_alloc_msgdata()
{
	used_nmsgs = 0;
	if (alloced_nmsgs == 0) {
		alloced_nmsgs = ALLOCED_NMSGS_INCREMENT;
		msgdata = (double **)calloc(alloced_nmsgs, sizeof(double*));
	}
	SimReset();
}

double** alloc_msgdata(n, data)
int n;
double** data;
{

	memcpy((void *)(msgdata + used_nmsgs), (void *)data,
		n * sizeof(double *));
	used_nmsgs += n;
	if (used_nmsgs >= alloced_nmsgs) {
		alloced_nmsgs += ALLOCED_NMSGS_INCREMENT;
		if (msgdata)
			free(msgdata);
		msgdata = (double **)calloc(alloced_nmsgs, sizeof(double));
		Error();
		printf("Realloced message table\n");
	}
	return msgdata + (used_nmsgs - n);
}

static void do_reset(pool)
struct pool_type *pool;
{
	MsgIn	*msg;
	MsgOut	*mout;
	double	sumtotal;
	double	conserve;
	double	*tempdata[200]; /* over 200 msgs would be odd */
	double	**data;
	int i,j;

		if (pool->vol <= 0)
			pool->vol = 1;
		if (pool->keepconc) {
			pool->n = pool->Co * pool->vol;
			pool->nRemaining = pool->CoRemaining* pool->vol;
			pool->nTotal = pool->CoTotal * pool->vol;
			pool->nInit = pool->CoInit * pool->vol;
			pool->nMin = pool->CoMin * pool->vol;
		} else {
			pool->Co = pool->n / pool->vol;
			pool->CoRemaining = pool->nRemaining/pool->vol;
			pool->CoTotal = pool->nTotal / pool->vol;
			pool->CoInit = pool->nInit / pool->vol;
			pool->CoMin = pool->nMin / pool->vol;
		}
		pool->consv_flag = 0;
		pool->was_slaved = 0;
		sumtotal = 0;
		conserve = pool->nInit;

		/* Here we figure out if this pool is a src for a MIRROR msg */
		pool->mirror_src = 0;
		MSGOUTLOOP(pool,mout) {
			if (mout->type == 7) { /* TYPE = MIRROR */
				pool->mirror_src = 1;
				break;
			}
		}

		for (i = 0 ; i < KIN_POOL_MSGCOUNT; i++)
			pool->msgcount[i] = 0;

		/* During RESET, we need to get the msg pointers. */
		data = tempdata;
		MSGLOOP(pool,msg) {
			
   			case 0:		/* TYPE = REAC */
				*data++ = (double*)(MSGPTR(msg, 0));
				*data++ = (double*)(MSGPTR(msg, 1));
				pool->msgcount[0] += 2;
			break;
			default:
			check_msg_type(pool,msg);
			break;
		}
		MSGLOOP(pool,msg) {
   			case 1:		/* TYPE = MM_PRD */
				*data++ = (double*)(MSGPTR(msg, 0));
				pool->msgcount[1]++;
			break;
		}
		if (!(pool->slave_enable & POOL_BUFFER)) {
			MSGLOOP(pool,msg) {
			case 2:		/* TYPE = SLAVE */
				if (pool->slave_enable == POOL_CONCSLAVE ||
					pool->slave_enable == POOL_NSLAVE) {
					pool->was_slaved = 1;
					*data++ = (double*)(MSGPTR(msg, 0));
					pool->msgcount[2]++;
				}
			break;
			}
		}
		MSGLOOP(pool,msg) {
			case 3:		/* TYPE = REMAINING */
				*data++ = (double*)(MSGPTR(msg, 0));
				pool->msgcount[3]++;
			break;
		}
		MSGLOOP(pool,msg) {
			case 4:		/* TYPE = CONSERVE */
				*data++ = (double*)(MSGPTR(msg, 0));
				pool->msgcount[4]++;
				pool->consv_flag |= POOL_CONSERVE;
				conserve += MSGVALUE(msg,1);
			break;
		}
		MSGLOOP(pool,msg) {
			case 5:		/* TYPE = VOL */
				*data++ = (double*)(MSGPTR(msg, 0));
				pool->msgcount[5]++;
			break;
		}
		MSGLOOP(pool,msg) {
			case 6:		/* TYPE = SUMTOTAL */
				*data++ = (double*)(MSGPTR(msg, 0));
				pool->msgcount[6]++;

				pool->consv_flag |= POOL_SUMTOTAL;
				sumtotal += MSGVALUE(msg,1);
			break;
		}

		pool->mirrormsgoffset = data - tempdata;
		MSGLOOP(pool,msg) {
			case 7:		/* TYPE = MIRROR */
				/* This is ugly. The src elm ptr is cast as a double* */
				*data++ = (double*)(msg->src);
				pool->msgcount[7]++;
			break;
		}

		for (i = 0, j= 0 ; i < KIN_POOL_MSGCOUNT; i++)
			j += pool->msgcount[i];
		pool->msgdata = alloc_msgdata(j, tempdata);
		
		if (pool->consv_flag & POOL_SUMTOTAL) { /*This controls nTotal*/
			pool->nTotal = sumtotal;
		}
		if (pool->consv_flag & POOL_CONSERVE) { /* This controls n. */
			/* add up all the relevant nInits. */
			if (pool->consv_flag & POOL_SUMTOTAL) {
				if (sumtotal < conserve &&
					!(pool->slave_enable & POOL_BUFFER)) {
					printf("Warning: Initial values for sumtotal %g are less than for conserve=%g on %s\n",
					sumtotal,conserve, Pathname(pool));
					conserve = sumtotal;
				}
				if (!(pool->slave_enable & POOL_BUFFER))
					pool->nInit = sumtotal - conserve;
			} else {
				pool->nTotal = conserve;
			}
		}

	/* Only update these values if this is the master pool */
	if (pool->mirror_src == 0) {
		if (pool->nTotal < pool->nInit)
			pool->nTotal = pool->nInit;
		pool->n = pool->nInit;
		if (pool->consv_flag & POOL_SUMTOTAL)
			pool->n = sumtotal;

		/* Round up or down to an integral value, probabilistically */
		if (pool->stoch_flag) {
			pool->n = floor(pool->n) +
					(G_RNG()  < (pool->n - floor(pool->n)));
		}
		/* redo the Co values that might have changed in the RESET */
		pool->Co = pool->n / pool->vol;
		pool->CoTotal = pool->nTotal / pool->vol;

		/* Send out the n and Co values to mirrored pools */
		PoolDistribute(pool);
	}
}

#undef POOL_SUMTOTAL
#undef POOL_CONSERVE
#undef POOL_NSLAVE
#undef POOL_CONCSLAVE
#undef POOL_BUFFER

/* This function generates a random seed in a somewhat more
 * thorough manner than the version used by default in GENESIS.
 */
int do_srandom2(int argc, char **argv) {
		time_t seed;
		struct timeval tv;
		int i;

		seed = 0;
		initopt(argc, argv, "[seed]");
		if (G_getopt(argc, argv) < 0)
		{
				printoptusage(argc,argv);
				return 0;
		}

		if (optargc == 2)
			seed = (time_t) atoi(argv[1]);
		else {
			char *hostname = getenv("HOST");
			gettimeofday(&tv, NULL);
			for (i = 0; *hostname; hostname++, i++)
					tv.tv_usec += *hostname * i * i * 16;
			seed = tv.tv_usec;
		}

		G_SEEDRNG(seed);
		return (int)seed;
}

/* This function is declared as an extern void() */
void copyleft_kin()
{
	printf("The kinetics library is copylefted under the LGPL, see kinetics/COPYRIGHT.\n\n");
}
