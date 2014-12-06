/* $Id: xpix.c,v 1.2 2005/07/01 10:02:58 svitak Exp $ */
/*
 * $Log: xpix.c,v $
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
 * Revision 1.10  2000/06/12 04:15:59  mhucka
 * 1) Removed nested comments in RCS/CVS log lines, to quiet down the
 *    IRIX cc compiler.
 * 2) Added NOTREACHED comments where appropriate.
 *
 * Revision 1.9  2000/05/02 06:18:31  mhucka
 * Added type casts for certain function call arguments.
 *
 * Revision 1.8  1997/07/16 21:33:09  dhb
 * Change getopt() calls to avoid conflict with unix version.
 *
 * Revision 1.7  1995/09/27 00:09:24  venkat
 * XtVaSetValues()-calls-changed-to-XoXtVaSetValues()-to-avoid-alpha-FPE's
 *
 * Revision 1.6  1995/08/03  01:01:39  venkat
 * Replaced the - operator with ~ to indicate negation for settable pixflags in the usage statement
 * and made use of genesis getopt library for validating command args and options
 *
 * Revision 1.5  1995/07/08  01:12:58  venkat
 * do_pixflags was returning a pointer to static storage. Changed to
 * correctly return an alloced string from CopyString()
 *
 * Revision 1.4  1994/03/16  16:58:00  bhalla
 * Added '~' option for flipping pixflags. This was needed because
 * the arg checking stuff caught the '-' option and got confused.
 *
 * Revision 1.3  1994/02/08  22:33:37  bhalla
 * ci bug - I made no changes here
 *
 * Revision 1.2  1994/02/02  20:04:53  bhalla
 * eliminated soft actions
 * shuffled around some code for pixflags, but no real change
 *
 * Revision 1.2  1994/02/02  20:04:53  bhalla
 * eliminated soft actions
 * shuffled around some code for pixflags, but no real change
 * */
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include "Widg/Framed.h"
#include "Draw/Pix.h"
#include "draw_ext.h"
#include "_widg/xform_struct.h"



static Gen2Xo GXconvert[] = {
	{"tx",		XtNpixX},
	{"ty",		XtNpixY},
	{"tz",		XtNpixZ},
	{"fg",		XtNforeground},
};

int xoSetPixFlags();
static int xoCvtFlag();

