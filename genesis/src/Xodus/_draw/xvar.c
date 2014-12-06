/* $Id: xvar.c,v 1.2 2005/07/01 10:02:59 svitak Exp $ */
/*
 * $Log: xvar.c,v $
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
 * Revision 1.10  2001/06/29 21:12:14  mhucka
 * Added extra parens inside conditionals and { } groupings to quiet certain
 * compiler warnings.
 *
 * Revision 1.9  2001/04/18 22:39:35  mhucka
 * Miscellaneous portability fixes, mainly for SGI IRIX.
 *
 * Revision 1.8  2000/06/12 04:21:09  mhucka
 * 1) Removed nested comments in RCS/CVS log lines, to quiet down the
 *    IRIX cc compiler.
 * 2) Added NOTREACHED comments where appropriate.
 *
 * Revision 1.7  2000/05/02 06:18:32  mhucka
 * Added type casts for certain function call arguments.
 *
 * Revision 1.6  1995/09/27 00:09:36  venkat
 * XtVaSetValues()-calls-changed-to-XoXtVaSetValues()-to-avoid-alpha-FPE's
 *
 * Revision 1.5  1995/03/28  21:12:01  venkat
 * COPY
 *
 * Revision 1.4  1994/06/13  22:58:13  bhalla
 * Bugfix: moved xoParseCreateArgs after creation of child widgets
 * Added pixflags c for child widgets
 * Added updating of value_min etc in SET action
 * Added backwards compat viewmodes
 *
 * Revision 1.3  1994/02/08  22:33:37  bhalla
 * ci bug - I made no changes here
 *
 * Revision 1.2  1994/02/02  20:04:53  bhalla
 * Eliminated soft actions
 *
 * Revision 1.2  1994/02/02  20:04:53  bhalla
 * Eliminated soft actions
 * */
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include "Widg/Framed.h"
#include "Draw/Pix.h"
#include "Draw/Shape.h"
#include "Draw/Var.h"
#include "draw_ext.h"
#include "_widg/xform_struct.h"
#include "Xo/Xo_ext.h"

#define XO_VAR_MAXSHAPES 50


static Gen2Xo GXconvert[] = {
	{"tx",		XtNpixX},
	{"ty",		XtNpixY},
	{"tz",		XtNpixZ},
	{"varmode",		XtNvarmode},
	{"viewmode",	XtNvarmode},
	{"color_val",	XtNvarColorVal},
	{"morph_val",	XtNvarMorphVal},
	{"xoffset_val",	XtNvarXOffsetVal},
	{"yoffset_val",	XtNvarYOffsetVal},
	{"zoffset_val",	XtNvarZOffsetVal},
	{"text_val",	XtNvarTextVal},
	{"textcolor_val",	XtNvarTextColorVal},
	{"linethickness_val",	XtNvarLineThicknessVal},
};

