static char rcsid[] = "$Id: projutil.c,v 1.2 2005/07/01 10:03:06 svitak Exp $";

/*
** $Log: projutil.c,v $
** Revision 1.2  2005/07/01 10:03:06  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.1  1994/09/23 16:12:46  dhb
** Initial revision
**
 * Revision 1.2  1993/07/21  21:32:47  dhb
 * fixed rcsid variable type
 *
 * Revision 1.1  1992/10/27  20:24:17  dhb
 * Initial revision
 *
*/

#include "simconn_ext.h"

int CountProjections(projection)
Projection *projection;
{
int i = 0;

    for(;projection;projection=(Projection *)(projection->next)) i++;
    return(i);
}

int CountConnections(connection)
Connection *connection;
{
int i = 0;

    for(;connection;connection=connection->next) i++;
    return(i);
}

void DeleteProjection(projection)
Projection 	*projection;
{
Connection	*connection;
Connection	*old;

    /*
    ** free connections
    */
    connection=projection->connection;
    while(connection){
	old = connection;
	connection = connection->next;
	free(old);
    }
    /*
    ** free the projection
    */
    free(projection);
}
