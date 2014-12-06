/*
** $Id: sim_struct.h,v 1.1.1.1 2005/06/14 04:38:28 svitak Exp $
** $Log: sim_struct.h,v $
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.12  2001/04/25 17:17:03  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.11  1997/05/28 23:56:21  dhb
** Changes from Antwerp GENESIS 21e
**
 *
 * EDS21d revision: EDS BBF-UIA 97/01/28-97/01/28
 * Added Gelement and Gsegment defs
 *
 * EDS20i-k revison: EDS BBF-UIA 95/06/08-95/09/28
 * Added interpol2d_struct, made it overlap completely with
 *  interpol_struct, except for new fields, so that we can use an interpol2d
 *  pointer to access an interpol.
 *
 * Revision 1.10  1996/05/16  21:46:31  dhb
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
 * Revision 1.9  1994/12/20  00:42:09  dhb
 * Changed the lastmsg field for MsgIn to msgflags to support several
 * flags for each MsgIn.  MSGINLAST() and MSGINFORW() macros for testing
 * for last message and forwarded message.  Updated MSGINNEXT() to use
 * MSGINLAST().
 *
 * Revision 1.8  1994/05/27  22:30:55  dhb
 * Added ExtFieldInfo and IndFieldInfo structures
 *
 * Removed ExtField structure
 *
 * Put MsgIn pointer back into MsgOut structure
 *
 * Revision 1.7  1993/12/23  03:00:58  dhb
 * Extended Objects (mostly complete)
 *
 * Revision 1.6  1993/06/29  18:56:07  dhb
 * Extended objects (incomplete)
 *
 * Revision 1.5  1993/03/18  17:20:48  dhb
 * Element field protection.
 *
 * Revision 1.4  1993/03/10  23:15:58  dhb
 * Extended element fields.
 *
 * Revision 1.3  1993/03/08  18:27:37  dhb
 * Added default element field values.
 *
 * Revision 1.2  1992/12/18  21:19:06  dhb
 * Compact messages
 *
 * Revision 1.1  1992/10/27  21:19:36  dhb
 * Initial revision
 *
*/

#ifndef SIM_STRUCT_H
#define SIM_STRUCT_H

#include "struct_defs.h"

typedef struct projection_type			Projection;
typedef struct projection_parm_type		Parm;
typedef struct element_type			Element;
typedef struct Gelement_type			GElement;
typedef struct segment_type			Segment;
typedef struct Gsegment_type			GSegment;
typedef struct event_type			Event;
typedef struct buffer_type			Buffer;
typedef struct connection_type			Connection;
typedef struct object_type			GenesisObject;
typedef struct interpol_struct			Interpol;
typedef struct interpol2d_struct		Interpol2D;
#include "shell_struct.h"

/*
** integer time fields include
** projection->tmax
** buffer->tmin
** event->time
** connection->delay
**
** these have been changed to float and the code should be compiled 
** with FLOATTIME defined
*/

typedef struct {
    char		*name;		/* action name */
    int			type;		/* action type */
    char		*data;		/* action data structure */
    int			argc;		/* optional action arg list */
    char		**argv;	
    char		*passback;	/* return data */
} Action;

typedef struct alist_type {
    char		*name;		/* action name */
    int			type;		/* action type */
    int			flags;		/* action flags */
    int			scriptfunc;	/* function is a script function name */
    PFI			function;	/* function to call with this action */
    GenesisObject	*object;	/* object which defines the action
					   (which could be a super object) */
    struct alist_type	*next;
} ActionList;

typedef struct mlist_type {
    char		*name;		/* msg name */
    int			type;		/* msg type */
    int			flags;
    int			slots;		/* number of msg slots */
    char		**slotname;	/* names for the slots */
    struct mlist_type	*next;
} MsgList;

typedef struct flist_ext {
    int			ext_index;	/* index of extended field */
} ExtFieldInfo;

typedef struct flist_ind {
    char*		ind_elm;	/* Path to element */
    char*		ind_field;	/* field in element */
} IndFieldInfo;

typedef struct flist_type {
    char		*name;		/* field name */
    struct flist_type	*next;
    int			flags;		/* protections bits, etc. */
    char*		desc;
    union
      {
	ExtFieldInfo	ext;
	IndFieldInfo	ind;
      } info;
} FieldList;

typedef struct flist_iter_type {
    GenesisObject*	fli_object;
    FieldList*		fli_fl;
} FLI;

typedef struct clist_type {
    int			id;
    int			flags;
    struct clist_type 	*next;
} ClassList;

struct object_type {
    char		*name;		/* object name */
    char		*type;		/* name of the object data structure */
    int			size;		/* size of the data structure */
    PFI			function;	/* object function */
    int			flags;
    ClassList		*class;		/* object class list */
    ActionList		*valid_actions;
    MsgList		*valid_msgs;
    FieldList		*fieldlist;	/* list of fields of this object */
    char		*description;	/* object description */
    char		*author;	/* object author */
    short		method;		/* integration method */
    Element		*defaults;	/* default values for element */
    GenesisObject	*super;		/* Object from which this object is
					   derived, NULL if built in */
};

