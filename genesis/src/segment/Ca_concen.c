static char rcsid[] = "$Id: Ca_concen.c,v 1.2 2005/07/20 20:02:03 svitak Exp $";

/*
** $Log: Ca_concen.c,v $
** Revision 1.2  2005/07/20 20:02:03  svitak
** Added standard header files needed by some architectures.
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.2  1995/03/23 15:17:00  dhb
** Added number of values to SAVE2 file format
**
** Revision 1.1  1992/12/11  19:03:56  dhb
** Initial revision
**
*/

#include <math.h>
#include "seg_ext.h"

static double		savedata[1];

/* Implementation of single shell Ca concentration computation */
/* 1/89 Matt Wilson */
/* Added INCREASE, DECREASE messages, E De Schutter 4/91 */
/* Added BASE message, E De Schutter 5/91 */
/* Added SAVE2 and RESTORE2 actions, E De Schutter 10/91 */

int CaConcen(pool,action)
register struct Ca_concen_type *pool;
Action		*action;
{
double 	dt;
MsgIn 	*msg;
int	n;

    if(debug > 1){
	ActionHeader("CaConcen",pool,action);
    }
    SELECT_ACTION(action){
    case INIT:
	pool->activation = 0;
	break;
    case PROCESS:
	dt = Clockrate(pool);
	MSGLOOP(pool,msg){
	    case 0:		/* pool activation (I_Ca) */
		pool->activation += MSGVALUE(msg,0);
		break;
	    case 1:		/* fractional pool activation (I_Ca) */
		pool->activation += MSGVALUE(msg,0)*MSGVALUE(msg,1);
		break;
	    case 2:		/* INCREASE: independent of sign of float Ik or I_Ca */
		pool->activation += fabs(MSGVALUE(msg,0));
		break;
	    case 3:		/* DECREASE: independent of sign of float Ik or I_Ca */
		pool->activation -= fabs(MSGVALUE(msg,0));
		break;
		case 4:     /* BASE: set Ca_base */
		pool->Ca_base = MSGVALUE(msg,0);
		break;
	}
	/* dC/dt = B*I_Ca - C/tau */
	pool->C = IntegrateMethod(0,pool,
	    pool->C,
	    pool->B*pool->activation,
	    1/pool->tau,
	    dt,"C");
	/* set the new values of the state variables */
	pool->Ca = pool->Ca_base + pool->C;
	break;
    case RESET:
	pool->activation = 0;
	pool->C = 0;
	pool->Ca = pool->Ca_base;
	break;
    case CHECK:
	if(pool->tau <= 0.0){
	    ErrorMessage("CaConcen", "Invalid tau parameters.", pool);
	}
	if(pool->Ca_base < 0.0){
	    ErrorMessage("CaConcen", "Base Ca level must be >= 0.", pool);
	}
	break;
    case SAVE2:
	savedata[0] = pool->Ca;
	/* action->data contains the file pointer */
	n=1;
	fwrite(&n,sizeof(int),1,(FILE*)action->data);
	fwrite(savedata,sizeof(double),1,(FILE*)action->data);
	break;
    case RESTORE2:
	/* action->data contains the file pointer */
	fread(&n,sizeof(int),1,(FILE*)action->data);
	if (n != 1) {
	    ErrorMessage("CaConcen","Invalid savedata length",pool);
	    return(n);
	}
	fread(savedata,sizeof(double),1,(FILE*)action->data);
	pool->Ca = savedata[0];
	pool->C=pool->Ca-pool->Ca_base;
	break;
    }
    return(0);
}
