/* $Id: newconn_defs.h,v 1.1.1.1 2005/06/14 04:38:29 svitak Exp $
** $Log: newconn_defs.h,v $
** Revision 1.1.1.1  2005/06/14 04:38:29  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.3  2000/09/21 19:32:43  mhucka
** Wrapped the definition of M_E inside #ifndef M_E, because on some systems
** like AIX, it ends up getting defined twice.
**
** Revision 1.2  1996/07/22 23:38:42  dhb
** Changes from Makw Vanier:
**   Generic handling of synchan types
**
 * Revision 1.1  1995/01/11  23:09:02  dhb
 * Initial revision
 *
*/

#ifndef M_E
#define M_E   2.7182818284590452354 
#endif

#ifndef TINY
#define TINY 1e-8
#endif

/* message types for randomspike */

#define RATE 0
#define MINMAX 1

/* message types for synchan */
 
#define SPIKE             -1 
#define VOLTAGE            0
#define ACTIVATION         1
#define RAND_ACTIVATION    2
#define MOD                3 
#define WEIGHT_CHANGE_MOD  4

/* extra action for synchan2 */
#define RESETBUFFER 1001


/*
 * Macros for setting and testing flags.
 */

#define SPIKE_THIS_TIME_STEP  0x1
#define SPIKE_BETWEEN_UPDATES 0x2
 
#define IS_FLAG_SET(A,B) ((A)&(B))
#define SET_FLAG(A,B)    ((B)=(B)|(A))
#define CLEAR_FLAG(A,B)  ((B)=((B)&(~(A))))


