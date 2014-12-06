/* $Id: xshape.c,v 1.2 2005/07/01 10:02:59 svitak Exp $ */
/*
 * $Log: xshape.c,v $
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
 * Revision 1.22  2000/06/12 04:20:46  mhucka
 * 1) Removed nested comments in RCS/CVS log lines, to quiet down the
 *    IRIX cc compiler.
 * 2) Added NOTREACHED comments where appropriate.
 *
 * Revision 1.21  2000/05/02 06:18:31  mhucka
 * Added type casts for certain function call arguments.
 *
 * Revision 1.20  1999/10/17 04:02:32  mhucka
 * Applied additional patches from Upi dated July 1998 that never seem to have
 * made it into DR 2.2.1.
 *
 * Revision 1.19  1998/07/15 06:23:58  dhb
 * Upi update
 *
 * Revision 1.18  1997/07/18 23:55:56  dhb
 * Updated ShowInterpol() calls to add dimension arg
 *
 * Revision 1.17  1996/07/02 18:17:20  venkat
 * RESET and XSHAPE_ADDPTS actions cleaned up to initialize additonal
 * entries in reallocated interpol structs for the element.
 *
 * Revision 1.16  1995/09/27  00:09:28  venkat
 * XtVaSetValues()-calls-changed-to-XoXtVaSetValues()-to-avoid-alpha-FPE's
 *
 * Revision 1.15  1995/08/05  02:32:10  venkat
 * The COPY action does a deep-copy of the fields that are CopyString'ed
 * and FreeString'ed in the DELETE action.
 *
 * Revision 1.14  1995/08/02  01:58:45  venkat
 * Some memory leak fixes
 *
 * Revision 1.13  1995/07/28  23:57:02  venkat
 * Changed the call to set the foreground for the widget to include XtVaTypedArg
 * and thus invoke the type converter. Although this was intended, XtVaTyped was missing
 * in the call and thus shapes were not displaying the right colors. Also fixed a memory
 * leak problem with the shape->fg and the shape->textfont fields.
 *
 * Revision 1.12  1995/07/21  20:48:36  venkat
 * Added code to allow multiple args after the -coords option.
 *
 * Revision 1.11  1995/03/28  21:12:01  venkat
 * COPY
 *
 * Revision 1.10  1995/01/20  23:54:41  venkat
 * Made arguments to create_interpol as (int, float, float)
 * from (int, int, int)
 *
 * Revision 1.9  1994/12/19  22:49:15  venkat
 * Modified DELETE in XShape() to check for NULL before
 * freeing members of the shape
 *
 * Revision 1.8  1994/11/07  22:08:27  bhalla
 * Put in XoProjectAndDrawPix after the SET action on coords, for
 * obvious refreshing reasons.
 *
 * Revision 1.7  1994/06/06  16:02:45  bhalla
 * Added SHOW action to use ShowInterpol to show coords.
 *
 * Revision 1.6  1994/05/25  13:38:34  bhalla
 * Updated syntax for SetTable
 *
 * Revision 1.5  1994/04/25  17:57:32  bhalla
 * Fixed some problems with realloc calls causing dumps
 * Added two new actions: XSHAPE_ADDPT and XSHAPE_MOVEPT to
 * help with editing. They are pretty obvious
 *
 * Revision 1.4  1994/03/16  17:01:18  bhalla
 * Added a bunch of defaults
 * Added options for setting tables directly using SetTable
 * Added DUMP action
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
/*#include "olf_struct.h" */
#include "draw_ext.h"
#include "_widg/xform_struct.h"

#define MAX_VALUE_LEN 200


static Gen2Xo GXconvert[] = {
	{"tx",		XtNpixX},
	{"ty",		XtNpixY},
	{"tz",		XtNpixZ},
	{"fg",		XtNforeground},
	{"drawmode",		XtNdrawmode},
	{"npts",		XtNnpts},
	{"linewidth",		XtNlinewidth},
	{"linestyle",		XtNlinestyle},
	{"capstyle",		XtNcapstyle},
	{"joinstyle",		XtNjoinstyle},
	{"text",		XtNtext},
	{"textcolor",		XtNtextcolor},
	{"textmode",		XtNtextmode},
	{"textfont",		XtNtextfont},
	{"font",			XtNtextfont},
};


