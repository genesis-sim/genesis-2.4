static char rcsid[] = "$Id: fura2.c,v 1.2 2005/06/27 19:00:32 svitak Exp $";

/*
** $Log: fura2.c,v $
** Revision 1.2  2005/06/27 19:00:32  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:29  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.3  2003/03/28 20:51:40  gen-dbeeman
**
** Fix from Mikael Lindqvist for missing break in PROCESS case
** and incorrect MSGVALUE arguments for CONCEN message.
**
** Revision 1.2  1997/05/28 21:23:50  dhb
** Added RCS id and log headers
**
*/

/* Version EDS20i 95/06/02, Erik De Schutter, BBF-UIA 1/95-6/95 */

#include "conc_ext.h"
/* Computes the fura2 signal by adding together information from a
**  series of diffbuffers (representing the fura2 in shells) */
/* Reference: Blumenfeld et. al., Biophys. J., v. 63, p. 1146-64, 1992. */

#define SB1 1.00
#define SF1 0.455
#define SB2 0.051
#define SF2 1.006

void FuraRatio(fura,action)
register Fura2 *fura;
Action		*action;
{
MsgIn 	*msg;
double	Bfree,Bbound,vol;

    if(debug > 1){
		ActionHeader("fura2",fura,action);
    }

    SELECT_ACTION(action){
    case PROCESS:
	fura->F340=fura->F380=0.0;
	/* Read the msgs to get the buffer concentrations */
	MSGLOOP(fura,msg){
		case CONCEN:
				/* 0 = Bfree Bbound vol */
			Bfree = MSGVALUE(msg,0);
			Bbound = MSGVALUE(msg,1);
			vol = MSGVALUE(msg,2);
			fura->F340+= (SB1*Bbound + SF1*Bfree)*vol;
			fura->F380+= (SB2*Bbound + SF2*Bfree)*vol;
			break;
	}
	if (fura->F380>0.0) {
		fura->ratio=fura->F340/fura->F380;
	} else {
		fura->ratio=0.0;
	}
	break;

    case RESET:
	fura->ratio=0.0;
	break;
	}
}

