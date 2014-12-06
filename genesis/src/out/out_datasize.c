static char rcsid[] = "$Id: out_datasize.c,v 1.2 2005/07/01 10:03:07 svitak Exp $";

/*
** $Log: out_datasize.c,v $
** Revision 1.2  2005/07/01 10:03:07  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.1  1992/12/11 19:03:21  dhb
** Initial revision
**
*/

#include "out_ext.h"

int DataSize(datatype)
int	datatype;
{
int	datasize = 0;
    switch(datatype){
    case INT :
	datasize = sizeof(int);
	break;
    case FLOAT :
	datasize = sizeof(float);
	break;
    case DOUBLE :
	datasize = sizeof(double);
	break;
    case SHORT :
	datasize = sizeof(short);
	break;
    case CHAR :
	datasize = sizeof(char);
	break;
    default:
	datasize = 0;
	break;
    }
    return(datasize);
}
