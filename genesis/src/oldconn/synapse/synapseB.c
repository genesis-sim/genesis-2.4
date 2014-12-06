static char rcsid[] = "$Id: synapseB.c,v 1.3 2005/07/20 20:02:01 svitak Exp $";

/*
** $Log: synapseB.c,v $
** Revision 1.3  2005/07/20 20:02:01  svitak
** Added standard header files needed by some architectures.
**
** Revision 1.2  2005/07/01 10:03:06  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.1  1992/12/11 19:05:20  dhb
** Initial revision
**
*/

#include <math.h>
#include "syn_ext.h"

/* M.Wilson Caltech 8/88 */
void SynapseB(connection,action,projection,event,offset_time)
register struct synapseB_type	*connection;
Action		*action;
Projection	*projection;
register Event	*event;
float		offset_time;
{
float	old_state;

    /*
    ** update the facilitation weighting
    */
    old_state = connection->state*
	exp(connection->rate*(connection->time - simulation_time));

    connection->target->activation +=
    event->magnitude *(old_state + connection->weight);
    /*
    ** set the new state
    */
    connection->state = old_state + 
	connection->scale*(connection->maxweight -  old_state);
    connection->time = simulation_time;
}

/* M.Wilson Caltech 2/89 */
void RESET_SynapseB(connection,action,projection)
register struct synapseB_type	*connection;
Action		*action;
Projection	*projection;
{
    connection->time = 0;
    connection->state =  0;
}
