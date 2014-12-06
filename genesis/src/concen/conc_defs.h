/* Version EDS22h 99/03/23, Erik De Schutter, BBF-UIA 8/94-3/99 */

/*
** $Id: conc_defs.h,v 1.1.1.1 2005/06/14 04:38:29 svitak Exp $
**
** $Log: conc_defs.h,v $
** Revision 1.1.1.1  2005/06/14 04:38:29  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.4  1999/10/17 22:13:23  mhucka
** New version from Erik De Schutter, dated circa March 1999.
**
** Revision 1.2  1997/05/28 21:07:29  dhb
** Added RCS id and log headers
**
*/

#define FARADAY 9.6487e4            /* C / mol */
#define UTINY 1.0e-30

/* shape_mode types */
#define SHELL 0
#define SLAB 1
#define TUBE 2
#define USERDEF 3

/* msg types */
#define INFLUX 0
#define OUTFLUX 1
#define FINFLUX 2
#define FOUTFLUX 3
#define STOREINFLUX 4
#define STOREOUTFLUX 5
#define DIFF_DOWN 6
#define DIFF_UP 7
#define BUFFER 10
#define TAUPUMP 11
#define EQTAUPUMP 12
#define MMPUMP 13
#define HILLPUMP 14
#define CONCEN 0
#define BDIFF_DOWN 1
#define BDIFF_UP 2
#define MMKD 1


