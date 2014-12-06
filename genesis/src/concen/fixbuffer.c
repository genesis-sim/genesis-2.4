static char rcsid[] = "$Id: fixbuffer.c,v 1.2 2005/06/27 19:00:32 svitak Exp $";

/*
** $Log: fixbuffer.c,v $
** Revision 1.2  2005/06/27 19:00:32  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:29  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.2  1997/05/28 21:23:50  dhb
** Added RCS id and log headers
**
*/

/* Version EDS20k 95/09/27, Erik De Schutter, BBF-UIA 8/94-9/95 */
/* derived from poolbuf, Erik De Schutter, Caltech 11/90, which it replaces */

#include "conc_ext.h"
/* Implementation of a first-order fixed buffer.  Should be 
**	coupled to a difshell or concpool, were the change in the [buffered ion]
**  is computed. */

void FixBuffer(buffer,action)
register Fbuffer *buffer;
Action		*action;
{
double	A,B;		/* integrate dBfree/dt = A - B*Bfree */
double	dt;
MsgIn 	*msg;
double	savedata[2];
int		nc = 0;

    if(debug > 1){
		ActionHeader("fixbuffer",buffer,action);
    }

    SELECT_ACTION(action){
    case INIT:
	buffer->prev_free = buffer->Bfree;
	buffer->prev_bound = buffer->Bbound;
	break;

    case PROCESS:
	dt = Clockrate(buffer);

	/* Read the msgs to get the difshell concentration */
	MSGLOOP(buffer,msg){
		case CONCEN:
				/* 0 = C */
			buffer->activation = MSGVALUE(msg,0);
			break;
	}
	/* dBfree/dt = -kBf.C.Bfree + kBb.Bbound
	** Bbound = Btot - Bfree
	** A  = kBb * Btot
	** B = kBf * C + kBb */
	A = buffer->kBb * buffer->Bbound;
	B = buffer->kBf * buffer->activation;
	buffer->Bfree = IntegrateMethod(buffer->object->method,buffer,
				buffer->Bfree,A,B,dt,"Bfree");
	buffer->Bbound=buffer->Btot - buffer->Bfree;
	break;

    case RESET:
	/* check the messages to the buffer */
	MSGLOOP(buffer,msg){
		case CONCEN:	/* 0 = C */
			buffer->activation = MSGVALUE(msg,0); 
			break;
	}
	/* calculate the steady state of the free buffer */
	buffer->prev_free = buffer->Bfree = buffer->Btot/(1.0 + buffer->activation *
											(buffer->kBf / buffer->kBb));
	buffer->prev_bound = buffer->Bbound=buffer->Btot - buffer->Bfree;
	break;
    
	case CHECK:
	if(buffer->Btot <= 0.0)
	    ErrorMessage("fixbuffer", "Invalid Btot.", buffer);
	if(buffer->kBf <= 0.0)
	    ErrorMessage("fixbuffer", "Invalid kBf.", buffer);
	if(buffer->kBb <= 0.0)
	    ErrorMessage("fixbuffer", "Invalid kBb.", buffer);
	MSGLOOP(buffer,msg){
		case CONCEN:	/* only one concentration signal allowed! */
			nc++;
			break;
	}
	if(nc!=1)
	    ErrorMessage("fixbuffer", "Should get exactly one CONCEN msg.", buffer);
	break;
    
	case SAVE2:
	savedata[0] = buffer->Bfree;
	savedata[1] = buffer->prev_free;
	/* action->data contains the file pointer */
	nc=2;
	fwrite(&nc,sizeof(int),1,(FILE*)action->data);
	fwrite(savedata,sizeof(double),nc,(FILE*)action->data);
	break;

    case RESTORE2:
	/* action->data contains the file pointer */
	fread(&nc,sizeof(int),1,(FILE*)action->data);
	if (nc!=2) ErrorMessage("fixbuffer", "Invalid savedata length.",buffer);
	fread(savedata,sizeof(double),2,(FILE*)action->data);
	buffer->Bfree = savedata[0];
	buffer->prev_free = savedata[1];
	buffer->Bbound=buffer->Btot - buffer->Bfree;
	buffer->prev_bound=buffer->Btot - buffer->prev_free;
	break;
	}
}

