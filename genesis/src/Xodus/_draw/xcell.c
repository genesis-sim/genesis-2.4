/* $Id: xcell.c,v 1.4 2005/07/29 16:17:28 svitak Exp $ */
/*
 * $Log: xcell.c,v $
 * Revision 1.4  2005/07/29 16:17:28  svitak
 * Various changes to address MIPSpro compiler warnings.
 *
 * Revision 1.3  2005/07/20 20:01:57  svitak
 * Added standard header files needed by some architectures.
 *
 * Revision 1.2  2005/07/01 10:02:58  svitak
 * Misc fixes to address compiler warnings, esp. providing explicit types
 * for all functions and cleaning up unused variables.
 *
 * Revision 1.1  2005/06/17 21:23:51  svitak
 * This file was relocated from a directory with the same name minus the
 * leading underscore. This was done to allow comiling on case-insensitive
 * file systems. Makefile was changed to reflect the relocations.
 *
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.21  2000/06/12 04:15:04  mhucka
 * 1) Removed nested comments in RCS/CVS log lines, to quiet down the
 *    IRIX cc compiler.
 * 2) Added NOTREACHED comments where appropriate.
 *
 * Revision 1.20  2000/05/02 06:18:31  mhucka
 * Added type casts for certain function call arguments.
 *
 * Revision 1.19  1996/07/09 19:05:50  venkat
 * Merged the 1.17.1.1 merge with the 1.18 fix for reallocated and unitialized array fies
 *
 * Revision 1.17.1.1  1996/07/09  18:48:32  venkat
 * This is overlaps fixed merge of the XoXtVaSetValues() fix and Upi's Symcompartment
 * fix which were both inadvertantly based on 1.15
 *
 * Revision 1.18  1996/07/02  17:53:32  venkat
 * xoExpandCell modified to initialize additionally allocated interpols and
 * integer list arrays with a call to xomemzero(). The allocated_interps field
 * for the xcell is set to the appropriate value to track the interpol allocations
 *
 * Revision 1.17  1995/09/27  00:09:17  venkat
 * XtVaSetValues()-calls-changed-to-XoXtVaSetValues()-to-avoid-alpha-FPE's
 *
 * Revision 1.16  1995/09/26  22:11:20  dhb
 * Fix from Upi Bhalla to correctly display cells with symcompartments.
 *
 *
 * Upi Bhalla Aug 27 Fixed symcompartment-cell display bug
 *
 * Revision 1.15  1995/08/05  02:33:28  venkat
 * xoSetCellPath() changed to set alloc the temp and ftemp int * to
 * be allocated a size equal to the number of segments/elements in the path
 * of the cell and not the number of connections
 *
 * Revision 1.14  1995/07/08  03:24:00  dhb
 * Backed out preceeding change using DIRTY flag to control field/path/fieldpath
 * inconsistencies.
 *
 * Bad combinations of field/path/fieldpath are now silently ignored.
 *
 * Now updates maxdia/mindia on all autodia modes.
 *
 * Fix from Upi to special case handling of single compartment cells.
 *
 * Fix to get_extrema(): was not checking first value in table.
 *
 * Revision 1.13  1995/06/23  22:05:00  dhb
 * Merged in 1.12.1.2.1.1 and 1.12.1.3 changes.
 *
 * Revision 1.12.1.3  1995/06/21  23:32:05  venkat
 * The message which warns the user to set the path field
 * as absolute path names now appears only if the path has been set as a relativpath name
 *
 * Revision 1.12.1.2.1.1  1995/06/23  21:39:21  dhb
 * Messages for color value inputs are now set up at RESET rather
 * than at SET time.  Doing the latter causes problems in passing
 * through path/fieldpath/field value sets which are not consistent
 * and results in errors setting up the messages.  The messages are
 * recalculated only if one of the three fields have been changed
 * before the RESET.
 *
 * Moved code in the SET action which handles setting of the autodia
 * and autocol fields before autodia code which recalculates diameters
 * as the later code returns and doesn't allow the former code to run.
 *
 * Revision 1.12.1.2  1995/06/16  05:14:52  dhb
 * Merged in 1.12.2.1 changes.
 *
 * Revision 1.12.1.1  1995/06/15  20:24:34  venkat
 * WildcardGetElement() always returns a valid pointer to the head of a locally
 * created element list. So checking this for null is not reasonable but
 * checkig whether the no of elements found in the path is. Also the warning message
 * now makes a note of using absolute pathnames for setting the path field.
 *
 * Revision 1.12.2.1  1995/06/16  05:10:53  dhb
 * Fixes from Upi Bhalla.  See his notes below.
 *
 * Upi Bhalla 1995/06/14 
 * Fixed bug leading to missing dendrites in 2-compartment cells.
 *
 * Revision 1.12  1995/03/25  00:10:29  dhb
 * - Removed fat field compatibility code
 * - Changed use of strdup() to CopyString()
 *
 * Revision 1.11  1995/03/17  20:52:13  venkat
 * Upi inclusions
 *
 * Revision 1.8  1995/02/20  22:26:12  bhalla
 * 1. Added a new field, cellmode, for switching between old and new
 * ways of displaying cell
 * 2. Changed the 'value' field updating method to be the same as for
 * the view and tree widgets.
 * 3. Put in code for the old cellmode to sort out the orientation of
 * the compartments in the from-to lists.
 *
 * Revision 1.7  1995/01/23  03:25:25  bhalla
 * 1. Correctly defined the create_interpol function with the argument
 * types.
 * 2. Fixed bug in routine to assign tolist and fromlist.
 *
 * Revision 1.6  1994/11/07  22:16:04  bhalla
 * Got rid of an erroneous check on compartment termination conditions.
 *
 * Revision 1.5  1994/05/25  13:32:40  bhalla
 * Added SetTable for handling SET action on interpols
 *
 * Revision 1.4  1994/03/22  15:10:56  bhalla
 * RCS error in previous check in. No code changes
 *
 * Revision 1.3  1994/02/08  17:47:26  bhalla
 * Added VERSION2 code for handling msgs
 *
 * Revision 1.3  1994/02/08  17:47:26  bhalla
 * Added VERSION2 code for handling msgs
 *
 * Revision 1.2  1994/02/02  20:04:53  bhalla
 * Eliminated soft actions, put in hooks for version 2 but not yet
 * implemented them
 * */
