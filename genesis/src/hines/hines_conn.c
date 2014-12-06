static char rcsid[] = "$Id: hines_conn.c,v 1.2 2005/07/01 10:03:04 svitak Exp $";

/* Version EDS22c 97/12/04, Erik De Schutter, BBF-UIA 6/94-12/97 */

/*
** $Log: hines_conn.c,v $
** Revision 1.2  2005/07/01 10:03:04  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:32  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.5  2000/10/12 22:25:49  mhucka
** New spikegen fixes from EDS and HC.
**
** Revision 1.4  2000/10/06 06:30:31  mhucka
** Fix from Erik De Schutter.
**
** Revision 1.3  1999/10/17 21:39:37  mhucka
** New version of Hines code from Erik De Schutter, dated 30 March 1999
** and including an update to hines.c from Erik from June 1999.  This version
** does not include changes to support Red Hat 6.0 and the Cray T3E, which
** will need to be incorporated in a separate revision update.
**
 * EDS22c revison: EDS BBF-UIA 97/12/04-97/12/04
 * Corrected SYNCHAN time normalization and bug in frequency
 *
** Revision 1.1  1997/05/28 23:11:50  dhb
** Initial revision
**
*/

#include "hines_ext.h"

/* Contains routines that setup and execute interaction with the newconn
** objects. */
/* Not the most efficient way of handling things, as events are still
** send from the original spikegen to the original synchan instead of 
** hsolve to hsolve.
*/

/* execute Spikegen event */
void h_dospike_event(hsolve)
	Hsolve  *hsolve;
{
	struct Spikegen_type *spike;
	MsgOut  *mo;

	spike = (struct Spikegen_type *) hsolve->spikegen;
	MSGOUTLOOP(spike,mo) {
		CallEventAction(spike, mo);
	}
        spike->lastevent = SimulationTime();
}

/* synapse fires: call synchan element, get weight and update counter */
/* this code is very inefficient! Should improve when synchan gets
*  completely incorporated */
void h_dosynchan(hsolve,stabindex,cindex)
	Hsolve  *hsolve;
	int	stabindex,cindex;
{
	Action  *action;
	Element	*child;
	S2chan	*s2chan;
	int	nop,nchip;
	SynapticEventPtr next_event;

	child=hsolve->children[cindex];
	action = GetAction("HPROCESS");
	CallElement(child,action,0);
	nchip=hsolve->childchips[cindex];
	nop=hsolve->childops[cindex];
	if (hsolve->ops[nop]==SYN3_OP) nchip++;	/* skip frequency */
	s2chan=(S2chan *)child;
	/* as both s2chan->activation and stablist[stabindex+3] are
	** normalized for dt, we need to remove this normalization once here
	*/
	hsolve->chip[nchip]+=hsolve->dt * s2chan->activation *
						hsolve->stablist[stabindex+3];
	next_event=s2chan->PendingSynapticEvents;
	if (next_event) {
		hsolve->ops[nop+2]=next_event->time;
	} else {
		hsolve->ops[nop+2]=-1;	/* no next firing time */
	}
}

/* implements event transmission to hines solvers */
int h_eventaction(src,dst,index,time)

Element *src,*dst;
int	index;
int	time;

{
    Action  action;

	action.name = "HSEVENT";
	action.type = HSEVENT;
	action.data = (char *)src;
	action.argc = index;
	action.argv = (char**) &time;

	return CallActionFunc(dst, &action);
} 