int XVar (var, action)
     register struct xvar_type *var;
     Action                     *action;
{
	int ac=action->argc;
	char** av=action->argv;
    Widget parentW, xoFindParentDraw();
	VarObject vW;
	int 	i,j;
	struct 	xvar_type *copy_var;
	MsgIn	*msg;
	Element	*elms[XO_VAR_MAXSHAPES];
	struct	xshape_type	*xshape;
	Action	newaction;
	char	*newargv[15];
  
  if (Debug(0) > 1)
    ActionHeader("XVar", var, action);

  SELECT_ACTION (action) {
  case INIT:
    break;
  case PROCESS:
	MSGLOOP(var,msg) {
		case 0: /* V1 */
			var->values[0] = MSGVALUE(msg,0);
		break;
		case 1: /* V2 */
			var->values[1] = MSGVALUE(msg,0);
		break;
		case 2: /* V3 */
			var->values[2] = MSGVALUE(msg,0);
		break;
		case 3: /* V4 */
			var->values[3] = MSGVALUE(msg,0);
		break;
		case 4: /* V5 */
			var->values[4] = MSGVALUE(msg,0);
		break;
	}
	XoUpdateVar((Widget)var->widget);
    break;
  case RESET: /* This case is most important: it scans the list
	of children to find the applicable shapes, and fills in
	the lists */
	for (i = 0; i < XO_VAR_MAXSHAPES; i++) {
		elms[i] = GetChildElement(var,"shape",i);
		if (!elms[i]) break;
	}
	if (i > 0) {
		if (i != var->nshapes) {
			if (var->nshapes > 0)
				free(var->shapes);
			var->shapes = (varShape *)calloc(i,sizeof(varShape));
			var->nshapes = i;
		}
		for(i = 0; i < var->nshapes; i++) {
			xshape = (struct xshape_type *)elms[i];
			var->shapes[i].shape = xshape->widget;
			var->shapes[i].fgindex = atoi(xshape->fg);
			var->shapes[i].textcolorindex = atoi(xshape->textcolor);
			if (var->autolimits && var->nshapes > 1) {
				for(j= 0; j < XoVarNValues; j++)
					var->shapes[i].value_limit[j] =
						var->value_min[j] +
						(double)i/(double)(var->nshapes-1) * 
						(var->value_max[j] - var->value_min[j]);
			}
		}
		if (var->widget) 
			XoXtVaSetValues((Widget)var->widget,
				XtNvarShapes, (XtPointer)var->shapes,
				XtNnshapes, var->nshapes,
				XtNshapeflag, (int)1,
				NULL);
		XoProjectVar((Widget)var->widget);
	}
    break;
  case CREATE:
	/* Must always create two child shapes for the default cases */
	/* If special cases are needed then they can be removed */
    /* arguments are: object_type name [field value] ... */
	if ((parentW = xoFindParentDraw(var)) == NULL) return(0);

	/* set up some defaults */
	var->autolimits = 1;
	var->sizescale = 1.0;
	var->varmode = "colorboxview";
	var->color_val = 1;
	var->morph_val = 1;
	for(i = 0; i < XoVarNValues; i++) {
		var->values[i] = var->value_min[i] = 0;
		var->value_max[i] = 1;
	}

	vW = (VarObject)XtCreateManagedWidget(
		av[1], varObjectClass, parentW, NULL,0);
    var->widget = (char *)vW;
	/* copying over the table pointers and defaults */
	XoXtVaSetValues(vW,
		XtNcolorscale, (XtPointer)XoColorscale(),
		XtNncols,(int)XoNcolors(),
		XtNvarValues,(XtPointer)var->values,
		XtNvarColorVal,(int)1,
		XtNvarMorphVal,(int)1,
		NULL);
	XtAddCallback((Widget) var->widget, XtNcallback,xoCallbackFn,
		(XtPointer)var);

	newaction.type = CREATE;
	newaction.name = "CREATE";
	newaction.data = (char *)var;
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
	Create("xshape","shape",var,&newaction,0);

	newaction.argv[1] = "shape[1]";
	newaction.argv[3] = "63";
	newaction.argv[5] = "[-0.5,-0.5,0][-0.5,0.5,0][0.5,0.5,0][0.5,-0.5,0]";
	Create("xshape","shape",var,&newaction,1);

    ac--, av++;			/* object type */
    ac--, av++;			/* path */
	xoParseCreateArgs(var,ac,av);
	return(1);
	/* NOTREACHED */
	break;
  case COPY:
	/* Note that the 'var' pointer refers to the original,
	** and the data contains the pointer to the copy */
	copy_var = (struct xvar_type *)action->data;
	if ((parentW = xoFindParentDraw(copy_var)) == NULL) return(0);

	/* We need to duplicate the shapes */
	/* we need to duplicate the value */
	/* creating a new widget */
		vW = (VarObject)XtCreateManagedWidget(
			copy_var->name, varObjectClass, parentW, NULL,0);
    	copy_var->widget = (char *)vW;
	/* copying over the table pointers */
/*
		XoXtVaSetValues(vW,
			XtNxpts,(XtPointer)((Interpol *)copy_var->xpts)->table,
			XtNypts,(XtPointer)((Interpol *)copy_var->ypts)->table,
			XtNzpts,(XtPointer)((Interpol *)copy_var->zpts)->table,
			XtNpixflags,copy_var->pixflags,
			NULL);
*/
		
	/* adding the callback */
		XtAddCallback((Widget) copy_var->widget, XtNcallback,xoCallbackFn,
			(XtPointer)copy_var);
	/* Converting the rest of the values */
		gx_convert_all(copy_var,GXconvert,XtNumber(GXconvert));
	return(1);
	/* NOTREACHED */
	break;
  case SET:
	if (xoSetPixFlags(var,ac,av)) return(1);
    if (ac) {			/* need to set fields */
		/* Special case : setting the shape array. At this
		** point there is no standard method in Genesis for
		** doing this, but we will have to work it out. */
		if (strncmp(av[0],"shape[",6) == 0) {
			i = atoi(av[0] + 6);
			return(1);
		}
		/* a useful function for messing with the display size */
		if (strcmp(av[0],"sizescale") == 0) {
			for (i = 0; i < XO_VAR_MAXSHAPES; i++) {
				xshape = (struct xshape_type *)
					GetChildElement(var,"shape",i);
				if (!xshape) break;
				scale_table(xshape->xpts,"sy",av[1]);
				scale_table(xshape->ypts,"sy",av[1]);
				scale_table(xshape->zpts,"sy",av[1]);
			}
			if (i > 0) {
				XoProjectVar((Widget)var->widget);
				return(0);
			}
		}
        /* Check if the value_min, value_max or values arrays have
        ** been set. These cases require updates */
        if (strncmp(av[0],"value",5) == 0) {
            DirectSetElement(var,av[0],av[1]);
            for(i = 0; i < var->nshapes; i++) {
                if (var->autolimits && var->nshapes > 1) {
                    for(j= 0; j < XoVarNValues; j++)
                        var->shapes[i].value_limit[j] =
                            var->value_min[j] +
                            (double)i/(double)(var->nshapes-1) * 
                            (var->value_max[j] - var->value_min[j]);
                }
            }
            XoUpdateVar((Widget)var->widget);
            return(1);
        }

		/* Do special things here for backwards compatibility */
		if (strcmp(av[0],"varmode") == 0 || 
			strcmp(av[0],"viewmode") == 0) {
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
						GetChildElement(var,"shape",0))) {
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
						GetChildElement(var,"shape",1))) {
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
						GetChildElement(var,"shape",0))) {
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
						GetChildElement(var,"shape",1))) {
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
			gx_convert(var,GXconvert,XtNumber(GXconvert),16,valargv);
		}
		gx_convert(var,GXconvert,XtNumber(GXconvert),ac,av);
		XoUpdateVar((Widget)var->widget);
		xoUpdateParentDraw(var);
		return(0);
	}
	break;
  case DELETE:
	if (var->shapes && var->nshapes > 0)
		free(var->shapes);
	if (!(var->widget)) return(0);
	XtDestroyWidget((Widget)var->widget);
	/* get rid of shapes here */
	break;
  case XUPDATE : /* update var fields due to changes in widget */
		xg_convert(var,GXconvert,XtNumber(GXconvert),ac,av);
	break;
  default:
	xoExecuteFunction(var,action,var->script,var->value);
	break;
  }
  return 0;
}
#undef XO_VAR_MAXSHAPES /* 50 */

