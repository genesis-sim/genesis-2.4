static char rcsid[] = "$Id: checkproj.c,v 1.3 2005/07/01 10:03:05 svitak Exp $";

/*
** $Log: checkproj.c,v $
** Revision 1.3  2005/07/01 10:03:05  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.2  2005/06/27 19:00:44  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.2  1995/01/26 00:35:55  dhb
** Fixed errant call to GetActionFunc() from CheckProjections().  Was
** missing two of the four arguments.
**
 * Revision 1.1  1994/06/15  20:35:47  dhb
 * Initial revision
 *
 * Revision 1.3  1993/12/23  02:23:54  dhb
 * Changed direct call to element action function to use CallActionFunc()
 * in order to support extended objects.
 *
 * Revision 1.2  1993/02/16  22:27:21  dhb
 * 1.4 to 2.0 command argument changes.
 *
 * 	CheckSimulation (check) required no changes.
 *
 * Fixed type error in rcsid variable declaration.
 *
 * Revision 1.1  1992/10/27  19:41:41  dhb
 * Initial revision
 *
*/

#include "sim_ext.h"

/*
** check the consistency of the time intervals for the 
** buffers projections and segments
*/
void CheckProjections(projection)
Projection 		*projection;
{
Connection 		*connection;
float 			simulation_dt;
PFI 			func;
Action			*action = NULL;

	simulation_dt = ClockValue(0);
	if(Clockrate(projection) < simulation_dt){
	    Error();
	    printf("the projection interval for '%s' (%f) ",
	    Pathname(projection),
	    Clockrate(projection));
	    printf("is less than the simulation dt (%f)\n",
	    simulation_dt);
	}
	/*
	** do the connections have a check action?
	*/
	if(projection->connection_object){
	    func = GetActionFunc(projection->connection_object,CHECK,NULL,NULL);
	} else {
	    func = NULL;
	}
	/*
	** check each connection
	*/
	for(connection=projection->connection;
	connection;connection = connection->next){
	    if(connection->target == NULL){
		Error();
		printf("NULL destination segment for projection '%s'\n",
		Pathname(projection));
	    } else
	    /*
	    ** check the time intervals of each destination
	    ** segment for each absolute connection
	    */
	    if(Clockrate(projection) <
	    Clockrate(connection->target)){
		/*
		** if they are not the same then flag it
		*/
		Warning();
		printf("projection '%s' updates more rapidly than segment '%s'\n",
		Pathname(projection),
		Pathname(connection->target));
	    }
	    if(func){
		/*
		** call the function which will check the connection
		*/
		func(connection,action,projection);
	    }
	}
} 
