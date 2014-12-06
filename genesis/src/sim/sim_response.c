static char rcsid[] = "$Id: sim_response.c,v 1.2 2005/06/27 19:01:09 svitak Exp $";

/*
** $Log: sim_response.c,v $
** Revision 1.2  2005/06/27 19:01:09  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.3  2001/04/25 17:17:03  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.2  1993/07/21 21:32:47  dhb
** fixed rcsid variable type
**
 * Revision 1.1  1992/10/27  20:26:43  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include "shell_func_ext.h"
#include "sim_ext.h"

void ErrorMessage(header,message,element)
char	*header;
char	*message;
Element	*element;
{
    Error();
    printf("%s : %s '%s'\n",header,message,Pathname(element));
}

void ActionHeader(header,element,action)
char	*header;
Element	*element;
Action	*action;
{
    printf("%s : '%s' action '%s'\n",
    header,
    Pathname(element),
    Actionname(action));
}

void InvalidAction(header,element,action)
char	*header;
Element	*element;
Action	*action;
{
    Warning();
    printf("%s : '%s' can't handle action '%s'\n",
    header,
    Pathname(element),
    Actionname(action));
}

void InvalidPath(header,path)
char	*header;
char	*path;
{
    Error();
    printf("%s : cannot find element '%s'\n",header,path);
}

void InvalidField(header,element,field)
char	*header;
Element	*element;
char	*field;
{
    Error();
    printf("%s : cannot find field '%s' on '%s'\n",
    header,
    field,
    Pathname(element));
}
