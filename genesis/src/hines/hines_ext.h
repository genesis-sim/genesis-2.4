/* Version EDS22c 97/12/02 Erik De Schutter,  BBF-UIA 1/95-12/97 */

/*
** $Id: hines_ext.h,v 1.3 2005/07/20 20:02:00 svitak Exp $
** $Log: hines_ext.h,v $
** Revision 1.3  2005/07/20 20:02:00  svitak
** Added standard header files needed by some architectures.
**
** Revision 1.2  2005/06/27 19:00:40  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:32  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.7  2001/04/25 17:16:58  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.6  1999/10/17 21:47:27  mhucka
** Added return value declarations for a number of functions, in an effort to
** reduce the number of compile-time warnings.
**
** Revision 1.5  1999/10/17 21:39:37  mhucka
** New version of Hines code from Erik De Schutter, dated 30 March 1999
** and including an update to hines.c from Erik from June 1999.  This version
** does not include changes to support Red Hat 6.0 and the Cray T3E, which
** will need to be incorporated in a separate revision update.
**
 * EDS22c revison: EDS BBF-UIA 97/12/02-97/12/02
 * Moved all typedefs to this hines_struct.h file
 #
** Revision 1.2  1997/05/28 22:53:56  dhb
** Replaced with the version from Antwerp GENESIS 21e
**
** Revision 1.1  1992/12/11  19:03:12  dhb
** Initial revision
**
*/

#ifndef HINES_EXT_H
#define HINES_EXT_H

#include <math.h>
#include <stdio.h>
#include "sim_ext.h"
#include "shell_func_ext.h"
#include "conc_struct.h"
#include "dev_struct.h"
#include "hines_defs.h"
#include "hines_struct.h"
#include "olf_defs.h"
#include "olf_struct.h"
#include "seg_struct.h"
#include "newconn_struct.h"
#include "synaptic_event.h"

#ifdef __STDC__
int h_eventaction( Element *src, Element *dst, int index, int time );
#else
int h_init();
int h2_init();
int do_hget_children();
int h_hh_chip_init();
int h_init_conc_chip();
int h_init_conc_solve();
int do_fast_hsetup();
int h_hh_init();
int do_duplicate();
int hfind_elm();
int h_nernst_init();
int hstore_name();
int hchild_compare();
int h_has_output();
int duplicate_tables();
#endif

#endif
