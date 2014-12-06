/*
** $Id: sim_defs.h,v 1.1.1.1 2005/06/14 04:38:28 svitak Exp $
** $Log: sim_defs.h,v $
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.21  1997/07/22 17:30:08  venkat
** Added #define for the QUIT action
**
 * Revision 1.20  1996/10/05  17:21:36  dhb
 * Added USECLOCK action
 *
 * Revision 1.19  1996/05/16  21:46:31  dhb
 * Change messages back to a linked structure.
 *
 * MsgIn and MsgOut are now unified into a single Msg structure which
 * is linked on two lists (inbound and outbound).
 *
 * GetMsgInByMsgOut() and GetMsgOutByMsgIn() are now macros which just
 * return the Msg structure.
 *
 * The MsgIn and MsgOut types are still supported but are now aliases
 * for Msg.
 *
 * Revision 1.18  1995/12/06  00:49:42  venkat
 * MSGLOOP changed to check for the passed msg pointer explicitly not
 * equal to NULL
 *
 * Revision 1.17  1995/09/27  00:24:14  venkat
 * MSGOUTLOOP macro now exits the for loop if the nmsgout for the passed
 * element is <= 0. Was causing core dumps on the alpha, otherwise.
 *
 * Revision 1.16  1995/04/01  17:14:11  dhb
 * Moved VERSION and VERSIONSTR macros into sim_header.h so that
 * a Makefile dependency for version changes could be set up.
 *
 * Revision 1.15  1995/02/11  06:07:42  dhb
 * Updated for 2.0.9 development release.
 *
 * Revision 1.14  1994/12/21  22:10:03  dhb
 * Updated VERSION and VERSIONSTR for 2.0.8 Development release.
 *
 * Revision 1.13  1994/12/20  00:42:09  dhb
 * Changed the lastmsg field for MsgIn to msgflags to support several
 * flags for each MsgIn.  MSGINLAST() and MSGINFORW() macros for testing
 * for last message and forwarded message.  Updated MSGINNEXT() to use
 * MSGINLAST().
 *
 * Revision 1.12  1994/12/15  17:47:49  dhb
 * Added MSGINDELETED and MSGOUTDELETED actions which are called AFTER
 * a MsgIn or MsgOut is deleted.  This is in addition to DELETEMSGIN and
 * DELETEMSGOUT which are called prior to deleting MsgIn or MsgOut.
 *
 * Revision 1.11  1994/09/02  18:15:39  dhb
 * Changes revision numbers to 2.0.6
 *
 * Revision 1.10  1994/08/31  02:56:44  dhb
 * Changed integration method numbering.
 *
 * Revision 1.9  1994/06/04  01:51:37  dhb
 * Changed version number to 2.05.
 *
 * Revision 1.8  1994/03/22  18:15:21  dhb
 * Added DUMP and UNDUMP action numbers.
 *
 * Revision 1.7  1994/03/19  00:45:39  dhb
 * Changed default field protection from readonly to readwrite.
 *
 * Revision 1.6  1993/12/23  03:00:58  dhb
 * Extended Objects (mostly complete)
 *
 * Revision 1.5  1993/06/29  18:56:07  dhb
 * Extended objects (incomplete)
 *
 * Revision 1.4  1993/03/18  17:20:48  dhb
 * Element field protection.
 *
 * Revision 1.3  1993/02/03  22:33:20  dhb
 * fixed MSGLOOP and MSGOUTLOOP macros to check the message count for zero
 * messages rather than expecting msgin or msgout to be NULL
 *
 * Revision 1.2  1992/12/18  21:20:49  dhb
 * Compact messages
 *
 * Revision 1.1  1992/10/27  21:19:36  dhb
 * Initial revision
 *
*/

#include "sim_version.h"
#define FLOATTIME

/*
** bit usage of the element flag (16 bits)
** 0-4		5 bits		clock
** 5-6		2 bits 		enable
** 7		1 bit 		internal marker
** 8		1 bit		local disable for hsolve.
** 8-12		4 bits 		reserved for future expansion
** 12-15	4 bits 		user
*/
#define CLOCKMASK		0x1F
#define BLOCKMASK		0x20
#define VISIBILITYMASK		0x40
#define MARKERMASK		0x80
#define USERMASK		0xF0
#define USERBITSHIFT		12
#define HSOLVEMASK		0x0100

/*
** usable flags = 0-3
*/
#define SetElementFlag(E,N) (E)->flags |= (1<<(N + USERBITSHIFT))
#define ClearElementFlag(E,N) (E)->flags &= ~(1<<(N + USERBITSHIFT))
#define CheckElementFlag(E,N) (((E)->flags & (1<<(N + USERBITSHIFT))) != 0)

/*
** pre-defined classes
*/
#define	INVALID_CLASS		0
#define	ELEMENT_ELEMENT		1
#define	SEGMENT_ELEMENT		2
#define	BUFFER_ELEMENT		3
#define	PROJECTION_ELEMENT	4
#define	CONNECTION_ELEMENT	5

#define NCLOCKS 		100
#define SIM_CLOCK 		0

