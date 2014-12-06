/* Version EDS22g 99/02/11 Erik De Schutter, Caltech & BBF-UIA 4/92-2/99 */

/*
** $Id: hines_struct.h,v 1.1.1.1 2005/06/14 04:38:32 svitak Exp $
** $Log: hines_struct.h,v $
** Revision 1.1.1.1  2005/06/14 04:38:32  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.10  2001/01/01 01:19:19  mhucka
** Fix from Hugo Cornelis for a bug in hsolve in which Vm is loaded too late
** under sone circumstances.
**
** Revision 1.9  2000/09/07 02:22:55  mhucka
** Fixes from Hugo Cornelis for various initialization problems.
**
** EDS22k revision HC BBF-UIA 00/07/24
** no normalization for incoming ACTIVATION msg's on hsolve'd synchans
** Fix for case when Vm was loaded to late (for Ca channel only with nernst Ek)
**
** Revision 1.8  2000/05/26 22:26:32  mhucka
** Comment cleanup.
**
** Revision 1.7  2000/05/26 08:20:08  mhucka
** ** EDS22j revison: HC BBF-UIA 00/03/13
** ** Added computeIm field
**
** Revision 1.6  1999/10/17 21:39:37  mhucka
** New version of Hines code from Erik De Schutter, dated 30 March 1999
** and including an update to hines.c from Erik from June 1999.  This version
** does not include changes to support Red Hat 6.0 and the Cray T3E, which
** will need to be incorporated in a separate revision update.
**
** EDS22g revison: EDS BBF-UIA 99/02/11-99/02/11
** Added chanmode 5
**
** EDS22f revison: EDS BBF-UIA 98/11/23-98/11/23
** Improved comments
**
** EDS22d revison: EDS BBF-UIA 98/09/29-98/09/30
** Added tabinteg field
**
** EDS22c revison: EDS BBF-UIA 97/12/02-98/03/12
** Moved all typedefs to this file
** Removed values, cip, ri and diag arrays; renamed symcomparts to symflag
**
** Revision 1.4  1997/05/28 22:57:26  dhb
** Replaced with version from Antwerp GENESIS 21e
**
** Revision 1.3  1995/12/06 00:47:50  venkat
** Pointer to an array of non-hsolved element pointers added to the hines struct
**
** Revision 1.2  1995/08/01  19:29:03  dhb
** Changes from Erik DeSchutter described below.
**
** Revision 1.4  1994/03/22 eds
** added msgcompts field
**
** Revision 1.1  1992/12/11  19:03:14  dhb
** Initial revision
**
*/

/* 
** 95/06/02 eds
** removed Vm_reset flag 
** Revision 1.4  1995/03/22 eds
** added msgcompts field
*/

#include "struct_defs.h"
typedef struct symcompartment_type Ncomp;
typedef struct compartment_type Comp;
typedef struct hsolve_type Hsolve;
typedef struct nernst_type Nernst;
typedef struct channelC3_type Schan;
typedef struct Synchan_type S2chan;
typedef struct Spikegen_type Spike;
typedef struct Ca_concen_type Concen;
typedef struct ghk_type Ghk;
typedef struct Mg_block_type Mblock;

typedef struct tchaninfo_type {
	Element	*chan;		/* pointer to channel object */
	Nernst *nernst;		/* pointer to nernst msg if exists */
	MsgIn  *conc;		/* pointer to concentration value if exists */
	struct tchaninfo_type *next;
} Tcinfo;

typedef struct chaninfo_type {
	Element	*chan;		/* pointer to channel object */
	struct chaninfo_type *next;
} Cinfo;

/* this is used only by read_hines and replaces the compartment elements */
typedef struct comp_type {
	char	*name;
	float	Cm;
	float	Rm;
	float	Ra;
	float	Em;
	float	inject;
	float	x,y,z;
	float   dia;
	float   len;
	Ncomp	*proto_compt;
	struct  comp_type *parent;
	struct  comp_type *next;
} Compinfo;

typedef struct outinfo_type {
	int	op;
	int	cindex;
	int	comptno;
	struct  outinfo_type *next;
} Oinfo;

typedef struct ininfo_type {
	int	type;
	short	sChildType;
	MsgIn	*msgin;
	int	nop;
	int	nchip;
	struct ininfo_type *next;
} Iinfo;

