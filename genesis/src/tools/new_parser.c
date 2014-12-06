static char rcsid[] = "$Id: new_parser.c,v 1.4 2006/01/09 16:28:50 svitak Exp $";

/*
** $Log: new_parser.c,v $
** Revision 1.4  2006/01/09 16:28:50  svitak
** Increases in size of storage for comparment names. These can be very long
** when cvapp converts from Neurolucida format.
**
** Revision 1.3  2005/07/20 20:02:04  svitak
** Added standard header files needed by some architectures.
**
** Revision 1.2  2005/07/01 10:03:10  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:33  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.42  2003/03/28 21:15:28  gen-dbeeman
** Fix by Hugo Cornelis for core dump with difshell STOREINFLUX message.
**
** Revision 1.41  2002/01/11 04:56:39  mhucka
** Added fix from Dave Beeman for the following problem:
** "If you invoke readcell twice, in order to create two different cells,
** Em in all the compartments of the second cell gets set to 0, instead of
** the value of EREST_ACT that is specified in the cell parameter file.
** (This doesn't happen if ELEAK is specifically set in the cell parameter
** file, but it should not have to be set unless you want Em to be different
** from initVm.  In fact, ELEAK didn't exist in earlier versions of
** GENESIS.)" [D. Beeman, 24 Oct. 2001]
**
** Revision 1.40  2001/06/29 21:33:04  mhucka
** Added extra parens inside conditionals and { } groupings to quiet certain
** compiler warnings.
**
** Revision 1.39  2001/05/30 15:56:43  mhucka
** Bug fix from Chuck Charman for uninitialized variables.
**
** Revision 1.38  2001/05/10 16:17:19  mhucka
** Fix by Jose Manuel Ibarz del Olmo to prevent negative shell volumes.
**
** Revision 1.37  2001/04/01 19:28:13  mhucka
** Make the test for "tabcurrent" specific to 10 chars in
** add_channel().
**
** Revision 1.36  2001/01/01 01:23:11  mhucka
** Fix to provide tabcurrent elements created by readcell with the needed
** VOLTAGE and CHANNEL messages.
**
** Revision 1.35  2000/06/12 04:57:12  mhucka
** Needs to include <time.h>, and one of the clock variables needs to be a
** time_t instead of a long.
**
** Revision 1.34  2000/05/26 21:09:26  mhucka
** Added Steve Van Hooser's Nov. 1998 modifications to implement a "double
** endpoint" mode in .p files.  According to Steve: "Simply put
** *double_endpoint near the top of the file where one would normally put
** things like *relative, *absolute, *cartesian, etc., and then you can
** enter both endpoints for all of your compartments.  The syntax is:
**
**    name parent x0 y0 z0 x y z chans dens... for cartesian mode, and
**    name parent r0 t0 p0 r t p chans dens... for polar mode.
**
** The user may toggle between double endpoint and regular mode with
** *double_endpoint_off and *double_endpoint."
**
** Revision 1.33  2000/05/26 19:44:59  mhucka
** Added Steve Van Hooser's Oct. 1998 modifications to *lambda_warn to support
** the new form "*lambda_warn MIN MAX".  According to Steve, "this form will
** turn on lambda_warn except that it will produce a warning in the event that
** the electrotonic length is outside the range MIN..MAX.  This is useful for
** those making models directly from anatomical data files.  In addition, one
** can get the lengths (albeit with warning messages) of every compartment if
** one specifies MIN and MAX as the same (and one might pipe the output to a
** file for examination).
**
** Revision 1.32  2000/05/26 18:59:41  mhucka
** Cleaned up the log comments a tiny bit.
**
** Revision 1.31  1999/10/17 22:03:20  mhucka
** New version from Erik De Schutter, dated circa March 1999.
**
** EDS22h revison: EDS BBF-UIA 99/03/10-99/03/29
** updated scaling of concpool
**
** EDS22e revison: EDS BBF-UIA 98/09/23-98/09/29
** Implemented -lambdasplit command
**
** EDS22c revison: EDS BBF-UIA 97/12/02-98/04/29
** Improved and corrected support for symmetric and spherical compartments
** Corrected bug in x coordinate
** Improved implementation of spines
**
** EDS22b revison: EDS BBF-UIA 97/11/17-97/11/27
** Added hillpump and concpool
** Corrected -hsolve option for use of symmetric compartments
**
** EDS22a revison: EDS BBF-UIA 97/10/19-97/10/20
** Added -split command
**
** Revision 1.28  1997/07/24 00:36:18  dhb
** Fixes from PSC:
**   Typo in add_compartment() tlen ==> Vlen in rangauss() call.
**   In do_read_cell() abs() call replaced with fabs()
**   In do_write_cell() lastelm was uninitialized
**
** Revision 1.27  1997/07/18 20:07:24  dhb
** Merged in 1.25.1.1 changes
**
** Revision 1.26  1997/07/18 03:02:36  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.25.1.1  1997/07/18 16:09:24  dhb
** Changes from PSC: fix for uninitialized variables.
**
** Revision 1.25  1997/05/29 18:42:35  dhb
** Fixed broken RCS Id line
**
** Revision 1.24  1997/05/29 09:12:42  dhb
** Update from Antwerp GENESIS version 21e
**
** EDS21e revison: EDS BBF-UIA 97/01/28-97/05/03
** Added GELEMENT changes and *origin and *distribute_spines option
** Changed *rand_spines for AV_LENGTH=0.
** Handle script variables properly: do a SymtabLook first.
**
** EDS21c revison: EDS BBF-UIA 96/10/16
** Adapted to concen changes
**
** EDS20l4 revison: EDS BBF-UIA 96/05/07
** Added -hsolve command line option: creates hsolve element instead of
**  neutral as root.
**
** EDS20l revison: EDS BBF-UIA 95/12/25
** When the child of a comp is a comp, it now inherits the father's index.
** This helps findsolvefield in hsolve.
**
** EDS20k revison: EDS BBF-UIA 95/09/29-95/10/19
** Updated difshell/difbuffer scaling to fuse small core with next shell.
**
** EDS20j revison: EDS BBF-UIA 95/07/26-95/08/03
** Added code to update surface field in tabchannels and added tabcurrent
** Corrected bug in difshell/difbuffer scaling
**
** EDS20i revison: EDS BBF-UIA 95/06/08
** Modified tree search routine to locate children at any depth
** Modified by Erik De Schutter, UIA-BBF 1/95
**  added -prand and -rand options 
** Modified by Erik De Schutter, UIA-BBF  8-10/94
**  added tab2Dchannel and ghk and new concen objects 
**
** Revision 1.23  1996/08/06  21:05:05  dhb
** Replaced uses of {get|set}_script_float() with
** {Get|Set}ScriptDouble().
**
** Revision 1.22  1996/07/25  19:33:48  dhb
** Changed references to Synchan2_type and Hebbsynchan_type to
** Synchan_type and HebbSynchan_type respectively.
**
** Revision 1.21  1996/07/16  22:10:57  dhb
** Expanded number of channels per line to 30 and the max channel
** type name to 100.
**
** Revision 1.20  1995/09/27  22:18:00  venkat
** Mike's additions for synchan2 and hebbsynchan
**
** Revision 1.19  1995/08/01  17:46:44  dhb
** EDS: this code is subsumed by the calc_shell_vol call
**
** Revision 1.18  1995/07/18  18:19:46  dhb
** Changed all uses of struct Ca_shell_type to use struct Ca_concen_type
** which now does all the Ca_shell calculations.  The Ca_concen uses the
** field name thick rather than the Ca_shell dia.
**
** Revision 1.17  1995/07/17  21:35:47  dhb
** Readcell now prints an error message if the channel name on a
** compartment lines doesn't name a channel in the library or a
** field of the compartment.
**
** Revision 1.16  1995/06/24  00:12:28  dhb
** Change from Upi Bhalla which allows naming of an existing neutral
** which has no children to be used to build a cell under.
**
** Revision 1.15  1995/04/13  00:15:32  dhb
** *set_global and *set_compt_param now allow user to use a script
** variable name for the value to set.  The user can use the curly
** brace syntax or not.
**
** Revision 1.14  1995/04/05  01:30:54  dhb
** Fixed bug in unscale_kids() handling the unscaling of difpools.
** The dens2 value code was erroneously scaling the dens value.
**
** Revision 1.13  1995/03/23  18:21:21  dhb
** Folded in Erik DeSchutter's code for setting/using the len field
** of compartments.  Compartment len == 0 now implied SPHERICAL and
** len > 0 implies CYLINDRICAL.  Use of the Shape extended field is
** no longer supported.  Use of the Len extended field (used in old
** neurokit code) is no longer supported.
**
** Revision 1.12  1995/01/24  21:53:31  dhb
** Added code to do_read_cell() to create the RM, CM, RA and EREST_ACT
** global script variables if not already defined.
**
** Revision 1.11  1994/08/31  22:01:45  dhb
** Allow setting of ELEAK using *set_global too.
**
** Revision 1.10  1994/08/31  20:43:59  dhb
** Added *set_compt_param directive to set readcell internal values for
** compartment membrane parameters without setting global script values.
** A new parameter ELEAK was added (only to *set_compt_param) to specify
** a value for Em which differs from EREST_ACT.  All internal variable
** cntinue to be initialized from script variables.  ELEAK is set to
** EREST_ACT and follows changes to that value unless ELEAK is set via
** *set_compt_param.
**
** Also, changed readcell so that it no longer has the side effect of
** changing to the root element of the cell just read.  The working
** element before calling readcell is preserved.
**
** Revision 1.9  1994/05/31  21:15:17  dhb
** Added synchan and spikegen as readcellable objects.
**
** Revision 1.8  1994/05/27  23:47:22  dhb
** All tests on object names now check against the BaseObject() name (e.g. the
** name of the builtin object on which the object is based) to allow extened
** objects to be used.
**
** Changed the extended object readcell uses for additional messages from
** sendmsg# to addmsg#.  Actually, sendmsg# will still be accepted too.
**
** Revision 1.7  1994/03/18  00:20:19  dhb
** Missed the declaration of filename in the line continuation patches.
** (So now you know I did them by hand :^)
**
** Revision 1.6  1994/03/15  19:41:14  dhb
** Modified By Mike Vanier and Dave Bilitch, Caltech 9/24
**  corrected parser bug in reading lines broken by \<ret>
**
** Revision 1.5  1993/08/12  17:08:31  dhb
** Modified by Erik De Schutter, Caltech  6/92
**  added *ifequal command
** Modified by Erik De Schutter, Caltech  11/92
**  added *fixed_spines option
** Modified by Erik De Schutter, Caltech  7/93
**  corrected AXIAL and RAXIAL msgs,
**  added *m*rand_spines and *mfixed_spines option
**
** Revision 1.4  1993/03/11  23:22:54  dhb
** Changed use of environment variables to use extended fields.
** NOTE: while previously one was able to use any environment variable name
** for sendmsg env vars, you must now use sendmsg# where # must start at one
** and # must increase by one for each sendmsg# variable.
**
** Revision 1.3  1993/02/24  01:32:17  dhb
** 1.4 to 2.0 command argument changes.
**
** Revision 1.2  1992/10/29  18:50:08  dhb
** Replaced explicit msgin looping with MSGLOOP macro.
**
** Revision 1.1  1992/10/29  18:48:27  dhb
** Initial revision
**
*/

/* Upi Bhalla, Caltech 11/90 */
/* Modified by Erik De Schutter, Caltech 11/90, 1/91
**  implemented symmetric compartments and axial diffusion pools */
/* Modified by Upi Bhalla, Caltech 4/91
**  implemented channel counting and in-line prototyping of compartments */
/* Modified by Erik De Schutter, Caltech  6/91
**  implemented dia field */
/* Modified by Erik De Schutter, Caltech  9/91
**  added Ca_shell object and corrected membrane scaling for PASSIVE spines */
/* Modified by Erik De Schutter, Caltech  10/91
**  added rand_spines option */
/* Modified by Erik De Schutter, Caltech  10/91
**  added rand_branch option */
/* Modified by Erik De Schutter, Caltech  11/91
**  added channelC3 object */
/* Modified by Erik De Schutter, Caltech  6/92
**  added *ifequal command */
/* Modified by Erik De Schutter, Caltech  11/92
**  added *fixed_spines option */
/* Modified by Erik De Schutter, Caltech  7/93
**  corrected AXIAL and RAXIAL msgs,
**  added *mrand_spines and *mfixed_spines option */

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "sim_ext.h"
#include "tools.h"
#include "seg_struct.h"
#include "hh_struct.h"
#include "dev_struct.h"
#include "olf_struct.h"
#include "buf_struct.h"
#include "conc_struct.h"
#include "conc_defs.h"
#include "hines_struct.h"
#include "newconn_struct.h"
#include "result.h"
#include "symtab.h"

/* flags bit definitions */
#define NEW_CELL 0x01
#define RELATIVE 0x02
#define POLAR 0x04
#define LAMBDA_WARNING 0x08
#define SYN_DENS 0x10
#define SPHERICAL 0x20
#define SPINES 0x40
#define HSOLVE 0x80
#define SYMMETRIC 0x100
#define RAND_BRANCH 0x200
#define MEMB_FLAG 0x400
#define PROPRANDSIZE 0x800
#define RANDSIZE 0x1000
#define DISTSPINE 0x2000
#define DOUBLE_ENDPOINT 0x4000

#define NAMELEN 1024
#define DATA_MODE 1
#define FLAG_MODE 2
#define COMMENT_MODE 3
#define SCRIPT_MODE 4
#define ERR -1
#define INPUT 0
#define VOLTAGE 0
#define MAX_NCHANS 30	/* MAX_NCHANS > 30 requires mods to read_data() */
#define MAX_LINELEN 80
#define INPUT_LINELEN 300
#define ARRAY_LINELEN 301
#define MTINY 1.0e-12