int XPix (pix, action)
     register struct xpix_type *pix;
     Action                     *action;
{
	int ac=action->argc;
	char** av=action->argv;
    Widget parentW,xoFindParentDraw();
	PixObject pW;
	int 	i;
  
  if (Debug(0) > 1)
    ActionHeader("XPix", pix, action);

  SELECT_ACTION (action) {
  case INIT:
    break;
  case PROCESS:
    break;
  case RESET:
    break;
  case CREATE:
    /* arguments are: object_type name [field value] ... */

	if ((parentW = xoFindParentDraw(pix)) == NULL) return(0);

	pW = (PixObject)XtCreateManagedWidget(
		av[1], pixObjectClass, parentW, NULL,0);
    pix->widget = (char *)pW;
	XtAddCallback((Widget) pix->widget, XtNcallback,xoCallbackFn,
		(XtPointer)pix);

    ac--, av++;			/* object type */
    ac--, av++;			/* path */
	xoParseCreateArgs(pix,ac,av);
	return(1);
	/* NOTREACHED */
	break;
  case SET:
	if (xoSetPixFlags(pix,ac,av)) return(1);
    if (ac) {			/* need to set fields */
		gx_convert(pix,GXconvert,XtNumber(GXconvert),ac,av);
		return(0);
	}
	break;
  case DELETE:
	if (!(pix->widget)) return(0);
	XtDestroyWidget((Widget)pix->widget);
	break;
  case XUPDATE : /* update pix fields due to changes in widget */
		if(ac > 0)
		xg_convert(pix,GXconvert,XtNumber(GXconvert),ac,av);
	break;
  default:
	xoExecuteFunction(pix,action,pix->script,pix->value);
  	break;
  }
  return 0;
}

 
/* Ideally we would have some general mechanism using hash tables
** for setting up flags like this */
int xoSetPixFlags(pix,ac,av)
     struct xpix_type *pix;
     int    ac;
     char   **av;
{
    if (ac != 2) return(0);
    if (strcmp(av[0],"pixflags") == 0) {
        if (av[1][0] == '-' || av[1][0] == '~') {
            pix->pixflags &= ~xoCvtFlag(av[1]+1);
        } else {
            pix->pixflags |= xoCvtFlag(av[1]);
        }
        XoXtVaSetValues((Widget)(pix->widget),
            XtNpixflags,pix->pixflags,
            NULL);
        return(1);
    }
    return(0);
}

 
static int xoCvtFlag(value)
     char   *value;
{
    switch(value[0]) {
        case 'v': return(XO_VISIBLE_NOT);
	    /* NOTREACHED */
            break;
        case 'c': return(XO_CLICKABLE_NOT);
	    /* NOTREACHED */
            break;
        case 'i':
            if (strncmp(value,"is_selected",strlen("is_s")) == 0)
                return(XO_IS_SELECTED);
            if (strncmp(value,"is_icon",strlen("is_i")) == 0)
                return(XO_IS_ICON);
            if (strncmp(value,"is_flat",strlen("is_f")) == 0)
                return(XO_IS_FLAT);
	    /* NOTREACHED */
            break;
        case 'u':
            if (strcmp(value,"update_all_on_set") == 0)
                return(XO_UPDATE_ALL_ON_SET);
            if (strncmp(value,"update_self_on_set",
                strlen("update_self_on_set")) == 0)
                return(XO_UPDATE_SELF_ON_SET_NOT);
            if (strcmp(value,"update_with_undraw") == 0)
                return(XO_UPDATE_WITH_UNDRAW);
            if (strcmp(value,"update_all_on_step") == 0)
                return(XO_UPDATE_ALL_ON_STEP);
            if (strncmp(value,"update_self_on_step",
                strlen("update_self_on_step")) == 0)
                return(XO_UPDATE_SELF_ON_STEP_NOT);
	    /* NOTREACHED */
            break;
        case 'm': return(XO_MOVE_SENSITIVE);
	    /* NOTREACHED */
            break;
        case 'r': return(XO_RESIZABLE_NOT);
	    /* NOTREACHED */
            break;
        case 'p':
            if (value[1] == 'c') /* pcoords */
                return(XO_PIXELCOORDS);
            if (value[1] == 'o') /* poffset */
                return(XO_PIXELOFFSET);
            if (value[1] == 'b') /* pboth */
                return(XO_PIXELOFFSET | XO_PIXELCOORDS);
	    /* NOTREACHED */
            break;
        case '%':
            if (value[1] == 'c') /* %coords */
                return(XO_PCTCOORDS);
            if (value[1] == 'o') /* %offset */
                return(XO_PCTOFFSET);
            if (value[1] == 'b') /* %both */
                return(XO_PCTOFFSET | XO_PCTCOORDS);
	    /* NOTREACHED */
            break;
        case 'h':
            if (strncmp(value,"hlt1",strlen("hlt1")) == 0)
                return(XO_HLT1_NOT);
            if (strcmp(value,"hlt2") == 0)
                return(XO_HLT2);
            if (strcmp(value,"hlt3") == 0)
                return(XO_HLT3);
	    /* NOTREACHED */
            break;
        case 'f': return(XO_FLUSH_NOT);
	    /* NOTREACHED */
            break;
        case 't': return(XO_TRANSLATE_NOT);
	    /* NOTREACHED */
            break;
        default:
            return(atoi(value));
	    /* NOTREACHED */
        break;
    }
	return 0;
}
 
/* This is an information function to simplify the use of pixflags
** If used without args, it gives a list of the possible pixflags
** if used with a numerical arg it gives a list of flags that are set
** if used with a non-numerical arg it returns the number corresponding
** to that pixflag.
*/
 