/*
** pre-defined action types
*/
#define PROCESS			0
#define INIT			1
#define RESET			2
#define CHECK			3
#define COPY			4
#define SET			5
#define CREATE			6
#define RECALC			7
#define SHOW			8
#define DELETE			9
#define SAVE			10
#define RESTORE			11
#define SAVE2			12
#define RESTORE2		13
#define SETUP			14
#define RESULTS			15
#define	EVENT			16
#define	ADDMSGIN		17
#define	ADDMSGOUT		18
#define	DELETEMSGIN		19
#define	DELETEMSGOUT		20
#define DUMP			21
#define UNDUMP			22
#define MSGINDELETED		23
#define MSGOUTDELETED		24
#define USECLOCK		25
#define QUIT			26

/*
** pre-defined integration methods
*/
#define RK_INT			-102
#define TRAPEZOIDAL_INT		-101
#define GEAR_INT		-100
#define EPC_INT			-2
#define FEULER_INT		-1
#define EEULER_INT		0
#define AB2_INT			2
#define AB3_INT			3
#define AB4_INT			4
#define AB5_INT			5
#define BEULER_INT		10
#define CRANK_INT		11

/*
** element field protection and type flags
*/

#define	FIELD_PROT		0x7	/* Three bits reserved for protection */
#define	FIELD_READWRITE		0
#define	FIELD_READONLY		1
#define	FIELD_HIDDEN		2
#define	FIELD_PROT_DEFAULT	FIELD_READWRITE

#define	FIELD_PERMANENT		0x8	/* Field is permanent */

#define	FIELD_TYPE		0x70	/* Three bits reserved for type */
#define	FIELD_BUILTIN		0x0
#define	FIELD_EXTENDED		0x10
#define	FIELD_INDIRECT		0x20


/*
** Object flag definitions
*/

#define	OBJECT_MODIFIED 0x1	/* This object is a modified copy */

/*
** Interface macros to the object flags and object
*/

#define	ObjectSetModified(obj)		((obj)->flags |= OBJECT_MODIFIED)
#define	ObjectResetModified(obj)	((obj)->flags = (obj)->flags&~OBJECT_MODIFIED)
#define	ObjectIsModified(obj)		((obj)->flags & OBJECT_MODIFIED)
#define	SuperObject(obj)		((obj)->super)

/*
** Action flag defs
*/

#define	ACTION_PERMANENT	0x1


/*
** Class flag defs
*/

#define	CLASS_PERMANENT		0x1


/*
** Message list flag defs
*/

#define	MSGLIST_PERMANENT	0x1


#ifndef PI
#define	PI			3.1415926536
#endif

#ifndef TRUE
#define	TRUE			1
#define	FALSE			0
#endif

#define BUFFER_INCREASE_HINT	10
#define WRAP 			0
#define EXPAND 			1

#define CurrentEventTime(B) 	((B)->event[(B)->current].time)	
#define CurrentEvent(B)		((B)->event[(B)->current])	
#define ValidEvent(B)		((B)->start != (B)->current)
#define BufferFull(B) 		((((B)->end +1) % (B)->size) == (B)->start)

#define Euler(y,dy,dt) 		((y) + (dy)*(dt))
#define CheckInterval(T,DT)	(GetCurrentStep() % (int)(0.5 + (T)/(DT)) == 0)
#define MATCH(A,B) 		(strncmp(A,B,strlen(A)) == 0)

/*
** MsgIn flags byte definitions (only 8 bit available)
*/

#define	MSGFLAGS_LAST		0x1
#define	MSGFLAGS_FORW		0x2

#define MSGINLAST(M)		((M)->msgflags&MSGFLAGS_LAST)
#define MSGINFORW(M)		((M)->msgflags&MSGFLAGS_FORW)

#define	MSGSLOT(M)		((Slot*) (M+1))
#define	MSGINNEXT(M)		((M)->next_in)
#define	MSGOUTNEXT(M)		((M)->next_out)
#define MSGLOOP(C,M) 		for(M = (C)->msgin; \
					M != NULL; \
					M = MSGINNEXT(M)) \
				    switch(M->type)
#define MSGOUTLOOP(C,M)		for(M = (C)->msgout; \
					M != NULL; \
					M = MSGOUTNEXT(M))

#define MSGVALUE(M,N) 		((*(MSGSLOT(M)[N].func))(MSGSLOT(M) + N))
#define MSGPTR(M,N) 		(MSGSLOT(M)[N].data)

#define GetMsgInByMsgOut(elm, msgout)	((MsgIn*) (msgout))
#define GetMsgOutByMsgIn(elm, msgin)	((MsgOut*) (msgin))

/*
** quick but dangerous
#define MsgValue(M,T,N) 	(*((T *)((M)->data[N])))
*/

/* slow but safe */
#define MsgValue(M,T,N)		((*(MSGSLOT(M)[N].func))(MSGSLOT(M) + N))

#define ClockValue(C)		(clock_value[C])
/*
** get clock information from the element
*/
#define Clockrate(I)		(ClockValue(((I)->flags) & CLOCKMASK))
#define Clock(I)		(((I)->flags) & CLOCKMASK)

#define CORRECTION 		(ClockValue(0)/10.0)

#define SimulationTime()	simulation_time

#define Type(I) 		((I)->object->type)
#define Size(I) 		((I)->object->size)
#define Function(I) 		((I)->object->function)

#define SELECT_ACTION(A)	switch((A)->type)

#define ENABLED(E)	(!((E)->flags & BLOCKMASK))
#define ACTIVE(E)	((E)->object->function)
#define	VISIBLE(E)	((E)->componentof == ActiveElement || \
				(E)->componentof == NULL)