#define LAMBDA_MIN_DEFAULT 0.01
#define LAMBDA_MAX_DEFAULT 0.20

/* cellname passed by read_cell call */
static char *cellname;
static char lbracket[2],rbracket[2];
/* These fields are set from the cell definition file and changed
**  with *setglobal commands */
static int TAILWEIGHT=1;
static float RM,CM,RA,EREST_ACT,ELEAK,X0,Y0,Z0;
static int ELeakSet;
/* Fields used by *compt option */
static char comptname[NAMELEN];
/* Fields used by *lambda_warn option  - SDV 10/98 */
static float LAMBDA_MIN=LAMBDA_MIN_DEFAULT, LAMBDA_MAX=LAMBDA_MAX_DEFAULT;
/* Fields used by *add_spines, *fixed_spines and *rand_spines options */
static int NUM_SPINES=0;
static float RDENDR_MIN,RDENDR_DIAM,FDENDR_MIN,FDENDR_DIAM,AV_LENGTH,
	SPINE_SURF,SPINE_DENS,SPINE_FREQ;
static int SPINE_NUM;
static float SPINE_SKIP,SKIP_COUNT;
static char spine_proto[NAMELEN],fspine_proto[NAMELEN];
/* Fields used by *memb_factor option */
static float MEMB_FACTOR;
/* Fields used by *rand_branch option */
#define MAX_ORDERS 5
static int NUM_ORDERS,NUM_COMPS[MAX_ORDERS];
static float MAX_DIA,RAND_FREQ;
static float MIN_L[MAX_ORDERS],MAX_L[MAX_ORDERS];
static float MIN_D[MAX_ORDERS],MAX_D[MAX_ORDERS];
static char rand_postfix[10];
static char lastname[NAMELEN];
static int nindex=0;
/* fields used by -rand and -prand options */
static float Vlen,Vdia;
/* fields used by -split command */
static int NSplit,MaxSplit;
static float MLambda;

static char *chomp_leading_spaces();
float	calc_dia(),calc_len();
int		fill_arglist();
void add_spines();
void add_fspine();
void read_script();
void unscale_kids();
void scale_kids();
void traverse_cell();
void makeproto_func();
void set_compt_field();
void unscale_shells();

float calc_surf(len,dia)
	float len,dia;
{
	float surface;

	if (len == 0.0) {		/* SPHERICAL */
	    surface = dia * dia * PI;
	} else {			/* CYLINDRICAL */
	    surface = len * dia * PI;
	}
	return(surface);
}

float calc_vol(len,dia)
	float len,dia;
{
	float	volume; 

	if (len == 0.0)		/* SPHERICAL */
	    volume =  dia * dia * dia * PI / 6.0;
	else 			/* CYLINDRICAL */
	    volume = len * dia * dia * PI / 4.0;
	return(volume);
}

float calc_shell_vol(len,dia,thick)
/* computes volume of shell (thick==0, volume equals compartment volume
**  or submembrane shell (thick>0) */
    float len,dia,thick;
{
    float d,v1,v2,volume;

	if (thick>0.0) {
		d=dia-2*thick;
                if (d<0.0) d=0.0; /* JMI */
	} else {
		d=0.0;
	}
	if (len == 0.0) {       /* SPHERICAL */
	    v1 =  dia * dia * dia;
	    v2 =  d * d * d;
	    volume=(v1-v2)*PI/6.0;
	} else {            /* CYLINDRICAL */
	    v1 = dia * dia;
	    v2 = d * d;
	    volume=len*(v1-v2)*PI/4.0;
	}
	return(volume);
}

Element *add_compartment(flags,name,link,len,dia,surface,volume,x0,y0,z0,x,y,z,split)
	int	flags;
	char	*name;
	char	*link;
	float	len;
	float	dia;
	float	*surface;
	float	*volume;
	float	x0,y0,z0,x,y,z;
	int	split;
{
	int 	i,argc;
	char	*argv[10];
	struct symcompartment_type  *compt,*lcompt;
	char	src[MAX_LINELEN],dest[MAX_LINELEN];
	MsgIn	*msgin;
	Element *elm,*lelm,*nelm;
	float   tsurface,tvolume,tlen,tdia,val,val2,val3;
	int     dochild;
	float   rangauss();
	int     Strindex();

	compt = (struct symcompartment_type *)(GetElement(name));
	if (compt) {
	    fprintf(stderr,"double definition of (sym)compartment '%s'\n",name);
	    return(NULL);
	}
	/* copy the predefined prototype compartment with all its 
	** subelements */
	if (flags & NEW_CELL) {
	    argv[0] = "c_do_copy";
	    argv[1] = comptname;
	    argv[2] = name;
	    do_copy(3,argv);
	}

	compt = (struct symcompartment_type *)(GetElement(name));
	if (!compt) {
	    fprintf(stderr,"could not find (sym)compartment '%s'\n",name);
	    return(NULL);
	}

	if (flags & RANDSIZE) {
	    if ((len>0.0)&&(Vlen>0.0)) len=rangauss(len,Vlen);
	    if (Vdia>0.0) dia=rangauss(dia,Vdia);
	} else if (flags & PROPRANDSIZE) {
	    if ((len>0.0)&&(Vlen>0.0)) len=rangauss(len,Vlen);
	    if (Vdia>0.0) dia=rangauss(dia,Vdia*dia*dia);
	}
	*surface = calc_surf(len,dia);
	*volume = calc_vol(len,dia);

	/* Rescaling all kids of copied element to its dimensions
	**  copied element is assumed to be cylindrical */
	if ((elm=compt->child)) {
	    tlen = compt->len;
	    tdia = compt->dia;
	    if (tdia <= 0.0) {
		    fprintf(stderr,"Compartment '%s' has zero diameter\n",
			    compt->name);
		    return(NULL);
	    }
	    tsurface = calc_surf(tlen,tdia);
	    tvolume = calc_vol(tlen,tdia);
	    /* do any depth of tree */
	    dochild=1;
	    while (elm) {
		/* go first to bottom of tree, as we may need to delete the top 
		** only if we did not move back up last */
		while (dochild) {
		    if (elm->child) {           /* do subtree first */
			elm=elm->child;
		    } else {
			break;
		    }
		}
		if (!dochild) dochild=1;    /* next time do it again */
		/* find nelm first: elm may be deleted by scale_kids */
		if (elm->next) {   /* continue doing this subtree */
		    nelm=elm->next;
		} else {                    /* subtrees done */
		    nelm=elm->parent;       /* move one up */
		    dochild=0;
		    if (nelm==(Element *)compt) {   /* done all subtrees */
			nelm=NULL;
		    }
		}
		/* EDS20l modification to copy index to compartment children */
		if (Strindex(BaseObject(elm)->name,"compartment")>=0) {
		    /* elm is a compartment, as it is not scaled it is assumed
		    ** to have correct Cm, Rm, Ra and correct msgs. */
		    lcompt=(struct symcompartment_type *)(elm);
		    lcompt->index=compt->index;
		    lcompt->initVm = EREST_ACT;
		    lcompt->Em = ELEAK;
		} else {	/* elm is not a compartment */
		    unscale_kids(elm,&val,&val2,&val3,tdia,tlen,tsurface,tvolume,flags);
		    scale_kids(elm,val,val2,val3,dia,len,surface,volume,flags);
		}
		elm=nelm;
	    }

	}

	/* make messages to connect with parent compartment */
	if (flags & NEW_CELL) {
	    if (strcmp(link,"none") != 0) {
		if (split==0) {
		    /* check if splitting has changed name of parent */
		    for (i=MaxSplit; i>1; i--) {
			sprintf(src,"%d%s",i,link);
			lcompt = (struct symcompartment_type *)(GetElement(src));
			if (lcompt) {
			    strcpy(link,src);
			    break;
			}
		    }
		}
		/* for the messages we need to distinguish between 
		**	asymmetric or symmetric compartments */
		if (strcmp(BaseObject(compt)->name,"compartment") == 0) {
		    argv[0] = "c_do_add_msg";
		    argv[1] = link;
		    argv[2] = name;
		    argv[3] = "AXIAL";
		    argv[4] = "previous_state";
		    do_add_msg(5,argv);
	    
		    argv[1] = name;
		    argv[2] = link;
		    argv[3] = "RAXIAL";
		    argv[4] = "Ra";
		    argv[5] = "previous_state";
		    for (i=1; i<=TAILWEIGHT; i+=1) {
			do_add_msg(6,argv);
		    }
		} else if (strcmp(BaseObject(compt)->name,"symcompartment")==0) {
		/* Check shape of parent compartment, if not found will default 
		**  to cylinder */
		    /* get pointer to parent compartment */
		    lcompt=(struct symcompartment_type *)(GetElement(link));
		    if (!lcompt) {
			Error();
			printf(" could not find symcompartment '%s'\n",link);
			return(NULL);
		    }
		    /* setup axial current into head of name_compt */ 
		    argv[0] = "c_do_add_msg";
		    argv[1] = link;
		    argv[2] = name;
		    argv[3] = "CONNECTHEAD";
		    argv[4] = "Ra";
		    argv[5] = "previous_state";
		    do_add_msg(6,argv);

		    /* if another compartment is already the child of 
		    ** parent of link_compt, cross link with it */
		    MSGLOOP(lcompt, msgin) {
			case 1: /* CONNECTTAIL */
			    /* another child compartment exists! */
			    /* crossing axial R to head of name_compt */
			    lelm=msgin->src;
			    sprintf(src,"%s[%d]",lelm->name,lelm->index);
			    argv[1] = src;
			    argv[2] = name;
			    argv[3] = "CONNECTCROSS";
			    do_add_msg(6,argv);
			    /* crossing axial R to head cross_compt */
			    argv[1] = name;
			    argv[2] = src;
			    do_add_msg(6,argv);
			    break;
			default:
			    break;
		    }
		    /* setup axial current out to tail of link_compt */
		    argv[1] = name;
		    argv[2] = link;
		    argv[3] = "CONNECTTAIL";
		    for (i=1; i<=TAILWEIGHT; i+=1) {
			do_add_msg(6,argv);
		    }
		} else {  /* neither compartment or symcompartment */
		    Error();
		    printf("'%s' is not a (sym)compartment!\n",comptname);
		    return(NULL);
		}
		/* This handles the case where we want to send messages
		**  to the compartment proximal to current compartment,
		**  using the -env variables. The less exotic cases are
		**  handled later on.
		** These messages may be present in any subelement
		**  of the prototype and should be of the form:
		**	sendmsg#	"-/...	./...	MSGTYPE	MSGVARS"
		**  or
		**	sendmsg#	"./...	-/...	MSGTYPE	MSGVARS"
		**  where the - symbol refers to the parent element and
		**  the (first) . symbol to the new compartment.  The
		**  rest of the subpath has to be specified completely
		**
		**  Since environment vars have been replaced by extended
		**  fields, the code below has been changed to look for
		**  the specific variable names of the form described above
		**  and the names must begin with sendmsg1 and go in numer-
		**  ical order.  The old code allowed for any variable which
		**  started with sendmsg.  All occurances of the sendmsg
		**  variables in neurokit appear to be consistent with the
		**  new requirements.
		*/
		if ((elm=compt->child)) {
		    for(;elm;elm=elm->next){
			for(i = 1 ; 1; i++) {
			    char	varname[20];
			    char*	varvalue;

			    sprintf(varname, "addmsg%d", i);
			    varvalue = GetExtField(elm, varname);
			    if (varvalue == NULL) {
				sprintf(varname, "sendmsg%d", i);
				varvalue = GetExtField(elm,varname);
			    }

			    if (varvalue == NULL)
				break;

			    /* since the zeroth argv is already filled*/
			    argc=fill_arglist(argv+1,varvalue)+1;
			    if (strncmp(argv[1],"-/",2)==0) {
				/* source is parent sub-element */
				argv[1] +=1;  /* skip the '-' */
				sprintf(src,"%s%s",link,argv[1]);
				argv[2] +=1;  /* skip the '.' */
				sprintf(dest,"%s%s",name,argv[2]);
				argv[1] = src;
				argv[2] = dest;
				lelm=(GetElement(src));
				if (lelm) {
				    /* only if parent element exists */
				    do_add_msg(argc,argv);
				}
			    } else if (strncmp(argv[2],"-/",2)==0) {
				/* destination is parent sub-element */
				argv[1] +=1;  /* skip the '.' */
				sprintf(src,"%s%s",name,argv[1]);
				argv[2] +=1;  /* skip the '-' */
				sprintf(dest,"%s%s",link,argv[2]);
				argv[1] = src;
				argv[2] = dest;
				lelm=(GetElement(dest));
				if (lelm) {
					/* only if parent element exists */
					do_add_msg(argc,argv);
				}
			    }
			}
		    }
		}
	    }
	}

	/* compute membrane surface and RA */
	if (len==0.0) {	/* SPHERICAL */
	    /* Thinking of the 'one-dimensional' cable resistance of a 
	    ** sphere is a bit of a challenge...  As an approximation we
	    ** choose here the Ra of an equivalent cylinder C with the same
	    ** surface and volume as the sphere S: 
	    **  lenC = 3/2 diaS  and  diaC = 2/3 diaS
	    */
	    compt->Ra = 13.50 * RA / (dia * PI);
	}
	else {		/* CYLINDRICAL */
	    compt->Ra = 4.0 * RA * len /(dia * dia * PI);
	}
	compt->dia = dia;	/* needed by spine routines */
	compt->len = len;
	Gabs_position(compt,x0,y0,z0,x,y,z);
	tsurface = *surface;
	if (len > 0.0) { /* CYLINDRICAL */
	    if (flags & SPINES) {
			if (dia <= RDENDR_DIAM) {
				val = len * SPINE_DENS;  /* number of collapsed spines */
				if ((dia > RDENDR_MIN) && (SPINE_FREQ != 0.0)) {
					add_spines(flags,compt,name,&val,len);
				}
			} else {
				val=0;
			}
			if ((dia > FDENDR_MIN) && (dia <= FDENDR_DIAM)) {
				if (SPINE_NUM>0) {
					if (SKIP_COUNT>=SPINE_SKIP) {
						add_fspine(flags,compt,name,&val,0,0);
						SPINE_NUM--;
						SKIP_COUNT-=SPINE_SKIP;
					}
				}
				SKIP_COUNT+=1.0;
			}
			if (val>0) tsurface = tsurface + val * SPINE_SURF;
		}
	}
	if (flags & MEMB_FLAG)
		tsurface *= MEMB_FACTOR;

	compt->Cm = CM * tsurface;
	compt->Rm = RM / tsurface;
	compt->initVm = EREST_ACT;
	compt->Em = ELEAK;
	return((Element *)compt);
}

