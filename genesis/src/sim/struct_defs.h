/*
** $Id: struct_defs.h,v 1.1.1.1 2005/06/14 04:38:28 svitak Exp $
** $Log: struct_defs.h,v $
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.8  2001/04/25 17:17:04  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.7  1997/05/28 23:56:21  dhb
** Changes from Antwerp GENESIS 21e
**
 *
 * EDS21d revision: EDS BBF-UIA 97/01/28-97/04/22
 * Added Gelement and Gsegment defs
 *
 * Version EDS20l 95/12/21, Erik De Schutter, BBF-UIA 6/94-12/95
 * Introduced CHANNELC2_TYPE definition
 * Revision 1.5  1994/05/27  23:31:44  dhb
 * Changed element extended fields from an allocated array of ExtField
 * structures to an allocated array of char*.  The extfields array now
 * holds extended field values only rather than field name/value pairs.
 *
 * Revision 1.6  1996/05/16  21:46:31  dhb
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
 * Revision 1.4  1993/06/29  18:56:07  dhb
 * Extended objects (incomplete)
 *
 * Revision 1.3  1993/03/10  23:15:58  dhb
 * Extended element fields.
 *
 * Revision 1.2  1992/12/18  21:19:06  dhb
 * Compact messages
 *
 * Revision 1.1  1992/10/27  21:19:36  dhb
 * Initial revision
 *
*/

#ifndef STRUCT_DEFS_H
#define STRUCT_DEFS_H

/* EDS: FUTURE changes needed (requiring full code revision): 
**  GELEMENTs (x,y,z) and (x0,y0,z0) should be doubles for accuracy in
**  microscopic domain.
**  Requires: move x,y,z to end of ELEMENT struct
**            check always if ELEMENT or nor before using GELEMENT (sigh...)
**  Alternative: consider dropping x,y,z from standard element
*/

#define		ELEMENT_TYPE \
    char					*name;\
    int						index;\
    struct object_type				*object;\
    short					flags;\
    short					nextfields;\
    char					**extfields;\
    float					x;\
    float					y;\
    float					z;\
    unsigned int				nmsgin;\
    Msg						*msgin;\
    Msg						*msgintail;\
    unsigned int				nmsgout;\
    Msg						*msgout;\
    Msg						*msgouttail;\
    GenesisObject				*compositeobject;\
    Element 					*componentof;\
    Element 					*parent;\
    Element					*child;\
    Element					*next;

#define		GELEMENT_TYPE \
    char					*name;\
    int						index;\
    struct object_type				*object;\
    short					flags;\
    short					nextfields;\
    char					**extfields;\
    float					x;\
    float					y;\
    float					z;\
    unsigned int				nmsgin;\
    Msg						*msgin;\
    Msg						*msgintail;\
    unsigned int				nmsgout;\
    Msg						*msgout;\
    Msg						*msgouttail;\
    GenesisObject				*compositeobject;\
    Element 					*componentof;\
    Element 					*parent;\
    Element					*child;\
    Element					*next; \
    float					x0;\
    float					y0;\
    float					z0;


#define SEGMENT_TYPE \
    ELEMENT_TYPE  \
    float					activation;

#define GSEGMENT_TYPE \
    GELEMENT_TYPE  \
    float					activation;

#define CHAN_TYPE \
	SEGMENT_TYPE \
	double	Ik;\
	double	Gk;\
	float	Ek;

#define CHANNELC2_TYPE \
    double  X; \
    double  Y; \
    float   tau1; \
    float   tau2; \
    float   gmax; \
    double  xconst1; \
    double  xconst2; \
    double  yconst1; \
    double  yconst2; \
    float   norm;

/*
** BUFFER
**
** tmin 	: the minimum time interval which must be held by the buffer
** size 	: buffer size in number of events
** current 	: current buffer location
** start 	: starting buffer location - 1
** end 		: last occupied buffer location
** event_size	: size of each event data structure
** event 	: circular event buffer
*/
#define BUFFER_TYPE \
    ELEMENT_TYPE  \
    float					tmin; \
    int						size; \
    int						current; \
    int						start; \
    int						end; \
    int						event_size; \
    struct event_type				*event; 

#define PROJECTION_TYPE \
    ELEMENT_TYPE  \
    GenesisObject				*connection_object; \
    Connection 					*connection; 

#define LINK_TYPE \
    Segment					*target;\
    Connection 					*next; \
    float					weight; 

/*
** CONNECTION
**
** dst_segment : pointer to the destination segment
** delay : time delay between src and dst cell
** this delay does not include the fixed latency
** of individual projections
** weight : synaptic weight used by many synaptic functions
*/
#define CONNECTION_TYPE \
    Segment					*target;\
    Connection 					*next; \
    float					weight; \
    float					delay; 

#define OUTPUT_TYPE \
    ELEMENT_TYPE \
    short					datatype; \
    short					datasize; \
    short					mixed_data; \
    struct output_data_type 			*data;


#endif /* STRUCT_DEFS_H */