#include <math.h>	/* fabs() */
#include <string.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include "Widg/Framed.h"
#include "Draw/Pix.h"
#include "Draw/Cell.h"
#include "draw_ext.h"
#include "_widg/xform_struct.h"
#include "hash.h"
#include "../Xo/XoDefs.h"
#include "../Xo/Xo_ext.h"	/* XoColorscale() */

#ifndef MAX_NTARGS
#define MAX_NTARGS 20
#endif

/* autoscaling modes */
#define XO_CELL_SCALE_ON_SET 1
#define XO_CELL_EXPAND 2
#define XO_CELL_CONTINUOUS 3


static Gen2Xo GXconvert[] = {
	{"tx",		XtNpixX},
	{"ty",		XtNpixY},
	{"tz",		XtNpixZ},
	{"soma",		XtNsoma},
	{"colmin",		XtNcolmin},
	{"colmax",		XtNcolmax},
	{"diamin",		XtNdiamin},
	{"diamax",		XtNdiamax},
	{"diarange",	XtNdiarange},
	{"fatmin",		XtNdiamin},
	{"fatmax",		XtNdiamax},
	{"fatrange",	XtNdiarange},
	{"cellmode",	XtNcellmode},
};

/* returns 1 if OK, otherwise 0 */
static int xoSetCellPath(); 

static void xoSetCellField();
static void xoExpandCell();
static int get_extrema();