void add_spines(flags,dendr,name,spinenum,len)
	int     flags;
	struct symcompartment_type  *dendr;
	char	*name;
	float	*spinenum;
	float	len;
{
	float	l,u,v;
	int	i,r;

	if (SPINE_FREQ == 1.0) {
		/* give every comp 1 spine */
		r = 1.0;
	} else if (SPINE_FREQ > 1.0) {
		if (AV_LENGTH > 0.0) {
		/* If the user wants more than 1 spine per comp then the best
		** distibution is at least one per comp. We try to spread
		** additional ones preferentially over longer comps */ 
		    v = len / AV_LENGTH;
		    if (v > 1.0) {
			    l = SPINE_FREQ;
			    u = SPINE_FREQ + v;
		    } else {
			    l = SPINE_FREQ - 1.0 + v;
			    if (l < 1) {
			    /* always at least one spine */
				    l = 1.0;
			    }
			    u = SPINE_FREQ;
		    }
		    r = frandom(l,u);
		} else {
		    r = SPINE_FREQ;
		}
	} else { 	/* SPINE_FREQ < 1.0 */
		/* less than one spine per comp, random distribution independent
		** of length */
		l = SPINE_FREQ / 2;
		u = SPINE_FREQ * 2;
		r = frandom(l,u);
	}
	for (i=1; i<=r; i++) {
		add_fspine(flags,dendr,name,spinenum,i,r);
	}
}

void add_fspine(flags,dendr,name,spinenum,num,total)
	int     flags;
	struct symcompartment_type  *dendr;
	char	*name;
	float	*spinenum;
	int	num,total;
{
	struct  symcompartment_type  *spine;
	Element	*elm;
	int	oldflags;
	char	spinename[20],oldcomptname[NAMELEN];
	float   len,dia,lratio,x = 0.0,y = 0.0,z = 0.0,surf,vol;
	float   rangauss();

	/* store values */
	strcpy(oldcomptname,comptname);
	oldflags=flags;
	if (num>0) {
	    strcpy(comptname,spine_proto);
	} else {
	    strcpy(comptname,fspine_proto);
	}
	flags &= ~SPINES;

	*spinenum = *spinenum - 1;
	strcpy(spinename,"spine[");
	strcat(spinename,itoa(NUM_SPINES));
	strcat(spinename,rbracket);
	spine = (struct symcompartment_type *)(GetElement(comptname));
	if (!spine) {
	    fprintf(stderr,"'%s' not found\n",comptname);
	    return;
	}
	len=spine->len;
	dia=spine->dia;
	if (flags & RANDSIZE) {
	    if ((len>0.0)&&(Vlen>0.0)) len=rangauss(len,Vlen);
	    if (Vdia>0.0) dia=rangauss(dia,Vdia);
	} else if (flags & PROPRANDSIZE) {
	    if ((len>0.0)&&(Vlen>0.0)) len=rangauss(len,Vlen*len*len);
	    if (Vdia>0.0) dia=rangauss(dia,Vdia*dia*dia);
	}
	if (flags & DISTSPINE) {
	/* distribute spines evenly over the surface of the parent cylinder */
	    /*
	    set_spine_position(dendr,spine,num,total);
	    */
	} else {
	    if (len > 0.0) {
		lratio=len/spine->len;
	    } else {
		lratio=1.0;
	    }
	    x = dendr->x + lratio * spine->x;
	    y = dendr->y + lratio * spine->y;
	    z = dendr->z + lratio * spine->z;
	}
	if (!(elm=add_compartment(flags,spinename,name,len,dia,&surf,&vol,dendr->x,dendr->y,dendr->z,x,y,z,0))) return;
	NUM_SPINES++;

	/* restore values */
	strcpy(comptname,oldcomptname);
	flags=oldflags;
}

Element *add_channel(name,parent)
	char	*name;
	char	*parent;
{
	char	*argv[10];
	Element	*elm;
	static char	dest[NAMELEN];
	char	*oname;

	if (!(elm = GetElement(name)))
		return(NULL);
	
	sprintf (dest,"%s/%s[%d]",parent,elm->name,elm->index);
	argv[0] = "c_do_copy";
	argv[1] = name;
	argv[2] = dest;
	do_copy(3,argv);

	if (!(elm = GetElement(dest)))
		return(NULL);

	abs_position(elm,elm->parent->x,elm->parent->y,elm->parent->z);

	oname = BaseObject(elm)->name;

	argv[0] = "c_do_add_msg";
	if (strcmp(oname,"channelC") == 0) {
		argv[1] = dest;
		argv[2] = parent;
		argv[3] = "CHANNEL";
		argv[4] = "Gk";
		argv[5] = "Ek";
		do_add_msg(6,argv);
	} else if (strcmp(oname,"hh_channel") == 0 ||
		strcmp(oname,"tabchannel") == 0 ||
                strcmp(oname,"tab2Dchannel") == 0 ||
		strncmp(oname,"tabcurrent",10) == 0 ||
		strncmp(oname,"channel",7) == 0 ||
		strcmp(oname,"synchan") == 0 ||
		strcmp(oname,"synchan2") == 0 ||
		strcmp(oname,"hebbsynchan") == 0 ||
		strcmp(oname,"ddsyn") == 0 ||
		strcmp(oname,"receptor2") == 0) {
		argv[1] = dest;
		argv[2] = parent;
		argv[3] = "CHANNEL";
		argv[4] = "Gk";
		argv[5] = "Ek";
		do_add_msg(6,argv);

		argv[1] = parent;
		argv[2] = dest;
		argv[3] = "VOLTAGE";
		argv[4] = "Vm";
		do_add_msg(5,argv);
	} else if (strcmp(oname,"vdep_channel") == 0) {
		argv[1] = dest;
		argv[2] = parent;
		argv[3] = "CHANNEL";
		argv[4] = "Gk";
		argv[5] = "Ek";
		do_add_msg(6,argv);

		argv[1] = parent;
		argv[2] = dest;
		argv[3] = "VOLTAGE";
		argv[4] = "Vm";
		do_add_msg(5,argv);
	} else if (strcmp(oname,"graded") == 0 ||
		strcmp(oname,"spikegen") == 0 ||
		strcmp(oname,"spike") == 0) {
		argv[1] = parent;
		argv[2] = dest;
		argv[3] = "INPUT";
		argv[4] = "Vm";
		do_add_msg(5,argv);
	}
	return(elm);
}


void parse_compartment(flags,name,parent,x,y,z,x00,y00,z00,d,nargs,ch,dens)
	int	flags;
	char	*name;
	char	*parent;
	float	x,y,z,x00,y00,z00,d;
	int	nargs;
	char	ch[MAX_NCHANS][NAMELEN];
	float	*dens;
{
	float   nlambda;
	char	*ch_name;
	float	tx,ty,tz;
	int 	i,j,k;
	float	len = 0.0,surf,vol;
	float	val2 = 0.;    /* MUST initialize */
	float	val3 = 0.;    /* MUST initialize */
	Element	*elm, *compt = NULL, *parent_compt;
	Element *chanlist[MAX_NCHANS];
	char	src[MAX_LINELEN];
	char	dest[MAX_LINELEN];
	char	*argv[10];
	int	argc;
	float	x0,y0,z0;
	char	newname[NAMELEN+2],newpname[NAMELEN+2];
	int	split;

	if (strcmp(parent,"none") == 0) {
                if (flags & DOUBLE_ENDPOINT) {
                   x0=x00; y0=y00;z0=z00;
                } else {
               	   x0=X0; y0=Y0; z0=Y0;
                }
	} else {
		parent_compt = GetElement(parent);
		if (!parent_compt)  {
		    fprintf(stderr,"could not find parent compt %s\n",parent);
		    return;
		}
                if (flags & DOUBLE_ENDPOINT) {
                   if (flags & RELATIVE) {
                     x0=x00+parent_compt->x; y0=y00+parent_compt->y;
                     z0=z00+parent_compt->z;
                   } else {
                     x0=x00; y0=y00;z0=z00;
                   }
		} else {
                   x0=parent_compt->x; y0=parent_compt->y; z0=parent_compt->z;
                }
	}
	strcpy(newname,name);
	strcpy(newpname,parent);
	tx = x;
	ty = y;
	tz = z;
	k=NSplit;
	for (i=1; i<=k; i++) {
	/* split compartments if desired */
	    if (k==1) {	/* compute length */
		if (!(flags & SPHERICAL)) {
		    if (flags & RELATIVE) {
                        /* calc len & convert to absolute coords */
			len = sqrt(x * x + y * y + z * z);
		    } else {
                        /* calc len, already in absolute coords */
			tx = x - x0;
			ty = y - y0;
			tz = z - z0;
			len = sqrt(tx * tx + ty * ty + tz * tz);
		    }
		    nlambda = len / sqrt(RM * d * 0.25 / RA);
		    if (nlambda>MLambda) {
			/* Split compartment into equal parts */
		        k=1+(int )(nlambda/MLambda);
			if (k>MaxSplit) MaxSplit=k;
			len=len/(float )k;
			tx = x/(float )k;
			ty = y/(float )k;
			tz = z/(float )k;
		        printf("Splitting %s (%f) into %d\n",name,nlambda,k);
		    }
		    x = x0 + tx;
		    y = y0 + ty;
		    z = z0 + tz;
		} else {
		    len = 0;
		}
		split=0;
	    } else {
		if (i==1) {		/* compute split length */
		    if (!(flags & SPHERICAL)) {
			if (flags & RELATIVE) {
			    len = sqrt(x * x + y * y + z * z)/(float )NSplit;
			} else {
			    tx = x - x0;
			    ty = y - y0;
			    tz = z - z0;
			    len = sqrt(tx * tx + ty * ty + tz * tz)/(float )NSplit;
			}
		        tx = tx/(float )NSplit;
		        ty = ty/(float )NSplit;
		        tz = tz/(float )NSplit;
			x = x0 + tx;
			y = y0 + ty;
			z = z0 + tz;
		    } else {
			len = 0;
			i=NSplit;
		    }
		    split=0;
		} else {			/* compute split x,y,z) */
		    x0 = x;
		    y0 = y;
		    z0 = z;
		    x = x0 + tx;
		    y = y0 + ty;
		    z = z0 + tz;
		    strcpy(newpname,newname);
		    sprintf(newname,"%d%s",i,name);
		    split=1;
		}
	    }
	    if (!(compt = add_compartment(flags,newname,newpname,len,d,&surf,&vol,x0,y0,z0,x,y,z,split)))
		return;
	}
	/*
	We might also want to put in options for random rotating here
	*/

	if (!(flags & SPHERICAL)) {
		if (len == 0.0) {
			printf("ERROR: cylindrical compartment '%s' has zero length!\n",name);
			return;
		}
	}
	if ((flags & LAMBDA_WARNING) && !(flags & SPHERICAL)) {
	        nlambda = len / sqrt(RM * d * 0.25 / RA);
 		if (nlambda <= LAMBDA_MIN)
			printf("WARNING: compartment '%s' is only %f lambdas\n",
				name,nlambda);
 		else if (nlambda >= LAMBDA_MAX)
			printf("WARNING: compartment '%s' too long: %f lambdas\n",
				name,nlambda);
	}

	for (j = 7,k=0 ; j < nargs ; j += 2,k++) {
		sprintf(src,"/library/%s",ch[k]);
		chanlist[k] = NULL;
		if (!(elm = add_channel(src,name))) {
			set_compt_field(compt,ch[k],dens[k],len,d,flags);
			continue;
		}
		chanlist[k] = elm;
		scale_kids(elm,dens[k],val2,val3,d,len,&surf,&vol,flags);
	}
	argv[0] = "c_do_add_msg";
	for (j = 7,k=0 ; j < nargs ; j += 2,k++) {
		if (!(elm = chanlist[k]))
			continue;
		ch_name = ch[k];
		for (i = 1; 1; i++) {
		    char	varname[20];
		    char*	varvalue;

		    sprintf(varname, "addmsg%d", i);
		    varvalue = GetExtField(elm, varname);
		    if (varvalue == NULL) {
			    sprintf(varname, "sendmsg%d", i);
			    varvalue = GetExtField(elm, varname);
		    }

		    if (varvalue == NULL)
			break;

		    /* since the zeroth argv is already filled */
		    argc = fill_arglist(argv + 1,varvalue)+1;
		    sprintf(src,"%s/%s/%s",name,ch_name,argv[1]);
		    sprintf(dest,"%s/%s/%s",name,ch_name,argv[2]);
		    argv[1] = src;
		    argv[2] = dest;
		    do_add_msg(argc,argv);
		}
	}
}