char *do_pixflags(argc,argv)
    int argc;
    char **argv;
{
    int flag;
    static char str[200];
 
    initopt(argc, argv, "pixflag_name_or_number");
    if (G_getopt(argc, argv) <  0){
	printoptusage(argc,argv);
        printf("Pix setting syntax: set <element> pixflags [~]<pixflag>\n");
        printf("The optional '~' prefix unsets the pixflag\n");
        printf("\nAll pixflags default to zero (unset).\n");
        printf("Available pixflags:\n");
        printf("v[isible_not]   -   not visible when set\n");
        printf("c[lickable_not] -   insensitive to clicks\n");
        printf("is_s[elected]   -   selected by a previous click\n");
        printf("is_i[con]   -   acts as an icon\n");
        printf("is_f[lat]   -   Always uses xy coord system\n");
        printf("update_all_on_set   -   forces entire draw to refresh when a field is set\n");
        printf("update_self_on_set_not  -   disables self redraw when a field is set\n");
        printf("update_all_on_step  -   forces entire draw to refresh every clock tick\n");
        printf("update_self_on_step -   disables redraw of self every clock tick\n");
        printf("update_with_undraw  -   undraws self before redrawing\n");
        printf("m[ove-sensitive -   Enables sensistivity to move events\n");
        printf("r[esizable-not] -   Disables resizing of pix\n");
        printf("pc[oords]   -   Use pixel coords\n");
        printf("po[ffset]   -   Use pixel offset\n");
        printf("pb[oth]     -   Use both pixel coords and pixel offset\n");
        printf("%%c[oords]  -   Use percent of screen for coords\n");
        printf("%%o[ffset]  -   Use percent of screen for offset\n");
        printf("%%b[oth]        -   Use percent for both coords and offset\n");
        printf("hlt1        -   Disable highlight # 1\n");
        printf("hlt2        -   Enable highlight # 2\n");
        printf("hlt3        -   Enable highlight # 3\n");
        printf("f[lush_not] -   Disable XFlush in clocked pixes\n");
        printf("t[translate_not]    -   Disable translations of pix\n");
        printf("\nIn general you can abbreviate the pixflag name to the shortest unique name\n");
        return(CopyString(""));
    }
    flag = atoi(optargv[1]);
    if (flag == 0) { /* Assume it is a flag name, so we want to return
                     ** the corresponding number */
        sprintf(str,"%d",xoCvtFlag(optargv[1]));
        return(CopyString(str));
    } else {
        str[0] = '\0';
        if (flag & XO_VISIBLE_NOT) strcat(str,"v ");
        if (flag & XO_CLICKABLE_NOT) strcat(str,"c ");
        if (flag & XO_IS_SELECTED) strcat(str,"is_sel ");
        if (flag & XO_IS_ICON) strcat(str,"is_icon ");
        if (flag & XO_IS_FLAT) strcat(str,"is_flat ");
        if (flag & XO_UPDATE_SELF_ON_SET_NOT)
            strcat(str,"update_self_on_set_not ");
        if (flag & XO_UPDATE_SELF_ON_STEP_NOT)
            strcat(str,"update_self_on_step_not ");
        if (flag & XO_UPDATE_ALL_ON_SET)
            strcat(str,"update_all_on_set ");
        if (flag & XO_UPDATE_ALL_ON_STEP)
            strcat(str,"update_all_on_step ");
        if (flag & XO_UPDATE_WITH_UNDRAW)
            strcat(str,"update_with_undraw ");
        if (flag & XO_MOVE_SENSITIVE) strcat(str,"m ");
        if (flag & XO_RESIZABLE_NOT) strcat(str,"r ");
        if (flag & XO_PIXELCOORDS) strcat(str,"pc ");
        if (flag & XO_PIXELOFFSET) strcat(str,"po ");
        if (flag & XO_PCTCOORDS) strcat(str,"%c ");
        if (flag & XO_PCTOFFSET) strcat(str,"%o ");
        if (flag & XO_HLT1_NOT) strcat(str,"hlt1 ");
        if (flag & XO_HLT2) strcat(str,"hlt2 ");
        if (flag & XO_HLT3) strcat(str,"hlt3 ");
        if (flag & XO_FLUSH_NOT) strcat(str,"f ");
        if (flag & XO_TRANSLATE_NOT) strcat(str,"t ");
        return(CopyString(str));
    }
}

