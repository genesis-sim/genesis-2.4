/* $Id: xview.c,v 1.2 2005/07/01 10:02:59 svitak Exp $ */
/*
 * $Log: xview.c,v $
 * Revision 1.2  2005/07/01 10:02:59  svitak
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
 * Revision 1.22  2001/06/29 21:12:27  mhucka
 * Added extra parens inside conditionals and { } groupings to quiet certain
 * compiler warnings.
 *
 * Revision 1.21  2001/04/18 22:39:35  mhucka
 * Miscellaneous portability fixes, mainly for SGI IRIX.
 *
 * Revision 1.20  2000/06/12 04:21:19  mhucka
 * 1) Removed nested comments in RCS/CVS log lines, to quiet down the
 *    IRIX cc compiler.
 * 2) Added NOTREACHED comments where appropriate.
 *
 * Revision 1.19  2000/05/02 06:18:32  mhucka
 * Added type casts for certain function call arguments.
 *
 * Revision 1.18  1997/07/18 23:55:56  dhb
 * Updated ShowInterpol() calls to add dimension arg
 *
 * Revision 1.17  1997/07/18 19:52:53  dhb
 * Changes from PSC: new messages for parallel xviewing
 *
 * Revision 1.16  1995/09/27  00:09:39  venkat
 * XtVaSetValues()-calls-changed-to-XoXtVaSetValues()-to-avoid-alpha-FPE's
 *
 * Revision 1.15  1995/07/08  02:56:41  dhb
 * Merged in 1.12.1.2.1.1 changes
 *
 * Revision 1.14  1995/07/08  01:15:35  venkat
 * Fixed bug where view was wrongly allocating wrong number of cells to its
 * interpol tables and setting its xdivs to a value higher than required
 * This was causing core dumps on reset in certain scripts
 *
 * Revision 1.13  1995/06/26  23:39:43  dhb
 * Merged in 1.12.1.3
 *
 * Revision 1.12.1.3  1995/06/21  23:32:39  venkat
 * The message which warns the user to set the path field
 * relative path name
 *
 * Revision 1.12.1.2.1.1  1995/07/08  02:48:21  dhb
 * Contribution:
 *
 *
 * Upi Bhalla Mt. Sinai School of Medicine July 6 1995 
 * Added code for doing autoscaling
 *
 * Revision 1.12.1.2  1995/06/15  20:28:52  venkat
 *  WildcardGetElement() always returns a valid pointer to the head of a locally
 * created element list. So checking this for null is not reasonable but checking whether the no of elements found in the path is. Also the warning message
 * now makes a note of using absolute pathnames for setting the path field.
 *
 * Revision 1.12.1.1  1995/05/12  23:18:14  dhb
 * Changes to xview to support 1.4 xfileview type functionality
 * using disk_in and xview elements.
 *
 * Revision 1.12  1995/03/28  21:12:01  venkat
 * COPY
 *
 * Revision 1.11  1994/06/29  14:18:41  bhalla
 * Fixed bug with messag-based viewing in the 'default' action,
 * where I was doing a strcmp on null strings
 *
 * Revision 1.10  1994/06/13  23:01:17  bhalla
 * Bugfix: moved xoParseCreateArgs after creation of child widgets
 * Added pixflags c for child widgets
 *  Added updating of value_min etc in SET action
 * Added backwards compat viewmodes
 *
 * Revision 1.9  1994/05/25  13:39:19  bhalla
 * Bugfix: added check for null coord_elist in ElistGetValues
 *
 * Revision 1.8  1994/04/25  17:55:23  bhalla
 * Put in missing code to generate a sensible value when a view is
 * clicked on. Added new field 'valuemode' to decide whether to use
 * index, lookup or path (default) as the value
 *
 * Revision 1.7  1994/03/22  15:12:19  bhalla
 *  RCS error in previous check in. No code changes
 *
 * Revision 1.6  1994/02/08  17:47:26  bhalla
 * Added code for VERSION2 msg handling, reorganized code to use
 * GetMsgValuByMsgtype function
 *
 * Revision 1.6  1994/02/08  17:47:26  bhalla
 * Added code for VERSION2 msg handling, reorganized code to use
 * GetMsgValuByMsgtype function
 *
 * Revision 1.5  1994/02/02  20:04:53  bhalla
 * eliminated soft actions
 * Put in hooks for Genesis2 msg functions, but those are not yet implemented.
 *
 * Revision 1.4  1994/01/08  15:47:20  bhalla
 * incorporated display of msg fields and assorted fixes
 *
 * Revision 1.4  1994/01/08  15:47:20  bhalla
 * incorporated display of msg fields and assorted fixes
 *
 * Revision 1.3  1994/01/01  19:18:54  bhalla
 * most path/relpath/msgpath options work. text_val is broken
 * */

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include "Widg/Framed.h"
#include "Draw/Pix.h"
#include "Draw/Shape.h"
#include "Draw/View.h"
#include "draw_ext.h"
#include "_widg/xform_struct.h"
#include "Xo/XoDefs.h"
#include "Xo/Xo_ext.h"

#define XO_VIEW_MAXSHAPES 50

#define XO_VIEW_USE_MSGS 0x01
#define XO_VIEW_USE_FIELD1 0x02
#define XO_VIEW_USE_FIELD2 0x04
#define XO_VIEW_USE_MSGIN 0x08
#define XO_VIEW_USE_MSGOUT 0x10
#define XO_VIEW_USE_MSGSRC 0x20
#define XO_VIEW_USE_MSGDEST 0x40

