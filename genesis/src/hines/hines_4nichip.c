static char rcsid[] = "$Id: hines_4nichip.c,v 1.1.1.1 2005/06/14 04:38:32 svitak Exp $";

/* Version EDS21d 97/03/21, Erik De Schutter, Caltech & BBF-UIA 4/92-3/97 */

/*
** $Log: hines_4nichip.c,v $
** Revision 1.1.1.1  2005/06/14 04:38:32  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.1  1997/05/28 23:11:50  dhb
** Initial revision
**
*/

/* Defines variables to create do_chip_hh4ni_update(hsolve) function in
**  hines_chip.c
*/

#include "hines_ext.h"
#define CHANMODE4
#include "hines_chip.c"
#undef CHANMODE4
