static char rcsid[] = "$Id: nodelay.c,v 1.2 2005/07/01 10:03:05 svitak Exp $";

/*
** $Log: nodelay.c,v $
** Revision 1.2  2005/07/01 10:03:05  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.5  1995/03/29 21:15:32  dhb
** CREATE and COPY actions now return valid status results.
**
** Revision 1.4  1994/10/20  22:30:07  dhb
** Added CREATE and COPY actions to copy connections for projections.
**
** Revision 1.3  1994/09/16  20:24:58  dhb
** Moved connection RESET code from sim_reset.c into RESET action.
**
** Revision 1.2  1994/06/15  20:56:20  dhb
** Moved check of projections from sim_check.c into projections.
**
** Revision 1.1  1992/12/11  19:02:27  dhb
** Initial revision
**
*/

#include "axon_ext.h"

int	DEBUG_NoDelayConnection = 0;

/*
** performs event distribution along absolute connections with
** no latency, no delay, and single clock duration
*/
int NoDelayConnection(projection,action)
Projection	*projection;
Action		*action;
{
register Connection		*connection;
Buffer		*buffer = NULL;
Event		*event;
PFI		connection_func;
MsgIn		*msg;
Element*	orig_elm;

    if(Debug(DEBUG_NoDelayConnection) > 1){
	ActionHeader("NoDelayConnection",projection,action);
    }

    SELECT_ACTION(action){
    case RESET:
	ResetProjections(projection);
	/* Fall through to RECALC code */

    case RECALC:
	buffer = NULL;
	MSGLOOP(projection,msg){
	    case 0:			/* buffer */
		buffer = (Buffer *)MSGPTR(msg,0);
		break;
	}
#ifdef FLOATTIME
	if(buffer){
	    /*
	    ** if the duration is 0 then use dt instead
	    */
	    if(buffer->tmin < Clockrate(projection)){
		buffer->tmin = Clockrate(projection);
	    }
	}
#endif
	break;
    case PROCESS:
	if(projection->connection == NULL) return 0;
	/*
	** get the buffer to be used from the msg
	*/
	MSGLOOP(projection,msg){
	    case 0:			/* buffer */
		buffer = (Buffer *)MSGPTR(msg,0);
		break;
	}
	/*
	** check to see if there is anything in the buffer
	*/
	if(!buffer || (buffer->current == buffer->start)){
	    /*
	    ** there isnt so return
	    */
	    return 0;
	}

	/*
	** get the connection function
	** and check its validity
	*/
	if((connection_func = projection->connection_object->function) == NULL){
	    Error();
	    printf("no connection func for '%s'.\n",
	    Pathname(projection));
	    return 0;
	}

	/*
	** get the buffer event
	*/
	event = buffer->event + buffer->current;
	/*
	** loop through all the connections
	*/
	for(connection=projection->connection;connection;
	connection=connection->next){
	    /*
	    ** call the connection function
	    */
#ifdef FLOATTIME
	    connection_func(
		connection,
		action,
		projection,
		event,
		0.0);
#else
	    connection_func(projection,connection,PROCESS,0,event);
#endif
	}
	break;
    case CHECK:
	CheckProjections(projection);
	break;
    case CREATE:
	orig_elm = (Element*) action->passback;
	if (orig_elm != NULL)
	    CopyConnections(orig_elm, projection);
	return 1;
    case COPY:
	orig_elm = (Element*) action->data;
	if (orig_elm != NULL)
	    CopyConnections(orig_elm, projection);
	return 1;
    default:
	InvalidAction("NoDelayConnection",projection,action);
	break;
    }

    return 0;
}