void append_to_cell(name,flags)
	char	*name;
	int		*flags;
{
	char	*argvar[5];
	Element	*elm;

	if ((elm = GetElement(name)) == NULL) {
		*flags |= NEW_CELL;
		argvar[0] = "c_do_create";
		argvar[1] = "neutral";
		argvar[2] = name;
		do_create(3,argvar);
	}
	ChangeWorkingElement(name);
}

void start_cell(name,flags)
	char	*name;
	int		*flags;
{
	char	*argvar[5];
	Element	*elm;

	if ((elm = GetElement(name)) != NULL) {
		if (elm->child || strcmp(BaseObject(elm)->name,"neutral")!=0) {
			printf("Warning : cell '%s' already exists. Deleting previous version\n",name);
			DeleteElement(elm);
		} else {
			ChangeWorkingElement(name);
			*flags |= NEW_CELL;
			return;
		}
	}
	*flags |= NEW_CELL;
	argvar[0] = "c_do_create";
	argvar[2] = name;
	if (*flags & HSOLVE) {
		/* creating the hsolve element */
		argvar[1] = "hsolve";
	} else {
		argvar[1] = "neutral";
	}
	do_create(3,argvar);
	ChangeWorkingElement(name);
}

extern Symtab GlobalSymbols;

void do_read_cell(argc,argv)
	int argc;
	char	**argv;
{
	int	i;
	int	ncompts=0,nchans=0,nshells=0,nothers=0;
	int	len;
	char	rawline[ARRAY_LINELEN];
	char	templine[ARRAY_LINELEN];
	char	*templine_start;
	char	*line;
	FILE	*fp,*fopen();
	char    filename[NAMELEN];
	Element	*elm;
	Element	*orig_working_elm;
	int	flags = SYN_DENS;
	int	parse_mode = DATA_MODE;
	double	GetScriptDouble();
	char	*endit;
	char	command[NAMELEN];
	char	svar[NAMELEN];
	float	svalue,value;
	int     nargs;
	Result  *rp,*SymtabLook();
	int	status;
	Hsolve	*hsolve;

	initopt(argc, argv, "file-name cell-name -hsolve -prand SDlen SDdia -rand SDlen SDdia -split Number -lambdasplit maxl");
	MaxSplit=NSplit=1;
	MLambda=1.0e30;
	while ((status = G_getopt(argc, argv)) == 1) {
	    if (strcmp(G_optopt, "-prand") == 0) {
		Vlen=Atof(optargv[1]);
		Vdia=Atof(optargv[2]);
		Vlen*=Vlen; /* convert SD into V */
		Vdia*=Vdia; /* convert SD into V */
		flags |=PROPRANDSIZE;
	    } else if (strcmp(G_optopt, "-rand") == 0) {
		Vlen=Atof(optargv[1])*1.0e-6;
		Vdia=Atof(optargv[2])*1.0e-6;
		Vlen*=Vlen; /* convert SD into V */
		Vdia*=Vdia; /* convert SD into V */
		flags |=RANDSIZE;
	    } else if (strcmp(G_optopt, "-hsolve") == 0) {
		flags |= HSOLVE;
	    } else if (strcmp(G_optopt, "-split") == 0) {
		MaxSplit=NSplit=atoi(optargv[1]);
		if ((NSplit<1)||(NSplit>99)) {
		    Error();
		    printf(" during readcell: -split Number should be between 1 and 99\n");
		    return;
		}
	    } else if (strcmp(G_optopt, "-lambdasplit") == 0) {
		MLambda=Atof(optargv[1]);
		if ((MLambda<=0.0)||(MLambda>0.5)) {
		    Error();
		    printf(" during readcell: -lambdasplit maxl should be between 0.0 and 0.5\n");
		    return;
		}
	    }
	}

	if (status < 0) {
		printoptusage(argc, argv);
		return;
	}

	if (!(fp = fopen(optargv[1],"r"))) {
		fprintf(stderr,"can't open file '%s'\n",optargv[1]);
		return;
	}

	strcpy(filename, optargv[1]);

	X0=Y0=Z0=0.0;

	/* getting values from script */
	rp=SymtabLook(&GlobalSymbols,"CM"); /* check if script global exists */
	if (!rp) CreateScriptFloat("CM");
	CM = GetScriptDouble("CM");
	rp=SymtabLook(&GlobalSymbols,"RM"); /* check if script global exists */
	if (!rp) CreateScriptFloat("RM");
	RM = GetScriptDouble("RM");
	rp=SymtabLook(&GlobalSymbols,"RA"); /* check if script global exists */
	if (!rp) CreateScriptFloat("RA");
	RA = GetScriptDouble("RA");
	rp=SymtabLook(&GlobalSymbols,"EREST_ACT"); /* check if script global exists */
	if (!rp) CreateScriptFloat("EREST_ACT");
	EREST_ACT = GetScriptDouble("EREST_ACT");
	rp=SymtabLook(&GlobalSymbols,"ELEAK"); /* check if script global exists */
	if (!rp) {
	    /* dbeeman 10/23/01 - don't declare global ELEAK */
	    ELEAK = EREST_ACT;	/* assume it equals EREST_ACT */
	    ELeakSet = 0;
	} else {
	    ELEAK = GetScriptDouble("ELEAK");	/* get real value */
	    ELeakSet = 1;
	}
	strcpy(lbracket,"[");
	strcpy(rbracket,"]");
	strcpy(lastname,"none");


	/* setting the default compartment name */
	strcpy(comptname,"/library/compartment");

	/* setting the default memb_factor */
	MEMB_FACTOR = 1.0;
	SPINE_FREQ = 0.0;
	SPINE_NUM=0;
	
	if (IsSilent() < 2)
		fprintf(stderr,"reading '%s'.... \n",filename);

	cellname = optargv[2];
	orig_working_elm = (Element*) WorkingElement();
	start_cell(cellname,&flags);

	for (i = 1, endit = fgets(rawline,INPUT_LINELEN,fp); endit != NULL ;
	    endit = fgets(rawline,INPUT_LINELEN,fp), i++ ) {
		line = chomp_leading_spaces(rawline);
		len = strlen(line);
		while (line[len-2] == '\\' && endit != NULL) {
			line[len-2] = ' ';
			endit = fgets(templine,INPUT_LINELEN,fp);
			templine_start = chomp_leading_spaces(templine);
			strcpy(line+len-1,templine_start);
			len = strlen(line);
			i++;
		}

		switch (*line) {
			case '\n' :
			case '\r' :
			case '\0' : continue;
			case '/' :
				if (line[1] == '/')
					continue;
				if (line[1] == '*') {
					parse_mode = COMMENT_MODE;
				} else {
					parse_mode = DATA_MODE;
				}
				break;
			case '*' :
				if (line[1] == '/') {
					parse_mode = DATA_MODE;
					continue;
				} else if (line[1] ==  'i') {
					nargs = sscanf(line,"%s %s %f",command,svar,&value);
					if ((nargs==3) && (strcmp(command,"*ifequal") == 0)) {
						svalue=GetScriptDouble(svar);
						if (fabs(svalue-value)<MTINY) {
							parse_mode = COMMENT_MODE;
						} else {
							parse_mode = DATA_MODE;
						}
						continue;
					}
				} else if (line[1] ==  'e') {
					nargs = sscanf(line,"%s",command);
					if ((nargs==1) && (strcmp(command,"*endif") == 0)) {
						parse_mode = DATA_MODE;
						continue;
					}
				}
				parse_mode = SCRIPT_MODE;
				break;
			default :
				break;
		}

		switch(parse_mode) {
			case DATA_MODE :
				if (read_data(line,i,flags) == ERR) {
				}
				break;
			/*
			case FLAG_MODE :
				if (read_flag(line,i,flags) == ERR) {
				}
				break;
			*/
			case SCRIPT_MODE :
				read_script(line,i,&flags);
				parse_mode = DATA_MODE;
				break;
			case COMMENT_MODE :
				len = strlen(line);
				if (line[len-2] == '*' && line[len-1] == '/')
					parse_mode = DATA_MODE;
				break;
			default :
				break;
		}
	}
	elm = (Element *)WorkingElement();
	if (!elm) {
		printf ("No current element for traverse cell\n");
	} else {
		traverse_cell(elm, &ncompts,&nchans,&nshells,&nothers);
	}
	if (flags & HSOLVE) {
	    hsolve = (Hsolve *)(GetElement(cellname));
	    if (!hsolve) {
		Error();
		printf(" could not create hsolve '%s'\n",cellname);
	    } else {
		if (!hsolve->path) hsolve->path=(char *)calloc(30,sizeof(char));
		if (flags & SYMMETRIC) {
		    strcpy(hsolve->path,"./##[][TYPE=symcompartment]");
		    hsolve->symflag=1;
		} else {
		    strcpy(hsolve->path,"./##[][TYPE=compartment]");
		}
	    }
	} 
	if (IsSilent() < 2) {
	    if (flags & SYMMETRIC) {
		printf("%s read: %d symcompartments",filename,ncompts);
	    } else {
		printf("%s read: %d compartments",filename,ncompts);
	    }
	    printf(", %d channels, %d shells, %d others\n",nchans,nshells,nothers);
	    if ((flags & SPINES) && (SPINE_FREQ > 0.0)) {
		fprintf(stderr,"  %d spines attached\n",NUM_SPINES);
	    }
	}
	fclose(fp);
	SetWorkingElement(orig_working_elm);
}


void add_branch(flags,order,up,maxorder,dp,x0,xp,y0,yp,z0,zp,name,index,parent)
/* recursively called, adds maxorder binary branches */
	int		flags,order,maxorder;
	int		*index;
	short	up;
	float   dp,x0,y0,z0,xp,yp,zp;
	char	*name,*parent;
{
	int 	i;
	float   dn,maxd,ln,xn = 0.0,yn = 0.0,zn = 0.0,sn,vn;
	char	new[NAMELEN];
	Element  *compt;

	if (MIN_D[order] < dp) {
		maxd = MAX_D[order];
		for (i=0; i<NUM_COMPS[order]; i++) {
			strcpy(new,name);
			strcat(new,itoa(*index));
			strcat(new,rbracket);
			(*index)++;
			ln=frandom(MIN_L[order],MAX_L[order]);
			dn=frandom(MIN_D[order],maxd);
			maxd = dn; /* next comp of branch cannot be thicker */
			xn = x0 + xp * ln;
			if (up) {
				yn = y0 + yp * ln;
			} else {
				yn = y0 - yp * ln;
			}
			zn = z0 + zp * ln;
			compt=add_compartment(flags,new,parent,ln,dn,&sn,&vn,x0,y0,z0,xn,yn,zn,0);
			if (!compt) {
				fprintf(stderr,"could not create random compt %s\n",new);
				return;
			}
		}
		if (up) {
			/* make down branch also (of pair) */
			add_branch(flags,order,0,maxorder,dp,x0,xp,y0,yp,z0,zp,name,index,parent);
		}	
	} else {
		strcpy(new,parent);
		xn = x0;
		yn = y0;
		zn = z0;
	}
	order++;
	if (order<maxorder) {
		/* make next order of branch */
		add_branch(flags,order,1,maxorder,dp,xn,xp,yn,yp,zn,zp,name,index,new);
	}	
}

