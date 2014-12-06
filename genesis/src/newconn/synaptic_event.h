/* $Id: synaptic_event.h,v 1.2 2005/06/27 19:00:44 svitak Exp $
** $Log: synaptic_event.h,v $
** Revision 1.2  2005/06/27 19:00:44  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:29  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.2  1997/08/12 22:22:26  dhb
** Removed short qulaified from bitfield declatations in synaptic_event.
** This is an over specification which some compilers complain about.
**
** Revision 1.1  1995/01/11 23:09:02  dhb
** Initial revision
**
*/

/* 
 * The synaptic event structure is defined here rather than in the struct.h
 * file because the use of bit fields causes problems otherwise.
 * The alloced field designates whether or not this node
 * was the first node in a malloc'ed group of nodes.  This is used
 * in freeing the nodes later.
 */

#ifndef SYNAPTIC_EVENT_H
#define SYNAPTIC_EVENT_H

typedef struct SynapticEvent_type SynapticEvent;

struct SynapticEvent_type {
    unsigned       alloced:1;   
    unsigned       syn_num:15;
    unsigned int   time;
    SynapticEvent *next;
};

extern void FreeSynapticEventLists();
extern int  AllocateSynapticEventNodes();

#endif
