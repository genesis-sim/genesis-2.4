static char rcsid[] = "$Id: sim_buffer.c,v 1.2 2005/06/27 19:00:58 svitak Exp $";

/*
** $Log: sim_buffer.c,v $
** Revision 1.2  2005/06/27 19:00:58  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.3  2001/04/25 17:17:01  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.2  1993/07/21 21:31:57  dhb
** fixed rcsid variable type
**
 * Revision 1.1  1992/10/27  19:38:34  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include "shell_func_ext.h"
#include "sim_ext.h"

void DeleteBuffer(buffer)
Buffer *buffer;
{
    /*
    ** free the events
    */
    if(buffer->event){
	free(buffer->event);
    }
    /*
    ** free the buffer
    */
    free(buffer);
}

int BufferCopy(buffer,action)
Buffer	*buffer;
Action	*action;
{

    /*
    ** action->data contains the copy of the buffer
    */
    if(action->data){
	((Buffer *)(action->data))->event = CopyEvents(buffer);
	return(1);
    } else {
	Error();
	printf("unable to copy events from '%s'\n",Pathname(buffer));
	return(0);
    }
}

int BufferCreate(buffer,action)
Buffer	*buffer;
Action	*action;
{
    buffer->event_size = sizeof(struct event_type);
    return(1);
}