int read_data(line,lineno,flags)
	char	*line;
	int		lineno;
	int		flags;
{
	float	x,y,z,d, x0, y0, z0;
	float	r,theta,phi, r0, theta0, phi0;
	static char	ch[MAX_NCHANS][NAMELEN];
	float	dens[MAX_NCHANS];
    float   dp,lp,xp,yp,zp;
	static char	prevname[NAMELEN];
    char    randname[NAMELEN];
    struct symcompartment_type  *tip;
	Element	*ptip,*find_parent_dend();
    char    name[NAMELEN],parent[NAMELEN];
    int     nargs;

    x = y = z = d = x0 = y0 = z0 = 0.0;
    dp = lp = xp = yp = zp = 0.0;

    if (flags & DOUBLE_ENDPOINT) {  /* explicitly define both endpoints */
         nargs = sscanf(line,
        "%s%s%f%f%f%f%f%f%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f",
        name,parent,&x0,&y0,&z0,&x,&y,&z,&d,
        ch[0],&dens[0],ch[1],&dens[1],ch[2],&dens[2],
        ch[3],&dens[3],ch[4],&dens[4],ch[5],&dens[5],
        ch[6],&dens[6],ch[7],&dens[7],ch[8],&dens[8],ch[9],&dens[9],
        ch[10],&dens[10],ch[11],&dens[11],ch[12],&dens[12],
        ch[13],&dens[13],ch[14],&dens[14],ch[15],&dens[15],
        ch[16],&dens[16],ch[17],&dens[17],ch[18],&dens[18],ch[19],&dens[19],
        ch[20],&dens[20],ch[21],&dens[21],ch[22],&dens[22],
        ch[23],&dens[23],ch[24],&dens[24],ch[25],&dens[25],
        ch[26],&dens[26],ch[27],&dens[27],ch[28],&dens[28],ch[29],&dens[29]);

        if (nargs < 9) {
             /* Not enough inputs matched. */
             fprintf(stderr,"error on line %d :\n", lineno);
             fprintf(stderr,"    %s\nFields incorrect\n",line);
             return(ERR);
        }
        nargs -= 3; /* parse_compartment assumes 6 argument standard */
    } else {  /* defines one endpoint to be end of parent */
	nargs = sscanf(line,
        "%s%s%f%f%f%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f%s%f",
        name,parent,&x,&y,&z,&d,
        ch[0],&dens[0],ch[1],&dens[1],ch[2],&dens[2],
        ch[3],&dens[3],ch[4],&dens[4],ch[5],&dens[5],
        ch[6],&dens[6],ch[7],&dens[7],ch[8],&dens[8],ch[9],&dens[9],
        ch[10],&dens[10],ch[11],&dens[11],ch[12],&dens[12],
        ch[13],&dens[13],ch[14],&dens[14],ch[15],&dens[15],
        ch[16],&dens[16],ch[17],&dens[17],ch[18],&dens[18],ch[19],&dens[19],
        ch[20],&dens[20],ch[21],&dens[21],ch[22],&dens[22],
        ch[23],&dens[23],ch[24],&dens[24],ch[25],&dens[25],
        ch[26],&dens[26],ch[27],&dens[27],ch[28],&dens[28],ch[29],&dens[29]);

	if (nargs < 6) {
	    /* Not enough inputs matched. */
	    fprintf(stderr,"error on line %d :\n", lineno);
	    fprintf(stderr,"    %s\nFields incorrect\n",line);
	    return(ERR);
	}
    }

    /* A shortcut for branches : use '.' instead of full name if
    ** the parent is the previous element */
    if (strcmp(parent,".") == 0)
        strcpy(parent,lastname);
/*
** Cartesian coord mode
**  name parent     x       y       z       dia     ch dens...
**
** polar coord mode
**  name parent     r       theta   phi     dia     ch dens...
**
** double_endpoint entry mode...
**  name parent  x0 y0     z0   x y     z   dia ch dens... if cartesian, or
**  name parent  r0 theta0 phi0 r theta phi dia ch dens... if polar
**
*/
    if ((flags & RAND_BRANCH) && (strcmp(parent,lastname) != 0)) {
        /* attached to a different parent, thus the "lastname" is probably
       ** a dendritic tip */
        tip = (struct symcompartment_type *)(GetElement(lastname));
        if (!tip) {
            fprintf(stderr,"could not find parent compt %s\n",lastname);
        } else if ((tip->dia <= MAX_DIA) && (urandom() <= RAND_FREQ)) {
            dp = tip->dia;
			strcpy(randname,tip->name);
			strcat(randname,rand_postfix);
			if (strcmp(prevname,randname) != 0) {
				/* a different name -> start indexing from zero */
				nindex = 0;
			}
			strcpy(prevname,randname);
            /* compute orientation of branches */
			ptip = find_parent_dend(tip);
			if (ptip) {
				xp = tip->x - ptip->x;
				yp = tip->y - ptip->y;
				zp = tip->z - ptip->z;
			} else {
				xp = 0.866;  /* 30 degree angle */
				yp = 0.500;
				zp = 0.0;
			}
            lp = sqrt(xp*xp + yp*yp + zp*zp);
            xp = xp/lp;
            yp = yp/lp;
            zp = zp/lp;
            /* add random branches recursively */
	    add_branch(flags,0,1,NUM_ORDERS,dp,tip->x,xp,tip->y,yp,tip->z,zp,randname,&nindex,lastname);
        }
    }
	if (flags & POLAR) {
		r = x * 1.0e-6;
		theta = y * PI/180.0;
		phi = z * PI/180.0;
		x = r * sin(phi) * cos(theta);
		y = r * sin(phi) * sin(theta);
		z = r * cos(phi);

                if (flags & DOUBLE_ENDPOINT) {
                    r0 = x0 * 1.0e-6;
		    theta0 = y0 * PI/180.0;
		    phi0 = z0 * PI/180.0;
		    x0 = r0 * sin(phi0) * cos(theta0);
		    y0 = r0 * sin(phi0) * sin(theta0);
		    z0 = r0 * cos(phi0);
                }
	} else {
		x *= 1.0e-6;
		y *= 1.0e-6;
		z *= 1.0e-6;
                if (flags & DOUBLE_ENDPOINT) {
		    x0 *= 1.0e-6;
		    y0 *= 1.0e-6;
		    z0 *= 1.0e-6;
                }
	}
	d *= 1.0e-6;
 	parse_compartment(flags,name,parent,x,y,z,x0,y0,z0,d,nargs,ch,dens);
	strcpy(lastname,name);
	return(1);
}

void read_script(line,lineno,flags)
	char	*line;
	int	lineno;
	int	*flags;
{
	double	GetScriptDouble();

	char	command[NAMELEN];
	char	field[NAMELEN];
	char	POSTFIX[NAMELEN];
	char	strvalue[NAMELEN];
	float	value;
	int	fvalue;
	int	i,nargs;
	struct symcompartment_type  *compt;
	char	*oname;

	nargs = sscanf(line,"%s %s %s",command,field,strvalue);
	fvalue = sscanf(strvalue, "%f", &value) == 1;
	if (strcmp(command,"*add_spines") == 0) {
		/* Extra membrane surface will be added to all dendritic
		** compartments with d<=DENDR_DIAM, "collapses" spines
		** DENDR_DIAM in um, SPINE_DENS in 1/um, SPINE_SURF in um^2 */
		nargs = sscanf(line,"%s %f %f %f",command,&RDENDR_DIAM,
						&SPINE_DENS,&SPINE_SURF);
		if (nargs == 4) {
		    *flags |= SPINES;
		    RDENDR_DIAM *= 1.0e-6;
		    SPINE_DENS *= 1.0e6;
		    SPINE_SURF *= 1.0e-12;
		    SPINE_FREQ = -1.0;
		} else {
		    printf("** Warning - *add_spines: wrong number of arguments.\n");
		}
	} else if (strcmp(command,"*rand_spines") == 0) {
	    /* Like above, but a random number of spines will be attached
	    ** as copies of the compartment(s) spine_proto. Number of spines
	    ** added depends on SPINE_FREQ and AV_LENGTH.  On the average
	    ** a compartment with dia<=DENDR_DIAM and len=AV_LENGTH will receive
	    ** SPINE_FREQ spines.  If AV_LENGTH==0 all compartments with
	    ** dia <=DENDR_DIAM will receive exactly SPINE_FREQ spines,
	    ** otherwise if SPINE_FREQ >= 1.0 then all compartments
	    ** with dia <=DENDR_DIAM will receive at least one spine.
	    ** The dendritic surface area is still corrected for the 'missing' 
	    ** spines. */
	    nargs = sscanf(line,"%s %f %f %f %f %f %s",command,&RDENDR_DIAM,
                        &SPINE_DENS,&SPINE_SURF,&AV_LENGTH,&SPINE_FREQ,
						spine_proto);
	    if (nargs == 7) {
		*flags |= SPINES;
		RDENDR_MIN = 0.0;
		RDENDR_DIAM *= 1.0e-6;
		SPINE_DENS *= 1.0e6;
		SPINE_SURF *= 1.0e-12;
		AV_LENGTH *= 1.0e-6;
		compt = (struct symcompartment_type *)(GetElement(spine_proto));
		if (!compt) {
		    fprintf(stderr,"spine prototype '%s' not found\n",spine_proto);
		}
	    } else {
		printf("** Warning - *rand_spines: wrong number of arguments.\n");
	    }
	} else if (strcmp(command,"*mrand_spines") == 0) {
	    /* Like above, but a random number of spines will be attached
	    ** as copies of the compartment(s) spine_proto. Number of spines
	    ** added depends on SPINE_FREQ and AV_LENGTH.  On the average
	    ** a compartment with DENDR_MIN<dia<=DENDR_DIAM and len=AV_LENGTH will 
	    ** receive n=SPINE_FREQ spines.  If SPINE_FREQ >= 1.0 then all comps
	    ** with DENDR_MIN<dia<=DENDR_DIAM will receive at least one spine.
	    ** The dendritic surface area is still corrected for the 'missing' 
	    ** spines on dendrites with dia<=DENDR_DIAM. */
	    nargs = sscanf(line,"%s %f %f %f %f %f %f %s",command,&RDENDR_MIN,
			    &RDENDR_DIAM,&SPINE_DENS,&SPINE_SURF,&AV_LENGTH,
			    &SPINE_FREQ,spine_proto);
	    if (nargs == 8) {
		*flags |= SPINES;
		RDENDR_MIN *= 1.0e-6;
		RDENDR_DIAM *= 1.0e-6;
		SPINE_DENS *= 1.0e6;
		SPINE_SURF *= 1.0e-12;
		AV_LENGTH *= 1.0e-6;
		compt = (struct symcompartment_type *)(GetElement(spine_proto));
		if (!compt) {
		    fprintf(stderr,"spine prototype '%s' not found\n",spine_proto);
		}
	    } else {
		printf("** Warning - *mrand_spines: wrong number of arguments.\n");
	    }
	} else if (strcmp(command,"*fixed_spines") == 0) {
	    /* Like above, but a fixed number of spines will be attached
	    ** as copies of the compartment(s) spine_proto. Number of spines
	    ** added is SPINE_NUM, from this command onward a spine will be
	    ** added every SPINE_SKIP compartments (if dia <=DENDR_DIAM) 
	    ** The dendritic surface area is still corrected for the 'missing' 
	    ** spines. */
	    nargs = sscanf(line,"%s %f %d %f %f %s",command,&FDENDR_DIAM,
                        &SPINE_NUM,&SPINE_SURF,&SPINE_SKIP,fspine_proto);
	    if (nargs == 6) {
		*flags |= SPINES;
		FDENDR_MIN = 0.0;
		FDENDR_DIAM *= 1.0e-6;
		SPINE_SURF *= 1.0e-12;
			    SKIP_COUNT = SPINE_SKIP/2;
		compt = (struct symcompartment_type *)(GetElement(fspine_proto));
		if (!compt) {
		    fprintf(stderr,"spine prototype '%s' not found\n",spine_proto);
		}
	    } else {
		printf("** Warning - *fixed_spines: wrong number of arguments.\n");
	    }
	} else if (strcmp(command,"*mfixed_spines") == 0) {
	    /* Like above, but a fixed number of spines will be attached
	    ** as copies of the compartment(s) spine_proto. Number of spines
	    ** added is SPINE_NUM, from this command onward a spine will be
	    ** added every SPINE_SKIP compartments (if DENDR_MIN< dia <=DENDR_DIAM) 
	    ** The dendritic surface area is still corrected for the 'missing' 
	    ** spines. */
	    nargs = sscanf(line,"%s %f %f %d %f %f %s",command,&FDENDR_MIN,
			&FDENDR_DIAM,&SPINE_NUM,&SPINE_SURF,&SPINE_SKIP,
			fspine_proto);
	    if (nargs == 7) {
		*flags |= SPINES;
		FDENDR_MIN *= 1.0e-6;
		FDENDR_DIAM *= 1.0e-6;
		SPINE_SURF *= 1.0e-12;
			    SKIP_COUNT = SPINE_SKIP/2;
		compt = (struct symcompartment_type *)(GetElement(fspine_proto));
		if (!compt) {
		    fprintf(stderr,"spine prototype '%s' not found\n",spine_proto);
		}
	    } else {
		printf("** Warning - *mfixed_spines: wrong number of arguments.\n");
	    }
	} else if (strcmp(command,"*rand_branches") == 0) {
	    /* Add at the end of dendrites more branches randomly.  The routine
	    ** assumes that the .p file has a logical order, where each branch
	    ** segment is defined in continuing lines and the first segment
	    ** after a branchpoint is defined immediately after the segment
	    ** before the branchpoint (if this is not the case the things
	    ** might still work correctly by the MAX_DIAM control).  The
	    ** branching is binary (thus either 2 or 4 or 8 or .. segments
	    ** are added.  NUM_ORDERS determines the number of branchpoints
	    ** that are added.  For each branchpoint one gives the MIN_L and
	    ** MAX_L (length) and MIN_D and MAX_D (diameter) and NUM_COMPS,
	    ** the number of compartments per segment.  Adds smartly: will
	    ** skip thick segments if the existing dendritic tip is thin and
	    ** will only add to 'tips' with a diameter < MAX_DIAM.
	    ** Frequency of addtion of these branches is controlled by
	    ** RAND_FREQ (0-1): if RAND_FREQ == 1 all tips smaller than MAX_DIAM
	    ** will have branches appended.
	    ** The name will be the name of the parent + POSTFIX, indexed
	    ** from 0 to 2^NUM_ORDERS-1.
	    ** For a "full" addition (to a thick dendritic tip) the number
	    ** of added comps is 2*NUM_COMPS[1] +  ... + 2*NUM_COMPS[NUM_ORDERS]. */
	    nargs = sscanf(line,
		"%s%f%f%d%s%d%f%f%f%f%d%f%f%f%f%d%f%f%f%f%d%f%f%f%f%d%f%f%f%f"
		,command,&MAX_DIA,&RAND_FREQ,&NUM_ORDERS,POSTFIX,
		&NUM_COMPS[0],&MIN_L[0],&MAX_L[0],&MIN_D[0],&MAX_D[0],
		&NUM_COMPS[1],&MIN_L[1],&MAX_L[1],&MIN_D[1],&MAX_D[1],
		&NUM_COMPS[2],&MIN_L[2],&MAX_L[2],&MIN_D[2],&MAX_D[2],
		&NUM_COMPS[3],&MIN_L[3],&MAX_L[3],&MIN_D[3],&MAX_D[3],
		&NUM_COMPS[4],&MIN_L[4],&MAX_L[4],&MIN_D[4],&MAX_D[4]);
	    if ((nargs > 3) && (NUM_ORDERS <= MAX_ORDERS) && (nargs == (NUM_ORDERS*5+5))) {
		    *flags |= RAND_BRANCH;
		    MAX_DIA *= 1.0e-6;
		    strncpy(rand_postfix,POSTFIX,9);
		    strcat(rand_postfix,lbracket);
		    for (i=0; i<NUM_ORDERS; i++) {
			MIN_L[i] *= 1.0e-6;
			MAX_L[i] *= 1.0e-6;
			MIN_D[i] *= 1.0e-6;
			MAX_D[i] *= 1.0e-6;
		    }
	    } else {
		printf("** Warning - *rand_branches: wrong number of arguments.\n");
	    }
	} else if (strcmp(command,"*origin") == 0) {
	    /* Define origin of the cell */
	    nargs = sscanf(line,"%s %f %f %f",command,&X0,&Y0,&Z0);
	    if (nargs != 4) {
		printf("** Warning - *origin: wrong number of arguments.\n");
		X0=0;
		Y0=0;
		Z0=0;
	    }
	} else if (nargs == 3 && strcmp(command,"*lambda_warn") == 0) {
              /* set LAMBDA_MIN to 2nd argument (i.e., 'field'),    */
              /* and LAMBDA_MAX to 3rd argument (i.e., 'strvalue') */
                sscanf(field,"%f", &LAMBDA_MIN);
                sscanf(strvalue,"%f", &LAMBDA_MAX);
                *flags |= LAMBDA_WARNING;
                if ((LAMBDA_MIN<=0) || (LAMBDA_MAX<=0))
                {
                   printf("** Warning - *lambda_warn argument negative or zero - using defaults **\n");
                   LAMBDA_MIN = LAMBDA_MIN_DEFAULT;
                   LAMBDA_MAX = LAMBDA_MAX_DEFAULT;
                } else if (LAMBDA_MIN>LAMBDA_MAX) {
                   printf("** Warning - *lambda_warn min > max - using defaults **\n");
                   LAMBDA_MIN = LAMBDA_MIN_DEFAULT;
                   LAMBDA_MAX = LAMBDA_MAX_DEFAULT;
                } else if ((LAMBDA_MIN>0.20)||(LAMBDA_MAX>1.0)) {
                   printf("** Warning - *lambda_warn min > 0.20 or max > 1.0 **\n");
                }
	} else if (nargs == 3 && strcmp(command,"*set_global") == 0) {
		/* setting a global */
		if (!fvalue) {
		    char*	varname;

		    /* try treating strvalue as a global script variable */
		    varname = strvalue;
		    if (*varname == '{') {
			char*	otherbrace;

			varname++;
			otherbrace = strchr(varname, '}');
			if (otherbrace != NULL)
			    *otherbrace = '\0';
			value = GetScriptDouble(varname);
		    }
		}
		if (strcmp(field,"CONNECTTAIL") == 0) {
		    /* implements Nodus weight function */
		    TAILWEIGHT = value;
		} else {
		    SetScriptDouble(field,value);
		    if (strcmp(field,"RM") == 0)
			RM = value;
		    if (strcmp(field,"RA") == 0)
			RA = value;
		    if (strcmp(field,"CM") == 0)
			CM = value;
		    if (strcmp(field,"EREST_ACT") == 0) {
			if (!ELeakSet) ELEAK = value;
			EREST_ACT = value;
		    }
		    if (strcmp(field,"ELEAK") == 0) {
			ELEAK = value;
			ELeakSet = 1;
		    }
		}
	} else if (nargs == 3 && strcmp(command,"*set_compt_param") == 0) {
		/* setting a compartment parameter */
		if (!fvalue) {
		    /* try treating strvalue as a global script variable */

		    char*	varname;

		    varname = strvalue;
		    if (*varname == '{') {
			/* strip braces */

			char*	otherbrace;

			varname++;
			otherbrace = strchr(varname, '}');
			if (otherbrace != NULL)
			    *otherbrace = '\0';
			value = GetScriptDouble(varname);
		    }
		}
		if (strcmp(field,"CONNECTTAIL") == 0) 
		    /* implements Nodus weight function */
		    TAILWEIGHT = value;
		if (strcmp(field,"RM") == 0)
		    RM = value;
		if (strcmp(field,"RA") == 0)
		    RA = value;
		if (strcmp(field,"CM") == 0)
		    CM = value;
		if (strcmp(field,"EREST_ACT") == 0) {
		    if (!ELeakSet)
			ELEAK = value;
		    EREST_ACT = value;
		}
		if (strcmp(field,"ELEAK") == 0) {
		    ELEAK = value;
		    ELeakSet = 1;
		}
	} else if (nargs == 2 && strcmp(command,"*memb_factor") == 0) {
		*flags |= MEMB_FLAG;
		sscanf(line,"%s %f",command,&MEMB_FACTOR);
		if (MEMB_FACTOR > 10.0 || MEMB_FACTOR < 1.0e-10)
			printf("Warning: memb_factor = '%f'; probably out of range\n",MEMB_FACTOR);
	} else if (strcmp(command,"*start_cell") == 0) {
		/* making a new cell or a prototype set of compts */
		if (nargs == 2)
			start_cell(field,flags);
		else
			start_cell(cellname,flags);
	} else if (strcmp(command,"*append_to_cell") == 0) {
		/* appending onto an existing cell */
		if (nargs == 2)
			append_to_cell(field,flags);
		else
			append_to_cell(cellname,flags);
	} else if (strcmp(command,"*makeproto") == 0) {
		if (nargs != 2) {
			printf("One compartment must be defined to be prototype\n");
		} else {
			makeproto_func(field);
		}
	} else if (strcmp(command,"*hsolve") == 0) {
			printf("Warning: *hsolve option is obsolote.\n");
	} else if (nargs == 1) /* just setting flags */ {
		if (strcmp(command,"*relative") == 0)
			*flags |= RELATIVE;
		else if (strcmp(command,"*absolute") == 0)
			*flags &= ~RELATIVE;
		else if (strcmp(command,"*polar") == 0)
			*flags |= POLAR;
		else if (strcmp(command,"*cartesian") == 0)
			*flags &= ~POLAR;
		else if (strcmp(command,"*lambda_warn") == 0)
			*flags |= LAMBDA_WARNING;
		else if (strcmp(command,"*lambda_unwarn") == 0)
			*flags &= ~LAMBDA_WARNING;
		else if (strcmp(command,"*syn_dens") == 0)
			*flags |= SYN_DENS;
		else if (strcmp(command,"*syn_cond") == 0)
			*flags &= ~SYN_DENS;
		else if (strncmp(command,"*spherical",6) == 0)
			*flags |= SPHERICAL;
		else if (strncmp(command,"*cylindrical",7) == 0)
			*flags &= ~SPHERICAL;
		else if (strncmp(command,"*distribute_spines",17) == 0)
			*flags |= DISTSPINE;
		else if (strcmp(command,"*symmetric") == 0) {
			if (strcmp(comptname,"/library/compartment") == 0)
				strcpy(comptname,"/library/symcompartment");
			*flags |= SYMMETRIC;
		} else if (strcmp(command,"*asymmetric") == 0) {
			if (strcmp(comptname,"/library/symcompartment") == 0)
				strcpy(comptname,"/library/compartment");
			*flags &= ~SYMMETRIC;
		} else if (strcmp(command,"*double_endpoint")==0) {
                        *flags |= DOUBLE_ENDPOINT;
                } else if (strcmp(command,"*double_endpoint_off")==0) {
                        *flags &= ~DOUBLE_ENDPOINT;
		}
	} else if (nargs == 2) { /* setting various other values */
		if (strcmp(command,"*compt") == 0) {
			/* setting the prototype compt */
			strcpy(comptname,field);
			/* set SPHERICAL flag correctly */
			compt = (struct symcompartment_type *)(GetElement(comptname));
			if (!compt) {
			    fprintf(stderr,"could not find prototype compartment '%s'\n",comptname);
			} else {
				oname = BaseObject(compt)->name;
				if (strcmp(oname,"compartment")==0) {
				    *flags &= ~SYMMETRIC;
				} else if (strcmp(oname,"symcompartment")==0) {
				    *flags |= SYMMETRIC;
				} else {
				    fprintf(stderr,"'%s' is not a (sym)compartment\n",comptname);
				}
#if 0
			/* For compatibility with scripts that use the -env Len
			**  field (older neurokit version) */
				char*   lenvalue;

				len = 1.0;    /* default is cylinder */
				lenvalue = GetExtField(compt, "Len");
				if (lenvalue != NULL) {
					len=Atof(lenvalue);
				}
#endif

				if (compt->len == 0.0) {  /* is SPHERICAL */
					*flags |= SPHERICAL;
				} else {
					*flags &= ~SPHERICAL;
				}
			}
		}
	} else {
	    /* Else (probably) no valid script command.  Give some error msg.
            ** This also gives an error msg when you have things like this
	    ** in your cell file :
	    ** 
	    **  *relative // some comments
	    */
	    printf("Error: During readcell: undefined script command '%s'\n",
		   line);
	}

}

