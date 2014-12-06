/*
** $Id: newconn_ext.h,v 1.1.1.1 2005/06/14 04:38:29 svitak Exp $
** $Log: newconn_ext.h,v $
** Revision 1.1.1.1  2005/06/14 04:38:29  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.4  2001/04/25 17:16:59  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.3  1996/07/22 23:52:54  dhb
** Changes from Mike Vanier:
**   Generic handling of synchan types
**
 * Revision 1.2  1995/03/28  20:41:20  mvanier
 * Added extern declarations for GetSynapseType and GetSynapseAddress
 *
 * Revision 1.1  1995/01/11  23:09:02  dhb
 * Initial revision
 *
*/

#include "sim_ext.h"
#include "newconn_defs.h"
#include "newconn_struct.h"
#include "synaptic_event.h"
#include "shell_func_ext.h"
#include "seg_struct.h"  /* for compartment definition */

extern float    rangauss();
extern void     randomize_value(); 
extern Synapse *GetSynapseAddress();



