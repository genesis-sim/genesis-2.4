static char rcsid[] = "$Id: simple.c,v 1.2 2005/07/01 10:03:06 svitak Exp $";

/*
** $Log: simple.c,v $
** Revision 1.2  2005/07/01 10:03:06  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.1  1992/12/11 19:05:18  dhb
** Initial revision
**
*/

#include "syn_ext.h"

/* M.Wilson Caltech 6/88 */
void SimpleSynapse(connection,action,projection,event,offset_time)
register Connection	*connection;
Action			*action;
Projection		*projection;
register Event		*event;
float			offset_time;
{
    connection->target->activation += event->magnitude*connection->weight;
}

/* M.Wilson Caltech 2/89 */
int CREATE_SimpleSynapse(connection,action,projection,target)
Connection	*connection;
Action		*action;
Projection	*projection;
Element		*target;
{
    /*
    ** check the target type
    if(!CheckClass(target,SEGMENT_ELEMENT)){
	return(0);
    }
    */
    connection->weight = 1;
    /*
    ** indicate success
    */
    return(1);
}

/* M.Wilson Caltech 2/89 */
int CHECK_SimpleSynapse(connection,action,projection)
Connection	*connection;
Action		*action;
Projection	*projection;
{
int status = 1;

    if(connection->weight < 0){
	Warning();
	printf("connection from '%s' to '%s' has a negative weight\n",
	Pathname(projection),Pathname(connection->target));
	status = 0;
    } 
    if(connection->delay < 0){
	Error();
	printf("connection from '%s' to '%s' has a negative delay\n",
	Pathname(projection),Pathname(connection->target));
	status = 0;
    } 
    /*
    ** indicate success
    */
    return(status);
}