static char *chomp_leading_spaces(line)
	char	*line;
{
	char	*str;

	for(str = line; (*str == ' ' || *str == '	') ; str++);

	return(str);
}

/* WARNING: this output routine does not handle the spherical/
**	cylindrical and asymmetric/symmetric options correctly! It
**  cannot handle changing global values either (EDS) */
void do_write_cell(argc,argv)
	int argc;
	char	**argv;
{
	FILE	*fp,*fopen();
	Element	*neuron;
	Element	*elm,*lastelm;
	struct symcompartment_type      *compt;
	Element	*chan;
	Element	*parent,*find_parent_dend();
	ElementList	el;
	Element *elms[MAX_EL_SIZE];
	float	calc_dia();
	float	calc_len();
	float	cond,calc_cond();
	double	GetScriptDouble();
	int	i;
	float	x,y,z,r,theta,phi;
	int	relative_flag = 1, cartesian_flag = 1, syn_dens_flag = 1;
	char	*author;
	time_t	clock;
	int	status;

	el.nelements = 0;
	el.element = elms;
	author = NULL;

	initopt(argc, argv, "file-name cell-name -cartesian -polar -relative -absolute -syndens -syncond -author author");
	while ((status = G_getopt(argc, argv)) == 1)
	  {
	    if (strcmp(G_optopt, "-polar") == 0)
		cartesian_flag = 0;
	    else if (strcmp(G_optopt, "-absolute") == 0)
		relative_flag = 0;
	    else if (strcmp(G_optopt, "-syncond") == 0)
		syn_dens_flag = 0;
	    else if (strcmp(G_optopt, "-author") == 0)
		author = optargv[1];
	    else if (strcmp(G_optopt, "-cartesian") == 0)
		cartesian_flag = 1;
	    else if (strcmp(G_optopt, "-relative") == 0)
		relative_flag = 1;
	    else if (strcmp(G_optopt, "-syndens") == 0)
		syn_dens_flag = 1;
	  }

	if (status < 0) {
		printoptusage(argc, argv);
		printf("\tDefault options are -cartesian -relative -syndens\n");
		return;
	}

	if (!(fp = fopen(optargv[1],"w"))) {
		fprintf(stderr,"can't open file '%s' for writing\n",optargv[1]);
		return;
	}

	if ((neuron = GetElement(optargv[2])) == NULL) {
		fprintf(stderr,"Can't find cell '%s' for writing\n",optargv[2]);
		return;
	}

	/*
	** Finding all dendrites, but not their channels
	*/
	find_all_dends(neuron,0,&el);

	elm = elms[0];
	if (!elm || el.nelements == 0) {
		fprintf(stderr,"Incorrect neuron specification\n");
		return;
	}

	if (IsSilent() < 2)
	    fprintf(stderr,"writing '%s'.... \n",optargv[1]);

	/*
	** Writing out the header info
	*/
	fprintf(fp,"//	PARAMETER FILE FOR NEURON '%s'\n",neuron->name);
	if (author)
		fprintf(fp,"//	Author : %s \n",author);
	clock = time(0);
	fprintf(fp,"//	%s\n",ctime(&clock));
	fprintf(fp,"\n//	Format of file :\n");
	fprintf(fp,"// x,y,z,dia are in microns, all other units are SI (Meter Kilogram Second Amp)\n");
	fprintf(fp,"// In polar mode 'r' is in microns, theta and phi in degrees \n");
	fprintf(fp,"// Control line options start with a '*'\n");
	fprintf(fp,"// The format for each compartment parameter line is :\n");

	fprintf(fp,"//name	parent	r	theta	phi	d	ch	dens ...\n");
	fprintf(fp,"//in polar mode, and in cartesian mode :\n");
	fprintf(fp,"//name	parent	x	y	z	d	ch	dens ...\n\n\n");
	/*
	** Specifying coordinate modes
	*/
	fprintf(fp,"//		Coordinate mode\n");
	if (cartesian_flag)
		fprintf(fp,"*cartesian\n");
	else
		fprintf(fp,"*polar\n");
	if (relative_flag)
		fprintf(fp,"*relative\n");
	else
		fprintf(fp,"*absolute\n");

	/*
	** Writing out the constants
	*/
	fprintf(fp,"\n//		Specifying constants\n");
	CM = GetScriptDouble("CM");
	RM = GetScriptDouble("RM");
	RA = GetScriptDouble("RA");
	fprintf(fp,"*set_global	RM	%g\n",RM);
	fprintf(fp,"*set_global	RA	%g\n",RA);
	fprintf(fp,"*set_global	CM	%g\n\n",CM);

	/*
	** Writing out soma
	*/
	compt=(struct symcompartment_type *)elm;
	fprintf(fp,"%s	none	%g	0	0	%g",elm->name,
		1e6 * compt->len,1e6 * compt->dia);
	for (chan = elm->child ; chan ; chan = chan->next) {
		if ((cond = calc_cond(elm,chan,RM,RA,syn_dens_flag)) > 0) {
			fprintf(fp,"	%s	%g",chan->name,cond);
		}
	}
	fprintf(fp,"\n\n");

	/*
	** loop for writing out each element
	*/
	lastelm = NULL;
	for (i = 1 ; i < el.nelements ; i++) {
	/* Starting up */
		elm = elms[i];
		parent = find_parent_dend(elm);

	/* Printing name of elm */
		if (elm->index == 0)
			fprintf(fp,"%s	",elm->name);
		else
			fprintf(fp,"%s[%d]	",elm->name,elm->index);

	/* Printing parent of elm */
		if (parent == lastelm)
			fprintf(fp,".	");
		else if (parent->index == 0)
			fprintf(fp,"%s	",parent->name);
		else
			fprintf(fp,"%s[%d]	",parent->name,parent->index);
		lastelm = elm;

	/* printing coords of elm */
		if (relative_flag) {
			x = elm->x - parent->x;
			y = elm->y - parent->y;
			z = elm->z - parent->z;
		} else {		/* absolute coordinates */
			x = elm->x;
			y = elm->y;
			z = elm->z;
		}
		x *= 1e6;
		y *= 1e6;
		z *= 1e6;

		if (cartesian_flag) {
			fprintf(fp,"%g	%g	%g	",x,y,z);
		} else { 		/* polar coords : r,theta,phi */
			r = sqrt(x * x + y * y);
			if (x < r) /* To compensate for rounding errors */
				theta = acos(x/r);
			else
				theta = 0;

			if (y < 0)
				theta = 2 * PI - theta;
			theta *= 180.0/PI;
			
			r = sqrt(x * x + y * y + z * z);

			if (z < r) /* To compensate for rounding errors */
				phi = (acos(z/r)) * 180.0 / PI;
			else
				phi = 0;

			fprintf(fp,"%g	%g	%g	",r,theta,phi);
		}

	/* printing diameter of elm */
	compt=(struct symcompartment_type *)elm;
		fprintf(fp,"%g",1e6 * compt->dia);

	/* Loop for printing the channels and their densities */
		for (chan = elm->child ; chan ; chan = chan->next) {
			if ((cond = calc_cond(elm,chan,RM,RA,syn_dens_flag)) > 0) {
				fprintf(fp,"	%s	%g",chan->name,cond);
			}
		}

	/* Finish off the line. Extra line every five, for readability */
		fprintf(fp,"\n");
		if ((i%5) == 0)
		fprintf(fp,"\n");
	}
	fclose(fp);

	if (IsSilent() < 2)
	    fprintf(stderr,"Writing done\n");
}