typedef struct {
    PFI			function;
    int			argc;
    char		**argv;
    int			spec_argc;
    char		**spec_argv;
} Schedule;

typedef struct	{
    int			argc;
    char		**argv;
} Arglist;

typedef struct {
    char 		*name;
    char		*adr;
} VarList;

typedef char *InternalMsg;

typedef struct {
    char		*data;
    PFD			func;
} Slot;

typedef struct msg_type {
    short		type;
    unsigned char	nslots;
    unsigned char	msgflags;
    Element		*src;
    Element		*dst;
    struct msg_type	*next_in;
    struct msg_type	*next_out;
} Msg;

typedef struct msg_type	MsgIn;
typedef struct msg_type	MsgOut;

typedef struct {
    Element	**element;
    Element	**sp;
    short	sp_used;
} ElementStack;

struct mask_type { 
    short		type;
    float 		xl,yl,xh,yh; 
}; 

struct region_type { 
    short		type;
    int 		xl,yl,xh,yh; 
}; 

struct region_mask_type {
    int			xmin,xmax,ymin,ymax;
    char		**map;
};

struct delay_table_type {
    int			xmin,xmax,ymin,ymax;
    float		**table;
};

/*
** the grid structure provides a means of accessing elements
** according to their xy coordinates
*/
struct map_type {
    float		wxmin;
    float		wymin;		/* max world coordinates for the grid */
    float		dx; 		/* the spacing between grid points in */
    float		dy; 		/* world coordinates */
    int			xmax,ymax; 	/* max grid indices */
    Element		***grid; 	/* a 2D array of pointers to elements */
};

struct segment_map_type {
    /*
    ** two-dimensional map of destination segments
    */
    float					wxmin,wymin;
    float					dx,dy;
    int						xmax,ymax;
    /*
    ** a 2D array of pointers to destination segment structures
    */
    Segment					***grid;
};

struct rel_connection_type {
    /*
    ** relative coordinates of the dst cell
    */
    short 					x;
    short					y;
    short					weight;
    char 					*specific;
};

struct converging_connection_type {
    ELEMENT_TYPE
    /*
    ** linked list containing pointers to connections 
    */
    Connection			 		*connection;
    Projection					*src_projection;
};

struct from_element_type {
    Element					*element;
    struct from_element_type			*next;
};

struct projection_parm_type {
    char					*name;
    char					*type;

    struct segment_map_type			*dst_map;
    struct region_mask_type			*src_region_mask;
    struct region_mask_type			*dst_region_mask;
    struct delay_table_type			*delay_table;


    /*
    ** dst segment used in relative connections 
    */
    int						target;
    int						rel_connection_tmax;
    float					time_resolution;
    /*
    ** interval between event propagations along the projection
    ** this should be consistent with the interval for 
    ** all destination segments the event buffer to
    ** which it is attached
    */
    short					transform;
    float					matrix[3][3];
    float					translate[3];

    /*
    ** relative connection information
    */
    int						*rel_connection_nentries;
    struct rel_connection_type			**rel_connection;
    PFI						rel_connection_func;
    PFI						fill_rel_connection_func;
};

/*
** the element list is a means of grouping elements together
*/
typedef struct element_list_type {
    int 					size;
    int 					nelements;
    Element		 			**element;
} ElementList;

struct info_type {
    char 		*name;
    char 		*type;
    int	 		address;
    short 		indirection;
    int 		type_size;
    int 		offset;
    short 		field_indirection;
    int 		field_size;
    short 		dimensions;
    int 		dimension_size[4];
    short 		function;
    short 		function_type;
    short 		struct_type;
};

typedef struct {
    char		*name;
    int			size;
    char		*data;
} SaveStruct;

/*
** ELEMENT STRUCTURES
*/

struct element_type {
    ELEMENT_TYPE
};

struct Gelement_type {
    GELEMENT_TYPE
};

struct projection_type {
    PROJECTION_TYPE
};

struct event_type {
    float					time;
    double					magnitude;
};

struct buffer_type {
    BUFFER_TYPE
};

struct segment_type {
    SEGMENT_TYPE
};

struct Gsegment_type {
    GSEGMENT_TYPE
};

struct basic_element_type {
    ELEMENT_TYPE
    /*
    ** a pointer to a two-dimensional map of elements in the group
    */
    struct map_type				*map;
};

struct connection_type {
    CONNECTION_TYPE
};

#define INTERPOLX_TYPE \
    double  xmin;   \
    double  xmax;   \
    int     xdivs;  \
    double  dx;     \
    double  invdx;  \
    double  sx,sy;  \
    double  ox,oy;  \
    short   allocated; \
    short   calc_mode;	/* This can be one of : NO_INTERP, LIN_INTERP, FIXED */

struct interpol_struct {
    INTERPOLX_TYPE
    double  *table;
};

struct interpol2d_struct {
    INTERPOLX_TYPE
    double  **table;
    double  *xgrid;
    short   xgrid_allocated;

    double  ymin;
    double  ymax;
    int     ydivs;
    double  dy;
    double  invdy;
    double  *ygrid;
    short   ygrid_allocated;

    double  sz;
    double  oz;

};

#endif  /* STRUCT_DEFS_H */
