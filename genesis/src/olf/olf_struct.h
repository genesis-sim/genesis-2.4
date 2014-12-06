/*
** $Id: olf_struct.h,v 1.3 2006/02/23 03:49:41 svitak Exp $
**
** $Log: olf_struct.h,v $
** Revision 1.3  2006/02/23 03:49:41  svitak
** Changed a comment from // to /*
**
** Revision 1.2  2006/01/10 19:56:39  svitak
** Changes to allow voltage-dependent Z-gate.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.8  1999/10/17 22:28:42  mhucka
** Dave Beeman said that hh_tabchan should be considered a failed experiment,
** and be removed, so I've removed hh_tab_channel_type and the inclusion of
** hh_struct_defs.h.
**
** Revision 1.7  1998/06/30 23:28:37  dhb
** EDS 22d merge
**
** Revision 1.6  1998/06/30 21:19:37  dhb
** Added include of hh_struct_defs.h
** Added hh_tab_channel_type structure
**
** Revision 1.5.1.1  1998/06/30 23:25:43  dhb
** EDS 22d merge
**
 * EDS22d revison: EDS BBF-UIA 98/04/30-98/05/04
 * Added instantaneous gates to tab(2)channels
 *
** Revision 1.5  1997/05/29 08:32:23  dhb
** Update from Antwerp GENESIS version 21e
**
 * EDS21e revison: EDS BBF-UIA 95/08/1-97/04/16
 * Updated tabcurrent.
 *
 * EDS20j revison: EDS BBF-UIA 95/08/02
 * Added surface field to tab_current_type, table2d_type and tab2channel_type
 *
 * EDS20i revison: EDS BBF-UIA 95/06/08
 * Added tab_current_type, table2d_type and tab2channel_type
 * Added typedefs
 * Moved #defines to olf_defs.h
 *
 * Revision 1.4  1995/05/13  01:35:12  dhb
 * Changes to support xfileview like functionality using disk_in
 * and xview elements.
 *
 * Revision 1.3  1994/08/08  22:29:04  dhb
 * Changes from Upi.
 *
 * Revision 1.4  1994/06/13  22:52:59  bhalla
 * Added actions for TAB2FIELDS and TABOP
 *
 * Revision 1.3  1994/05/09  20:09:07  bhalla
 * Added defines for TAB_BUF and TAB_SPIKE modes of table.c
 *
 * Revision 1.2  1994/03/15  17:52:47  dhb
 * Removed interpol_struct definition and Interpol typedef.  These are now
 * in sim_struct.h
 *
 * Revision 1.1  1992/11/14  00:37:21  dhb
 * Initial revision
 *
*/

#include "struct_defs.h"

typedef struct tab_channel_type Tchan;
typedef struct tab2channel_type T2chan;
typedef struct tab_current_type Tcurr;

#define NODORS 10
struct olf_receptor_type {
	SEGMENT_TYPE
	float	response[NODORS];
	float	Vm;
	float	Vmin;
	float	Vmax;
	float	max_rate;
	float	min_rate;
	float	state;
	float	gain;
	int		transfer_form;
};

struct olf_receptor2_type {
	CHAN_TYPE
	float	*response;
	float	modulation;
	int		nodors;
	short	allocated;
	/*
	double	Ik;  Repositioned
	double	Gk;
	float	Ek;
	*/
	double	X,Y;	/* State variables for time response */
	float	tau1;
	float	tau2;
	float	gmax;
	double	xconst1;
	double	xconst2;
	double	yconst1;
	double	yconst2;
	float	norm;
};

struct tab_gate_type {
    SEGMENT_TYPE
    double	m;
	struct interpol_struct *alpha;
	short	alpha_alloced;
	struct interpol_struct *beta;
	short	beta_alloced;
    short	instantaneous;
};

struct table_type {
	ELEMENT_TYPE
	double	input;
	double	output;
	struct	interpol_struct *table;
	short	alloced;
	short	step_mode;
	double	stepsize;
};

struct table2d_type {
    ELEMENT_TYPE
    double  input;
    double  input2;
    double  output;
    double  negoutput;
    struct  interpol2d_struct *table;
    short   alloced;
    short   step_mode;
    double  stepsize;
};

#define TABCHAN_TYPE \
    float   Gbar;   \
    double  X;      \
    double  Y;      \
    double  Z;      \
    float   Xpower; \
    float   Ypower; \
    float   Zpower; \
    short   instant; \
    short   X_alloced; \
    short   Y_alloced; \
    short   Z_alloced; \
    float   surface; \
    short   Z_conc;
/* TODO: Add X_conc, Y_conc too? */

struct tab_channel_type {
    CHAN_TYPE
    TABCHAN_TYPE
    struct interpol_struct *X_A;
    struct interpol_struct *X_B;
    struct interpol_struct *Y_A;
    struct interpol_struct *Y_B;
    struct interpol_struct *Z_A;
    struct interpol_struct *Z_B;
};

struct tab2channel_type {
    CHAN_TYPE
    TABCHAN_TYPE
    short   Xindex;
    short   Yindex;
    short   Zindex;
    struct interpol2d_struct *X_A;
    struct interpol2d_struct *X_B;
    struct interpol2d_struct *Y_A;
    struct interpol2d_struct *Y_B;
    struct interpol2d_struct *Z_A;
    struct interpol2d_struct *Z_B;
};

struct tab_current_type {
    CHAN_TYPE
    float   Gbar;
    short   Gindex;
    short   alloced;
    float   surface;
    struct interpol2d_struct *G_tab;
    struct interpol2d_struct *I_tab;
};

struct disk_in_type {
	SEGMENT_TYPE
	FILE	*fp;
	char	*filename;
	short	fileformat;	/* 0 is default is ASCII, 1 is FMT1 */
	char	*tempdata;	/* data array used for FMT1 reading */
	float	start_time;	/* Fields used for FMT1 handling */
	float	dt;
	int	datatype;
	int	header_size;
	float	time_offset;	/* offset from sim time for FMT1 files */
	short	is_open;	/* is the file open yet ? */
	short	leave_open; /* leave the file open between cycles */
	long	lastpos;	/* Field for ASCII handling */
	short	loop;		/* Loop back to start when eof */
	int		nx,ny;		/* dimensions of array */
	short	allocated;	/* is the array allocated yet ? */
	float	**val;		/* array */
	struct	interpol_struct	*xpts;
	struct	interpol_struct	*ypts;
	struct	interpol_struct	*zpts;
};


struct dd_syn_type {
	CHAN_TYPE
	struct	interpol_struct *transf;
	short	transf_alloced;
	float	gmax;
	double	X,Y;	/* State variables for time response */
	float	tau1;
	float	tau2;
	double	xconst1;
	double	xconst2;
	double	yconst1;
	double	yconst2;
	float	norm;
};