void unscale_kids(elm,dens,dens2,dens3,dia,length,surface,volume,flags)
	Element	*elm;
	float	*dens,*dens2,*dens3;
	float	dia,length,surface,volume;
	int	flags;
{
	char*	oname;
	struct Ca_concen_type *shell;
	Dshell	*dshell;
	Dbuffer *dbuffer;
	Cpool	*cpool;

	oname = BaseObject(elm)->name;
	if (strcmp(oname,"hh_channel") == 0) {
		*dens = ((struct hh_channel_type *)elm)->Gbar;
		if (*dens > 0.0)
			*dens /= surface;
	} else if ((strcmp(oname,"tabchannel") == 0) ||
	           (strcmp(oname,"tab2Dchannel") == 0)) {
		*dens = ((struct tab_channel_type *)elm)->Gbar;
		if (*dens > 0.0)
			*dens /= surface;
	} else if (strcmp(oname,"tabcurrent") == 0) {
		*dens = ((struct tab_current_type *)elm)->Gbar;
		if (*dens > 0.0)
			*dens /= surface;
	} else if (strcmp(oname,"vdep_channel") == 0) {
		*dens = ((struct vdep_channel_type *)elm)->gbar;
		if (*dens > 0.0)
			*dens /= surface;
	} else if ((strcmp(oname,"channelC2") == 0) ||
			   (strcmp(oname,"channelC3") == 0)) {
		*dens = ((struct channelC2_type *)elm)->gmax;
		if (*dens > 0.0)
			*dens /= surface;
	} else if (strcmp(oname,"synchan") == 0) {
		*dens = ((struct Synchan_type *)elm)->gmax;
		if (*dens > 0.0)
			*dens /= surface;
	} else if (strcmp(oname,"synchan2") == 0) {
		*dens = ((struct Synchan_type *)elm)->gmax;
		if (*dens > 0.0)
			*dens /= surface;
	} else if (strcmp(oname,"hebbsynchan") == 0) {
		*dens = ((struct HebbSynchan_type *)elm)->gmax;
		if (*dens > 0.0)
			*dens /= surface;
	} else if (strcmp(oname,"receptor2") == 0) {
		*dens = ((struct olf_receptor2_type *)elm)->gmax;
		if (*dens > 0.0)
			*dens /= surface;
	} else if (strcmp(oname,"channelC") == 0) {
		*dens = ((struct channelC_type *)elm)->gmax;
		if (*dens > 0.0)
			*dens /= surface;
	} else if (strcmp(oname,"manuelconduct") == 0) {
		*dens = ((struct manuelconduct_type *)elm)->gmax;
		if (*dens > 0.0)
			*dens /= surface;
	} else if (strcmp(oname,"ddsyn") == 0) {
		*dens = ((struct dd_syn_type *)elm)->gmax;
		if (*dens > 0.0)
			*dens /= surface;
	/* Shortcut for specifying spike thresholds */
	} else if (strcmp(oname,"spike") == 0) {
		*dens = ((struct spike_type *)elm)->thresh;
	} else if (strcmp(oname,"spikegen") == 0) {
		*dens = ((struct Spikegen_type *)elm)->thresh;
	} else if (strcmp(oname,"Ca_concen") == 0) {
        	shell = (struct Ca_concen_type *)elm;
        	*dens = shell->B;
		if (*dens > 0.0) *dens *= calc_shell_vol(length,dia,shell->thick);
	} else if (strcmp(oname,"difshell") == 0) {
		dshell = (Dshell *)elm;
       		*dens = dshell->dia;
        	*dens2 = dshell->len;
        	*dens3 = dshell->thick;
		unscale_shells(elm,dshell->shape_mode,dia,length,dens,dens2,dens3,oname);
	} else if (strcmp(oname,"difbuffer") == 0) {
        	dbuffer = (Dbuffer *)elm;
        	*dens = dbuffer->dia;
        	*dens2 = dbuffer->len;
        	*dens3 = dbuffer->thick;
		unscale_shells(elm,dbuffer->shape_mode,dia,length,dens,dens2,dens3,oname);
	} else if (strcmp(oname,"concpool") == 0) {
        	cpool = (Cpool *)elm;
        	*dens = cpool->dia;
        	*dens2 = cpool->len;
        	*dens3 = cpool->thick;
		unscale_shells(elm,cpool->shape_mode,dia,length,dens,dens2,dens3,oname);
	} else if (strcmp(oname,"mmpump") == 0) {
		*dens = ((Mpump *)elm)->vmax;
		if (*dens > 0.0)
			*dens /= surface;
	} else if (strcmp(oname,"hillpump") == 0) {
		/* positive values are handled by scale_shells, negative
		** ones are changed by RESET action */
	}
}

void unscale_shells(elm,shape,dia,length,dens,dens2,dens3,oname)
	Element	*elm;
	short	shape;
	float	dia,length,*dens,*dens2,*dens3;
	char	*oname;
{
	Dshell	*dshell;

	dshell = (Dshell *)elm;
	if (shape==SHELL) {
	    if (*dens3 >= 0.0) { /* scale everything proportional */
		*dens3 /= dia;
		if (*dens > 0.0)
		    *dens /= dia;
		if ((length!=0.0) && (*dens2 > 0.0))
		    *dens2 /= length;
	    } else {            /* scale dia, keep correct thick */
		/* precompute surface and volume to enable correct scaling of concpool and hillpump */
		if (strcmp(oname,"difshell") == 0) {
		    dshell->vol= calc_shell_vol(fabs(*dens2),fabs(*dens),fabs(*dens3));
		    dshell->surf_up=calc_surf(fabs(*dens2),fabs(*dens));
		}
		if (*dens < 0.0) fprintf(stderr,"Warning: dia and thick should not be both negative in '%s'\n",elm->name);
		*dens = -(dia-*dens) / *dens3;   /* 2 times number of shell */
		if (fabs(*dens) > 1000.0) {
		    fprintf(stderr,"Warning: apparently very large number of shells at '%s'.\n",elm->name);
		    fprintf(stderr,"Is prototype compartment dia same as first shell dia?\n");
		}
		if (*dens2 > 0.0)
		    *dens2 /= length;
	    }
	} else if (shape==SLAB) {
	    if (*dens > 0.0)
		*dens /= dia;
	    if ((length!=0.0) && (*dens3 > 0.0))
		*dens3 /= length;
	} else if (shape==TUBE) {
	    if (*dens > 0.0) { 
		/* scale everything proportional for negative dia case */
		*dens /= dia;
		*dens2 /= length;
	    } /* else: done inside scale_shells */
	} else {
	    Error();
	    printf(" unknown shape_mode '%d' in '%s'\n",shape,Pathname(elm));
	}
}

void scale_kids(elm,dens,dens2,dens3,dia,length,surface,volume,flags)
	Element	*elm;
	float	dens,dens2,dens3;
	float	dia,length,*surface,*volume;
	int	flags;
{
	char	*oname;
	struct Ca_concen_type *shell;
	int scale_shells();
	Mpump	*mpump;

	oname = BaseObject(elm)->name;
	if (strcmp(oname,"hh_channel") == 0) {
		if (dens < 0.0)
			((struct hh_channel_type *)elm)->Gbar = -1.0 * dens;
		else
			((struct hh_channel_type *)elm)->Gbar = dens * *surface;
	} else if ((strcmp(oname,"tabchannel") == 0) ||
	           (strcmp(oname,"tab2Dchannel") == 0)) {
		if (dens < 0.0) 
			((struct tab_channel_type *)elm)->Gbar = -1.0 * dens;
		else
			((struct tab_channel_type *)elm)->Gbar = dens * *surface;
		((struct tab_channel_type *)elm)->surface = *surface;
	} else if (strcmp(oname,"tabcurrent") == 0) {
		if (dens < 0.0) 
			((struct tab_current_type *)elm)->Gbar = -1.0 * dens;
		else
			((struct tab_current_type *)elm)->Gbar = dens * *surface;
		((struct tab_current_type *)elm)->surface = *surface;
	} else if (strcmp(oname,"vdep_channel") == 0) {
		if (dens < 0.0)
			((struct vdep_channel_type *)elm)->gbar = -1.0 * dens;
		else
			((struct vdep_channel_type *)elm)->gbar=dens * *surface;
	} else if ((strcmp(oname,"channelC2") == 0) ||
			   (strcmp(oname,"channelC3") == 0)) {
		if (dens < 0.0)
			((struct channelC2_type *)elm)->gmax = -1.0 * dens;
		else
			((struct channelC2_type *)elm)->gmax = dens * *surface;
	} else if (strcmp(oname,"synchan") == 0) {
		if (dens < 0.0)
			((struct Synchan_type *)elm)->gmax = -1.0 * dens;
		else
			((struct Synchan_type *)elm)->gmax = dens * *surface;
	} else if (strcmp(oname,"synchan2") == 0) {
		if (dens < 0.0)
			((struct Synchan_type *)elm)->gmax = -1.0 * dens;
		else
			((struct Synchan_type *)elm)->gmax = dens * *surface;
	} else if (strcmp(oname,"hebbsynchan") == 0) {
		if (dens < 0.0)
			((struct HebbSynchan_type *)elm)->gmax = -1.0 * dens;
		else
			((struct HebbSynchan_type *)elm)->gmax = dens * *surface;
	} else if (strcmp(oname,"manuelconduct") == 0) {
		if (dens < 0.0)
			((struct manuelconduct_type *)elm)->gmax = -1.0 * dens;
		else
			((struct manuelconduct_type *)elm)->gmax = dens * *surface;
	} else if (strcmp(oname,"receptor2") == 0) {
		if (dens < 0.0)
			((struct olf_receptor2_type *)elm)->gmax = -1.0 * dens;
		else
			((struct olf_receptor2_type *)elm)->gmax=dens * *surface;
	} else if (strcmp(oname,"channelC") == 0) {
		if (dens < 0.0)
			((struct channelC_type *)elm)->gmax = -1.0 * dens;
		else
			((struct channelC_type *)elm)->gmax = dens * *surface; 
	} else if (strcmp(oname,"ddsyn") == 0) {
		if (dens < 0.0)
			((struct dd_syn_type *)elm)->gmax = -1.0 * dens;
		else
			((struct dd_syn_type *)elm)->gmax = dens * *surface;
	/* Shortcut for specifying spike thresholds */
	} else if (strcmp(oname,"spike") == 0) {
		((struct spike_type *)elm)->thresh = dens;
	} else if (strcmp(oname,"spikegen") == 0) {
		((struct Spikegen_type *)elm)->thresh = dens;
	} else if (strcmp(oname,"Ca_concen") == 0) {
        	shell = (struct Ca_concen_type *)elm;
        	if (dens < 0.0)
         		shell->B = -1.0 * dens;
        	else
       			shell->B = dens / calc_shell_vol(length,dia,shell->thick);
	} else if ((strcmp(oname,"difshell") == 0) ||
			   (strcmp(oname,"difbuffer") == 0) ||
			   (strcmp(oname,"concpool") == 0)) {
		if (scale_shells(elm,dens,dia,length,dens,dens2,dens3)) {
		    /* delete dshell and all its children */
		    /* For some reason  DeleteElement does not delete a
		    ** concpool properly and crashes upon deleting its
		    ** parent difshell next, therefore this hack: */
		    /*
		    pname = BaseObject(elm->parent)->name;
		    if (strcmp(pname,"difshell") != 0) DeleteElement(elm);
		    */
		    DeleteElement(elm);
		}
	} else if (strcmp(oname,"mmpump") == 0) {
		mpump = (Mpump *)elm;
		if (dens < 0.0)
			mpump->vmax = -1.0 * dens;
		else
			mpump->vmax = dens * *surface;
	} else if (strcmp(oname,"hillpump") == 0) {
		/* positive values are handled by scale_shells, negative
		** ones are changed by RESET action */
	}
}

