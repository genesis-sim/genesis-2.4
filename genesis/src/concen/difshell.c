static char rcsid[] = "$Id: difshell.c,v 1.2 2005/06/27 19:00:32 svitak Exp $";

/* Version EDS22k 00/07/05, Erik De Schutter, BBF-UIA 4/94-7/00 */

/*
** $Log: difshell.c,v $
** Revision 1.2  2005/06/27 19:00:32  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:29  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.5  2000/07/09 23:45:27  mhucka
** Fix from Erik De Schutter to solve a problem in calculations for diffusion.
** The bug involved the calculation of surface area of contacts of shells.
**
** EDS22k revison: EDS BBF-UIA 00/07/05
** Checks contact surfaces of shells
**
** Revision 1.4  1999/10/17 22:13:23  mhucka
** New version from Erik De Schutter, dated circa March 1999.
**
** EDS22b revisions: EDS BBF-UIA 97/11/14-97/11/19
** added HILLPUMP message
**
** Revision 1.2  1997/05/28 21:23:50  dhb
** Added RCS id and log headers
**
*/

/* derived from difpool, Erik De Schutter, Caltech 11/90, which it replaces */

#include "conc_ext.h"

/* Implementation of a concentration shell can have miscellaneous 
**  in/flows (controlled by other process) and one-dimensional 
**  diffusion to other difshells.
** Onion shell or slice model (depends on shape_mode)
** This scheme works fine for single step integration methods with
**  small time step */