struct hsolve_type {
	ELEMENT_TYPE
	/* DO NOT CHANGE THIS FIELDS AFTER THE SETUP CALL ! */
	char	*path;		/* the wildcard list of elms to be integrated */
	int	comptmode;	/* no longer used */
	int	chanmode;	/* chooses option for channel calculations:
				**
				**    0 == default is the most general, uses
				** least memory and is slowest.
				**
				**    1   tabchannel equations are optimized.
				** Has limitations use of EK msg (nernst only)
				**
                        	**    2-5 assume integer powers. They are the 
				** fastest but use a large 'chip' array which 
				** necessitates use of special input/output 
				** routines. They differ in variable output:
				**    2: all compartments Vm fields are updated
				**    3: but all elements that had outgoing msg
				**       to non-hines objects during the SETUP 
				**       call are updated at a rate determined 
				**       by outclock.
				**    4: no elements are updated, but all state
				**       variables are stored in local arrays
				**       and can be accessed directly (use 
				**       findsolvefield function to get name).
				**    5: as 4 but all currents and 
				**       conductances are normalized for 
				**       compartment surface area.
				*/
	int	calcmode;	/* chooses option for interpolation of
				** tabulated equations, applies to chanmodes
				** 2-4 only:
				**
				**    0: no interpolation, is equivalent
				** to GENESIS 1.4 and 2.0 hsolve modes
				**
				**    1 == default: all tables are interpolated
				*/
	int	no_elminfo;	/* boolean, if false (default) the HPUT and 
				**  HSET commands can be used to get element 
				**  field values in/out of the hsolve chip 
				**  array and findsolvefield function is 
				**  available.  This field is relevant for
				**  chanmode>=2 only.
				*/
	int	outclock;	/* number [0-99] of the clock used for all
				** outgoing msg (only used if chanmode >= 3)
				**  default: element clock.  outclock should 
				**  have dt equal to or larger than hsolve clock
				*/
	int	storemode;	/* chooses options for storage of total currents
				**  model (only chanmode>=2).
				**
				**	0 = default: nothing is stored
				**
				**  1 stores all ionic and synaptic currents in 
				**    itotal (requires that no_elminfo==0)
				**
				**  2 stores all ionic and synaptic conductances
				**    in itotal (requires that no_elminfo==0)
				*/
	int computeIm;		/* chanmode 0,1 only
				** 0 = default : do not compute Im
				** 1 : compute Im (and store in compartments)
				*/
    /* all the following fields are initialized by SETUP and should not
	** be changed by user */
	int	symflag;	/* boolean, determines if symmetric compart */
	int	ncompts;	/* number of compartments in path */
	Element	**compts;	/* array of element pointers IN ORDER of EL */
	/* The order of EL is important : the following arrays depend on it ! */
	int	*parents;	/* array of indices for parent elms */
	int	*nkids;		/* array for number of child elms */
	int	**kids;		/* array of indices for child elms */
	int	*hnum;		/* array of indices for hines elmnumeration */
	int	*elmnum;	/* inverse of above */

	double	*results;	/* array of previous and new state vars:
				** mixed right-side and diagonals of ODEs */
	double  *vm;		/* array of Vm values */

	int	nfuncs;	
	int	*funcs;		/* big array for all ops for the actual hines
				** method part (solving compartment voltages) */
	int	**ravptr;	/* temporary storage for setting up funcs */

/* chanmode 0 arrays */
	Cinfo	**chan;		/* array of channel element link lists  */
	short   *msgcompts; 	/* array of flags for local compt update */
	double	*diagonals;	/* array of constant part diagonals */

/* chanmode 1 arrays */
	Tcinfo	**hh;		/* array of tabchannel info link lists, with 
				** ptr to conc and nernst fields */

/* chanmode 2-4 arrays */
	int	*piCompFlags;	/* flags for comp, 1 : NEWVOLT_OP needed */
	int	nchildren;
	Element **children;	/* array of element ptrs to compt children */
	short	*childtypes;	/* array identifying all children */
	float	*childdata; 	/* array having msg data for all children */
	int	*childmsg;	/* array having extra msg for all children */
	int	*childlink;	/* array having link info for all children */
	int 	*childcode;	/* array having code info for all children */
	int	*childpos;	/* array specifying position for all children */
	int	*childstart;	/* array index of first child for each comp */
	int	nravals;	/* size of Ra values array */
	double	*ravals;	/* array scaled Ra values (replaces values) */
	Iinfo	*ininfo;	/* input msg info list.  Used by chanmode>=2 to 
				** update the appropriate elements */
	Oinfo	*outinfo;	/* output msg info list.  Used by chanmode==3 to
				** update the appropriate elements */
	int	nchips;		/* number of entries in chip array */
	double	*chip;		/* array for channel element fields */
	int	nops;		/* number of entries in ops array */
	int	*ops;		/* array for channel instructions  */
	Element *spikegen;	/* pointer to Spikegen element */
	double  *vmlast;	/* pointer to end of vm array */