int scale_shells(elm,number,dia,length,shdia,shlen,shthick)
/* boolean: if true elm does not exist and needs to be deleted, otherwise
** sets dia,len and thick of a difshell, difbuffer or concpool */
	Element	*elm;
	float	number,dia,length,shdia,shlen,shthick;
{
	char*	oname;
	Dshell	*dshell = NULL;
	Dbuffer *dbuffer = NULL;
	Cpool	*cpool = NULL;
	Dshell	*ssrc;
	Hpump	*pump;
	MsgIn   *msgin,*smsgin;
	MsgOut  *msgout;
	short	shape = 0;
	double	dvol,cvol,csurf0,csurf;

	oname = BaseObject(elm)->name;
	if (strcmp(oname,"difshell") == 0) {
	    dshell = (Dshell *)elm;
	    shape=dshell->shape_mode;
	} else if (strcmp(oname,"difbuffer") == 0) {
	    dbuffer = (Dbuffer *)elm;
	    shape=dbuffer->shape_mode;
	} else if (strcmp(oname,"concpool") == 0) {
	    cpool = (Cpool *)elm;
	    shape=cpool->shape_mode;
	}
	if (shape==SHELL) {
	    if (shlen < 0.0) {
		shlen = -1.0 * shlen;
	    } else if (length!=0.0) {
		shlen = shlen * length;
	    }
	    if (shthick >= 0.0) {  /* scale everything proportional */
		if (shdia < 0.0) {
		    shdia = -1.0 * shdia;
		} else {
		    shdia = shdia * dia;
		}
		shthick = shthick * dia;
	    } else {            /* scale dia, keep correct thick */
		/* number is 2 * number of shell, 0 being submembrane */
		shthick=-1.0 * shthick;
		shdia = dia - (shthick*number);
		if (shdia <= 0.0) {  
		    /* does not exist physically */
		    return(1);
		} else if ((number>0)&&(shdia <= shthick/2.0)) {  
		    /* assuming that shell above is same shthick, this
		    **  would result in a shell that has 25% or smaller
		    **  thickness than the one above, which will cause
		    **  convergence problems -> fuse it with preceding one */
		    MSGLOOP(elm, msgin) {	/* find preceding elm */
			case DIFF_DOWN: /* found it */
			    ssrc=(Dshell *)msgin->src;
			    /* make it a core shell: note that we are
			    ** not guaranteed that this shell has been
			    ** handled by scale_kids yet, we need to
			    ** check for this! */
			    shdia = dia - (shthick*(number-2));
			    if (ssrc->thick<0) {	/* not scaled yet */
				ssrc->thick=-shdia/2.0;
			    } else {	/* dia has been scaled */
				ssrc->thick=ssrc->dia/2.0;
			    }
			    MSGLOOP(ssrc, smsgin) {
			    /* find any diffusable buffer */
				case BUFFER: 
				case STOREINFLUX:
				case STOREOUTFLUX:
				    oname = BaseObject(smsgin->src)->name;
				    if (strcmp(oname,"difbuffer") == 0) {
					/* fuse diffusable buffer also,
					** the innermost one may be deleted
					** together with innermost shell */
					dbuffer=(Dbuffer *)smsgin->src;
					if (dbuffer->shape_mode==SHELL) {
					    if (dbuffer->thick<0) {
						dbuffer->thick=-shdia/2.0;
					    } else {  /* dia has been scaled */
						dbuffer->thick=dbuffer->dia/2.0;
					    }
					}
				    } else if (strcmp(oname,"concpool") == 0) {
					cpool=(Cpool *)smsgin->src;
					if (cpool->shape_mode==SHELL) {
					    if (cpool->thick<0) {
						cpool->thick=-shdia/2.0;
					    } else {	
						cpool->thick=cpool->dia/2.0;
					    }
					}
				    }
				    break;
			    }
			    break;
		    }
		    return(1); /* delete elm and all its children */
		} else {
		    if (shthick+shthick>shdia) shthick=shdia/2.0; /* make it a core shell */
		}
	    }
	} else if (shape==SLAB) {
	    if (shdia < 0.0) {
		shdia = -1.0 * shdia;
	    } else {
		shdia = shdia * dia;
	    }
	    if (shthick < 0.0) {
		shthick = -1.0 * shthick;
	    } else if (length!=0.0) {
		shthick = shthick * length;
	    }
	} else if (shape==TUBE) {	/* thick field is dia of tube */
	    if (shdia > 0.0) { /* scale everything proportional */
		shdia = shdia * dia;
		shlen = shlen * length;
	    } 	/* else: recompute length so as to conserve relative volume with shell is done in call from difshell */
	}
	if (strcmp(oname,"difshell") == 0) {
		dshell->dia = shdia;
		dshell->len = shlen;
		dshell->thick = shthick;
		/* Rescale any attached concpools and hillpumps */
		cpool=NULL;
		pump=NULL;
		MSGLOOP(elm, msgin) {	/* find preceding elm */
		    case HILLPUMP: /* found it */
			pump=(Hpump *)msgin->src;
			/* find attached concpool */
			MSGOUTLOOP(pump, msgout) {
			    oname = BaseObject((Element *)msgout->dst)->name;
			    if (strcmp(oname,"concpool")==0) cpool=(Cpool *)msgout->dst;
			}
			break;

		    case STOREINFLUX:
		    case STOREOUTFLUX:
			oname = BaseObject(msgin->src)->name;
			if (strcmp(oname,"concpool") == 0) 
			    cpool=(Cpool *)msgin->src;
			break;
		    
		    default:
			break;
		}
		/* scale any attached concpool or hillpump */
		if (cpool && (cpool->shape_mode==TUBE) && (cpool->dia<0.0)) {
		    cpool->dia = -1.0 * cpool->dia;
		    /* new difshell volume */
		    dvol= calc_shell_vol(shlen,shdia,shthick);
		    /* unscaled concpool volume and surface */
		    cvol=calc_vol(cpool->len,cpool->dia);
		    csurf0=calc_surf(cpool->len,cpool->dia);
		    /* properly scaled concpool relative volume */
		    cpool->vol = cvol*(dvol/dshell->vol);
		    /* keep dia, but rescale length for correct volume */
		    cpool->len = cpool->vol/(cpool->dia*cpool->dia/4.0*PI);
		    if (pump && (pump->vmax>0)) {
			csurf=calc_surf(cpool->len,cpool->dia);
			pump->vmax = pump->vmax * csurf/csurf0;
		    }
		} else if (pump) {
		    /* perform standard surface scaling proportional to difshell */
		    csurf=calc_surf(shlen,shdia);
		    pump->vmax = pump->vmax * csurf/dshell->surf_up;
		}
	} else if (strcmp(oname,"difbuffer") == 0) {
		dbuffer->dia = shdia;
		dbuffer->len = shlen;
		dbuffer->thick = shthick;
	} else if ((strcmp(oname,"concpool") == 0) && (cpool->vol==0.0)) {
		/* not scaled from inside difshell */
		cpool->dia = shdia;
		cpool->len = shlen;
		cpool->thick = shthick;
	}
	return(0);
}

int fill_arglist(argv,line)
/* parses a message of the form "src dst type var1 var2 ..."
** into separate arguments */
	char	**argv;
	char	*line;
{
	int nargs = 0;
	static char	str[INPUT_LINELEN];
	char	*s;
	int		new=1;

	strcpy(str,line);

	for (s=str;*s;s++) {
		if (*s == ' ' || *s == '\t') {
			*s = '\0';
			new = 1;
		} else if (new) {
			argv[nargs] = s;
			nargs++;
			new = 0;
		}
	}
	return(nargs);
}

void do_paste_channel(argc,argv)
	int argc;
	char	**argv;
{
	Element *elm;
	Element *add_channel();
	int		i;
	char	*argtab[20];
	char	*name,*ch_name;
	char	*oname;
	char	src[MAX_LINELEN],dest[MAX_LINELEN];
	
	initopt(argc, argv, "source-channel dest-compartment");
	if (G_getopt(argc, argv) != 0)
	  {
		printoptusage(argc, argv);
		return;
	  }

	ch_name = optargv[1];
	name = optargv[2];

	if ((elm = GetElement(ch_name)) == NULL) {
		printf("Could not find channel '%s'\n",ch_name);
		return;
	}

	if ((elm = GetElement(name)) == NULL) {
		printf("Could not find compartment '%s'\n",name);
		return;
	}

	oname = BaseObject(elm)->name;
	if ((strcmp(oname,"compartment") != 0) &&
		(strcmp(oname,"symcompartment") != 0)) {
		printf("Element '%s' is not a (sym)compartment.\n",name);
		return;
	}

	elm = add_channel(ch_name,name);

	if (!elm) {
		/*
		** is this supposed to name the channel or the compartment?
		** is naming the compartment which seem wrong
		*/
		printf("Could not add channel '%s'\n",name);
		return;
	}


	/* Filling in the setenv messages */
	argtab[0] = "c_do_add_msg";
	for(i = 1; 1; i++) {
		char	varname[20];
		char	*varvalue;

		sprintf(varname, "addmsg%d", i);
		varvalue = GetExtField(elm, varname);
		if (varvalue == NULL) {
			sprintf(varname, "sendmsg%d", i);
			varvalue = GetExtField(elm, varname);
		}

		if (varvalue == NULL)
			break;

		/* since the zeroth argtab is already filled */
		argc = fill_arglist(argtab + 1,varvalue)+1;
		sprintf(src,"%s/%s/%s",name,ch_name,argtab[1]);
		sprintf(dest,"%s/%s/%s",name,ch_name,argtab[2]);
		argtab[1] = src;
		argtab[2] = dest;
		do_add_msg(argc,argtab);
	}
}

void traverse_cell(elm,ncompts,nchans,nshells,nothers)
	Element	*elm;
	int	*ncompts,*nchans,*nshells,*nothers;
{

	for (;elm ; elm = elm->next) {
		char*	oname;

		oname = BaseObject(elm)->name;
		if (strcmp(oname,"compartment") == 0 ||
			strcmp(oname,"symcompartment") == 0)
			*ncompts += 1;
		else if (strcmp(oname,"hh_channel") == 0 ||
			strcmp(oname,"vdep_channel") == 0 || 
			strcmp(oname,"tabchannel") == 0 || 
			strcmp(oname,"tab2Dchannel") == 0 || 
			strncmp(oname,"channel",7) == 0 ||
			strcmp(oname,"synchan") == 0 ||
			strcmp(oname,"synchan2") == 0 ||
			strcmp(oname,"hebbsynchan") == 0 ||
			strcmp(oname,"ddsyn") == 0 ||
			strcmp(oname,"receptor2") == 0) 
			*nchans += 1;
		else if (strcmp(oname,"difshell") == 0)
			*nshells += 1;
		else
			*nothers += 1;

		if (elm->child)
			traverse_cell(elm->child,ncompts,nchans,nshells,nothers);
	}
}


/*
** Converting a new cell to the prototype configuration. This is
** done by changing the 'flat' element heirarchy to one where the
** parent is the specified compartment.
** We need to do some tricky element juggling here. First,
** remove the proto from the childlist of elm. Then,
** add the childlist of elm to the childlist of proto. Then,
** replace elm by proto
*/
void makeproto_func(field)
	char	*field;
{
	Element	*elm,*child,*proto;

	elm = (Element *)GetElement(field);
	proto = NULL;
	if (!elm) {
		printf("could not find cell %s\n",field);
		return;
	}

	/* Removing proto from elm's childlist */
	if (strcmp(elm->child->name,elm->name) == 0) {
		proto=elm->child;
		elm->child=proto->next;
	} else {
		for (child=elm->child;child;child=child->next){
			if (child->next) {
				if(strcmp(elm->name,child->next->name)==0) {
					proto=child->next;
					child->next = proto->next;
				}
			}
		}
	}
	if (!proto) {
		printf("could not find compartment %s\n",field);
		return;
	}

	/* Add childlist of elm to childlist of proto */
	if (!(proto->child)) {
		proto->child = elm->child;
	} else {
		for (child=proto->child;child;child=child->next) {
			if (!(child->next)) {
				child->next = elm->child;
				break;
			}
		}
	}

	/* Replace elm by proto */
	if (elm == elm->parent->child) {
		elm->parent->child = proto;
		proto->next = elm->next;
		proto->parent = elm->parent;
	} else {
		for(child=elm->parent->child;child;child=child->next) {
			if (elm == child->next) {
				child->next = proto;
				proto->next = elm->next;
				proto->parent = elm->parent;
				break;
			}
		}
	}
	elm->next = NULL;
	elm->parent = NULL;
	elm->child = NULL;
	sfree(elm);
}

void set_compt_field(compt,field,value,len,dia,flags)
	struct symcompartment_type	*compt;
	char	*field;
	float	value,len,dia;
	int		flags;
{
	float calc_surf();

	if (strcmp(field,"RA") == 0){
		if (compt->len==0.0) {
	    	compt->Ra = 8.0 * RA / (dia * PI);
		}
		else {		/* CYLINDRICAL */
	    	compt->Ra = 4.0 * RA * len /(dia * dia * PI);
		}
	} else if (strcmp(field,"RM") == 0){
		compt->Rm = RM / calc_surf(len,dia);
	} else if (strcmp(field,"CM") == 0){
		compt->Cm = CM * calc_surf(len,dia);
	} else {
		if (!SetElement(compt,field,ftoa(value)))
		    printf("readcell: unable to set field '%s' on compartment '%s'\n",
			    field, Pathname(compt));
	}
}

#undef INPUT
#undef VOLTAGE