void DifShell(shell,action)
register Dshell *shell;
Action		*action;
{
double	A,B;		/* integrate dC/dt= A - B*C */
double	Dif,dt,r1,r2;
MsgIn	*msg;
double	savedata[2];
double	pow();
int	n0,n1;
Dshell	*src;

    if(debug > 1){
	ActionHeader("difshell",shell,action);
    }

    SELECT_ACTION(action){
    case INIT:
	shell->prev_C = shell->C;
	break;

    case PROCESS:
	dt = Clockrate(shell);
	A = shell->leak;
	B = 0.0;
	/* Read the msgs.  */
	MSGLOOP(shell,msg){
	    case INFLUX:	/* current inflow at membrane */
				/* 0 = I */
		/* d[C]/dt = I/(e*Faraday*Vol) */
		A += MSGVALUE(msg,0)/(shell->val*FARADAY*shell->vol);
		break;
	    case OUTFLUX:	/* current outflow at membrane */
				/* 0 = I */
		/* d[C]/dt = -I/(e*Faraday*Vol) */
		A -= MSGVALUE(msg,0)/(shell->val*FARADAY*shell->vol);
		break;
	    case FINFLUX:	/* scaled current inflow */
				/* 0 = I */
				/* 1 = fraction */
		/* d[C]/dt = f*I/(e*Faraday*Vol) */
		A +=MSGVALUE(msg,0)*MSGVALUE(msg,1)/(shell->val*FARADAY*shell->vol);
		break;
	    case FOUTFLUX:	/* scaled current outflow */
				/* 0 = I */
				/* 1 = fraction */
		/* d[C]/dt = -f*I/(e*Faraday*Vol) */
		A -=MSGVALUE(msg,0)*MSGVALUE(msg,1)/(shell->val*FARADAY*shell->vol);
		break;
	    case STOREINFLUX:	/* influx into store */
				/* 0 = flux */
		/* d[C]/dt = flux/Vol */
		A += MSGVALUE(msg,0)/shell->vol;
		break;
	    case STOREOUTFLUX:	/* outflux out store */
				/* 0 = flux */
		/* d[C]/dt = flux/Vol */
		A -= MSGVALUE(msg,0)/shell->vol;
		break;
	    case DIFF_UP:	/* diffusion up from inner shell */
				/* 0 = C */
				/* 1 = thick */
		/* dC/dt = -2*D*surf_down/(thicki+thickj)*(Ci-Cj)/vol */
		Dif=2*shell->D*shell->surf_down/(shell->thick+MSGVALUE(msg,1))/shell->vol;
		A += Dif * MSGVALUE(msg,0);
		B += Dif;
		break;
	    case DIFF_DOWN:	/* diffusion down from outer shell */
				/* 0 = C */
				/* 1 = thick */
		/* dC/dt = -2*D*surf_up/(thicki+thickj)*(Ci-Cj)/vol */
		Dif=2*shell->D*shell->surf_up/(shell->thick+MSGVALUE(msg,1))/shell->vol;
		A += Dif * MSGVALUE(msg,0);
		B += Dif;
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
	    case TAUPUMP:	/* interaction with tau pump having its own Ceq */
				/* 0 = kPump */
				/* 1 = Ceq */
		/* dC/dt = -kPump*(C-Ceq) */
		/* F = kPump */
		Dif = MSGVALUE(msg,0);
		A += Dif*MSGVALUE(msg,1);
		B += Dif;
		break;
	    case EQTAUPUMP:	/* interaction with tau pump having same Ceq */
				/* 0 = kPump */
		/* dC/dt = -kPump*(C-Ceq) */
		/* Dif = kPump */
		Dif = MSGVALUE(msg,0);
		A += Dif*shell->Ceq;
		B += Dif;
		break;
	    case MMPUMP:	/* interaction with Michaelis Menten pump */
				/* 0 = vmax */
				/* 1 = Kd */
		/* dC/dt=-vmax.surf/vol.C/(C+Kd) */
		/* numerically this is a bit of a hack, but if vmax is small
		** it will not cause any instabilities */
		B += MSGVALUE(msg,0)/shell->vol/(shell->C+MSGVALUE(msg,1));
		break;
	    case HILLPUMP:	/* interaction with Michaelis Menten pump */
				/* 0 = vmax */
				/* 1 = Kd */
				/* 2 = Hill */
		/* dC/dt=-vmax.surf/vol.C^Hill/(C^Hill+Kd) */
		/* numerically this is a hack, especially if Hill>1.
		** But if vmax is small it should not cause any 
		** instabilities */
		r1=MSGVALUE(msg,2);
		if (r1>1.1) {
		    /* r2=C^(Hill) */
		    r2=pow(shell->C,r1);
		    /* r1=C^(Hill-1) */
		    r1=r1-1.0;
		    r1=pow(shell->C,r1);
		} else {	/* Hill coeff is 1 */
		    r2=shell->C;
		    r1=1.0;
		}
		B += MSGVALUE(msg,0)*r1/shell->vol/(r2+MSGVALUE(msg,1));
		break;
	}
	
	/* Integrate to get the net shell concentration */ 
	if ((B==0)&&(shell->object->method==EEULER_INT)) {
	    ErrorMessage("difshell", "Exponential Euler method cannot be used unless a BUFFER or PUMP msg is added.", shell);
	} else {
	    shell->C = IntegrateMethod(shell->object->method,shell,shell->C,A,B,dt,"C");
	}
	break;

    case RESET:
	shell->prev_C = shell->C = shell->Ceq;
	/* no break! */

    case RECALC:
	if (shell->initialized) {
	    MSGLOOP(shell,msg){
		case DIFF_UP:	/* diffusion up */
		    src=(Dshell *)msg->src;
		    if (src->initialized) {
			if (src->surf_up<shell->surf_down) 
				shell->surf_down=src->surf_up;
		    }
		    break;
		case DIFF_DOWN:	/* diffusion down */
		    src=(Dshell *)msg->src;
		    if (src->initialized) {
			if (src->surf_down<shell->surf_up) 
				shell->surf_up=src->surf_down;
		    }
		    break;
	    }
	    shell->initialized=2;
	} else {
	    if (shell->shape_mode==SHELL) {
		r1=shell->dia/2.0;
		r2=r1 - shell->thick;
		if (r2<0.0) r2=0.0;
		if (shell->len>0.0) {	/* cylindrical */
			shell->vol=(r1*r1-r2*r2)*shell->len*PI;
			shell->surf_up=2.0*r1*shell->len*PI;
			shell->surf_down=2.0*r2*shell->len*PI;
		} else {				/* spherical */
			shell->vol=4.0/3.0*(r1*r1*r1-r2*r2*r2)*PI;
			shell->surf_up=4.0*r1*r1*PI;
			shell->surf_down=4.0*r2*r2*PI;
		}
	    } else if (shell->shape_mode==SLAB) {
		r1=shell->dia/2.0;
		shell->vol=r1*r1*shell->thick*PI;
		shell->surf_up=r1*r1*PI;
		shell->surf_down=shell->surf_up;
	    }
	    shell->initialized=1;
	}
	if (shell->vol<UTINY){
			ErrorMessage("difshell", "Invalid vol.", shell);
	}
	break;

    case CHECK:
	/* minimum concentration */
	if(shell->Ceq < 0.0){
	    ErrorMessage("difshell", "Invalid Ceq.", shell);
	}
	/* diffusion constant */
	if(shell->D <= 0.0){
	    ErrorMessage("difshell", "Invalid D.", shell);
	}
	n0=n1=0;
	MSGLOOP(shell,msg){
	    case DIFF_UP:	/* diffusion up */
		n0++;
		break;
	    case DIFF_DOWN:	/* diffusion down */
		n1++;
		break;
	    case TAUPUMP:		/* pump */
	    case MMPUMP:
		if((MSGVALUE(msg,0) <= 0.0)||(MSGVALUE(msg,1) < 0.0)){
		    ErrorMessage("difshell","Incoming pump msg invalid.",shell);
		}
		break;
	    case EQTAUPUMP:	/* pump */
		if (MSGVALUE(msg,0) <= 0.0){
		    ErrorMessage("difshell","Incoming pump EQTAUPUMP msg invalid.",shell);
		}
		break;
	}
	if ((n0>1)||(n1>1)) {
		ErrorMessage("difshell","Two diffusions sourcs of same type.",shell);
	}
	/* check dimensions */
	/* length */
	if(shell->len < 0.0){
		ErrorMessage("difshell", "Invalid len.", shell);
	}
	if (shell->shape_mode==SHELL) {
		/* length */
		if(shell->len < 0.0){
			ErrorMessage("difshell", "Invalid len.", shell);
		}
	}
	if (shell->shape_mode<USERDEF) {
		/* thickness */
		if(shell->thick <= 0.0){
			ErrorMessage("difshell", "Invalid thick.", shell);
		}
		/* diameter */
		if(shell->dia <= 0.0){
			ErrorMessage("difshell", "Invalid dia.", shell);
		}
	} else {
		/* volume */
		if(shell->vol <= 0.0){
			ErrorMessage("difshell", "Invalid vol.", shell);
		}
		if (shell->shape_mode==USERDEF) {
			/* surf_up */
			if(n0 && (shell->surf_up <= 0.0)){
				ErrorMessage("difshell", "Invalid surf_up.", shell);
			}
			/* surf_down */
			if(n1 && (shell->surf_down <= 0.0)){
			ErrorMessage("difshell", "Invalid surf_down.", shell);
			}
		}
	}
	break;

    case SAVE2:
	savedata[0] = shell->C;
	savedata[1] = shell->prev_C;
	/* action->data contains the file pointer */
	n0=2;
	fwrite(&n0,sizeof(int),1,(FILE*)action->data);
	fwrite(savedata,sizeof(double),n0,(FILE*)action->data);
	break;

    case RESTORE2:
	/* action->data contains the file pointer */
	fread(&n0,sizeof(int),1,(FILE*)action->data);
	if (n0!=2) ErrorMessage("difshell", "Invalid savedata length.",shell);
	fread(savedata,sizeof(double),2,(FILE*)action->data);
	shell->C = savedata[0];
	shell->prev_C = savedata[1];
	break;
    }
}