int XCell (cell, action)
     register struct xcell_type *cell;
     Action                     *action;
{
	int ac=action->argc;
	char** av=action->argv;
    Widget parentW,xoFindParentDraw();
	CellObject cw;
	int i;
	char    *targs[MAX_NTARGS];
	Interpol	*create_interpol(), *ip;
	MsgIn	*msg;

  
  if (Debug(0) > 1)
    ActionHeader("XCell", cell, action);

  SELECT_ACTION (action) {
  case INIT:
    break;
  case PROCESS:
	/* CELL, XYCELL msgs */
	if (cell->npts < 1) break;
	i = 0;
	MSGLOOP(cell,msg) {
		case 0: /* COLOR : Update all cell compartment colors */
			cell->color->table[cell->fieldref[i]] = MSGVALUE(msg,0);
			i++;
			if (i == cell->nfield) 
				XoUpdateCell((Widget)cell->widget);
			break;
		case 1: /* DIAMETER : Update all compartment diameters */
			cell->dia->table[i] = MSGVALUE(msg,0);
			i++;
			if (i == cell->npts) 
				XoUpdateCell((Widget)cell->widget);
			break;
		case 2: /* POSITION : Update compartment position */
			cell->xpts->table[i] = MSGVALUE(msg,0);
			cell->ypts->table[i] = MSGVALUE(msg,1);
			cell->zpts->table[i] = MSGVALUE(msg,2);
			i++;
			if (i == cell->npts) 
				XoProjectAndDrawPix((PixObject)cell->widget,1);
			break;
		case 3: /* COLDIA : Update compartment color and diameter */
			cell->color->table[i] = MSGVALUE(msg,0);
			cell->dia->table[i] = MSGVALUE(msg,1);
			i++;
			if (i == cell->npts) 
				XoUpdateCell((Widget)cell->widget);
			break;
		case 4: /* COLDIAPOS : Update compartment col, dia, pos */
			cell->color->table[i] = MSGVALUE(msg,0);
			cell->dia->table[i] = MSGVALUE(msg,1);
			cell->xpts->table[i] = MSGVALUE(msg,2);
			cell->ypts->table[i] = MSGVALUE(msg,3);
			cell->zpts->table[i] = MSGVALUE(msg,4);
			i++;
			if (i == cell->npts) 
				XoProjectAndDrawPix((PixObject)cell->widget,1);
			break;
	}

	/* check out the autoscale options */
	if (get_extrema(&cell->diamin,&cell->diamax,
		cell->dia->table,cell->npts,cell->autodia)) {
		targs[0] = "diamin";
		targs[1] = ftoa(cell->diamin);
		targs[2] = "diamax";
		targs[3] = ftoa(cell->diamax);
		gx_convert(cell,GXconvert,XtNumber(GXconvert),4,targs);
		free(targs[1]);
		free(targs[3]);
		/* the hook for the update_diascale routine should be here */
	}
	if (get_extrema(&cell->colmin,&cell->colmax,
		cell->color->table,cell->npts,cell->autocol)) {
		targs[0] = "colmin";
		targs[1] = ftoa(cell->colmin);
		targs[2] = "colmax";
		targs[3] = ftoa(cell->colmax);
		gx_convert(cell,GXconvert,XtNumber(GXconvert),4,targs);
		free(targs[1]);
		free(targs[3]);
		/* the hook for the update_colscale routine should be here */
	}
    break;
  case RESET:
	i = 0;
#if 0
	MSGLOOP(cell,msg) {
		case 0: /* COLOR */
			break;
		case 1: /* DIAMETER */
			break;
		case 2: /* POSITION */
			break;
		case 3: /* COLDIA */
			break;
		case 4: /* COLDIAPOS */
			break;
	}
	if (i > 0) { /* it was a wavecell */
		while (cell->allocated_pts < i) /* most unlikely */
			xoExpandCell(cell);
		cell->npts = i;
		cell->xmax = i;
		for(i = 0;i<cell->npts;i++)
			cell->xpts->table[i] = i;
			targs[0] = "npts";
			targs[1] = itoa(i);
			targs[2] = "xmax";
			targs[3] = itoa(i);
			gx_convert(cell,GXconvert,XtNumber(GXconvert),
				4,targs);
	}
#endif
	i = 0;
	if (cell->path && (int)strlen(cell->path) > 1) {
		if (cell->npts == 0) {
			if (xoSetCellPath(cell,cell->path))
				i = 1;
			else {
				i = 0;
				break;
			}
		}
		if (cell->nfield == 0) {
			xoSetCellField(cell);
			i = 1;
		}
		if (i)
			XoProjectAndDrawPix((PixObject)cell->widget,1);
	}
    break;
  case CREATE:
    /* arguments are: object_type name [field value] ... */

	if ((parentW = xoFindParentDraw(cell)) == NULL) return(0);

	/* assign memory for the value field */
	cell->value = (char *)calloc(200,sizeof(char));

	cell->xpts = create_interpol(XO_CELL_PTS_INCREMENT,0.0,1.0);
	cell->ypts = create_interpol(XO_CELL_PTS_INCREMENT,0.0,1.0);
	cell->zpts = create_interpol(XO_CELL_PTS_INCREMENT,0.0,1.0);
	cell->color = create_interpol(XO_CELL_PTS_INCREMENT,0.0,1.0);
	cell->dia = create_interpol(XO_CELL_PTS_INCREMENT,0.0,1.0);
	cell->fieldref =(int *)calloc(XO_CELL_PTS_INCREMENT,sizeof(int));
	cell->fromlist =(int *)calloc(XO_CELL_PTS_INCREMENT,sizeof(int));
	cell->tolist =(int *)calloc(XO_CELL_PTS_INCREMENT,sizeof(int));
	cell->shapelist =(int *)calloc(XO_CELL_PTS_INCREMENT,sizeof(int));
	cell->names =(char **)calloc(XO_CELL_PTS_INCREMENT,sizeof(char *));
	cell->allocated_pts = XO_CELL_PTS_INCREMENT;
	cell->allocated_interps = XO_CELL_PTS_INCREMENT + 1;
	cell->npts = 0;
	cell->soma = -1; /* find it by the default method */
	cell->autodia = 1; /* find it automatically */
	cell->diarange = -20; /* set it to a default 20 pixels */
	cell->cellmode = "endcoords";
	for(i=0;i<XO_CELL_PTS_INCREMENT;i++)
		cell->fieldref[i]=i;

#define NOTCELLTEST
#ifdef CELLTEST
	/* Just for testing */
	for(i=0;i<XO_CELL_PTS_INCREMENT;i++) {
		cell->xpts->table[i] = sin(i/10.0);
		cell->ypts->table[i] = cos(i/10.0);
		cell->zpts->table[i] = i/10.0;
		cell->fromlist[i] = i-1;
		cell->tolist[i] = i+1;
		cell->shapelist[i] = XO_CELL_BRANCH;
		cell->color->table[i] = i;
		cell->dia->table[i] = 1.0 + sin(i/5.0);
	}
	cell->npts = XO_CELL_PTS_INCREMENT;
	cell->soma = 0;

#endif /* CELLTEST */
    
	cw = (CellObject)XtCreateManagedWidget(
		av[1], cellObjectClass, parentW, NULL,0);
    cell->widget = (char *)cw;
	/* copying over the table pointers */
	XoXtVaSetValues(cw,
		XtNxpts,(XtPointer)((Interpol *)cell->xpts)->table,
		XtNypts,(XtPointer)((Interpol *)cell->ypts)->table,
		XtNzpts,(XtPointer)((Interpol *)cell->zpts)->table,
		XtNcolor,(XtPointer)((Interpol *)cell->color)->table,
		XtNdia,(XtPointer)((Interpol *)cell->dia)->table,
		XtNnames,(XtPointer)cell->names,
		XtNfromlist,(XtPointer)cell->fromlist,
		XtNtolist,(XtPointer)cell->tolist,
		XtNshapelist,(XtPointer)cell->shapelist,
		XtNcolorscale,(XtPointer)XoColorscale(),
		XtNncols,(int)XoNcolors(),
		XtNnpts,cell->npts,
		XtNnlist,cell->nlist,
		NULL);
	XtAddCallback((Widget) cell->widget, XtNcallback,xoCallbackFn,
		(XtPointer)cell);

    ac--, av++;			/* object type */
    ac--, av++;			/* path */
	/*
    if (ac) {
		gx_convert(cell,GXconvert,XtNumber(GXconvert),ac,av);
    }
	*/
	xoParseCreateArgs(cell,ac,av);
	return(1);
	/* NOTREACHED */
	break;
  case SET:
    if (ac) {			/* need to set fields */
		if (xoSetPixFlags(cell,ac,av)) return(1);
		/* Special case : setting the interpol arrays. */
		if (SetTable(cell,2,action->argv,action->data,
			"xpts ypts zpts color dia")) {
			return(1);
		}
		if (strncmp(av[0]+1,"pts->table",10) == 0) {
		/* need to set points and update */
			DirectSetElement(cell,av[0],av[1]);
			XoProjectAndDrawPix((PixObject)cell->widget,0);
			return(1);
		}
		if (strcmp(av[0],"path") == 0) {
		/* need to set up tables */
			DirectSetElement(cell,av[0],av[1]);
			if (!xoSetCellPath(cell,av[1]))
				return(0);
		/* Need to redo the dia etc if autoscaling is on */
			if (cell->autodia) {
				if (get_extrema(&cell->diamin,&cell->diamax,
					cell->dia->table,cell->npts,cell->autodia)) {
					char *ttargs[5];
					ttargs[0] = "diamin";
					ttargs[1] = ftoa(cell->diamin);
					ttargs[2] = "diamax";
					ttargs[3] = ftoa(cell->diamax);
					gx_convert(cell,GXconvert,XtNumber(GXconvert),4,ttargs);
					free(ttargs[1]);
					free(ttargs[3]);
				}
			}
			XoProjectAndDrawPix((PixObject)cell->widget,1);
			return(1);
		}
		if (strcmp(av[0],"autocol") == 0 ||
			strcmp(av[0],"autodia") == 0) {
			switch(av[1][0]) {
				case '0':
				case 'f':
				case 'F':
					i = 0;
					break;
				case '1':
				case 's':
				case 'S':
					i = XO_CELL_SCALE_ON_SET;
					break;
				case '2':
				case 't':
				case 'T':
					i = XO_CELL_EXPAND;
					break;
				case '3':
				case 'c':
				case 'C':
					i = XO_CELL_CONTINUOUS;
					break;
				default:
					printf("Warning: option %s for %s unknown. Ignoring.\n",av[1],av[0]);
					return(1);
					/* NOTREACHED */
					break;
			}
			if (strcmp(av[0],"autocol") == 0)
				cell->autocol = i;
			else {
				cell->autodia = i;
				if (cell->autodia) {
					if (get_extrema(&cell->diamin,&cell->diamax,
						cell->dia->table,cell->npts,cell->autodia)) {
						char *ttargs[5];
						ttargs[0] = "diamin";
						ttargs[1] = ftoa(cell->diamin);
						ttargs[2] = "diamax";
						ttargs[3] = ftoa(cell->diamax);
						gx_convert(cell,GXconvert,XtNumber(GXconvert),4,ttargs);
						free(ttargs[1]);
						free(ttargs[3]);
					}
				}
			}

			return 1;
		}
		if (strcmp(av[0],"colfield") == 0)
			av[0] = "field"; /* Backwards compat hack */
		if (strcmp(av[0],"field") == 0 ||
			strcmp(av[0],"fieldpath") == 0) {

			if (strcmp(av[0],"field") == 0 &&
				(av[1] == NULL || (int)strlen(av[1]) < 1)) {
				printf("Error in setting cell %s field to null \n",
					cell->name);
				return(1);
			}
		/* set up field and its path using messages */
			DirectSetElement(cell,av[0],av[1]);
			if (cell->npts > 0)
				xoSetCellField(cell);
			return(1); /* We do not need to go on to the gxconvert */
		}
		if (strcmp(av[0],"npts") == 0 ||
			strcmp(av[0],"nlist") == 0) {
			printf ("Warning: %s on %s is a protected field. Value not changed\n",av[0],cell->name);
			return(1);
		}

		gx_convert(cell,GXconvert,XtNumber(GXconvert),ac,av);
		return(0);
	}
	break;
  case DELETE:
	if (!(cell->widget)) return(0);
	/* the value field */
	if (cell->value)
		free(cell->value);
	/* the coord arrays */
	XtDestroyWidget((Widget)cell->widget);
	ip = (Interpol *)cell->xpts;
	if (ip->allocated) free(ip->table); free(ip);
	ip = (Interpol *)cell->ypts;
	if (ip->allocated) free(ip->table); free(ip);
	ip = (Interpol *)cell->zpts;
	if (ip->allocated) free(ip->table); free(ip);

	/* the color array */
	ip = (Interpol *)cell->color;
	if (ip->allocated) free(ip->table); free(ip);

	/* the diameter array */
	ip = (Interpol *)cell->dia;
	if (ip->allocated) free(ip->table); free(ip);

	free(cell->fieldref);
	/* The from and to list  array */
	free(cell->fromlist);
	free(cell->tolist);
	free(cell->shapelist);

	/* The names array */
	for(i=0;i<cell->npts;i++)
		if (cell->names[i] != NULL)
			free(cell->names[i]);
	free(cell->names);
	break;
  case XUPDATE : /* update cell fields due to changes in widget */
		xg_convert(cell,GXconvert,XtNumber(GXconvert),ac,av);
	break;
  default:
  	/* info->ret contains the index of the compt clicked on */
  	i = -1;
  	if (action->data)
  		i = *((int *)(((XoEventInfo *)(action->data))->ret));
  	if (i >= 0 && i < cell->npts) {
  		strcpy(cell->value, cell->names[i]);
  	} else {
  		cell->value[0] = '\0';
  	}
	/* cell->value = action->argv[0]; */
	xoExecuteFunction(cell,action,cell->script,cell->value);
	break;
  }
	return(0);
}