    /* concentration stuff */
	int	nconcs;	
	double  *conc;		/* array of concentration and buffer values */
	double  *flux;		/* array of flux values */
	int	nconcchips;	/* number of entries in concchip array */
	double	*concchip;	/* array for concen element fields */
	int	nconcops;	/* number of entries in concops array */
	int	*concops;   	/* array for concen instructions */
	int     *fconc; 	/* array indexes for each comp into conc array*/
	int	ndiffs;		/* total number of diffusion systems */
	int	*fdiffs;	/* array indexes to diffsystems for each compt*/
	int	*dchild; 	/* array indexes of diffsystems into children*/
	int	*dconc;		/* array indexes of diffsystems into conc */
	int	maxshells;	/* largest # of diffusion shells */
	int	maxshcols;	/* largest # of colums in diagonal diff array */
	int	*nshells;	/* array of number of shells each diffsystem */
	int	*nshrows;   	/* array of number of rows in each diffshell */
	int	*nshelms;   	/* array of number of elms in each diffshell */
	int	*nshfix;	/* array number fixbuffers in each diff shell */
	double	*covals0;	/* linear version of tridiag array diff pars */
	double	*covals;	/* scrap array to compute 1 diffusion system */
	double	*coresult;	/* results array of diffusion solution */
	double	*codiag;	/* diag array of diffusion solution */
	double	*cores;		/* scrap results array to compute 1 diffsystem*/
	int	*fcoval;	/* array indexes of diffsystems into covals0 */
	double	*bufvals;	/* array of buffer rate parameters */
	int	*fbufval;	/* array indexes of diffsystems into bufvals */

/* chanmode 4 arrays */
	int  	ngivals;	/* size of givals array */
	double  *givals;	/* contains computed variables (Gk,Ik,...) */
	double  *ovals;		/* pointer into givals, computed vars concchip*/
	int	*compgiv;	/* indexes into givals */
	int	*childgiv;	/* indexes into givals */

/* chanmode 2-4 arrays made if no_elminfo==0 */
	int	*childnames;	/* array pointing to elm_names of children */
	int     nelm_names;	/* number of different elm names */
	char    **elm_names;	/* stores different elm names */
	int	*compchips;	/* indexes compartment entries in chip array */
	int	*compops;	/* indexes compartment entries in ops array */
	int	*childops;	/* indexes child entries in ops array */
	int     *childchips;	/* indexes childentries in chip array */
	double  *itotal;	/* array that can store sum of all ionic and
				** synaptic currents or conductances */
/* chanmode 2-4 conductance factor tables */
	int	ntab;		/* number of tables in tabchannel table */
	int	ncols;		/* number of columns in table */
	double  **tables;	/* pointers to the original tabchannel tables */
	int	*tabcols;	/* starting column for tabchannels in table */
	int	*tabinteg;	/* codes type for tabchannels in table */
	double	invdx;		
	double	xmin;
	double	*xvals;		/* List of V values for composite table */
	int	xdivs;		/* number of rows-1 in table */
	double	*tablist;	/* The composite table. All voltage and conc 
				** indexed lookup tables are stored here so 
				** that values for the same voltages will be
				** represented in adjacent memory locations */
	double	invdy;		
	double	ymin;
	double	*yvals;		/* List of C values for composite table */
	int	ydivs;		/* number of columns in 2D tables */
	int	sntab;
	double	*stablist;	/* Table of constants for synaptic channels, 6
				** entries for each channel type */
	double  dt;		/* value of hsolve clock */
/* fields are only used when hsolve was created by duplicate or read_hines */
	int     readflag;	/* Controls initialization for read_hines */
	Hsolve	*origsolve; 	/* hsolve from which this one was DUPLICATED */
	Compinfo *comps;	/* linked list of compartment data */
	Compinfo **elmcomps;	/* Array of pointers to comps IN INVERSE 
				** ORDER of EL */
/* these fields are only used in partitioned hsolve (at present read_flag
**  always ==1 in parallel mode) */
	int     numnodes;	/* Is also flag: if >0 parallel mode */
	int     nodenumber;	/* Relative node number within model (0..) */
	int     masternode;	/* Node number of master node model: zero */
	int	*compnodes;	/* Array of node number for each compartment
				 ** (incomplete list on slave nodes)*/
	int	*startnodes;	/* Array of first comp elmnum on each node */
	int	*ncnode;	/* Array of number of comparts on all nodes */
	long    *numsbytes;	/* Array number of bytes to send to each node */
	long    *numrbytes;	/* Array number of bytes to receive from each */
	long	*sendid;    	/* Array Delta message IDs for each dest node */
	long	*netid;		/* Array of network dependent id */
};


/* Upinder S. Bhalla Caltech May-December 1991
** Erik De Schutter 1991-1995
**
** We would like to acknowlege the advice of Dr. Michael Mascagni
** during the development of this element.
*/