#define XO_VIEW_NCOORDS_INCREMENT 10

static int IsOnList();
static int ElistGetValues();
static int GetMsgValueByMsgtype();

static Gen2Xo GXconvert[] = {
	{"tx",		XtNpixX},
	{"ty",		XtNpixY},
	{"tz",		XtNpixZ},
	{"viewmode",	XtNviewmode},
	{"color_val",	XtNviewColorVal},
	{"morph_val",	XtNviewMorphVal},
	{"xoffset_val",	XtNviewXOffsetVal},
	{"yoffset_val",	XtNviewYOffsetVal},
	{"zoffset_val",	XtNviewZOffsetVal},
	{"text_val",	XtNviewTextVal},
	{"textcolor_val",	XtNviewTextColorVal},
	{"linethickness_val",	XtNviewLineThicknessVal},
};

int XView (view, action)
     register struct xview_type *view;
     Action                     *action;
{
	int ac=action->argc;
	char** av=action->argv;
    Widget parentW, xoFindParentDraw();
	ViewObject vW;
	int 	i,j=0;
	Interpol	*create_interpol();
	int 	alloc_interpoltable(); 
	struct 	xview_type *copy_view;
	MsgIn	*msg;
	Element	*elms[XO_VIEW_MAXSHAPES];
	struct	xshape_type	*xshape;
	Action	newaction;
	char	*newargv[15];
	int			ncoords;
	int			nval1,nval2,nval3,nval4,nval5;
	int			nv;
	int			nc;
  
  if (Debug(0) > 1)
    ActionHeader("XView", view, action);

  SELECT_ACTION (action) {
  case INIT:
    break;
  case PROCESS:
	nval1 = nval2 = nval3 = nval4 = nval5 = 0;
	if (view->viewflags & XO_VIEW_USE_MSGS) {
		MSGLOOP(view,msg) {
			case 0: /* V1 */
				if (nval1 >= view->ncoords) break;
				view->values[0][nval1] = MSGVALUE(msg,0);
				nval1++;
			break;
			case 1: /* V2 */
				if (nval2 >= view->ncoords) break;
				view->values[1][nval2] = MSGVALUE(msg,0);
				nval2++;
			break;
			case 2: /* V3 */
				if (nval3 >= view->ncoords) break;
				view->values[2][nval3] = MSGVALUE(msg,0);
				nval3++;
			break;
			case 3: /* V4 */
				if (nval4 >= view->ncoords) break;
				view->values[3][nval4] = MSGVALUE(msg,0);
				nval4++;
			break;
			case 4: /* V5 */
				if (nval5 >= view->ncoords) break;
				view->values[4][nval5] = MSGVALUE(msg,0);
				nval5++;
			break;
		        case 6: /* IVAL1 */
			        nval1 = MSGVALUE(msg,0);
				if (nval1 >= view->ncoords) break;
				view->values[0][nval1] = MSGVALUE(msg,1);
				break;
		        case 7: /* IVAL2 */
			        nval2 = MSGVALUE(msg,0);
				if (nval2 >= view->ncoords) break;
				view->values[1][nval2] = MSGVALUE(msg,1);
				break;
		        case 8: /* IVAL3 */
			        nval3 = MSGVALUE(msg,0);
				if (nval3 >= view->ncoords) break;
				view->values[2][nval3] = MSGVALUE(msg,1);
				break;
		        case 9: /* IVAL4 */
			        nval4 = MSGVALUE(msg,0);
				if (nval4 >= view->ncoords) break;
				view->values[3][nval4] = MSGVALUE(msg,1);
				break;
		        case 10: /* IVAL5 */
			        nval5 = MSGVALUE(msg,0);
				if (nval5 >= view->ncoords) break;
				view->values[4][nval5] = MSGVALUE(msg,1);
				break;
		        case 11: /* ICOORDS */
			        nc = MSGVALUE(msg,0);
				if (nc >= view->ncoords) break;
				view->xpts->table[nc] = MSGVALUE(msg,1);
				view->ypts->table[nc] = MSGVALUE(msg,2);
				view->zpts->table[nc] = MSGVALUE(msg,3);
				break;
		}
	} else {
		/* scan for view values */
		ElistGetValues(view->values[0],view->values[1],
				view->coord_elist,
				view->value_elist,view->msg_elist,
				view->field, view->field2,
				view->msgtype,view->msgslotno,
				view->viewflags);
	}
	if (view->autoscale && view->ncoords > 1) {
		int val_no;
		float *val_array;
		float vmin,vmax;
		/* Figure out which values need autoscaling */
		for (val_no = 1; val_no < 6; val_no++) {
			if (view->color_val == val_no ||
				view->morph_val == val_no ||
				view->xoffset_val == val_no ||
				view->yoffset_val == val_no ||
				view->zoffset_val == val_no ||
				view->text_val == val_no ||
				view->textcolor_val == val_no ||
				view->linethickness_val == val_no) {
				val_array = view->values[val_no-1];

				if (view->autoscale == 2) { /* 2 is ratchet autoscale */
					vmin = view->value_min[val_no - 1];
					vmax = view->value_max[val_no - 1];
				} else {
					vmax = vmin = val_array[0];
				}
				for(i = 1; i < view->ncoords; i++) {
					if (vmin > val_array[i])
						vmin = val_array[i];
					else if (vmax < val_array[i])
						vmax = val_array[i];
				}
				view->value_min[val_no - 1] = vmin;
				view->value_max[val_no - 1] = vmax;
			}
		}
		if (view->autolimits && view->nshapes > 1) {
			for(i = 0; i < view->nshapes; i++) {
				for(j= 0; j < XoVarNValues; j++) {
					view->shapes[i].value_limit[j] =
						view->value_min[j] +
						(double)i/(double)(view->nshapes-1) * 
						(view->value_max[j] - view->value_min[j]);
				}
			}
		}
	}
	XoUpdateView((Widget)view->widget);
    break;
  case RESET:
  /* This first step is inherited from the xvar. It scans the list of
	of children to find the applicable shapes, and fills in
	the lists */
	for (i = 0; i < XO_VIEW_MAXSHAPES; i++) {
		elms[i] = GetChildElement(view,"shape",i);
		if (!elms[i]) break;
	}
	if (i > 0) {
		if (i != view->nshapes) {
			if (view->nshapes > 0)
				free(view->shapes);
			view->shapes = (varShape *)calloc(i,sizeof(varShape));
			view->nshapes = i;
		}
		for(i = 0; i < view->nshapes; i++) {
			xshape = (struct xshape_type *)elms[i];
			view->shapes[i].shape = xshape->widget;
			view->shapes[i].fgindex = atoi(xshape->fg);
			view->shapes[i].textcolorindex = atoi(xshape->textcolor);
			if (view->autolimits && view->nshapes > 1) {
				for(j= 0; j < XoVarNValues; j++)
					view->shapes[i].value_limit[j] =
						view->value_min[j] +
						(double)i/(double)(view->nshapes-1) * 
						(view->value_max[j] - view->value_min[j]);
			}
		}

/* Here we fill in the values of the xview */

	/* First, check if there are coord or value msgs.
	** If there are coord msgs, this is used as the number of
	** coords, regardless of the number of value msgs. 
	** If there are only value msgs, then we assume someone else
	** is going to fill the coords. so we allocate as many coords
	** as the max number of value msgs.
	** If there are neither coord nor value msgs then we hope that
	** the path is set to something sensible.
	*/
		ncoords = 0;
		nval1 = nval2 = nval3 = nval4 = nval5 = 0;
		MSGLOOP(view,msg) {
			case 0: /* V1 */
				nval1++;
			break;
			case 1: /* V2 */
				nval2++;
			break;
			case 2: /* V3 */
				nval3++;
			break;
			case 3: /* V4 */
				nval4++;
			break;
			case 4: /* V5 */
				nval5++;
			break;
			case 5: /* COORDS */
				ncoords++;
			break;
		        case 6: /* IVAL1 */
			        nv = MSGVALUE(msg,0);
				if (nv >= nval1)
				  nval1 = nv + 1;
				break;
		        case 7: /* IVAL2 */
			        nv = MSGVALUE(msg,0);
				if (nv >= nval2)
				  nval2 = nv + 1;
				break;
		        case 8: /* IVAL3 */
			        nv = MSGVALUE(msg,0);
				if (nv >= nval3)
				  nval3 = nv + 1;
				break;
		        case 9: /* IVAL4 */
			        nv = MSGVALUE(msg,0);
				if (nv >= nval4)
				  nval4 = nv + 1;
				break;
		        case 10: /* IVAL5 */
			        nv = MSGVALUE(msg,0);
				if (nv >= nval5)
				  nval5 = nv + 1;
				break;
		        case 11: /* ICOORDS */
			        nc = MSGVALUE(msg,0);
				if (nc >= ncoords)
				  ncoords = nc + 1;
				break;
		}
		if (ncoords > 0) {
			view->viewflags |= XO_VIEW_USE_MSGS;
			view->ncoords = ncoords;
		} else if (nval1 > 0 || nval2 > 0 || nval3 > 0 || nval4 > 0 || nval5 > 0) {
			view->viewflags |= XO_VIEW_USE_MSGS;
			view->ncoords = (nval1 > nval2) ? nval1 : nval2;
			if (view->ncoords < nval3) view->ncoords = nval3;
			if (view->ncoords < nval4) view->ncoords = nval4;
			if (view->ncoords < nval5) view->ncoords = nval5;
		} else {
	/* If no coord msgs, check the paths */
			char	relpath[200];
			if (view->path && strcmp(view->path,"none") != 0 &&
				(int)strlen(view->path) > 0) {
	/* Set some flags */
				view->viewflags &= ~XO_VIEW_USE_MSGS;
				/* clearing old values */
				view->viewflags &= 
					~(XO_VIEW_USE_MSGOUT | XO_VIEW_USE_MSGIN |
					  XO_VIEW_USE_MSGSRC | XO_VIEW_USE_MSGDEST |
					  XO_VIEW_USE_FIELD1 | XO_VIEW_USE_FIELD2);
				if (view->field && (int)strlen(view->field) > 0) {
					/* setting new values */
					if (strcmp(view->field,"msgout") == 0)
						view->viewflags |= XO_VIEW_USE_MSGOUT;
					else if (strcmp(view->field,"msgin") == 0)
						view->viewflags |= XO_VIEW_USE_MSGIN;
					else 
						view->viewflags |= XO_VIEW_USE_FIELD1;
				}
				if (view->field2 && (int)strlen(view->field2) > 0) {
					view->viewflags |= XO_VIEW_USE_FIELD2;
				}

	/* Set up the elists */
				if (view->coord_elist)
					FreeElementList(view->coord_elist);
				view->coord_elist = WildcardGetElement(view->path,0);
				if (view->coord_elist->nelements==0) {
					printf("Warning: No elements found on path %s\n",
						view->path);
				if (view->path[0] != '/') 
					printf ("Note: Use absolute path names for the path field \n");
					view->ncoords = 0;
				} else {
					view->ncoords = view->coord_elist->nelements;
					if (view->relpath &&
						(int)strlen(view->relpath) > 0) {
						if (view->relpath[0] == '/')
						/* relpath is absolute */
							strcpy(relpath,view->relpath);
						else /* join path and relpath */
							sprintf(relpath,"%s/%s",
								view->path,view->relpath);
					} else { /* just use path */
							strcpy(relpath,view->path);
					}
					if (view->value_elist)
						FreeElementList(view->value_elist);
					view->value_elist = WildcardGetElement(relpath,0);
					if (view->value_elist->nelements==0) {
					printf("Warning: No elements found on relpath %s\n",
							relpath);
					}
					if (view->msgpath &&
						(int)strlen(view->msgpath) > 0) {
						if (view->msg_elist)
							FreeElementList(view->msg_elist);
						view->msg_elist =
							WildcardGetElement(view->msgpath,0);
						if (view->msg_elist->nelements==0) {
						printf("Warning: No elements found on msgpath %s\n",
								view->msgpath);
						}
						if (strcmp(view->field,"msgout") == 0)
							view->viewflags |= XO_VIEW_USE_MSGDEST;
						else if (strcmp(view->field,"msgin") == 0)
							view->viewflags |= XO_VIEW_USE_MSGSRC;
					}
				}
			}
		}

	/* Handling memory allocation */
		if (view->ncoords > view->max_ncoords) {
		/* Realloc the coords table*/
			if (view->max_ncoords > 0) {
				if (view->xpts && view->xpts->table &&
					view->xpts->allocated && view->xpts->xdivs > 0){
					free(view->xpts->table);
					view->xpts->table = NULL;
				}	
				if (view->ypts && view->ypts->table &&
					view->ypts->allocated && view->ypts->xdivs > 0){
					free(view->ypts->table);
					view->ypts->table = NULL;
				}
				if (view->zpts && view->zpts->table &&
					view->zpts->allocated && view->zpts->xdivs > 0){
					free(view->zpts->table);
					view->zpts->table = NULL;
				}
				for(i = 0; i < XoVarNValues; i++)
					free(view->values[i]);
			}

			if(alloc_interpoltable(view->xpts, view->ncoords-1)==0){
			/** Treat memory allocation probs here **/
			}
			if(alloc_interpoltable(view->ypts, view->ncoords-1)==0){
			/** Treat memory allocation probs here **/
			}
			if(alloc_interpoltable(view->zpts, view->ncoords-1)==0){
			/** Treat memory allocation probs here **/
			}
			
			for(i = 0; i < XoVarNValues; i++)
				view->values[i] =
					(float *)calloc(view->ncoords,sizeof(float));
			view->max_ncoords = view->ncoords;
		}

	/* If the view is monitoring msgs, use coords from msgs */
		if (view->viewflags & XO_VIEW_USE_MSGS) {
			ncoords = 0;
			MSGLOOP(view,msg) {
				case 5: /* COORDS */
					view->xpts->table[ncoords] = MSGVALUE(msg,0);
					view->ypts->table[ncoords] = MSGVALUE(msg,1);
					view->zpts->table[ncoords] = MSGVALUE(msg,2);
					ncoords++;
				break;
			        case 11: /* ICOORDS */
				        nc = MSGVALUE(msg,0);
				        view->xpts->table[nc] = MSGVALUE(msg,1);
				        view->ypts->table[nc] = MSGVALUE(msg,2);
				        view->zpts->table[nc] = MSGVALUE(msg,3);
					if (nc >= ncoords)
					  ncoords = nc + 1;
				break;
			}
		} else if (view->path && strcmp(view->path, "none") !=0) {
	/* use coords from the elist */ 
			for(i = 0; i < view->ncoords; i++) {
				view->xpts->table[i] = view->coord_elist->element[i]->x;
				view->ypts->table[i] = view->coord_elist->element[i]->y;
				view->zpts->table[i] = view->coord_elist->element[i]->z;
			}
		}

	/* Setting all the values and tables */
		if (view->widget) 
			XoXtVaSetValues((Widget)view->widget,
				XtNvarShapes, (XtPointer)view->shapes,
				XtNnshapes, view->nshapes,
				XtNshapeflag, (int)1,
				XtNxpts,(XtPointer)view->xpts->table,
				XtNypts,(XtPointer)view->ypts->table,
				XtNzpts,(XtPointer)view->zpts->table,
				XtNncoords,view->ncoords,
				NULL);
		XoProjectView((Widget)view->widget);
	}
    break;
  case CREATE:
	/* Must always create two child shapes for the default cases */
	/* If special cases are needed then they can be removed */
    /* arguments are: object_type name [field value] ... */
	if ((parentW = xoFindParentDraw(view)) == NULL) return(0);

	/* assign memory for the value field */
	view->value = (char *)calloc(200,sizeof(char));
	view->valuemode = "path";

	/* Make the arrays for the coords */
	view->xpts = create_interpol(XO_VIEW_NCOORDS_INCREMENT, (float)0.0,(float)1.0);
	view->ypts = create_interpol(XO_VIEW_NCOORDS_INCREMENT, (float)0.0,(float)1.0);
	view->zpts = create_interpol(XO_VIEW_NCOORDS_INCREMENT, (float)0.0,(float)1.0);
	/* Make the array for the values */
	for(i = 0; i < XoVarNValues; i++)
		view->values[i] =
			(float *)calloc(XO_VIEW_NCOORDS_INCREMENT,sizeof(float));
	view->max_ncoords =  XO_VIEW_NCOORDS_INCREMENT;
	/* Make the array for the values */
	/* set up some defaults */
	view->ncoords = 0;
	view->autolimits = 1;
	view->sizescale = 1.0;
	view->viewmode = "colorboxview";
	view->color_val = 1;
	view->morph_val = 1;
	for(i = 0; i < XoVarNValues; i++) {
		view->value_min[i] = 0;
		view->value_max[i] = 1;
	}

	vW = (ViewObject)XtCreateManagedWidget(
		av[1], viewObjectClass, parentW, NULL,0);
    view->widget = (char *)vW;
	/* copying over the table pointers and defaults */
	XoXtVaSetValues(vW,
		XtNcolorscale, (XtPointer)XoColorscale(),
		XtNncols,(int)XoNcolors(),
		XtNviewValues,(XtPointer)view->values,
		XtNviewColorVal,(int)1,
		XtNviewMorphVal,(int)1,
		NULL);
	XtAddCallback((Widget) view->widget, XtNcallback,xoCallbackFn,
		(XtPointer)view);

	newaction.type = CREATE;
	newaction.name = "CREATE";
	newaction.data = (char *)view;
	newaction.argc = 12;
	newaction.argv = newargv;
	newaction.argv[0] = "xshape";
	newaction.argv[1] = "shape[0]";
	newaction.argv[2] = "-fg";
	newaction.argv[3] = "1";
	newaction.argv[4] = "-coords";
	newaction.argv[5] = "[-0.01,-0.01,0][-0.01,0.01,0][0.01,0.01,0][0.01,-0.01,0]";
	newaction.argv[6] = "-drawmode";
	newaction.argv[7] = "FillPoly";
	newaction.argv[8] = "-pixflags";
	newaction.argv[9] = "v";
	newaction.argv[10] = "-pixflags";
	newaction.argv[11] = "c";
	Create("xshape","shape",view,&newaction,0);

	newaction.argv[1] = "shape[1]";
	newaction.argv[3] = "63";
	newaction.argv[5] = "[-0.5,-0.5,0][-0.5,0.5,0][0.5,0.5,0][0.5,-0.5,0]";
	Create("xshape","shape",view,&newaction,1);

    ac--, av++;			/* object type */
    ac--, av++;			/* path */
	xoParseCreateArgs(view,ac,av);

	return(1);
	/* NOTREACHED */
	break;
  case COPY:
	/* Note that the 'view' pointer refers to the original,
	** and the data contains the pointer to the copy */
	copy_view = (struct xview_type *)action->data;
	if ((parentW = xoFindParentDraw(copy_view)) == NULL) return(0);

	/* We need to duplicate the shapes */
	/* we need to duplicate the value */
	/* creating a new widget */
		vW = (ViewObject)XtCreateManagedWidget(
			copy_view->name, viewObjectClass, parentW, NULL,0);
    	copy_view->widget = (char *)vW;
	/* copying over the table pointers */
/*
		XoXtVaSetValues(vW,
			XtNxpts,(XtPointer)((Interpol *)copy_view->xpts)->table,
			XtNypts,(XtPointer)((Interpol *)copy_view->ypts)->table,
			XtNzpts,(XtPointer)((Interpol *)copy_view->zpts)->table,
			XtNpixflags,copy_view->pixflags,
			NULL);
*/
		
	/* adding the callback */
		XtAddCallback((Widget) copy_view->widget, XtNcallback,xoCallbackFn,
			(XtPointer)copy_view);
	/* Converting the rest of the values */
		gx_convert_all(copy_view,GXconvert,XtNumber(GXconvert));
	return(1);
	/* NOTREACHED */
	break;
  case SET:
	if (xoSetPixFlags(view,ac,av)) return(1);
    if (ac) {			/* need to set fields */
    	/* Special case: setting the coord interpols xpts,ypts,zpts */
    	if (SetTable(view,2,action->argv,action->data,
    		"xpts ypts zpts")) {
				XoProjectView((Widget)view->widget);
    			return(1);
		}
		/* Special case : setting the shape array. At this
		** point there is no standard method in Genesis for
		** doing this, but we will have to work it out. */
		if (strncmp(av[0],"shape[",6) == 0) {
			i = atoi(av[0] + 6);
			return(1);
		}
		/* a useful function for messing with the display size */
		if (strcmp(av[0],"sizescale") == 0) {
			for (i = 0; i < XO_VIEW_MAXSHAPES; i++) {
				xshape = (struct xshape_type *)
					GetChildElement(view,"shape",i);
				if (!xshape) break;
				scale_table(xshape->xpts,"sy",av[1]);
				scale_table(xshape->ypts,"sy",av[1]);
				scale_table(xshape->zpts,"sy",av[1]);
			}
			if (i > 0) {
				XoProjectView((Widget)view->widget);
				return(0);
			}
		}
		if (strcmp(av[0],"viewflags") == 0 ||
			strcmp(av[0],"allocated_pts") == 0 ||
			strcmp(av[0],"max_ncoords") == 0 ||
			strcmp(av[0],"ncoords") == 0 ||
			strcmp(av[0],"xpts") == 0 ||
			strcmp(av[0],"ypts") == 0 ||
			strcmp(av[0],"zpts") == 0 ||
			strcmp(av[0],"shapes") == 0 ||
			strcmp(av[0],"nshapes") == 0 ||
			strcmp(av[0],"coord_elist") == 0 ||
			strcmp(av[0],"value_elist") == 0 ||
			strcmp(av[0],"value") == 0 ||
			strcmp(av[0],"msg_elist") == 0
			) {
			printf("Warning: %s is a private field\n",av[0]);
			return(1);
		}

		/* Check if the value_min, value_max or values arrays have
		** been set. These cases require updates */
		if (strncmp(av[0],"value",5) == 0) {
			DirectSetElement(view,av[0],av[1]);
			for(i = 0; i < view->nshapes; i++) {
				if (view->autolimits && view->nshapes > 1) {
					for(j= 0; j < XoVarNValues; j++)
						view->shapes[i].value_limit[j] =
							view->value_min[j] +
							(double)i/(double)(view->nshapes-1) * 
							(view->value_max[j] - view->value_min[j]);
				}
			}
			XoUpdateView((Widget)view->widget);
			return(1);
		}

		/* Do special things here for backwards compatibility */
		if (strcmp(av[0],"viewmode") == 0) {
			static Gen2Xo SXConvert[] = {
				{"npts",	XtNnpts},
				{"drawmode",XtNdrawmode},
			};
			char *targv[5];
			char *valargv[20];

			targv[0] = "npts";
			targv[2] = "drawmode";
			valargv[0] = "morph_val";
			valargv[1] = "0";
			valargv[2] = "text_val";
			valargv[3] = "0";
			valargv[4] = "color_val";
			valargv[5] = "0";
			valargv[6] = "xoffset_val";
			valargv[7] = "0";
			valargv[8] = "yoffset_val";
			valargv[9] = "0";
			valargv[10] = "zoffset_val";
			valargv[11] = "0";
			valargv[12] = "textcolor_val";
			valargv[13] = "0";
			valargv[14] = "linethickness_val";
			valargv[15] = "0";
			
			if (strcmp(av[1],"shape") == 0) {
			}
			if (strcmp(av[1],"colorview") == 0) {
				valargv[5] = "1";
					/* set the coords here to assign the box size */
					if ((xshape = (struct xshape_type *)
						GetChildElement(view,"shape",0))) {
						if (xshape->xpts && xshape->xpts->xdivs > 3) {
							xshape->xpts->table[0] = -0.5;
							xshape->xpts->table[1] = -0.5;
							xshape->xpts->table[2] = 0.5;
							xshape->xpts->table[3] = 0.5;
							xshape->ypts->table[0] = -0.5;
							xshape->ypts->table[1] = 0.5;
							xshape->ypts->table[2] = 0.5;
							xshape->ypts->table[3] = -0.5;
							xshape->zpts->table[0] = 0;
							xshape->zpts->table[1] = 0;
							xshape->zpts->table[2] = 0;
							xshape->zpts->table[3] = 0;
							xshape->npts = 4;
							xshape->drawmode = "FillPoly";
							targv[1] = "4";
							targv[3] = "FillPoly";
							gx_convert(xshape,SXConvert,2,4,targv);
						}
					}
					if ((xshape = (struct xshape_type *)
						GetChildElement(view,"shape",1))) {
						if (xshape->xpts && xshape->xpts->xdivs > 3) {
							xshape->xpts->table[0] = -0.5;
							xshape->xpts->table[1] = -0.5;
							xshape->xpts->table[2] = 0.5;
							xshape->xpts->table[3] = 0.5;
							xshape->ypts->table[0] = -0.5;
							xshape->ypts->table[1] = 0.5;
							xshape->ypts->table[2] = 0.5;
							xshape->ypts->table[3] = -0.5;
							xshape->zpts->table[0] = 0;
							xshape->zpts->table[1] = 0;
							xshape->zpts->table[2] = 0;
							xshape->zpts->table[3] = 0;
							xshape->npts = 4;
							xshape->drawmode = "FillPoly";
							targv[1] = "4";
							targv[3] = "FillPoly";
							gx_convert(xshape,SXConvert,2,4,targv);
						}
					}
			}
			if (strcmp(av[1],"boxview") == 0) {
				valargv[1] = "1";
				targv[3] = "DrawLines";
			}
			if (strcmp(av[1],"fillboxview") == 0) {
				valargv[1] = "1";
				targv[3] = "FillPoly";
			}
			if (strcmp(av[1],"colorboxview") == 0) {
				valargv[1] = "1";
				valargv[5] = "1";
				targv[3] = "FillPoly";
			}
			if (strcmp(av[1],"boxview") == 0 ||
				strcmp(av[1],"fillboxview") == 0 ||
				strcmp(av[1],"colorboxview") == 0) {
					if ((xshape = (struct xshape_type *)
						GetChildElement(view,"shape",0))) {
						if (xshape->xpts && xshape->xpts->xdivs > 4) {
							xshape->xpts->table[0] = -0.01;
							xshape->xpts->table[1] = -0.01;
							xshape->xpts->table[2] = 0.01;
							xshape->xpts->table[3] = 0.01;
							xshape->xpts->table[4] = -0.01;
							xshape->ypts->table[0] = -0.01;
							xshape->ypts->table[1] = 0.01;
							xshape->ypts->table[2] = 0.01;
							xshape->ypts->table[3] = -0.01;
							xshape->ypts->table[4] = -0.01;
							xshape->zpts->table[0] = 0;
							xshape->zpts->table[1] = 0;
							xshape->zpts->table[2] = 0;
							xshape->zpts->table[3] = 0;
							xshape->zpts->table[4] = 0;
							xshape->npts = 5;
							targv[1] = "5";
							gx_convert(xshape,SXConvert,2,4,targv);
						}
					}
					if ((xshape = (struct xshape_type *)
						GetChildElement(view,"shape",1))) {
						if (xshape->xpts && xshape->xpts->xdivs > 4) {
							xshape->xpts->table[0] = -0.5;
							xshape->xpts->table[1] = -0.5;
							xshape->xpts->table[2] = 0.5;
							xshape->xpts->table[3] = 0.5;
							xshape->xpts->table[4] = -0.5;
							xshape->ypts->table[0] = -0.5;
							xshape->ypts->table[1] = 0.5;
							xshape->ypts->table[2] = 0.5;
							xshape->ypts->table[3] = -0.5;
							xshape->ypts->table[4] = -0.5;
							xshape->zpts->table[0] = 0;
							xshape->zpts->table[1] = 0;
							xshape->zpts->table[2] = 0;
							xshape->zpts->table[3] = 0;
							xshape->zpts->table[4] = 0;
							targv[1] = "5";
							xshape->npts = 5;
							gx_convert(xshape,SXConvert,2,4,targv);
						}
					}
			}
			gx_convert(view,GXconvert,XtNumber(GXconvert),16,valargv);
		}
		gx_convert(view,GXconvert,XtNumber(GXconvert),ac,av);
		XoUpdateView((Widget)view->widget);
		xoUpdateParentDraw(view);
		return(0);
	}
	break;
  case SHOW:
  		if (ShowInterpol(action,"xpts",view->xpts,1,50)) return(1);
  		if (ShowInterpol(action,"ypts",view->ypts,1,50)) return(1);
  		return (ShowInterpol(action,"zpts",view->zpts,1,50));
		/* NOTREACHED */
  	break;
  case DELETE:
	if (view->value) free(view->value);
	if (view->coord_elist) FreeElementList(view->coord_elist);
	if (view->value_elist) FreeElementList(view->value_elist);
	if (view->msg_elist) FreeElementList(view->msg_elist);
	if (view->max_ncoords > 0) {
		int i;
		if (view->xpts && view->xpts->table &&
			view->xpts->allocated && view->xpts->xdivs > 0)
			free(view->xpts->table);
		free(view->xpts);
		if (view->ypts && view->ypts->table &&
			view->ypts->allocated && view->ypts->xdivs > 0)
			free(view->ypts->table);
		free(view->ypts);
		if (view->zpts && view->zpts->table &&
			view->zpts->allocated && view->zpts->xdivs > 0)
			free(view->zpts->table);
		free(view->zpts);
		for(i=0;i<XoVarNValues;i++)
			free(view->values[i]);
	}
	/* get rid of shapes here */
	if (view->shapes && view->nshapes > 0)
		free(view->shapes);
	if (!(view->widget)) return(0);
	XtDestroyWidget((Widget)view->widget);
	break;
  case XUPDATE : /* update view fields due to changes in widget */
		if(ac > 0)
		xg_convert(view,GXconvert,XtNumber(GXconvert),ac,av);
	break;
  default:
	i = -1;
	if (action->data)
		i = *((int *)(((XoEventInfo *)(action->data))->ret));
	if (view->ncoords > 0 && i >= 0 && i < view->ncoords) {
		if (strcmp(view->valuemode,"path") == 0) {
			if (view->path && view->coord_elist && 
				strcmp(view->path,"none") != 0 &&
				i < view->coord_elist->nelements) {
					strcpy(view->value,
						Pathname(view->coord_elist->element[i]));
			} else { /* look through the msgs to find the path */
				MSGLOOP(view,msg) {
					case 5: /* COORDS */
						if (j == i) {
							strcpy(view->value,Pathname(msg->src));
							break;
						}
						j++;
						break;
				        case 11: /* ICOORDS */
					        nc = MSGVALUE(msg,0);
						if (nc == i) {
						        strcpy(view->value,Pathname(msg->src));	
							break;
						}
						break;
				}
			}
		} else if (strcmp(view->valuemode,"index") == 0) {
			sprintf(view->value,"%d",i);
		} else if (strcmp(view->valuemode,"lookup") == 0) {
			if (view->values)
				sprintf(view->value,"%f",view->values[0][i]);
		}
	}
	xoExecuteFunction(view,action,view->script,view->value);
	break;
  }
  return 0;
}