static int xoSetCellPath(cell,path)
     struct xcell_type *cell;
	 char	*path;
{
	Element	**elms;
	Element	*GetElement(),*RelGetElement();
	ElementList	*elmlist, *WildcardGetElement();
	int		nelms;
	int		i,j,k;
	int		MembId = ClassID("membrane");
	double	*dia;
	double	sumdia,maxdia;
	int		imaxdia;
	HASH	*hash_table,*hash_create();
	ENTRY	entry;
	ENTRY	*tempentry, *xo_hash_find(), *xo_hash_enter();
	int		*nterm;
	int		*tolist;
	int		*fromlist;
	int		*shapelist;
	MsgOut	*msgout;
	char	*CalculateAddress();
	char	*dia_addr;
	Info	info;

	cell->npts = 0;
	elmlist = WildcardGetElement(cell->path, 0);
	if (elmlist->nelements==0) {
		fprintf(stderr,"Error in xoSetCellPath (xcell): couldnt parse path %s\n",cell->path); 
	if (cell->path[0] != '/')
		fprintf(stderr, "Note: Use absolute path names for the path field\n");
		return 0;
	}
	nelms = elmlist->nelements;
	elms = elmlist->element;
	for(j=0,i=0;i<nelms;i++) {
		if (CheckClass(elms[i],MembId)) {
			if (i!=j)
				elms[j] = elms[i];
			j++;
		}
	}
	nelms = j;
	if (nelms == 0 || (!elms)) {
		printf("Error in xoSetCellPath (xcell): No elements found for this path (%s)\n", cell->path);
		return 0;
	}

	/* cell->npts is set in this routine */
	xoExpandCell(cell,nelms);

	/* Initialize the fieldrefs. Fieldrefs refer to  the compartment
	** relative to which the field is defined. By default, the fields
	** are on the compartment itself, so fieldref is simply the index
	** of the compartment */
	for(i=0;i<cell->npts;i++) cell->fieldref[i]=i;

	/* Initialize the names array. The names are the strings
	** associated with each compartment in the cell. By default
	** these are the paths of each compartment. */
	for(i=0;i<cell->npts;i++) cell->names[i]=CopyString(Pathname(elms[i]));

	tolist = cell->tolist;
	fromlist = cell->fromlist;
	shapelist = cell->shapelist;
	dia = cell->dia->table;

	/* create hash table to find index corresponding to elm pointer */
	hash_table = hash_create(nelms *2);
	for(i=0;i<nelms;i++) {
		entry.data = (char *)i;
		entry.key = (char *)elms[i];
		xo_hash_enter(&entry,hash_table);
	}

	nterm = (int *)calloc(nelms,sizeof(int));

	/* Set the compartment coords */
	for(i=0;i<nelms;i++) {
		cell->xpts->table[i] = elms[i]->x;
		cell->ypts->table[i] = elms[i]->y;
		cell->zpts->table[i] = elms[i]->z;
		dia_addr = CalculateAddress(elms[i],elms[i]->object->type,"dia",
			&info);
		if (info.type[0] == 'f') /* float */
			dia[i] = *(float *)dia_addr;
		else if (info.type[0] == 'd') /* double */
			dia[i] = *(double *)dia_addr;
	}

	/* Identify cell soma */
	/* A soma value of -2 means do not make a soma 
	** a value of -1 means find it by the default method
	** a value >= 0 means that it has already been specified 
	*/
	sumdia = maxdia = 0.5 * dia[0];
	imaxdia = -1;
	if (nelms == 1) { /* a 1 compt cell */
		cell->soma = 0;
	} else {
		if (cell->soma == -1) { /* try to find soma from diameters */
			for(i = 0 ; i < nelms; i++) {
				sumdia += dia[i];
				if (maxdia < dia[i]) {
					maxdia = dia[i];
					imaxdia = i;
				}
			}
			if (maxdia <= 0 || (maxdia * nelms - sumdia)/sumdia < 1e-6) {
				/* the compts had equal dia */
				cell->soma = -1;
			} else {
				cell->soma = imaxdia;
			}
		}
	}
	/* Construct the to-from lists. These are the indices of 
	** the beginning and end of every compt/line on the cell */
	j = 0;
	if (strcmp(BaseObject(elms[0])->name, "symcompartment") == 0) {
		for(i=0;i<nelms;i++) {
			int msgcount;
			MsgIn *msgin;
			k=0;
			msgout = elms[i]->msgout;
			for(msgcount = 0 ; msgcount < elms[i]->nmsgout; msgcount++) {
				msgin = GetMsgInByMsgOut(msgout->dst, msgout);
				/* message type 5 is the hideous CONNECTCROSS
				** message, which fouls up this routine */
				if (msgin->type != 5) {
					if ((tempentry=xo_hash_find(msgout->dst,hash_table))
						!= NULL){
						tolist[j] = (int)tempentry->data;
						k++;
						if (tolist[j] > i) {
							fromlist[j] = i;
							j++;
							if (j > nelms) { /* This can only happen if there
									 		** is a looped structure */
								printf("Warning: cell path %s is looped\n",path);
								cell->npts = 0;
								return 0;
							}
						}
					}
				}
				msgout = MSGOUTNEXT(msgout);
			}
			if (k == 0) { /* this dend has no attached compartments */
				tolist[j] = fromlist[j] = i; /* mark it as a solo soma */
				j++;
			}
		}
	} else {
		for(i=0;i<nelms;i++) {
			int msgcount;
			k=0;
			msgout = elms[i]->msgout;
			for(msgcount = 0 ; msgcount < elms[i]->nmsgout; msgcount++) {
				if ((tempentry=xo_hash_find(msgout->dst,hash_table))
					!= NULL){
					tolist[j] = (int)tempentry->data;
					k++;
					if (tolist[j] > i) {
						fromlist[j] = i;
						j++;
						if (j > nelms) { /* This can only happen if there
									 	** is a looped structure */
							printf("Warning: cell path %s is looped\n",path);
							cell->npts = 0;
							return 0;
						}
					}
				}
				msgout = MSGOUTNEXT(msgout);
			}
			if (k == 0) { /* this dend has no attached compartments */
				tolist[j] = fromlist[j] = i; /* mark it as a solo soma */
				j++;
			}
		}
	}
	cell->nlist = j;
	/* Figure out what shape each segment is by counting the 
	** number of segments terminating at each compartment */
	for(i=0;i<cell->npts;i++) nterm[i] = 0;

	/* count the number terminating */
	for(i=0;i<cell->nlist;i++) {
		if (tolist[i] == fromlist[i]) continue; /* solo soma */
		nterm[tolist[i]]++;
		nterm[fromlist[i]]++;
	}

	/* If we are in the default endcoord mode, we need to flip
	** the from/to lists so that the 'to' entry is always further
	** from the soma.
	*/
	if (strncmp(cell->cellmode,"m",1) != 0) {
		int *temp = (int *)calloc(nelms, sizeof(int));
		int *fttemp = (int *)calloc(nelms, sizeof(int));
		int swap;
		int nchecks;
		/* First pass: find all entries which terminate only once */
		/* Conveniently, we have just counted all termini */
		for(i=0;i<cell->nlist;i++) {
			if (i == cell->soma)
				continue;
			if (nterm[fromlist[i]] == 1) {
				/* swap the to and fromlist entries */
				swap = fromlist[i];
				fromlist[i] = tolist[i];
				tolist[i] = swap;
				temp[swap] = 1;
				fttemp[i] = 1;
			}
			if (nterm[tolist[i]] == 1) {
				temp[tolist[i]] = 1;
				fttemp[i] = 1;
			}
		}
		/* Second pass: Find all entries in which one of the
		** pair has been done. The done entry is closer to the
		** term. Flip if necessary. Repeat till there are no more
		** checked entries.
		*/
		for (nchecks = 1; nchecks > 0;) {
			nchecks = 0;
			for(i=0;i<cell->nlist;i++) {
				/* if this from_to entry has been done, ignore it */
				if (fttemp[i])
					continue;

				/* check if the from entry has been done */
				if (temp[fromlist[i]]) {
				/* At this point, we know that the 'to' entry
				** cannot have been done, because the fftemp
				** test should have excluded cases where it was.
				** So we need to swap here.
				*/
					swap = fromlist[i];
					fromlist[i] = tolist[i];
					tolist[i] = swap;
					temp[swap] = 1;
					fttemp[i] = 1;
					nchecks++;
					continue;
				}

				/* Check if the to entry has been done */
				if (temp[tolist[i]]) {
					temp[tolist[i]] = 1;
					fttemp[i] = 1;
					nchecks++;
				}
			}
		}
	}


	/* Go through each segment and find how many terminate at
	** each end */
	for(i=0;i<cell->nlist;i++) {
		shapelist[i] = 0;
		/* This check is unnecessary and invalid anyway. If one
		** is displaying a compartment all on its ownsome, then
		** both these error conditions will be true even though
		** the cell itself is perfectly valid. So we comment it out
		*/
		/*
		if (nterm[tolist[i]] == 0 || nterm[fromlist[i]] == 0) {
			printf("Error: nterm[%d] = %d,nterm[%d]=%d,i=%d\n",
				fromlist[i],nterm[fromlist[i]],
				tolist[i],nterm[tolist[i]],i);
		}
		*/
		/* assign the terminus conditions */
		if (nterm[tolist[i]] > 1) shapelist[i] |= XO_CELL_TO_DEND;
		if (nterm[tolist[i]] == 1) shapelist[i] |= XO_CELL_TO_TIP;
		if (tolist[i] == cell->soma) shapelist[i] |= XO_CELL_TO_SOMA;
		if (nterm[fromlist[i]] >1) shapelist[i] |= XO_CELL_FROM_DEND;
		if (nterm[fromlist[i]] ==1) shapelist[i] |= XO_CELL_FROM_TIP;
		if (fromlist[i] == cell->soma)
			shapelist[i] |= XO_CELL_FROM_SOMA;
		if (tolist[i] == fromlist[i]) /* set shapelist to a solo soma */
			shapelist[i] = XO_CELL_ONLY_TO_SOMA;
	}


	/* set some of the special xcell values that the user does
	** not have access to, which are therefore not in the 
	** gxconvert list 
	*/
	XoXtVaSetValues((Widget) cell->widget,
			XtNnpts,cell->npts,
			XtNnlist,cell->nlist,
			XtNsoma,cell->soma,
			NULL);

	/* free a lot of stuff */
	FreeElementList(elmlist);
	if (hash_table) {
		if (hash_table->size > 0)
			free(hash_table->entry);
		free(hash_table);
	}
	free(nterm);

	/* check if we still have a relative path to use */
	if (cell->fieldpath && (int)strlen(cell->fieldpath) > 0)
		xoSetCellField(cell);
	return 1;
}


