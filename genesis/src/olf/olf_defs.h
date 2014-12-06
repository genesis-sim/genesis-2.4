/* $Id: olf_defs.h,v 1.1.1.1 2005/06/14 04:38:34 svitak Exp $ */

/* Version EDS22d 98/05/04, Erik De Schutter, BBF-UIA 8/92-5/98 */

/*
** $Log: olf_defs.h,v $
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.6  2000/07/03 18:25:34  mhucka
** Added #ifndef/endif OLF_DEFS_H.
**
** Revision 1.5  2000/06/19 06:14:31  mhucka
** The xmin/ymin and xmax/ymax parameters in the interpol and hsolve
** structures are doubles.  Functions that read these values from input files
** (e.g., for implementing tabcreate actions) need to treat them as doubles,
** not as float, because on some platforms (e.g., IRIX 6.5.3 using IRIX's C
** compiler), treating them as float results in roundoff errors.
**
** Revision 1.4  2000/06/07 17:51:42  mhucka
** Changes from Upi Bhalla to add a TAB_DELAY step_mode in order to implement
** a delay line.
**
** Revision 1.3  1998/06/30 23:25:43  dhb
** EDS 22d merge
**
 * EDS22d revison: EDS BBF-UIA 98/04/30-98/05/04
 * Added instantaneous gates to tab(2)channels
 *
** Revision 1.2  1997/05/29 18:17:20  dhb
** Moved RCS Id tag into comment
**
** Revision 1.1  1997/05/29 08:21:37  dhb
** Initial revision
**
*/

/*
 * Revision 1.14  1995/11/04  20:40:13  dhb
 * Changed to always use the exp10 macro as other systems (SGI) are
 * also missing the function.
*/

#ifndef OLF_DEFS_H
#define OLF_DEFS_H

#define ERR -1
#define exp10(x) exp(2.302585093 * (x))
#define ZERO_CELSIUS	273.15			/* deg */
#define GAS_CONSTANT	8.314			/* (V * C)/(deg K * mol) */
#define FARADAY		9.6487e4		/* C / mol */

#define TABCREATE 200
#define TABFILL 201
#define TAB2FIELDS 207
#define TABOP 208

#define SING_TINY 1e-6 /* MCV addition: small number for detecting singularities in tables */
#define NO_INTERP 0
#define LIN_INTERP 1
#define FIXED 2

#define B_SPLINE_FILL 0
#define C_SPLINE_FILL 1
#define LINEAR_FILL 2

#define TAB_IO 0
#define TAB_LOOP 1
#define TAB_ONCE 2
#define TAB_BUF 3
#define TAB_SPIKE 4
#define TAB_FIELDS 5
#define TAB_DELAY 6

#define VOLT_INDEX 0
#define C1_INDEX 1
#define C2_INDEX 2
#define DOMAIN_INDEX 3
#define VOLT_C1_INDEX -1
#define VOLT_C2_INDEX -2
#define VOLT_DOMAIN_INDEX -3
#define C1_C2_INDEX -4
#define DOMAIN_C2_INDEX -5

#define INSTANTX 1
#define INSTANTY 2
#define INSTANTZ 4

#define DEFAULT_XDIVS 3000
#define DEFAULT_2DIVS 300
#define DEFAULT_XMIN ((double) -0.1)
#define DEFAULT_XMAX ((double) 0.05)
#define DEFAULT_YMIN ((double) 0.0)
#define DEFAULT_YMAX ((double) 0.01)
#define SETUP_ALPHA 0
#define SETUP_TAU 1

#define TAB_IO 0
#define TAB_LOOP 1
#define TAB_ONCE 2

#define TABCHAN_T 20
#define TAB2CHAN_T 21
#define TABCURR_T 26
#define TABFLUX_T 27

#define VOLTAGE     0
#define CONCEN1     1
#define CONCEN2     2
#define DOMAINCONC  3
#define EK          4
#define ADD_GBAR  5

#endif
