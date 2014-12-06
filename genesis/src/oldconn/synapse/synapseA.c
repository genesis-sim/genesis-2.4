static char rcsid[] = "$Id: synapseA.c,v 1.2 2005/07/01 10:03:06 svitak Exp $";

/*
** $Log: synapseA.c,v $
** Revision 1.2  2005/07/01 10:03:06  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.1  1992/12/11 19:05:19  dhb
** Initial revision
**
*/

#include "syn_ext.h"

/* M.Wilson Caltech 8/88 */
void SynapseA(connection,action,projection,event,offset_time)
register struct synapseA_type	*connection;
Action		*action;
Projection	*projection;
Event		*event;
float		offset_time;
{
    connection->target->activation += event->magnitude*connection->weight;
    connection->time = simulation_time;
    connection->state = event->magnitude;
}

