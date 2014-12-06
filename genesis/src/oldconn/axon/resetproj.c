static char rcsid[] = "$Id: resetproj.c,v 1.2 2005/07/01 10:03:05 svitak Exp $";

/*
** $Log: resetproj.c,v $
** Revision 1.2  2005/07/01 10:03:05  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.1  1994/09/16 20:22:20  dhb
** Initial revision
**
*/

#include "sim_ext.h"

/*
** ResetProjections
**
** FUNCTION
**
**	ResetProjections calls the RESET action on each connection in the
**	projection if a RESET action is defined for them.
**
** ARGUMENTS
**
**	Projection* projection	- The projection to RESET connections on
**
** RETURN VALUE
**	None.
**
** AUTHOR
**	David Bilitch (moved from sim_reset.c SimReset() function)
*/


void ResetProjections(projection)

Projection*	projection;

{	/* ResetProjections --- Reset each connection object in projection */

	Connection*	connection;
	PFI		func;
	Action		*action = NULL;

	/*
	** do the connections have a reset action?
	*/
	if(projection->connection_object){
	    func = GetActionFunc(projection->connection_object,RESET,NULL,NULL);
	} else {
	    func = NULL;
	}
	if(func){
	    /*
	    ** do each connection
	    */
	    for(connection=projection->connection;
	    connection;connection = connection->next){
		func(connection,action,projection);
	    }
	}

}	/* ResetProjections */
