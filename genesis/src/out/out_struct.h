/*
** $Id: out_struct.h,v 1.3 2006/03/08 05:03:39 svitak Exp $
** $Log: out_struct.h,v $
** Revision 1.3  2006/03/08 05:03:39  svitak
** Changed output values to double precision.
**
** Revision 1.2  2006/02/17 07:42:01  svitak
** Added float_format field and CREATE action to initialize it.
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.2  1997/05/29 08:51:28  dhb
** Update from Antwerp GENESIS version 21e
**    - ASCFILE_TYPE macro
**    - structs for new objects: spike_history, res_ascfile
**
** Revision 1.2  1997/05/29 08:51:28  dhb
** Update from Antwerp GENESIS version 21e
**    - ASCFILE_TYPE macro
**    - structs for new objects: spike_history, res_ascfile
**
** Revision 1.1  1992/12/11  19:03:30  dhb
** Initial revision
**
*/

/* Version EDS21b, Erik De Schutter, Caltech & UIA-BBF 91-08/96 */

#include "struct_defs.h"

typedef struct output_type			Output;

struct output_data_type {
    /*
    ** pointer to the element from which the data will be taken
    */
    Element					*element;
    /*
    ** location of the data
    */
    float					x,y,z;
    /*
    ** pointer to the data itself
    */
    float					*dataptr;
    /*
    ** data type (e.g. float, int etc)
    */
    short					datatype;
    /*
    ** size in bytes of each piece of data
    */
    short					datasize;
    /*
    ** operator on the data before output
    ** this should return a pointer to the transformed data
    */
    PFI						data_func;
    struct output_data_type 			*next;
};

struct output_type {
    OUTPUT_TYPE
};

struct file_type {
    ELEMENT_TYPE
    /*
    ** data file information
    */
    char	*filename;
    FILE	*fp;
    /*
    ** is the file currently open?
    */
    short	is_open;
    /*
    ** has the file been initialized?
    */
    short	initialize;
    /*
    ** should the file be left open?
    ** if the file is used frequently then this is a good idea.
    ** unfortunately there is a limit to the number of file
    ** which can be left open any any time
    */
    short	leave_open;
    /*
    ** should data be appended after resetting
    */
    short	append;
    /*
    ** should the data be flushed to disk at each interval
    */
    short	flush;
};

struct xviewfile_type {
    ELEMENT_TYPE
    char	*filename;
    FILE	*fp;
    short	is_open;
    short	initialize;
    short	leave_open;
    short	append;
    short	flush;
    int		xmax;
};

#define ASCFILE_TYPE \
    ELEMENT_TYPE \
    char    *filename; \
    FILE    *fp; \
    short   is_open; \
    short   initialize; \
    short   leave_open; \
    short   append; \
    short   flush; \
    char    *float_format; \

struct ascfile_type {
    ASCFILE_TYPE
    short   notime;
};

struct res_ascfile_type {
    ASCFILE_TYPE
    float   time_res;
    float   value_res;
    double   last_time;
    int     nvalues;
    double   *last_values;
};
 
struct spikehistory_type {
    ASCFILE_TYPE
    short     ident_toggle;   /* if 0, prints index of incoming message, */
                              /* if 1, prints full pathname of sender    */
};

struct oldgraph_type {
    ELEMENT_TYPE
    short	overlay;
    float	xoffset;
    float	yoffset;
    short	last_plot;
    short	phase_plot;
};

struct oldxview_type {
    OUTPUT_TYPE
    int		ndata;
    int		plotnum;
    int		display_mode;
    int		color;
};

struct script_output_type {
    ELEMENT_TYPE
    char	*command;
    int		argc;
    char	**argv;
};
