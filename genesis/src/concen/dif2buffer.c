static char rcsid[] = "$Id: dif2buffer.c,v 1.2 2005/06/27 19:00:31 svitak Exp $";

/*
** $Log: dif2buffer.c,v $
** Revision 1.2  2005/06/27 19:00:31  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:29  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.3  2000/07/09 23:38:56  mhucka
** Fix from Erik De Schutter to solve a problem in calculations for diffusion
** uncovered by Anand Chandrasekaran.  The bug involved the calculation of
** surface area of contacts of shells.
**
** EDS22k revison: EDS BBF-UIA 00/07/05
** Checks contact surfaces of shells
**
** Revision 1.2  1997/05/28 21:23:50  dhb
** Added RCS id and log headers
**
*/

/* Version EDS22k 00/07/05, Erik De Schutter, BBF-UIA 8/94-7/00 */

#include "conc_ext.h"
/* Implementation of a diffusable first-order buffer.  Should be 
**	coupled to a difshell, were the change in the [buffered ion]
**  is computed. Assumes Dfree != Dbound so that Btot may not
**  be constant */

void Dif2Buffer(buffer,action)
register D2buffer *buffer;
Action		*action;
{
double	Af=0.0,Bf=0.0;	/* integrate dBfree/dt = Af - Bf*Bfree */
double	Ab=0.0,Bb=0.0;	/* integrate dBbound/dt = Ab - Bb*Bbound */
double	Dif,dt,r1,r2;
MsgIn 	*msg;
double	savedata[4];
int	nc,n0,n1;
D2buffer *src;

    if(debug > 1){
		ActionHeader("difbuffer",buffer,action);
    }

    SELECT_ACTION(action){
    case INIT:
	buffer->prev_free = buffer->Bfree;
	buffer->prev_bound = buffer->Bbound;
	break;

    case PROCESS:
	dt = Clockrate(buffer);

	/* Read the msgs to get the difbuffer */
    MSGLOOP(buffer,msg){
	case CONCEN:
	    /* 0 = C */
	    buffer->activation = MSGVALUE(msg,0);
	    break;
	case BDIFF_UP:	/* diffusion up */
	    /* 0 = Bfree */
	    /* 1 = Bbound */
	    /* 2 = thick */
	    /* dB/dt = -2*D*surf_up/(thicki+thickj)*(Bi-Bj) */
	    Dif=2*buffer->surf_up/(buffer->thick+MSGVALUE(msg,2))/buffer->vol;
	    Af += Dif * buffer->Dfree*MSGVALUE(msg,0);
	    Bf += Dif * buffer->Dfree;
	    Ab += Dif * buffer->Dbound * MSGVALUE(msg,1);
	    Bb += Dif * buffer->Dbound;
	    break;
	case BDIFF_DOWN:	/* diffusion down */
	    /* 0 = Bfree */
	    /* 1 = Bbound */
	    /* 2 = thick */
	    /* dB/dt = -2*D*surf_up/(thicki+thickj)*(Bi-Bj) */
	    Dif=2*buffer->surf_down/(buffer->thick+MSGVALUE(msg,2))/buffer->vol;
	    Af += Dif * buffer->Dfree*MSGVALUE(msg,0);
	    Bf += Dif * buffer->Dfree;
	    Ab += Dif * buffer->Dbound * MSGVALUE(msg,1);
	    Bb += Dif * buffer->Dbound;
	    break;
	}

	/* dBfree/dt = -kBf.C.Bfree + kBb.Bbound 
	** dBbound/dt = kBf.C.Bfree - kBb.Bbound */
	Ab += buffer->kBf * buffer->activation * buffer->Bfree;
	Bb = Bb + buffer->kBb;
	buffer->Bbound = IntegrateMethod(buffer->object->method,buffer,
				buffer->Bbound,Ab,Bb,dt,"Bbound");
	Af += buffer->kBb * buffer->Bbound;
	Bf += buffer->kBf * buffer->activation;
	buffer->Bfree = IntegrateMethod(buffer->object->method,buffer,
				buffer->Bfree,Af,Bf,dt,"Bfree");
	break;

    case RESET:
	/* get all the messages to the buffer */
	MSGLOOP(buffer,msg){
	    case CONCEN:
		/* 0 = C */
		buffer->activation = MSGVALUE(msg,0); 
		break;
	}
	/* calculate the steady state of the free buffer */
	buffer->prev_free = buffer->Bfree = 
	    buffer->Btot/(1.0 + buffer->activation * (buffer->kBf/buffer->kBb));
	buffer->prev_bound = buffer->Bbound=buffer->Btot - buffer->Bfree;
	/* no break! */

    case RECALC:
	if (buffer->initialized) {
	    MSGLOOP(buffer,msg){
		case BDIFF_UP:	/* diffusion up */
		    src=(D2buffer *)msg->src;
		    if (src->initialized) {
			if (src->surf_up<buffer->surf_down) 
				buffer->surf_down=src->surf_up;
		    }
		    break;
		case BDIFF_DOWN:	/* diffusion down */
		    src=(D2buffer *)msg->src;
		    if (src->initialized) {
			if (src->surf_down<buffer->surf_up) 
				buffer->surf_up=src->surf_down;
		    }
		    break;
	    }
	    buffer->initialized=2;
	} else {
	    if (buffer->shape_mode==SHELL) {
		r1=buffer->dia/2.0;
		r2=r1 - buffer->thick;
		if (r2<0.0) r2=0.0;
		if (buffer->len>0.0) {   /* cylindrical */
		    buffer->vol=(r1*r1-r2*r2)*buffer->len*PI;
		    buffer->surf_up=2.0*r1*buffer->len*PI;
		    buffer->surf_down=2.0*r2*buffer->len*PI;
		} else {                /* spherical */
		    buffer->vol=4.0/3.0*(r1*r1*r1-r2*r2*r2)*PI;
		    buffer->surf_up=4.0*r1*r1*PI;
		    buffer->surf_down=4.0*r2*r2*PI;
		}
	    } else if (buffer->shape_mode==SLAB) {
		r1=buffer->dia/2.0;
		buffer->vol=r1*r1*buffer->thick*PI;
		buffer->surf_up=r1*r1*PI;
		buffer->surf_down=buffer->surf_up;
	    }
	    if (buffer->vol<UTINY){
		ErrorMessage("difbuffer", "Invalid vol.", buffer);
	    }
	    buffer->initialized=1;
	}
	break;
    
    case CHECK:
	if(buffer->Btot <= 0.0)
	    ErrorMessage("difbuffer", "Invalid Btot.", buffer);
	if(buffer->kBf <= 0.0)
	    ErrorMessage("difbuffer", "Invalid kBf.", buffer);
	if(buffer->kBb <= 0.0)
	    ErrorMessage("difbuffer", "Invalid kBb.", buffer);
	/* diffusion constant */
	if(buffer->Dfree <= 0.0){
	    ErrorMessage("difbuffer", "Invalid Dfree.", buffer);
	}
	if(buffer->Dbound <= 0.0){
	    ErrorMessage("difbuffer", "Invalid Dbound.", buffer);
	}
	/* check the messages to the buffer */
	nc=n0=n1=0;
	MSGLOOP(buffer,msg){
	    case CONCEN:
		nc++;
		break;
	    case BDIFF_UP:	/* diffusion up */
		n0++;
		break;
	    case BDIFF_DOWN:	/* diffusion down*/
		n1++;
		break;
	}
	if(nc!=1){
	    ErrorMessage("difbuffer", "Should get exactly one CONCEN msg.", buffer);
	}
	if ((n0>1)||(n1>1)) {
		ErrorMessage("difbuffer","Two diffusions sourcs of same type.",buffer);
	}
	/* check dimensions */
	/* length */
	if(buffer->len < 0.0){
	    ErrorMessage("difbuffer", "Invalid len.", buffer);
	}
	if (buffer->shape_mode==SHELL) {
	    /* length */
	    if(buffer->len < 0.0){
		    ErrorMessage("difbuffer", "Invalid len.", buffer);
	    }
	}
	if (buffer->shape_mode<USERDEF) {
	    /* thickness */
	    if(buffer->thick <= 0.0){
		ErrorMessage("difbuffer", "Invalid thick.", buffer);
	    }
	    /* diameter */
	    if(buffer->dia <= 0.0){
		ErrorMessage("difbuffer", "Invalid dia.", buffer);
	    }
	} else {
	    /* volume */
	    if(buffer->vol <= 0.0){
		ErrorMessage("difbuffer", "Invalid vol.", buffer);
	    }
	    if (buffer->shape_mode==USERDEF) {
		/* surf_up */
		if(n0 && (buffer->surf_up <= 0.0)){
		    ErrorMessage("difbuffer", "Invalid surf_up.", buffer);
		}
		/* surf_down */
		if(n1 && (buffer->surf_down <= 0.0)){
		    ErrorMessage("difbuffer", "Invalid surf_down.", buffer);
		}
	    }
	}
	break;
    
    case SAVE2:
	savedata[0] = buffer->Bfree;
	savedata[1] = buffer->prev_free;
	savedata[2] = buffer->Bbound;
	savedata[3] = buffer->prev_bound;
	/* action->data contains the file pointer */
	n0=4;
	fwrite(&n0,sizeof(int),1,(FILE*)action->data);
	fwrite(savedata,sizeof(double),n0,(FILE*)action->data);
	break;

    case RESTORE2:
	/* action->data contains the file pointer */
	fread(&n0,sizeof(int),1,(FILE*)action->data);
	if (n0!=4) ErrorMessage("difbuffer", "Invalid savedata length.", buffer);
	fread(savedata,sizeof(double),2,(FILE*)action->data);
	buffer->Bfree = savedata[0];
	buffer->prev_free = savedata[1];
	buffer->Bbound = savedata[2];
	buffer->prev_bound = savedata[3];
	break;
	}
}