static void xoStringToCoords();

int XShape (shape, action)
     register struct xshape_type *shape;
     Action                     *action;
{
	int ac=action->argc;
	char** av=action->argv;
	char* localav[100];
	int localac, tac;
	
    Widget parentW, xoFindParentDraw();
	ShapeObject sW;
	int 	 i;
	Interpol	*create_interpol(), *ip;
	struct 	xshape_type *copy_shape;
	MsgIn	*msg;

  
  if (Debug(0) > 1)
    ActionHeader("XShape", shape, action);

  SELECT_ACTION (action) {
  case INIT:
    break;
  case PROCESS:
	MSGLOOP(shape,msg) {
		case 0: /* PATH */
			/* copy the path of the source element into the
			** value field of the shape.
			*/
			strncpy(shape->value,Pathname(msg->src),MAX_VALUE_LEN);
		break;
	}
    break;
  case RESET:
    break;
  case CREATE:
    /* arguments are: object_type name [field value] ... */
	if ((parentW = xoFindParentDraw(shape)) == NULL) return(0);

	shape->xpts = create_interpol(20,0.0,1.0);
	shape->ypts = create_interpol(20,0.0,1.0);
	shape->zpts = create_interpol(20,0.0,1.0);
	shape->allocated_pts = ((Interpol *)(shape->xpts))->xdivs+1;
	shape->value = (char *)calloc(MAX_VALUE_LEN,sizeof(char));
	shape->fg = CopyString("black");
	shape->textcolor = CopyString("black");
	shape->drawmode = CopyString("DrawLines");
	shape->linestyle = CopyString("LineSolid");
	shape->capstyle = CopyString("CapButt");
	shape->joinstyle = CopyString("JoinMiter");
	shape->textmode = CopyString("draw");
	shape->textfont = CopyString("7x13bold");
    
	sW = (ShapeObject)XtCreateManagedWidget(
		av[1], shapeObjectClass, parentW, NULL,0);
    shape->widget = (char *)sW;
	/* copying over the table pointers */
	XoXtVaSetValues(sW,
		XtNxpts,(XtPointer)((Interpol *)shape->xpts)->table,
		XtNypts,(XtPointer)((Interpol *)shape->ypts)->table,
		XtNzpts,(XtPointer)((Interpol *)shape->zpts)->table,
		NULL);
	XtAddCallback((Widget) shape->widget, XtNcallback,xoCallbackFn,
		(XtPointer)shape);

    ac--, av++;			/* object type */
    ac--, av++;			/* path */

	/*** This section of code before the call to xoParseCreateArgs() allows multiple
	     args after the -coords option. localac and localav are copies of
	     ac and av respectively and these are passed in xoParseCreateArgs().
	     tac is just a running variable  ***/


	for(i=0; i<ac; i++)
		localav[i] = NULL; 

	for(tac = 0,localac=0; tac < ac; tac++,localac++){
	  if((localav[localac] = (char*) malloc(sizeof(char)*(strlen(av[tac])+1)))==NULL){
		/** Treat memory allocation probs here **/
	  }
	  else
	    strcpy(localav[localac],av[tac]);
	  if(strcmp(av[tac], "-coords")==0){
		localac++;
		tac++;
		if((tac == ac) || (av[tac][0]=='-')){
		 localac --;
		 tac --;
		 continue;
		}
		if((localav[localac] = (char*) malloc (sizeof(char)*(strlen(av[tac])+1)))==NULL){
		/** Treat memory allocation probs here **/
		}
		else
		 strcpy(localav[localac], av[tac]);
		tac++;
		if((tac == ac) || (av[tac][0]=='-')){
		 tac --;
		 continue;
		}
		while((tac < ac) && (av[tac][0]!='-')){
		 if((localav[localac] = (char*) realloc (localav[localac],strlen(localav[localac])+1+strlen(av[tac])+1))==NULL){
			/** Treat memory allocation probs here **/
		 }
		 else
		  strcat(localav[localac], av[tac]); 
		 tac++;
		}
	 	if((tac != ac) && (av[tac][0]=='-')) tac--;
	  }
	}

	xoParseCreateArgs(shape,localac,localav);
	for(i=0; i<ac; i++){
		if(localav[i] != NULL) free(localav[i]);
	}
	return(1);
	/* NOTREACHED */
	break;
  case COPY:
	/* Note that the 'shape' pointer refers to the original,
	** and the data contains the pointer to the copy */
	copy_shape = (struct xshape_type *)action->data;
	if ((parentW = xoFindParentDraw(copy_shape)) == NULL) return(0);

	/* We need to duplicate the interpols */
		one_tab_dup_func(&(copy_shape->xpts));
		one_tab_dup_func(&(copy_shape->ypts));
		one_tab_dup_func(&(copy_shape->zpts));

	/* we need to duplicate the value */
		copy_shape->value = (char *)calloc(MAX_VALUE_LEN,sizeof(char));
		strcpy(copy_shape->value,shape->value);

	/** Deep-copy the fields **/

	copy_shape->fg = CopyString(shape->fg);
	copy_shape->textcolor = CopyString(shape->textcolor);
	copy_shape->drawmode = CopyString(shape->drawmode);
	copy_shape->linestyle = CopyString(shape->linestyle);
	copy_shape->capstyle = CopyString(shape->capstyle);
	copy_shape->joinstyle = CopyString(shape->joinstyle);
	copy_shape->textmode = CopyString(shape->textmode);
	copy_shape->textfont = CopyString(shape->textfont);

	/* creating a new widget */
		sW = (ShapeObject)XtCreateManagedWidget(
			copy_shape->name, shapeObjectClass, parentW, NULL,0);
    	copy_shape->widget = (char *)sW;
	/* copying over the table pointers */
		XoXtVaSetValues(sW,
			XtNxpts,(XtPointer)((Interpol *)copy_shape->xpts)->table,
			XtNypts,(XtPointer)((Interpol *)copy_shape->ypts)->table,
			XtNzpts,(XtPointer)((Interpol *)copy_shape->zpts)->table,
	/* and the pixflags */
			XtNpixflags,copy_shape->pixflags,
			NULL);
		
	/* adding the callback */
		XtAddCallback((Widget) copy_shape->widget, XtNcallback,xoCallbackFn,
			(XtPointer)copy_shape);
	/* Converting the rest of the values */
		gx_convert_all(copy_shape,GXconvert,XtNumber(GXconvert));
	return(1);
	/* NOTREACHED */
	break;
  case SET:
	if (xoSetPixFlags(shape,ac,av)) return(1);
    if (ac) {			/* need to set fields */
		if (strcmp(av[0],"coords") == 0 || strcmp(av[0],"pts") == 0) {
			xoStringToCoords(shape,av[1]);
			/* We may want to eliminate this so that the pts
			** dinosaur dies */
			/* shape->pts = CopyString(av[1]); */
			XoXtVaSetValues((PixObject)shape->widget,
				XtNnpts,shape->npts,
				NULL);
			XoProjectAndDrawPix((PixObject)shape->widget,1);
			return(1);
		}
		/* dummy name for font, used for backwards compatibility */
		if (strcmp(av[0],"font") == 0) {
			if(shape->textfont != NULL) FreeString(shape->textfont);
			DirectSetElement(shape,"textfont",av[1]);
			gx_convert(shape,GXconvert,XtNumber(GXconvert),ac,av);
			return(1);
		}
		/* dummy name for fg, used for backwards compatibility */
		if (strcmp(av[0],"pixcolor") == 0) {
			if(shape->fg != NULL) FreeString(shape->fg);
			DirectSetElement(shape,"fg",av[1]);
			XoXtVaSetValues((PixObject)shape->widget,XtVaTypedArg,
				XtNforeground,XtRString,av[1],strlen(av[1])+1,
				NULL);
			/* gx_convert(shape,GXconvert,XtNumber(GXconvert),2,av); */
			return(1);
		}
		if (strcmp(av[0], "textcolor") == 0){
			if(shape->textcolor != NULL) FreeString(shape->textcolor);
		}
		if (strcmp(av[0], "drawmode") == 0){ 
			if(shape->drawmode != NULL) FreeString(shape->drawmode);
		}
		if (strcmp(av[0], "linestyle") == 0) {
			if(shape->linestyle != NULL) FreeString(shape->linestyle);
		}
		if (strcmp(av[0], "capstyle") == 0) {
			if (shape->capstyle!= NULL) FreeString(shape->capstyle);
		}
		if (strcmp(av[0], "joinstyle") == 0){
			if (shape->joinstyle != NULL)  FreeString(shape->joinstyle);
		}
		if (strcmp(av[0], "textmode") == 0){
			if (shape->textmode != NULL) FreeString(shape->textmode);
		}
		/* prevent overwriting the value field */
		if (strcmp(av[0],"value") == 0) {
			if(av[1])
				strncpy(shape->value,av[1],MAX_VALUE_LEN);
			return(1);
		}
		/* Special case : setting the interpol arrays. */
		if (SetTable(shape,ac,av,action->data,"xpts ypts zpts")) {
			/* Note that we do not change npts here unless it is
			** larger than the tables */
			if (shape->xpts && shape->npts > shape->xpts->xdivs + 1)
				shape->npts = shape->xpts->xdivs + 1;
			if (shape->ypts && shape->npts > shape->ypts->xdivs + 1)
				shape->npts = shape->ypts->xdivs + 1;
			if (shape->zpts && shape->npts > shape->zpts->xdivs + 1)
				shape->npts = shape->zpts->xdivs + 1;

			XoXtVaSetValues((PixObject)shape->widget,
				XtNxpts,(XtPointer)((Interpol *)shape->xpts)->table,
				XtNypts,(XtPointer)((Interpol *)shape->ypts)->table,
				XtNzpts,(XtPointer)((Interpol *)shape->zpts)->table,
				NULL);
			XoProjectAndDrawPix((PixObject)shape->widget,1);
			return(1);
		}
		if (strncmp(av[0]+1,"pts->table",10) == 0) {
		/* need to set from new-style points and update */
			DirectSetElement(shape,av[0],av[1]);
			XoProjectAndDrawPix((PixObject)shape->widget,1);
			return(1);
		}
		/* Need to reallocate storage for tables */
		if (strcmp(av[0],"npts") == 0) {
			i=atoi(av[1]);
			if (i < 0) {
				printf("error in %s: npts (%d) must be >= 0\n",
					Pathname(shape),i);
				return(1); /* Do nothing - bad value */
			}
			if (i > ((Interpol *)(shape->xpts))->xdivs) {
				Interpol *ip;
				int ap;
				shape->npts=i;

				ap = shape->npts + 20;
				((Interpol *)(shape->xpts))->table = (double *) 
					realloc(((Interpol *)(shape->xpts))->table,
					(shape->npts+20) * sizeof(double));
				ip = (Interpol *) shape->xpts;
				xomemzero((char*) &ip->table[shape->allocated_pts], (ap - shape->allocated_pts)*sizeof(double));
				((Interpol *)(shape->xpts))->xdivs = shape->npts + 20;

				((Interpol *)(shape->ypts))->table = (double *) 
					realloc(((Interpol *)(shape->ypts))->table,
					(shape->npts+20) * sizeof(double));
				ip = (Interpol *) shape->ypts;
				xomemzero((char*) &ip->table[shape->allocated_pts], (ap - shape->allocated_pts)*sizeof(double));
				((Interpol *)(shape->ypts))->xdivs = shape->npts+20;

				((Interpol *)(shape->zpts))->table = (double *) 
					realloc(((Interpol *)(shape->zpts))->table,
					(shape->npts+20) * sizeof(double));
				ip = (Interpol *) shape->zpts;
				xomemzero((char*) &ip->table[shape->allocated_pts], (ap - shape->allocated_pts)*sizeof(double));
				((Interpol *)(shape->zpts))->xdivs = shape->npts+20;
				shape->allocated_pts = ap;

				/* copying over the table pointers */
				XoXtVaSetValues((PixObject)shape->widget,
					XtNxpts,(XtPointer)((Interpol *)shape->xpts)->table,
					XtNypts,(XtPointer)((Interpol *)shape->ypts)->table,
					XtNzpts,(XtPointer)((Interpol *)shape->zpts)->table,
					XtNnpts,shape->npts,
					NULL);
				return(1);
			}
			/* if a more usual change to npts, update as usual */
			shape->npts=i;
			gx_convert(shape,GXconvert,XtNumber(GXconvert),ac,av);
			return(1);
		}
		gx_convert(shape,GXconvert,XtNumber(GXconvert),ac,av);
		return(0);
	}
	break;
  case SHOW:
		if (ShowInterpol(action,"xpts",shape->xpts,1,100)) return(1);
		if (ShowInterpol(action,"ypts",shape->ypts,1,100)) return(1);
		return(ShowInterpol(action,"zpts",shape->zpts,1,100));
	/* NOTREACHED */
	break;
  case XSHAPE_ADDPT: /* adds a point to the arrays */
	if (ac < 3) break;
	if (shape->npts < 0) break;
	/* Keep on top of memory allocation */
	if (shape->npts >= ((Interpol *)(shape->xpts))->xdivs) {
				Interpol  *ip;
				int 	ap;

				ap = shape->npts + 20;
				((Interpol *)(shape->xpts))->table = (double *) 
					realloc(((Interpol *)(shape->xpts))->table,
					(shape->npts+20) * sizeof(double));
				ip = (Interpol*) shape->xpts;
				xomemzero((char*) &ip->table[shape->allocated_pts], (ap-shape->allocated_pts)*sizeof(double));
				((Interpol *)(shape->xpts))->xdivs = shape->npts+20;

				((Interpol *)(shape->ypts))->table = (double *) 
					realloc(((Interpol *)(shape->ypts))->table,
					(shape->npts+20) * sizeof(double));
				ip = (Interpol*) shape->ypts;
				xomemzero((char*) &ip->table[shape->allocated_pts], (ap-shape->allocated_pts)*sizeof(double));
				((Interpol *)(shape->ypts))->xdivs = shape->npts+20;

				((Interpol *)(shape->zpts))->table = (double *) 
					realloc(((Interpol *)(shape->zpts))->table,
					(shape->npts+20) * sizeof(double));
				ip = (Interpol*) shape->zpts;
				xomemzero((char*) &ip->table[shape->allocated_pts], (ap-shape->allocated_pts)*sizeof(double));
				((Interpol *)(shape->zpts))->xdivs = shape->npts+20;

				shape->allocated_pts = shape->npts + 20;
				/* copying over the table pointers */
				XoXtVaSetValues((PixObject)shape->widget,
					XtNxpts,(XtPointer)((Interpol *)shape->xpts)->table,
					XtNypts,(XtPointer)((Interpol *)shape->ypts)->table,
					XtNzpts,(XtPointer)((Interpol *)shape->zpts)->table,
					XtNnpts,shape->npts,
					NULL);
				return(1);
	}
	((Interpol *)(shape->xpts))->table[shape->npts] = Atof(av[0]);
	((Interpol *)(shape->ypts))->table[shape->npts] = Atof(av[1]);
	((Interpol *)(shape->zpts))->table[shape->npts] = Atof(av[2]);
	shape->npts++;
	XoXtVaSetValues((PixObject)shape->widget,
			XtNnpts,shape->npts,
			NULL);
	XoProjectAndDrawPix((PixObject)shape->widget,1);
	break;
  case XSHAPE_MOVEPT:  /* finds the nearest point and moves it */
	if (ac == 3 && shape->npts > 0) {
		int i;
		double *xpts = ((Interpol *)(shape->xpts))->table;
		double *ypts = ((Interpol *)(shape->ypts))->table;
		double *zpts = ((Interpol *)(shape->zpts))->table;
		float x = Atof(av[0]);
		float y = Atof(av[1]);
		float z = Atof(av[2]);
		float dx = xpts[0] - x;
		float dy = ypts[0] - y;
		float dz = zpts[0] - z;
		float bestr = dx * dx + dy * dy + dz * dz;
		float dist;
		int besti = 0;

		for(i = 1; i < shape->npts; i++) {
			dx = xpts[i] - x;
			dy = ypts[i] - y;
			dz = zpts[i] - z;
			dist = dx * dx + dy * dy + dz * dz;
			if (dist < bestr) {
				bestr = dist;
				besti = i;
			}
		}
		xpts[besti] = x;
		ypts[besti] = y;
		zpts[besti] = z;
		XoProjectAndDrawPix((PixObject)shape->widget,1);
	}
	break;
  case DELETE:
	if (!(shape->widget)) return(0);
	XtDestroyWidget((Widget)shape->widget);
  	ip = (Interpol *)shape->xpts;
	if (ip != NULL && ip->allocated) free(ip->table); free(ip);
	ip = (Interpol *)shape->ypts;
	if (ip != NULL && ip->allocated) free(ip->table); free(ip);
	ip = (Interpol *)shape->zpts;
	if (ip != NULL && ip->allocated) free(ip->table); free(ip);
	if (shape->value!=NULL)
	free(shape->value);
	if (shape->fg!=NULL)
	FreeString(shape->fg);
	if(shape->textfont!=NULL)
	FreeString(shape->textfont);
	if(shape->drawmode != NULL)
	FreeString(shape->drawmode);
	if(shape->textcolor != NULL)
	FreeString(shape->textcolor);
	if(shape->capstyle != NULL)
	FreeString(shape->capstyle);
	if(shape->linestyle != NULL)
	FreeString(shape->linestyle);
	if(shape->joinstyle != NULL)
	FreeString(shape->joinstyle);
	if(shape->textmode != NULL)
	FreeString(shape->textmode);
	break;
  case DUMP:
	if (ac == 1) {
		if (strcmp(av[0],"pre") == 0) {
			return(0);
		}
		if (strcmp(av[0],"post") == 0) {
			FILE *fp = (FILE *)action->data ;
			DumpInterpol(fp,shape,shape->xpts,"xpts");
			DumpInterpol(fp,shape,shape->ypts,"ypts");
			DumpInterpol(fp,shape,shape->zpts,"zpts");
			return(0);
		}
	}
	break;
  case XUPDATE : /* update shape fields due to changes in widget */
	if (ac > 0)
	 xg_convert(shape,GXconvert,XtNumber(GXconvert),ac,av);
	break;
  default:
	xoExecuteFunction(shape,action,shape->script,shape->value);
	break;
  }
	return(0);
}


static void xoStringToCoords(pix,pts)
	struct xshape_type		*pix;
	char			*pts;
{
	int		len = 0;
	double	*x,*y,*z;
	int		i,index = 0;

	if (pts)
		len = strlen(pts);
	if (!pts || len == 0) return;

	x=((Interpol *)(pix->xpts))->table;
	y=((Interpol *)(pix->ypts))->table;
	z=((Interpol *)(pix->zpts))->table;

	for (i = 0 ; i < ((Interpol *)(pix->xpts))->xdivs ; i++) {
		for (; index < len &&  pts[index] != '[' ; index++);
		if (index == len) break;
	
		if (!(sscanf(pts + index,"[%lf,%lf,%lf]",
			x+i, y+i, z+i))) {
			fprintf(stderr,"error in syntax of coords at '%s'\n",
				pts + index);
			return;
		}
		index++;
	}
	pix->npts=i;
}

#undef MAX_VALUE_LEN

