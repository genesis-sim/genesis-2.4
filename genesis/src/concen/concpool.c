static char rcsid[] = "$Id: concpool.c,v 1.2 2005/06/27 19:00:23 svitak Exp $";

/* Version EDS22h 99/03/23, Erik De Schutter, BBF-UIA 9/95-3/99 */

/*
** $Log: concpool.c,v $
** Revision 1.2  2005/06/27 19:00:23  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:29  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.5  2003/03/28 20:52:36  gen-dbeeman
** Fix from Mikael Lindqvist for typo in test for dia <= 0.0.
**
** Revision 1.4  1999/10/17 22:13:23  mhucka
** New version from Erik De Schutter, dated circa March 1999.
**
 * EDS22h revison: EDS BBF-UIA 99/03/10-99/03/23
 * added TUBE shape_mode 
 *
 * EDS22b revison: EDS BBF-UIA 97/11/14-97/11/19
 * added HILLPUMP message
 *
** Revision 1.2  1997/05/28 21:23:50  dhb
** Added RCS id and log headers
**
*/

#include "conc_ext.h"
/* A 'well-mixed' concentration pool without diffusion.  Has better
**  controls over size/volume than Ca_concen and does not include
**  any equilibratrion mechanism */

void ConcPool(pool,action)
register Cpool *pool;
Action	*action;
{
double	A,B;		/* integrate dC/dt= A - B*C */
double	C = 0.0,dt,r1,r2;
MsgIn	*msg;
double	savedata[2];
double	pow();
int	n0;

    if(debug > 1){
	ActionHeader("concpool",pool,action);
    }

    SELECT_ACTION(action){
    case INIT:
	pool->prev_C = pool->C;
	break;

    case PROCESS:
	dt = Clockrate(pool);
	A = pool->leak;
	B = 0.0;
	/* Read the msgs.  */
	MSGLOOP(pool,msg){
	    case CONCEN:	/* difshell cytoplasmic concentration */
				/* 0 = C */
		C = MSGVALUE(msg,0);
		break;
	    case STOREINFLUX:	/* influx into store */
				/* 0 = flux */
		/* d[C]/dt = flux/Vol */
		A += MSGVALUE(msg,0)/pool->vol;
		break;
	    case STOREOUTFLUX:	/* outflux out store */
				/* 0 = flux */
		/* d[C]/dt = flux/Vol */
		A -= MSGVALUE(msg,0)/pool->vol;
		break;
	    case BUFFER:	/* interaction with buffer */
				/* 0 = kBf */
				/* 1 = kBb */
				/* 2 = Bfree */
				/* 3 = Bbound */
		/* dC/dt = - kBf*C*Bfree + kBb*Bbound */
		A += MSGVALUE(msg,1)*MSGVALUE(msg,3);
		B += MSGVALUE(msg,0)*MSGVALUE(msg,2);
		break;
	    case HILLPUMP:	/* interaction with Michaelis Menten pump */
				/* 0 = vmax */
				/* 1 = Kd */
				/* 2 = Hill */
		/* dC/dt=vmax.surf/vol.Ci^Hill/(Ci^Hill+Kd) */
		/* where Ci is the difshell concentration */
		r1=MSGVALUE(msg,2);
		r2=pow(C,r1);
		A += MSGVALUE(msg,0)*r2/pool->vol/(r2+MSGVALUE(msg,1));
		break;
	}
	
	/* Integrate to get the net pool concentration */ 
	/* Catch division by zero errors */
	if ((B==0)&&(pool->object->method==EEULER_INT)) {
	    ErrorMessage("concpool", "Exponential Euler method cannot be used unless a BUFFER msg is added.", pool);
	} else {
	    pool->C=IntegrateMethod(pool->object->method,pool,pool->C,A,B,dt,"C");
	}
	break;

    case RESET:
	pool->prev_C = pool->C = pool->Ceq;
	/* no break! */

    case RECALC:
	r1=pool->dia/2.0;
	if (pool->shape_mode==SHELL) {
	    r1=pool->dia/2.0;
	    r2=r1 - pool->thick;
	    if (r2<0.0) r2=0.0;
	    if (pool->len>0.0) {	/* cylindrical */
		pool->vol=(r1*r1-r2*r2)*pool->len*PI;
	    } else {		/* spherical */
		pool->vol=4.0/3.0*(r1*r1*r1-r2*r2*r2)*PI;
	    }
	} else if (pool->shape_mode==SLAB) {
	    pool->vol=r1*r1*pool->thick*PI;
	} else if (pool->shape_mode==TUBE) {
	    pool->vol=r1*r1*pool->len*PI;
	}
	if (pool->vol<UTINY){
	    ErrorMessage("concpool", "Invalid vol.", pool);
	}
	break;

    case CHECK:
	/* minimum concentration */
	if(pool->Ceq < 0.0){
	    ErrorMessage("concpool", "Invalid Ceq.", pool);
	}
	n0=0;
	MSGLOOP(pool,msg){
	    case CONCEN:		/* difshell concentration */
		n0++;
		break;

	    case TAUPUMP:		/* pump */
		if((MSGVALUE(msg,0) <= 0.0)||(MSGVALUE(msg,1) < 0.0)){
		    ErrorMessage("concpool","Incoming pump TAUPUMP msg invalid.",pool);
		}
		break;
	    case EQTAUPUMP:	/* pump */
		if (MSGVALUE(msg,0) <= 0.0){
		    ErrorMessage("concpool","Incoming pump EQTAUPUMP msg invalid.",pool);
		}
		break;
	    case HILLPUMP:		/* pump */
		if ((MSGVALUE(msg,0) <= 0.0) ||
		    (MSGVALUE(msg,1) < 0.0) ||
		    (MSGVALUE(msg,2) < 1)) {
			ErrorMessage("concpool","Incoming pump HILLPUMP msg invalid.",pool);
		}
		if (!n0) {
		    ErrorMessage("concpool","Should receive CONCEN msg before the HILLPUMP msg.",pool);
		}
		break;
	}
	/* check dimensions */
	/* length */
	if(pool->len < 0.0){
		ErrorMessage("concpool", "Invalid len.", pool);
	}
	if (pool->shape_mode<USERDEF) {
		/* thickness */
		if(pool->thick <= 0.0){
			ErrorMessage("concpool", "Invalid thick.", pool);
		}
		/* diameter */
		if(pool->dia <= 0.0){
			ErrorMessage("concpool", "Invalid dia.", pool);
		}
	} else {
		/* volume */
		if(pool->vol <= 0.0){
			ErrorMessage("concpool", "Invalid vol.", pool);
		}
	}
	break;

    case SAVE2:
	savedata[0] = pool->C;
	savedata[1] = pool->prev_C;
	/* action->data contains the file pointer */
	n0=2;
	fwrite(&n0,sizeof(int),1,(FILE*)action->data);
	fwrite(savedata,sizeof(double),n0,(FILE*)action->data);
	break;

    case RESTORE2:
	/* action->data contains the file pointer */
	fread(&n0,sizeof(int),1,(FILE*)action->data);
	if (n0!=2) ErrorMessage("concpool", "Invalid savedata length.",pool);
	fread(savedata,sizeof(double),2,(FILE*)action->data);
	pool->C = savedata[0];
	pool->prev_C = savedata[1];
	break;
    }
}
