/* $Id: par_ext.h,v 1.2 2005/09/29 23:15:33 ghood Exp $
 * $Log: par_ext.h,v $
 * Revision 1.2  2005/09/29 23:15:33  ghood
 * added InitComm & FinalizeComm routines
 *
 * Revision 1.1.1.1  2005/06/14 04:38:39  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.2  2002/01/17 18:44:57  dbeeman
 * Bug fixes from Greg Hood for pgenesis 2.2a, entered by dbeeman
 *
 * Revision 1.1  1999/12/19 03:59:41  mhucka
 * Adding PGENESIS from GENESIS 2.1 CDROM release.
 *
 * Revision 1.1  1995/04/03 22:04:03  ngoddard
 * Initial revision
 *
 * Revision 1.1  1994/09/07  19:01:00  ngoddard
 * passive messaging works, src figures map
 *
 * Revision 1.5  1993/12/30  00:06:22  ngoddard
 * Release 12/14/93
 *
 * Revision 1.2  1993/10/15  22:51:51  ngoddard
 * added Id and Log keywords
 *
 *
*/

#include "sim_ext.h"
#include "par_defs.h"
#include "par_struct.h"
#include <assert.h>
extern char* CopyString();
extern void InitComm();
extern void FinalizeComm();