static void xoSetCellField(cell)
     struct xcell_type *cell;
{
	char	*targs[5];
	char	fullpath[200];
	ElementList	*field_list,*main_list,*WildcardGetElement();
	Info		info;
	Element	*parent,*elm;
	int		i,j;

	if (cell->npts <= 0 || cell->field == NULL ||
		(int)strlen(cell->field) < 1) return;
#ifdef VERSION2
	DeleteAllMsgsByType(cell, 0 /* COLOR */);
	/*
	MSGLOOP(cell,msg) {
		case 0:	 ** COLOR **
			if (!DeleteMsgIn(cell,msg)) {
				printf("unable to delete COLOR msg into cell\n");
				printf("cell field set failed\n");
				return;
			}
		break;
	}
	*/
#else
	for(msg=cell->msg_in;msg;msg = msg->next) {
		if (msg->type == 0) { /* COLOR is type 0 */
			if (!DeleteMsgIn(cell,msg)) {
				printf("unable to delete COLOR msg into cell\n");
				printf("cell field set failed\n");
				return;
			}
		}
	}
#endif
	targs[0] = "set_cell_field";
	if (cell->fieldpath != NULL && ((int)strlen(cell->fieldpath) > 0)) {
		sprintf(fullpath,"%s/%s",cell->path,cell->fieldpath);
		field_list = WildcardGetElement(fullpath);
		if (field_list == NULL || field_list->nelements <= 0 ||
			CalculateAddress(field_list->element[0],
				 Type(field_list->element[0]),
				 cell->field, &info) == NULL) {
		    FreeElementList(field_list);
		    return;
		}
		/* NOTE: field_list is used below */

		targs[1] = fullpath;
		targs[2] = Pathname(cell);
		targs[3] = "COLOR";
		targs[4] = cell->field;
		do_add_msg(5,targs);
	} else {
		strcpy(fullpath,cell->path);
		field_list = WildcardGetElement(fullpath);
		if (field_list == NULL || field_list->nelements <= 0 ||
			CalculateAddress(field_list->element[0],
				 Type(field_list->element[0]),
				 cell->field, &info) == NULL) {
		    FreeElementList(field_list);
		    return;
		}
		FreeElementList(field_list);

		targs[1] = fullpath;
		targs[2] = Pathname(cell);
		targs[3] = "COLOR";
		targs[4] = cell->field;
		do_add_msg(5,targs);
		cell->nfield = cell->npts;
		return;
	}

