static char rcsid[] = "$Id: synshow.c,v 1.1.1.1 2005/06/14 04:38:34 svitak Exp $";

/*
** $Log: synshow.c,v $
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.1  1992/12/11 19:05:21  dhb
** Initial revision
**
*/

#include "syn_ext.h"

int SynapseField(connection,action)
Connection	*connection;
Action		*action;
{
char	*field;

    if((field = action->data) == NULL){
	action->passback = NULL;
    }
    if(strcmp(field,"target") == 0){
	action->passback = Pathname(connection->target);
	return(1);
    }
    return(0);
}