#undef XO_VIEW_MAXSHAPES /* 50 */

static int IsOnList(elm,elist)
	Element	*elm;
	ElementList *elist;
{
	int i;
	for(i = 0; i < elist->nelements; i++) {
		if (elm == elist->element[i]) 
			return(1);
	}
	return(0);
}

/* this function should really be in the sim directory */
static int ElistGetValues(values,values2,
	coord_elist,value_elist,msg_elist,
	field,field2,msgtype,slotno,flags)
	float	*values;	/* assumed to be same length as elist */
	float	*values2;	/* assumed to be same length as elist */
	ElementList *coord_elist;
	ElementList *value_elist;
	ElementList *msg_elist;
	char	*field;
	char	*field2;
	char	*msgtype;
	int		slotno;
	int		flags;
{
	int i;
	int ncoords;
	Element	*elm;
	double	value;

	if (!coord_elist)
		return(0);

	ncoords = coord_elist->nelements; 

	if (ncoords <= 0)
		return(0);

	if (flags & XO_VIEW_USE_FIELD1) {
		char *addr;
		Info	info;
	/* use the specified fields from value_elist */
		for(i = 0; i < ncoords && i < value_elist->nelements; i++) {
			elm = value_elist->element[i];
			addr = CalculateAddress(elm,Type(elm),field,&info);
			if (!addr) {
				values[i] = 0;
			} else if (strcmp(info.type,"double") == 0) {
				values[i] = *((double *)addr);
			} else if (strcmp(info.type,"float") == 0) {
				values[i] = *((float *)addr);
			} else if (strcmp(info.type,"int") == 0) {
				values[i] = *((int *)addr);
			} else if (strcmp(info.type,"short") == 0) {
				values[i] = *((short *)addr);
			} else {
				values[i] = 0;
			}
		}
	}

	if (flags & XO_VIEW_USE_FIELD2) {
		char *addr;
		Info	info;
	/* use the specified fields from value_elist */
		for(i = 0; i < ncoords && i < value_elist->nelements; i++) {
			elm = value_elist->element[i];
			addr = CalculateAddress(elm,Type(elm),field2,&info);
			if (!addr) {
				values2[i] = 0;
			} else if (strcmp(info.type,"double") == 0) {
				values2[i] = *((double *)addr);
			} else if (strcmp(info.type,"float") == 0) {
				values2[i] = *((float *)addr);
			} else if (strcmp(info.type,"int") == 0) {
				values2[i] = *((int *)addr);
			} else if (strcmp(info.type,"short") == 0) {
				values2[i] = *((short *)addr);
			} else {
				values2[i] = 0;
			}
		}
	}

	if (flags & XO_VIEW_USE_MSGIN) {
		/* use the specified msgno slot from msgins of the
		** specified type arriving at all the elms on the
		** relpath 
		*/
		GenesisObject *lastobject = NULL;

		for(i = 0;
			i < ncoords && i < value_elist->nelements;
			i++) {
			elm = value_elist->element[i];
			values[i] = 0;
			if (GetMsgValueByMsgtype(elm,1,flags,msgtype,slotno,
				&lastobject,msg_elist,&value))
				values[i] = value;
		}
	}

	if (flags & XO_VIEW_USE_MSGOUT) {
		/* use the specified msgno slot from msgouts of the
		** specified type sent out from all the elms on the
		** relpath 
		*/
		GenesisObject *lastobject = NULL;
		for(i = 0;
			i < ncoords && i < value_elist->nelements;
			i++) {
			elm = value_elist->element[i];
			values[i] = 0;
			if (GetMsgValueByMsgtype(elm,0,flags,msgtype,slotno,
				&lastobject,msg_elist,&value))
				values[i] = value;
		}
	}
}