	/* match up the elements in the main and field paths */
	main_list = WildcardGetElement(cell->path);
	cell->nfield = field_list->nelements;
	if (field_list->nelements > main_list->nelements) {
		printf("Warning: path %s/%s has more elements than cellpath %s\n",cell->path, cell->fieldpath, cell->path);
	} else if (field_list->nelements < main_list->nelements) {
		for(j=0,i=0;i<field_list->nelements;i++) {
			/* go through the main list */
			/* we rely on the order of the lists being equivalent */
			for (;j < cell->npts ; j++) {
				parent = main_list->element[j];
				/* look for the ancestor of the field element that
				** matches the parent */
				for(elm=field_list->element[i];elm; elm=elm->parent) {
					if (elm == parent) {
						cell->fieldref[i]=j;
						break;
					}
				}
				if (elm == parent) break;
			}
		}
	}
	FreeElementList(main_list);
	FreeElementList(field_list);
}

static void xoExpandCell(cell,npts)
	struct xcell_type 	*cell;
	int					npts;
{
	int ap;
	Interpol	*ip;
	Widget cw = (Widget)cell->widget;

	if (cell->npts == npts && npts <= cell->allocated_pts) return;
		cell->npts = npts;
	if (npts > cell->allocated_pts) {
		ap = (1+npts/XO_CELL_PTS_INCREMENT) *
			XO_CELL_PTS_INCREMENT;

		ip = (Interpol *)cell->xpts;
		ip->table=(double *)realloc(ip->table,ap * sizeof(double));
		xomemzero((char*) &ip->table[cell->allocated_interps], sizeof(double) * (ap-cell->allocated_interps));
		ip->xdivs=ap;
		ip = (Interpol *)cell->ypts;
		ip->table=(double *)realloc(ip->table,ap * sizeof(double));
		xomemzero((char*) &ip->table[cell->allocated_interps], sizeof(double) * (ap-cell->allocated_interps));
		ip->xdivs=ap;
		ip = (Interpol *)cell->zpts;
		ip->table=(double *)realloc(ip->table,ap * sizeof(double));
		xomemzero((char*) &ip->table[cell->allocated_interps], sizeof(double) * (ap-cell->allocated_interps));
		ip->xdivs=ap;
	
		/* the color array */
		ip = (Interpol *)cell->color;
		ip->table=(double *)realloc(ip->table,ap * sizeof(double));
		xomemzero((char*) &ip->table[cell->allocated_interps], sizeof(double) * (ap-cell->allocated_interps));
		ip->xdivs=ap;
	
		/* the diameter array */
		ip = (Interpol *)cell->dia;
		ip->table=(double *)realloc(ip->table,ap * sizeof(double));
		xomemzero((char*) &ip->table[cell->allocated_interps], sizeof(double) * (ap-cell->allocated_interps));
		ip->xdivs=ap;
	
		cell->fieldref = (int *)realloc(cell->fieldref,ap *
			sizeof(int));
		/* The from and to list  array */
		cell->fromlist = (int *)realloc(cell->fromlist,ap *
			sizeof(int));
		xomemzero((char*) &cell->fromlist[cell->allocated_pts], sizeof(int) * (ap - cell->allocated_pts));
		cell->tolist = (int *)realloc(cell->tolist,ap *
			sizeof(int));
		xomemzero((char*) &cell->tolist[cell->allocated_pts], sizeof(int) * (ap - cell->allocated_pts));
		cell->shapelist = (int *)realloc(cell->shapelist,ap *
			sizeof(int));
		xomemzero((char*) &cell->shapelist[cell->allocated_pts], sizeof(int) * (ap - cell->allocated_pts));
	
		/* The names array */
		cell->names = (char **)realloc(cell->names,ap * sizeof(char *));
		/*
		for(i=0;i<cell->npts;i++)
			if (cell->names[i] == NULL)
				*/
				/* allocate space for names here */;
		cell->allocated_interps = cell->allocated_pts=ap;

		XoXtVaSetValues(cw,
			XtNxpts,(XtPointer)((Interpol *)cell->xpts)->table,
			XtNypts,(XtPointer)((Interpol *)cell->ypts)->table,
			XtNzpts,(XtPointer)((Interpol *)cell->zpts)->table,
			XtNcolor,(XtPointer)((Interpol *)cell->color)->table,
			XtNdia,(XtPointer)((Interpol *)cell->dia)->table,
			XtNnames,(XtPointer)cell->names,
			XtNfromlist,(XtPointer)cell->fromlist,
			XtNtolist,(XtPointer)cell->tolist,
			XtNshapelist,(XtPointer)cell->shapelist,
			XtNnpts,cell->npts,
			XtNnlist,cell->nlist,
			NULL);
	} else {
		XoXtVaSetValues(cw,
			XtNnpts,cell->npts,
			NULL);
	}
}