/*
** This is another function that should be in sim since it
** provides a general mechanism for getting message values given
** the element and the messagetype and slotno.
*/

static int GetMsgValueByMsgtype(elm,is_msgin,flags,msgtype,slotno,
	lastobject,msg_elist,value)
	Element	*elm;
	int		is_msgin;
	int		flags;
	char	*msgtype;
	int		slotno;
	GenesisObject	**lastobject;
	ElementList	*msg_elist;
	double	*value;
{

	MsgList *msgspec = NULL;
	MsgList	*GetMsgListByName();
	int		mtype = 0;

	if (is_msgin) {
		int		msgcount;
		MsgIn	*msg;
		msg = elm->msgin;
		flags &= XO_VIEW_USE_MSGSRC;
		/* If the flags are true, then we need to check whether
		** the msg->src is on the msg_elist. Otherwise we just take
		** the first value we find.
		*/

		if (elm->object != *lastobject) {
			*lastobject = elm->object;
			msgspec = GetMsgListByName(*lastobject,msgtype);
			if (!msgspec) {
				return(0);
			}
			mtype = msgspec->type;
		}

		for(msgcount = 0; msgcount < elm->nmsgin; msgcount++) {
			if (msg->type == mtype &&
				(!flags || IsOnList(msg->src,msg_elist))) {
				*value = MSGVALUE(msg,slotno);
				return(1); /* Success! Skip back to the elm loop */
			}
			msg = MSGINNEXT(msg);
		}
		return(0);
	} else {
		MsgIn *msgin;
		MsgOut	*msgout;
		flags &= XO_VIEW_USE_MSGDEST;
		/* If the flags are true, then we need to check whether
		** the msgout->dest is on the msg_elist. Otherwise we just take
		** the first value we find.
		*/

		MSGOUTLOOP(elm,msgout) {
			if (msgout->dst->object != *lastobject) {
				*lastobject = msgout->dst->object;
				msgspec = GetMsgListByName(*lastobject,msgtype);
			}
			if (msgspec) {
				mtype = msgspec->type;
				msgin = GetMsgInByMsgOut(msgout->dst,msgout);
				if(mtype == msgin->type && 
					(!flags || IsOnList(msgout->dst,msg_elist))){
					*value = MSGVALUE(msgin,slotno);
					return(1); /* Success ! */
				}
			}
		}
		return(0);
	}
}