#define XO_CELL_EPSILON 1e-15
static int get_extrema(min,max,table,npts,auto_mode)
	float *min,*max;
	double *table;
	int	npts;
	int auto_mode;
{
	float tmin = *min;
	float tmax = *max;
	float temp;
	int i;

	switch(auto_mode) {
		case XO_CELL_SCALE_ON_SET: /* Start from scratch here */
		case XO_CELL_CONTINUOUS: /* the dia is adjusted every timestep,
						** both up and down */
			tmin =tmax = table[0];
		break;
		case XO_CELL_EXPAND: /* the dia is only adjusted upwards */
		break;
		default:
			return(0);
			/* NOTREACHED */
			break;
	}

	for(i=0;i<npts;i++) {
		temp = table[i];
		if (tmin > temp)
			tmin = temp;
		else if (tmax < temp)
			tmax = temp;
	}
	if (fabs((double)(tmin - tmax)) < XO_CELL_EPSILON) return(0);
	if (fabs((double)(tmin - *min)) < XO_CELL_EPSILON &&
		fabs((double)(tmax - *max)) < XO_CELL_EPSILON)
		return(0);

	*min = tmin;
	*max = tmax;
	return(1);
}
#undef XO_CELL_EPSILON
#undef XO_CELL_CONTINUOUS
#undef XO_CELL_EXPAND
#undef MAX_NTARGS
