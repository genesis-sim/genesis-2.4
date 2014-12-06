/* $Id: View.c,v 1.3 2005/07/01 10:02:31 svitak Exp $ */
/*
 * $Log: View.c,v $
 * Revision 1.3  2005/07/01 10:02:31  svitak
 * Misc fixes to address compiler warnings, esp. providing explicit types
 * for all functions and cleaning up unused variables.
 *
 * Revision 1.2  2005/06/18 18:50:25  svitak
 * Typecasts to quiet compiler warnings. From OSX Panther patch.
 *
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.19  2000/06/12 04:28:18  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.18  2000/05/02 06:06:42  mhucka
 * Added type casts for certain function call arguments.
 *
 * Revision 1.17  1998/01/20 00:24:42  venkat
 * Included-stdio.h-for-prototyping-sprintf.-Caused-undefined-results-on-64-bit-Irix
 *
 * Revision 1.16  1996/06/20  23:24:18  venkat
 * Changed type of the colorscale resource and any local variables attempting
 * to get the list of colortable indices, from int to long. On the alpha these
 * types are different sizes and information is lost in the scale-down process leading to
 * unpredictable color index values and behaviour.
 *
 * Revision 1.15  1995/11/07  19:17:21  dhb
 * Changed initialization code in growing the old_interps arrays from
 * bzero() to an explicit loop as not all systems have bzero().
 *
 * Revision 1.14  1995/09/27  00:00:17  venkat
 * Changes-to-avoid-Alpha-FPE's-and-core-dumps
 *
 * Revision 1.13  1995/08/07  18:50:16  dhb
 * Expose() wasn't calling DoOffset() in all cases causing vars to draw
 * in the wrong places.  In the case where we don't undraw the textcolor
 * was being set in pix.gc rather than view.textgc.
 *
 * CheckInterpColor() didn't calculate a reliable return value if the
 * val being interpolated didn't fall within any of the value ranges.
 * Now it will peg the color to the highest or lowest shape based on
 * which end the val falls off of.
 *
 * Revision 1.12  1995/08/05  17:28:09  dhb
 * Changed CheckInterpMorph() to set the interpolated value in the
 * case of a zero value range between shapes to zero.
 *
 * Revision 1.11  1995/08/03  18:37:39  venkat
 * The do_view bit is reintialized to 0 in Expose(). The points are drawn
 * whenever the expose method is called.
 *
 * Revision 1.10  1995/08/02  01:56:29  venkat
 * Code to check the interpolation range != 0 and if it is the case use the
 * lower limit
 *
 * Revision 1.9  1995/06/16  05:46:48  dhb
 * Merged in 1.6.1.1 changes.
 *
<<<<<<< 1.8
 * Revision 1.8  1995/06/12  17:13:26  venkat
 * More-Resource-Converter-clean-up-(fromVal->addr!=NULL)-is-checked-instead-of-checking-(fromVal->size)
 *
 * Revision 1.7  1995/06/02  19:46:02  venkat
 * Resource-converters-clean-up
 *
=======
 * Revision 1.6.1.1  1995/06/16  05:38:18  dhb
 * Fixes for arithmetic errors in color lookups, by Upi Bhalla.
 *
 *
 * Upi Bhalla Mt Sinai 7 June 1995 color lookup index checks now done
 * on a double to avoid arith overflows when converting to int.
 *
>>>>>>> 1.6.1.1
 * Revision 1.6  1995/04/17  16:56:04  venkat
 * Reinitialized the do_view bit in the Expose method to 1 in order
 * to do a redraw whenever the Expose is called.
 *
 * Revision 1.5  1994/06/13  23:09:19  bhalla
 * Major bugfix involving valindex
 *
 * Revision 1.4  1994/04/25  18:02:01  bhalla
 * Added stuff to handle selection and returning values for individual
 * items in view pix.
 *
 * Revision 1.3  1994/03/22  15:32:08  bhalla
 * RCS error: no code changes
 *
 * Revision 1.2  1994/01/13  19:35:36  bhalla
 * *** empty log message ***
 *
 * Revision 1.2  1994/01/13  19:35:36  bhalla
 * *** empty log message ***
 * */
#include <stdio.h>
#include <math.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include "Xo/XoDefs.h"
#include "CoreDrawP.h"
#include "ShapeP.h"
#include "ViewP.h"
#include "Xo/Xo_ext.h"

#define DRAWLINES 0
#define DRAWSEGS 1
#define DRAWPOINTS 2
#define DRAWRECTS 3
#define DRAWARROWS 4
#define FILLRECTS 5
#define FILLPOLY 6

#define	TEXTDRAW 0
#define	TEXTFILL 1
#define	TEXTNODRAW 2

#define XO_ARROWHEAD_LEN 8
#define XO_LINE_SEL_DIST 10

#define XO_VIEW_MAX_TEXT_CHARS 50
#define XO_VIEW_MAX_SEL_DIST 500

#define offset(field) XtOffset(ViewObject, view.field)
static XtResource resources[] = {
  {XtNviewmode, XtCViewmode, XtRViewmode, sizeof(int),
     offset(viewmode), XtRString, "shape"},
  {XtNnshapes, XtCNshapes, XtRInt, sizeof(int),
     offset(nshapes), XtRString, "0"},
  {XtNshapeflag, XtCShapeflag, XtRInt, sizeof(int),
     offset(shapeflag), XtRString, "0"},
  {XtNvarShapes, XtCVarShapes, XtRPointer, sizeof(varShape*),
     offset(shapes), XtRImmediate, (XtPointer)NULL},
  {XtNviewValues, XtCViewValues, XtRPointer, sizeof(float**),
     offset(view_values), XtRImmediate, (XtPointer)NULL},
  {XtNcolorscale, XtCColorscale, XtRPointer, sizeof(unsigned long*),
     offset(colorscale), XtRImmediate, (XtPointer)NULL},
  {XtNncols, XtCNcols, XtRInt, sizeof(int),
     offset(ncols), XtRString, "0"},
  {XtNviewColorVal, XtCViewSelect, XtRInt, sizeof(int),
     offset(color_val), XtRString, "1"},
  {XtNviewMorphVal, XtCViewSelect, XtRInt, sizeof(int),
     offset(morph_val), XtRString, "0"},
  {XtNviewXOffsetVal, XtCViewSelect, XtRInt, sizeof(int),
     offset(xoffset_val), XtRString, "0"},
  {XtNviewYOffsetVal, XtCViewSelect, XtRInt, sizeof(int),
     offset(yoffset_val), XtRString, "0"},
  {XtNviewZOffsetVal, XtCViewSelect, XtRInt, sizeof(int),
     offset(zoffset_val), XtRString, "0"},
  {XtNviewTextVal, XtCViewSelect, XtRInt, sizeof(int),
     offset(text_val), XtRString, "0"},
  {XtNviewTextColorVal, XtCViewSelect, XtRInt, sizeof(int),
     offset(textcolor_val), XtRString, "0"},
  {XtNviewLineThicknessVal, XtCViewSelect, XtRInt, sizeof(int),
     offset(linethickness_val), XtRString, "0"},
  {XtNxpts, XtCXpts, XtRPointer, sizeof(double*),
     offset(xpts), XtRImmediate, (XtPointer)NULL},
  {XtNypts, XtCYpts, XtRPointer, sizeof(double*),
     offset(ypts), XtRImmediate, (XtPointer)NULL},
  {XtNzpts, XtCZpts, XtRPointer, sizeof(double*),
     offset(zpts), XtRImmediate, (XtPointer)NULL},
  {XtNncoords, XtCNcoords, XtRInt, sizeof(int),
     offset(ncoords), XtRString, "0"},
};
#undef offset


/* methods */

/* I have no idea where Expose is getting defined
*/
#ifdef Expose
#undef Expose
#endif
static Boolean XoCvtStringToViewmode();
static void SelectDistance();
static void Select();
static void ClassInitialize();
static void Expose();
static void DrawPoints();
static void Initialize();
static Boolean SetValues();
static void Destroy();
static void Project();
static XtGeometryResult QueryGeometry();

static int CheckInterpColor();
static int CheckInterpMorph();
static int CheckInterpXOffset();
static int CheckInterpYOffset();
static int CheckInterpZOffset();
static int CheckInterpText();
static int CheckInterpTextColor();
static int CheckInterpLineThickness();

static int DoMorph();
static int DoOffset();
static int DoText();
static int DoLineThickness();

/* class record definition */


ViewClassRec viewClassRec = {
  { /* RectObj (Core) Fields */

    /* superclass         */    (WidgetClass) &pixClassRec,
    /* class_name         */    "View",
    /* size               */    sizeof(ViewRec),
    /* class_initialize   */    ClassInitialize,
    /* class_part_initialize*/  NULL,
    /* Class init'ed      */    FALSE,
    /* initialize         */    Initialize,
    /* initialize_hook    */    NULL,
    /* realize            */    NULL,
    /* actions            */    NULL,
    /* num_actions        */    0,
    /* resources          */    resources,
    /* resource_count     */    XtNumber(resources),
    /* xrm_class          */    NULLQUARK,
    /* compress_motion    */    FALSE,
    /* compress_exposure  */    FALSE,
    /* compress_enterleave*/    FALSE,
    /* visible_interest   */    FALSE,
    /* destroy            */    Destroy,
    /* resize             */    NULL,
    /* expose             */    Expose,
    /* set_values         */    SetValues,
    /* set_values_hook    */    NULL,
    /* set_values_almost  */    XtInheritSetValuesAlmost,
    /* get_values_hook    */    NULL,
    /* accept_focus       */    NULL,
    /* intrinsics version */    XtVersion,
    /* callback offsets   */    NULL,
    /* tm_table           */    NULL,
    /* query_geometry     */    NULL,
    /* display_accelerator*/    NULL,
    /* extension          */    NULL
  },
  {
    /* pix Fields */
    /* project */				Project,
    /* undraw */				XoInheritUndraw,
    /* select_distance */       SelectDistance,
    /* select */  	         	Select,
    /* unselect */ 	         	XoInheritUnSelect,
    /* motion */  	         	XoInheritMotion,
    /* hightlight */          	XoInheritHighlight,
    /* extension */             NULL
  },
  {
	/* view fields */
	/* make_compiler_happy */	0
  }
};

WidgetClass viewObjectClass = (WidgetClass)&viewClassRec;

static void ClassInitialize()
{
  XtSetTypeConverter (XtRString, XtRViewmode,
                      XoCvtStringToViewmode, NULL, 0 ,
                      XtCacheNone, NULL);
}

static void Initialize (req, new)
     Widget req, new;
{
  ViewObject view = (ViewObject) new;
  CoreDrawWidget dw = (CoreDrawWidget)XtParent(new);
  XGCValues values;
  XtGCMask mask = GCForeground | GCLineStyle | GCCapStyle | 
	GCJoinStyle | GCLineWidth;
  Display 	*disp = XtDisplay(XtParent(new));
  Drawable d = XtWindow(XtParent(new));
  int		i;

  values.foreground = XBlackPixel(disp,XDefaultScreen(disp));
  values.background = XWhitePixel(disp,XDefaultScreen(disp));
  values.line_width = 0;
  values.line_style = LineSolid;
  values.cap_style = CapNotLast;
  values.join_style = JoinMiter;

  /* Check that the initial values are reasonable */
	view->view.viewmode = XoVarShapeMode;
	view->view.textcolor = XBlackPixel((Display *)XgDisplay(),0);
		
  view->pix.gc=XCreateGC(disp,d,mask,&values);
  mask = GCForeground;
  view->view.textgc=XCreateGC(disp,d,mask,&values);
  /* 20 is a reasonable initial value for the size of the pts cache */
  view->view.text = XtCalloc(XO_VIEW_MAX_TEXT_CHARS,sizeof(char));
								/* The text field is permanently
								** allocated for doing numbers */
	/* Perhaps I should do an XtGetValues here ... */
	view->view.bg = dw->core.background_pixel;
  view->view.pts =
	(XPoint *) XtCalloc(20,sizeof(XPoint));
	view->view.alloced_pts = 20;
  view->view.ncoords = 20;
  view->view.coords = 
	(XPoint *) XtCalloc(20,sizeof(XPoint));
  view->view.old_interps = (float **)XtCalloc(XoVarNValues,
	sizeof(float*));
  for(i = 0 ; i < XoVarNValues; i++)
  	view->view.old_interps[i] = (float *)XtCalloc(20,
		sizeof(float));
}


static Boolean SetValues (curw, reqw, neww)
     Widget curw, reqw, neww;
{
  ViewObject curs = (ViewObject) curw;
  ViewObject news = (ViewObject) neww;
  ShapeObject sw;
  Boolean ret = False;
  int	max_npts = 0;
  int	i;

	/* if shapes or nshapes change then the number of points needs
	** recalculation and reallocation */
  if ( news->view.nshapes != curs->view.nshapes ||
  	news->view.shapes != curs->view.shapes ||
  	news->view.shapeflag != curs->view.shapeflag) {
	for(i = 0 ; i < news->view.nshapes; i++) {
		sw = (ShapeObject)(news->view.shapes[i].shape);
		if (max_npts < sw->shape.npts) 
			max_npts = sw->shape.npts;
	}
	if (max_npts > news->view.alloced_pts) {
		news->view.alloced_pts = max_npts + 20;
		news->view.pts =
			(XPoint *)XtRealloc((char *) news->view.pts,
				news->view.alloced_pts * sizeof(XPoint));
	}
	news->view.shapeflag = 0;
  }
  if (news->view.ncoords > curs->view.ncoords) {
		news->view.coords =
			(XPoint *)XtRealloc((char *) news->view.coords,
				news->view.ncoords * sizeof(XPoint));
		for(i = 0; i < XoVarNValues; i++){
			int j;

			news->view.old_interps[i] = 
				(float *)XtRealloc((char *) news->view.old_interps[i],
					news->view.ncoords * sizeof(float));
			for (j=curs->view.ncoords; j < news->view.ncoords; j++)
			    news->view.old_interps[i][j] = 0.0;
		}
  }

  /* These are the items which do need reprojection */
  if (news->view.viewmode != curs->view.viewmode ||
  	news->pix.tx != curs->pix.tx ||
  	news->pix.ty != curs->pix.ty ||
  	news->pix.tz != curs->pix.tz ||
  	news->view.xpts != curs->view.xpts ||
  	news->view.ypts != curs->view.ypts ||
  	news->view.zpts != curs->view.zpts ||
  	news->view.ncoords != curs->view.ncoords ||
  	news->view.nshapes != curs->view.nshapes ||
  	news->view.shapes != curs->view.shapes ||
  	news->view.view_values != curs->view.view_values ||
  	news->view.xoffset_val != curs->view.xoffset_val ||
  	news->view.yoffset_val != curs->view.yoffset_val ||
  	news->view.zoffset_val != curs->view.zoffset_val ||
  	news->view.linethickness_val != curs->view.linethickness_val ||
  	news->view.morph_val != curs->view.morph_val) {
		Project(news);
		ret = True;
	}
	if (ret && (news->pix.pixflags & XO_UPDATE_ALL_ON_SET))
		return(True);
	else 
		return(False);
}

static int CheckInterpColor(vw,new_interps,varno)
  ViewObject vw;
  double *new_interps;
  int	varno;
{
	int i;
	double val;
	double hival,loval,retval = 0.0,range;
	double highest, lowest;
	int hicol, locol;
  	unsigned long		*colscale = vw->view.colorscale;
  	int		col;
	int		valindex = vw->view.color_val -1;

	if (valindex >= 0 && valindex < XoVarNValues &&
	    vw->view.nshapes > 0 && vw->view.ncols > 0) {
		highest = vw->view.shapes[0].value_limit[valindex];
		hicol = vw->view.shapes[0].fgindex;
		lowest = vw->view.shapes[0].value_limit[valindex];
		locol = vw->view.shapes[0].fgindex;

		val = vw->view.view_values[valindex][varno];
		for(i = 1; i < vw->view.nshapes; i++) {
			hival = vw->view.shapes[i].value_limit[valindex];
			loval = vw->view.shapes[i-1].value_limit[valindex];

			if (hival > highest) {
			    highest = hival;
			    hicol = vw->view.shapes[i].fgindex;
			} else if (hival < lowest) {
			    lowest = hival;
			    locol = vw->view.shapes[i].fgindex;
			}

			range = hival-loval;
		        if(range != 0.0){
			 if ((val <= hival && val >= loval) || 
				(val >= hival && val <= loval)) {
				retval = vw->view.shapes[i-1].fgindex +
					(val - loval) *
					(double)(vw->view.shapes[i].fgindex -
						vw->view.shapes[i-1].fgindex) /
					(hival - loval);
				break;
			 }
			}
			else{
			 if(val == loval){
				 retval = vw->view.shapes[i-1].fgindex;
				 break;
			 }
			}
		}

		/*
		** If we didn't find val in any value range peg the
		** color at the color at the end neareest to val.
		*/
		if (i >= vw->view.nshapes) {
		    if (val < lowest)
			retval = (double) locol;
		    else
			retval = (double) hicol;
		}

		if (retval < 0.0) retval = 0.0;
		if (retval >= vw->view.ncols) retval = vw->view.ncols - 1;
		col = retval;

		new_interps[XoVarColorVal] = colscale[col];
		if (colscale[col] != vw->pix.fg) {
			vw->pix.fg = colscale[col];
			return(1);
		}
	}
	return(0);
}
    
static int CheckInterpMorph(vw,new_interps,shape_num,varno)
  ViewObject vw;
  double *new_interps;
  int	*shape_num;
  int	varno;
{
	int i;
	double val;
	double hival,loval,range;
	int		valindex = vw->view.morph_val -1;

	if (valindex >= 0 && valindex < XoVarNValues) {
		val = vw->view.view_values[valindex][varno];
		for(i = 1; i < vw->view.nshapes; i++) {
			hival = vw->view.shapes[i].value_limit[valindex];
			loval = vw->view.shapes[i-1].value_limit[valindex];
			range = hival - loval;
			if(range != 0.0){
			  if ((val <= hival && val >= loval) || 
				(val >= hival && val <= loval)) {
				new_interps[XoVarMorphVal] =
					(val - loval)/(hival - loval);
				*shape_num = i -1;
				return(1);
			  }
			}
			else{
			  if (val == loval) {
				new_interps[XoVarMorphVal] = 0.0; 
				*shape_num = i -1;
				return(1);
			  }
			}
		}
	} 
	*shape_num = 0;
	return(0);
}


/* For the X,Y,Z offsets we do a little hack: the new_interps for
** the X,Y,ZOffsetVal contain the actual calculated offsets to
** be applied in the DoOffset function.
** At this stage the ZOffsetVal does not contain anything.
*/
static int CheckInterpXOffset(vw,new_interps,varno)
  ViewObject vw;
  double *new_interps;
  int	varno;
{
	int i;
	double val;
	double hival,loval,range;
	int		valindex = vw->view.xoffset_val -1;

	if (valindex >= 0 && valindex < XoVarNValues) {
		val = vw->view.view_values[valindex][varno];
		for(i = 1; i < vw->view.nshapes; i++) {
			hival = vw->view.shapes[i].value_limit[valindex];
			loval = vw->view.shapes[i-1].value_limit[valindex];
			range = hival - loval;
			if(range != 0.0){
			 if ((val <= hival && val >= loval) || 
				(val >= hival && val <= loval)) {
				new_interps[XoVarXOffsetVal] = vw->view.shapes[i-1].xox +
					(val-loval) * 
					(vw->view.shapes[i].xox - vw->view.shapes[i-1].xox) /
					(hival - loval);
				new_interps[XoVarYOffsetVal] = vw->view.shapes[i-1].xoy +
					(val-loval) * 
					(vw->view.shapes[i].xoy - vw->view.shapes[i-1].xoy) /
					(hival - loval);
				return(1);
			 }
			}
			else{
			 if(val == loval){	
				new_interps[XoVarXOffsetVal] = vw->view.shapes[i-1].xox;
				new_interps[XoVarYOffsetVal] = vw->view.shapes[i-1].xoy;
				return(1);
			 }
			}
		}
	}
	return(0);
}

static int CheckInterpYOffset(vw,new_interps,varno)
  ViewObject vw;
  double *new_interps;
  int	varno;
{
	int i;
	double val;
	double hival,loval,range;
	int		valindex = vw->view.yoffset_val -1;

	if (valindex >= 0 && valindex < XoVarNValues) {
		val = vw->view.view_values[valindex][varno];
		for(i = 1; i < vw->view.nshapes; i++) {
			hival = vw->view.shapes[i].value_limit[valindex];
			loval = vw->view.shapes[i-1].value_limit[valindex];
			range = hival - loval;
			if(range != 0.0){
			 if ((val <= hival && val >= loval) || 
				(val >= hival && val <= loval)) {
				new_interps[XoVarXOffsetVal] +=
					vw->view.shapes[i-1].yox + (val-loval) * 
					(vw->view.shapes[i].yox - vw->view.shapes[i-1].yox) /
					(hival - loval);
				new_interps[XoVarYOffsetVal] += 
					vw->view.shapes[i-1].yoy + (val-loval) * 
					(vw->view.shapes[i].yoy - vw->view.shapes[i-1].yoy) /
					(hival - loval);
				return(1);
			 }
			}
			else{
			 if(val == loval){
				new_interps[XoVarXOffsetVal] += vw->view.shapes[i-1].yox;
				new_interps[XoVarYOffsetVal] += vw->view.shapes[i-1].yoy; 
				return(1);
			 }
			}
		}
	}
	return(0);
}

static int CheckInterpZOffset(vw,new_interps,varno)
  ViewObject vw;
  double *new_interps;
  int	varno;
{
	int i;
	double val;
	double hival,loval,range;
	int		valindex = vw->view.zoffset_val -1;

	if (valindex >= 0 && valindex < XoVarNValues) {
		val = vw->view.view_values[valindex][varno];
		for(i = 1; i < vw->view.nshapes; i++) {
			hival = vw->view.shapes[i].value_limit[valindex];
			loval = vw->view.shapes[i-1].value_limit[valindex];
			range = hival - loval;
			if(range != 0.0){
			 if ((val <= hival && val >= loval) || 
				(val >= hival && val <= loval)) {
				new_interps[XoVarXOffsetVal] +=
					vw->view.shapes[i-1].zox + (val-loval) * 
					(vw->view.shapes[i].zox - vw->view.shapes[i-1].zox) /
					(hival - loval);
				new_interps[XoVarYOffsetVal] +=
					vw->view.shapes[i-1].zoy + (val-loval) * 
					(vw->view.shapes[i].zoy - vw->view.shapes[i-1].zoy) /
					(hival - loval);
				return(1);
			 }
			}
			else{
			  if(val == loval){
				new_interps[XoVarXOffsetVal] += vw->view.shapes[i-1].zox; 
				new_interps[XoVarYOffsetVal] += vw->view.shapes[i-1].zoy;
				return(1); 
			  }
			}
		}
	}
	return(0);
}

static int CheckInterpText(vw,new_interps,varno)
  ViewObject vw;
  double *new_interps;
  int	varno;
{
  	if (vw->view.text_val > 0 && vw->view.text_val <= XoVarNValues) {
		new_interps[XoVarTextVal] =
			vw->view.view_values[vw->view.text_val-1][varno];
		return(1);
	}
	return(0);
}

static int CheckInterpTextColor(vw,new_interps,varno)
  ViewObject vw;
  double *new_interps;
  int	varno;
{
	int i;
	double val;
	double hival,loval,retval = 0.0,range;
  	unsigned long		*colscale = vw->view.colorscale;
  	int		col;
	int		valindex = vw->view.textcolor_val -1;

	if (valindex >= 0 && valindex < XoVarNValues) {
		val = vw->view.view_values[valindex][varno];
		for(i = 1; i < vw->view.nshapes; i++) {
			hival = vw->view.shapes[i].value_limit[valindex];
			loval = vw->view.shapes[i-1].value_limit[valindex];
			range = hival - loval;
			if(range != 0.0){
			 if ((val <= hival && val >= loval) || 
				(val >= hival && val <= loval)) {
				retval = vw->view.shapes[i-1].textcolorindex +
					(val - loval) *
					(double)(vw->view.shapes[i].textcolorindex -
						vw->view.shapes[i-1].textcolorindex) /
					(hival - loval);
				break;
			 }
			}
			else{
			 if(val == loval){
				retval = vw->view.shapes[i-1].textcolorindex;
				break; 
			 }
			}
		}
		if (retval < 0.0) retval = 0.0;
		if (retval >= vw->view.ncols) retval = vw->view.ncols -1;
		col = retval;
		/*
		if (col < 0) col = 0;
		if (col >= vw->view.ncols) col = vw->view.ncols -1;
		*/
		if (vw->view.ncols > 0 && colscale[col] != vw->view.textcolor) {
			vw->view.textcolor = colscale[col];
			new_interps[XoVarTextColorVal] = vw->view.textcolor;
			return(1);
		}
	}
	return(0);
}

static int CheckInterpLineThickness(vw,new_interps,varno)
  ViewObject vw;
  double *new_interps;
  int	varno;
{
	return(0);
}

static int DoMorph(vw,new_interps,shape_num,coordno)
  ViewObject vw;
  double *new_interps;
  int	shape_num;
  int	coordno;
{
	double val = new_interps[XoVarMorphVal];
	XPoint *points = vw->view.pts;
	int		maxnpts = vw->view.alloced_pts;
	XPoint *lopts,*hipts;
	int		lonpts,hinpts;
	int		i,j,k;
	ShapeObject sw;

	if (shape_num >= vw->view.nshapes -1) return(0);
	sw = (ShapeObject)vw->view.shapes[shape_num].shape;
	lopts = sw->shape.pts;
	lonpts = sw->shape.npts;
	sw = (ShapeObject)vw->view.shapes[shape_num + 1].shape;
	hipts = sw->shape.pts;
	hinpts = sw->shape.npts;

	if (maxnpts < hinpts || maxnpts < lonpts) {
		printf("Error in View.c DoMorph: number of allocated points < points in shape\n");
		return(0);
	}

	maxnpts = (hinpts > lonpts) ? hinpts : lonpts;

	/* doing the morphing */
	for(i = 0 ; i < maxnpts; i++) {
		j = (i >= lonpts) ? lonpts -1 : i;
		k = (i >= hinpts) ? hinpts -1 : i;
		points[i].x = lopts[j].x + val * (hipts[k].x - lopts[j].x);
		points[i].y = lopts[j].y + val * (hipts[k].y - lopts[j].y);
	}
	vw->view.npts = maxnpts;
	return(1);
}

/* Note that the new_interps values already have been offset
** by the coords[coordno] position for the var.
** This function is called even if there is no X Y or Z offset
** value, since we always need to calculate the position of this
** specific var according to the coords array */
static int DoOffset(vw,new_interps,was_morphed)
  ViewObject vw;
  double *new_interps;
  int	was_morphed;
{
	XPoint *points = vw->view.pts;
	int		maxnpts = vw->view.npts;
	int		ox = new_interps[XoVarXOffsetVal];
	int		oy = new_interps[XoVarYOffsetVal];
	int		i;

	if (vw->view.nshapes == 0) return(0);
	if (was_morphed) {
		if (maxnpts <= 0) return(0);
	
		/* doing the offsets */
		for(i = 0 ; i < maxnpts; i++) {
			points[i].x += ox;
			points[i].y += oy;
		}
		return(1);
	} else {
		ShapeObject sw = (ShapeObject)vw->view.shapes[0].shape;
		XPoint *origpts = sw->shape.pts;
		maxnpts = sw->shape.npts;
		if (maxnpts <= 0) return(0);
		if (maxnpts > vw->view.alloced_pts) {
			printf("Error in View.c DoOffset: number of allocated points < points in shape\n");
			return(0);
		}

		/* doing the offsets */
		for(i = 0 ; i < maxnpts; i++) {
			points[i].x = origpts[i].x + ox;
			points[i].y = origpts[i].y + oy;
		}
		return(1);
	}
}


static int DoText(vw,new_interps)
  ViewObject vw;
  double *new_interps;
{
	if (vw->view.textmode != TEXTNODRAW) {
		sprintf(vw->view.text,"%f",new_interps[XoVarTextVal]);
		return(1);
	}
	return(0);
}


static int DoLineThickness(vw,new_interps)
  ViewObject vw;
  double *new_interps;
{
	return(0);
}


static void Expose (w)
     Widget w;
{
  ViewObject vw = (ViewObject) w;
  int		i;
  int		do_view = 0; 
  int		do_undraw = 0;
  static double	new_interps[XoVarNVars];
			/* carries the result of
			** the interpolations, except for the Morph in which
			** it has the fractional value at which to do the
			** point interpolation */
  static double	old_interps[XoVarNVars]; /* ditto */

  int	new_shape_num; /* identifies the first of 2 shapes to interp */
  int	coordno;
  Pixel	next_fg;
  Pixel next_textfg;

  if (vw->pix.pixflags & XO_VISIBLE_NOT) return;
  if (vw->view.nshapes <= 0) return;
  if (vw->view.ncoords <= 0) return;

  for (coordno = 0 ; coordno < vw->view.ncoords; coordno++) {
	  do_view = 0;
	  for(i = 0; i < XoVarNValues; i++)
		old_interps[i] = vw->view.old_interps[i][coordno];

	  /* does color interpolation and setting if needed */
	  if (CheckInterpColor(vw,new_interps,coordno))
		do_view |= 1 << XoVarColorVal;
	
	  if (CheckInterpMorph(vw,new_interps,&new_shape_num,coordno))
		do_view |= 1 << XoVarMorphVal;
	  /* initialize the X,Y,Z offsets in the new_interps to the
	  ** offset of coordno */
	  new_interps[XoVarXOffsetVal] = vw->view.coords[coordno].x;
	  new_interps[XoVarYOffsetVal] = vw->view.coords[coordno].y;
	  new_interps[XoVarZOffsetVal] = 0.0;
	  if (CheckInterpXOffset(vw,new_interps,coordno))
		do_view |= 1 << XoVarXOffsetVal;
	  if (CheckInterpYOffset(vw,new_interps,coordno))
		do_view |= 1 << XoVarYOffsetVal;
	  if (CheckInterpZOffset(vw,new_interps,coordno))
		do_view |= 1 << XoVarZOffsetVal;
	  if (CheckInterpText(vw,new_interps,coordno))
		do_view |= 1 << XoVarTextVal;
	  if (CheckInterpTextColor(vw,new_interps,coordno))
		do_view |= 1 << XoVarTextColorVal;
	  if (CheckInterpLineThickness(vw,new_interps,coordno))
		do_view |= 1 << XoVarLineThicknessVal;
	
		/* Check for undraw */
		do_undraw = do_view & (
			(1 << XoVarMorphVal) |
			(1 << XoVarXOffsetVal) |
			(1 << XoVarYOffsetVal) |
			(1 << XoVarZOffsetVal) |
			(1 << XoVarTextVal) |
			(1 << XoVarLineThicknessVal));
		if (do_undraw) {
/* doing the undraw - basically redraw in bg */
	/* Set the foregrounds to the background */
			next_fg = vw->pix.fg;
	  		XSetForeground(XtDisplay(XtParent(w)),vw->pix.gc,
				vw->view.bg);
	
			if (do_view & (1 << XoVarTextColorVal))
				next_textfg = new_interps[XoVarTextColorVal];
			else
				next_textfg = vw->view.textcolor;
	  		XSetForeground(XtDisplay(XtParent(w)),vw->view.textgc,
				vw->view.bg);
	
	/* Doing the undraw */
	  		/* The morphing function fills the points array. */
			if (do_view & (1 << XoVarMorphVal))
				DoMorph(vw,old_interps,new_shape_num,coordno);
			/* This function handles all the offsets, including
			** the ones required to position this var according
			** to coords[coordno]. If there is no morphing,
			** the pts array is filled from shape 0. */
			if (do_view)
				DoOffset(vw,old_interps,
					(do_view & (1 << XoVarMorphVal)));
			if (do_view & (1 << XoVarTextVal))
				DoText(vw,old_interps);
			if (do_view & (1 << XoVarLineThicknessVal))
				DoLineThickness(vw,old_interps);
			DrawPoints(vw);
	
	/* Set the foregrounds to the next values */
	  		XSetForeground(XtDisplay(XtParent(w)),vw->pix.gc,
				next_fg);
	  		XSetForeground(XtDisplay(XtParent(w)),vw->view.textgc,
				next_textfg);
		} else {
			/* Set the foregrounds to the next values */
			XSetForeground(XtDisplay(XtParent(w)),vw->pix.gc,
					(Pixel)new_interps[XoVarColorVal]);
			XSetForeground(XtDisplay(XtParent(w)),vw->view.textgc,
					(Pixel)new_interps[XoVarTextColorVal]);
		}
	
/* Check for new draws */
			/* setting up the 'old_interps' for the next cycle */
			for(i = 0; i < XoVarNValues; i++)
				vw->view.old_interps[i][coordno] = new_interps[i];
	  		/* The morphing function fills the points array. */
			/* the fg has already been done while setting fgs above */
	
			if (do_view & (1 << XoVarMorphVal))
				DoMorph(vw,new_interps,new_shape_num,coordno);
			/* This function handles all the offsets, including
			** the ones required to position this var according
			** to coords[coordno]. If there is no morphing,
			** the pts array is filled from shape 0. */
			DoOffset(vw,new_interps,
				(do_view & (1 << XoVarMorphVal)));
			if (do_view & (1 << XoVarTextVal))
				DoText(vw,new_interps);
	
			/* the textcolor has already been done */
	
			if (do_view & (1 << XoVarLineThicknessVal))
				DoLineThickness(vw,new_interps);

			/** The points are drawn whenever expose is called **/
			DrawPoints(vw);
	}
}

static void DrawPoints(w)
	Widget w;
{
  ViewObject vw = (ViewObject) w;
  XPoint	*points;
  XSegment	*segs;
  XRectangle	*rects;
  int		nsegs;
  int		i,j;
  int		dx,dy;
  int		rx,ry;
  int		len;

  if (vw->view.npts > 0) {
		points=vw->view.pts;
  	switch(vw->view.drawmode) {
		case DRAWLINES:
  			XoDrawLines(XtDisplay(XtParent(w)), XtWindow(XtParent(w)),
			vw->pix.gc, points,vw->view.npts,CoordModeOrigin); 
			break;
		case DRAWSEGS:
			segs = (XSegment *)
				XtCalloc(vw->view.npts/2,sizeof(XSegment));
			for(i=0;i<vw->view.npts;i++) {
				j=i/2;
				segs[j].x1 = points[i].x;
				segs[j].y1 = points[i].y;
				i++;
				segs[j].x2 = points[i].x;
				segs[j].y2 = points[i].y;
			}
  			XoDrawSegments(XtDisplay(XtParent(w)),XtWindow(XtParent(w)),
			vw->pix.gc, segs,vw->view.npts/2,CoordModeOrigin); 
			XtFree((char *) segs);
			break;
		case DRAWPOINTS:
  			XoDrawPoints(XtDisplay(XtParent(w)), XtWindow(XtParent(w)),
			vw->pix.gc, points,vw->view.npts,CoordModeOrigin); 
			break;
		case DRAWARROWS:
			nsegs = vw->view.npts / 2;
			segs = (XSegment *)
				XtCalloc(3 * nsegs,sizeof(XSegment));
			for(i=0;i<vw->view.npts - 1;i++) {
				j=i/2;
				segs[j].x1 = points[i].x;
				segs[j].y1 = points[i].y;
				i++;
				segs[j].x2 = points[i].x;
				segs[j].y2 = points[i].y;
				rx = segs[j].x2 - segs[j].x1;
				ry = segs[j].y2 - segs[j].y1;
				dx = rx - ry;
				dy = rx + ry;
				len = sqrt((double)(dx * dx + dy * dy));
				if (len >=1) {
					dx = (dx * XO_ARROWHEAD_LEN)/len;
					dy = (dy * XO_ARROWHEAD_LEN)/len;
				} else {
					dx = dy = 0;
				}
				segs[j+nsegs] = segs[j];
				segs[j+nsegs].x1 = segs[j].x2 - dx;
				segs[j+nsegs].y1 = segs[j].y2 - dy;
				segs[j+nsegs+nsegs] = segs[j];
				segs[j+nsegs+nsegs].x1 = segs[j].x2 - dy;
				segs[j+nsegs+nsegs].y1 = segs[j].y2 + dx;
			}
  			XoDrawSegments(XtDisplay(XtParent(w)),
				XtWindow(XtParent(w)),
				vw->pix.gc, segs,3 * nsegs,CoordModeOrigin); 
			XtFree((char *) segs);
			break;
		case FILLRECTS:
			rects = (XRectangle *)
				XtCalloc(vw->view.npts/2,sizeof(XRectangle));
			for(i=0;i<vw->view.npts -1 ;i+=2) {
				j=i/2;
				dx = points[i].x-points[i+1].x;
				dy = points[i].y-points[i+1].y;
				if (dx < 0) {
					rects[j].x = points[i].x;
					rects[j].width = -dx;
				} else {
					rects[j].x = points[i+1].x;
					rects[j].width = dx;
				}
				if (dy < 0) {
					rects[j].y = points[i].y;
					rects[j].height = -dy;
				} else {
					rects[j].y = points[i+1].y;
					rects[j].height = dy;
				}
			}
  			XoFillRectangles(XtDisplay(XtParent(w)),
				XtWindow(XtParent(w)),
				vw->pix.gc, rects,vw->view.npts/2,CoordModeOrigin); 
			XtFree((char *) rects);
			break;
		case FILLPOLY:
  			XoFillPolygon(XtDisplay(XtParent(w)), XtWindow(XtParent(w)),
			vw->pix.gc, points,vw->view.npts,CoordModeOrigin); 
			break;
		default:
			break;
  	}
  }
  if (vw->view.text != NULL && (int)strlen(vw->view.text) > 0) {
  	switch(vw->view.textmode) {
		case TEXTDRAW:
			XoDrawCenteredString(XtDisplay(XtParent(w)),
				XtWindow(XtParent(w)), vw->view.textgc,
				vw->view.textfont,
				vw->pix.cx,vw->pix.cy,
				vw->view.text,strlen(vw->view.text));
			break;
		case TEXTFILL:
			XoFillCenteredString(XtDisplay(XtParent(w)),
				XtWindow(XtParent(w)), vw->view.textgc,
				vw->view.textfont,
				vw->pix.cx,vw->pix.cy,
				vw->view.text,strlen(vw->view.text));
			break;
		case TEXTNODRAW:
			/* do not draw anything */
			break;
		default:
			break;
  	}
  }
  if (vw->pix.pixflags & XO_IS_SELECTED) { /* do the highlight */
	vw->pix.pixflags &= ~XO_IS_SELECTED;
	(((PixObjectClass)
		(vw->object.widget_class))->pix_class.highlight)(vw,NULL);
  }
}

static void Destroy(w)
	Widget w;
{
  ViewObject vw = (ViewObject) w;
  int i;

  XtReleaseGC(XtParent(w),vw->pix.gc);
  XtReleaseGC(XtParent(w),vw->view.textgc);
  if (vw->view.text)
	XtFree(vw->view.text);
  if (vw->view.alloced_pts > 0 && vw->view.pts)
  	XtFree((char *)(vw->view.pts));
  if (vw->view.ncoords > 0 && vw->view.coords)
  	XtFree((char *)(vw->view.coords));
  if (vw->view.ncoords > 0 && vw->view.old_interps) {
	for(i = 0; i < XoVarNValues; i++) {
		if (vw->view.old_interps[i])
  			XtFree((char *)(vw->view.old_interps[i]));
	}
  	XtFree((char *)(vw->view.old_interps));
  }
}

/* We dont really need this since the parent widget has complete
** control over the gadget layout */
static XtGeometryResult QueryGeometry(w,intended,preferred)
	Widget w;
	XtWidgetGeometry *intended, *preferred;
{
	ViewObject vw = (ViewObject)w;
	preferred->x = vw->pix.x;
	preferred->y = vw->pix.y;
	preferred->width = vw->pix.w;
	preferred->height = vw->pix.h;
	if (intended->request_mode & (CWX | CWY | CWWidth | CWHeight)) {
		if (preferred->x == intended->x &&  
			preferred->y == intended->y &&  
			preferred->width == intended->width &&  
			preferred->height == intended->height)
			return(XtGeometryYes);
	} else {
		if (preferred->x == vw->pix.x &&  
			preferred->y == vw->pix.y &&  
			preferred->width == vw->pix.w &&  
			preferred->height == vw->pix.h)
			return(XtGeometryYes);
	}
	return(XtGeometryAlmost);
}




static void Project (w)
     Widget w;
{
  int x,y,z;
  int ox,oy,oz;
  int cx,cy,cz;
  ViewObject vw = (ViewObject) w;
  CoreDrawWidgetClass class = (CoreDrawWidgetClass)XtClass(XtParent(w));
  int parentht= ((CoreDrawWidget)XtParent(w))->core.height;
  int	*px,*py,*pz;
  int	i,j;
  int		npts;
  XPoint	*points;
  void	(*tpt)();
  void	(*tpts)();
  varShape	*vshape;
  ShapeObject	sw;
  double *xpts;
  double *ypts;
  double *zpts;
  int		maxx, maxy, minx, miny;

  if (vw->view.ncoords == 0 || vw->view.xpts == NULL ||
	vw->view.ypts == NULL || vw->view.zpts == NULL ||
	vw->view.nshapes == 0 || vw->view.shapes == NULL) return;
  if (vw->pix.pixflags & XO_VISIBLE_NOT) return;
  if (vw->pix.pixflags & XO_RESIZABLE_NOT){ /*Freeze size and position*/
		return;
  }
  if (vw->pix.pixflags & XO_PIXELCOORDS) { /* transform using pixels*/
		tpts = PixelTransformPoints;
  } else { /* use standard transformations */
		tpts = class->coredraw_class.transformpts;
  }
  if (vw->pix.pixflags & XO_PIXELOFFSET) { /* transform using pixels*/
		tpt = PixelTransformPoint;
  } else { /* use standard transformations */
		tpt = class->coredraw_class.transformpt;
  }

  if (vw->pix.pixflags & XO_PCTCOORDS) { /* transform using % of win */
		tpts = PctTransformPoints;
  }
  if (vw->pix.pixflags & XO_PCTOFFSET) { /* transform using % of win */
		tpt = PctTransformPoint;
  }

  /* Calculate offset on screen using parents TransformPoint routine */
  (tpt)(XtParent(w),
	vw->pix.tx,vw->pix.ty, vw->pix.tz,&x,&y,&z);
	vw->pix.cx = x; vw->pix.cy = y; vw->pix.cz=z;

  	/* Calculate origin on screen using parents
  	** TransformPoint routine, only if it is usual transforms */
	if (!(vw->pix.pixflags & (XO_PIXELCOORDS | XO_PCTCOORDS))) {
   		(tpt)(XtParent(w),
			(float)0.0,(float)0.0,(float)0.0, &ox,&oy,&oz);

		/* Figure out required offset for points being drawn */
		x-= ox;
		y-= oy;
		z-= oz;
	} else {
		/*needed because both transforms include it*/
		y -= parentht;
	}

/* Apply the transforms to all the view coords */
  	px = (int *) XtCalloc(vw->view.ncoords,sizeof(int));
  	py = (int *) XtCalloc(vw->view.ncoords,sizeof(int));
  	pz = (int *) XtCalloc(vw->view.ncoords,sizeof(int));
/* Apply the offsets */
	xpts = vw->view.xpts;
	ypts = vw->view.ypts;
	zpts = vw->view.zpts;
	for(j=0;j<vw->view.ncoords;j++) {
		xpts[j] += vw->pix.tx;
		ypts[j] += vw->pix.ty;
		zpts[j] += vw->pix.tz;
	}
/* do the transform */
	(tpts)(XtParent(w),
		xpts, ypts, zpts,
		px,py,pz,vw->view.ncoords);
/* store the transformed coords */
	points = (XPoint *)vw->view.coords;
	/* The ox and oy are subtracted here */
	if (!(vw->pix.pixflags & (XO_PIXELCOORDS | XO_PCTCOORDS))) {
		for(j=0;j<vw->view.ncoords;j++) {
			points[j].x = px[j] - ox;
			points[j].y = py[j] - oy;
		}
	} else {
		for(j=0;j<vw->view.ncoords;j++) {
			points[j].x = px[j];
			points[j].y = py[j] - parentht;
		}
	}

/* Find the bounds to use for the xview pix */
	maxx = minx = px[0];
    maxy = miny =  py[0];
    for(j=1;j<vw->view.ncoords;j++) {
        if (maxx < px[j]) maxx = px[j];
        if (maxy < py[j]) maxy = py[j];
        if (minx > px[j]) minx = px[j];
        if (miny > py[j]) miny = py[j];
    }
    vw->pix.x = minx - 10;
    vw->pix.y = miny - 10;
    vw->pix.w = maxx - minx + 20;
    vw->pix.h = maxy - miny + 20;

	XtFree((char *) px);
	XtFree((char *) py);
	XtFree((char *) pz);
	/* Remove the offsets */
	for(j=0;j<vw->view.ncoords;j++) {
		xpts[j] -= vw->pix.tx;
		ypts[j] -= vw->pix.ty;
		zpts[j] -= vw->pix.tz;
	}
	

/* Apply the transforms individualy to each varShape */
  for (i = 0 ; i < vw->view.nshapes; i++) {
	vshape = &(vw->view.shapes[i]);
	if (vshape != NULL) { /* this check is probably not needed */
  		sw = (ShapeObject)vshape->shape;

  		/* Calculate center of pix on screen using 
		TransformPoint routine */
  		(tpt)(XtParent(w),
			sw->pix.tx,sw->pix.ty,sw->pix.tz,&cx,&cy,&cz);
		if (vw->pix.pixflags & (XO_PIXELCOORDS | XO_PCTCOORDS)) {
			sw->pix.cx = cx; sw->pix.cy = cy-parentht ;
		} else {
			sw->pix.cx = cx -ox ; sw->pix.cy = cy-oy ;
		}

  		/* Calculate xoffset on screen using parents
		TransformPoint routine */
  		(tpt)(XtParent(w),
			sw->pix.tx,(float)0.0,(float)0.0,&cx,&cy,&cz);
		if (vw->pix.pixflags & (XO_PIXELCOORDS | XO_PCTCOORDS)) {
			vshape->xox = cx; vshape->xoy = cy-parentht ;
		} else {
			vshape->xox = cx-ox; vshape->xoy = cy-oy;
		}

  		/* Calculate yoffset on screen using parents
		TransformPoint routine */
  		(tpt)(XtParent(w),
			(float)0.0,sw->pix.ty,(float)0.0, &cx,&cy,&cz);
		if (vw->pix.pixflags & (XO_PIXELCOORDS | XO_PCTCOORDS)) {
			vshape->yox = cx; vshape->yoy = cy-parentht ;
		} else {
			vshape->yox = cx-ox; vshape->yoy = cy-oy;
		}

  		/* Calculate zoffset on screen using parents
		TransformPoint routine */
  		(tpt)(XtParent(w),
			(float)0.0,(float)0.0,sw->pix.tz,&cx,&cy,&cz);
		if (vw->pix.pixflags & (XO_PIXELCOORDS | XO_PCTCOORDS)) {
			vshape->zox = cx; vshape->zoy = cy-parentht ;
		} else {
			vshape->zox = cx-ox; vshape->zoy = cy-oy;
		}

  		if (sw->shape.npts > 0) {
	  		px = (int *) XtCalloc(sw->shape.npts,sizeof(int));
	  		py = (int *) XtCalloc(sw->shape.npts,sizeof(int));
	  		pz = (int *) XtCalloc(sw->shape.npts,sizeof(int));
		
	  		(tpts)(XtParent(w),
				sw->shape.xpts,sw->shape.ypts, sw->shape.zpts,
				px,py,pz,sw->shape.npts);

	  		points = (XPoint *)sw->shape.pts;
			/* Note that we do NOT add x and y here, which we did
			** in the var gadget. The values corresponding to
			** x and y are stored in the coords array and 
			** added when in the expose routine */
			for(j=0;j<sw->shape.npts;j++) {
				points[j].x = px[j];
				points[j].y = py[j];
			}
	  		XtFree((char *) px);
	  		XtFree((char *) py);
	  		XtFree((char *) pz);
		}
	}
  }
/* Copy over the pts for shape 0 to the pts array in case morphing
** is not being used */
	sw = (ShapeObject)(vw->view.shapes[0].shape);
	points = sw->shape.pts;
	npts = ((ShapeObject)(vw->view.shapes[0].shape))->shape.npts;
	if (npts > vw->view.alloced_pts) {
		printf("Error:  Project in View.c: npts allocation error\n");
	}
	for(i = 0 ; i < npts; i++) {
		vw->view.pts[i] = points[i];
	}
	vw->view.npts = npts;
/* Copy over the other standard values */
	vw->view.drawmode = sw->shape.drawmode;
	strncpy(vw->view.text,sw->shape.text,XO_VIEW_MAX_TEXT_CHARS);
	/* just to be safe */
	vw->view.text[XO_VIEW_MAX_TEXT_CHARS -1] = '\0';
	vw->view.textcolor = sw->shape.textcolor;
	vw->view.textfont = sw->shape.textfont;
	vw->pix.fg = sw->pix.fg;
}

/* dist is < 0 if always selected 
** 0 <= dist < max_dist if perhaps selected
** dist > max_dist if not selected
** The XoFindNearestPix routine has already excluded
** cases where the click point is outside the pix select region 
** so all we have to do here is to figure out dist
*/
#ifndef sqr
#define sqr(x) ((x) * (x))
#endif
/* Note that this routine has to pass back a sensible value in cz
** in order to tell click_select routines the z position of the 
** point that was clicked on
*/
static void SelectDistance (w,x,y,dist)
     Widget w;
	 Position x,y;
	 Dimension *dist;
{
	int ans;
	ViewObject vw = (ViewObject)w;
	ShapeObject sw = (ShapeObject)(vw->view.shapes[0].shape);
	int dx,dy;
	int i,besti;
	int	bestr;
	float meanx = 0;
	float meany = 0;

	/* use XO_MAX_CLICK_DIST */
	if (vw->pix.pixflags & XO_CLICKABLE_NOT) {
		*dist = XO_MAX_SEL_DIST;
		return;
	}

	/* This is ugly and may not work if there is a large tx etc */
	if (sw->shape.npts > 0) {
		for(i = 0 ; i < sw->shape.npts; i++) {
			meanx += sw->shape.pts[i].x;
			meany += sw->shape.pts[i].y;
		}
		x -= meanx/sw->shape.npts;
		y -= meany/sw->shape.npts;
	} else {
		x -= sw->pix.cx;
		y -= sw->pix.cy;
	}

	bestr = sqr(XO_MAX_SEL_DIST);
	besti = -1;
	for(i = 0; i < vw->view.ncoords; i++) {
		dx = vw->view.coords[i].x - x;
		dy = vw->view.coords[i].y - y;
		ans = dx * dx + dy * dy;
		if (ans < bestr) {
			bestr = ans;
			besti = i;
		}
	}
	/* This is the same hack as in Tree.c to avoid claiming
	** blank regions of the screen which are surrounded by View
	** components */
	if (bestr > XO_VIEW_MAX_SEL_DIST) {
		bestr = sqr(XO_MAX_SEL_DIST);
		besti = -1;
	}
	ans = (int)sqrt((double)bestr);
	*dist = (Dimension) (ans > 0 ? ans : 0);
	vw->pix.selected = besti;
	if (besti >= 0 && besti < vw->view.ncoords)
		vw->pix.cz = vw->view.zpts[besti];
	else
		vw->pix.cz = 0;
		
	return;
}
#undef sqr


static void Select(w,event,info)
    Widget w;
    XButtonEvent    *event;
    XoEventInfo     *info;
{
    ViewObject  vw = (ViewObject)w;
    PixObjectClass class = (PixObjectClass) vw->object.widget_class;
    static int ret;
 
    if (vw->pix.selected < 0 || vw->pix.selected >= vw->view.ncoords)
        return;
 
    (class->pix_class.highlight)(w,event);
 
    ret = vw->pix.selected;
    info->ret = (XtPointer)(&ret);
 
    /*
    ** The following original code makes no sense, because pix.w and
    ** pix.h are unsigned quantities.  I'm eliminating the tests.
    ** wid = (vw->pix.w < 0) ? 0 : vw->pix.w;
    ** h = (vw->pix.h < 0) ? 0 : vw->pix.h;
    */
    XoCopyCursor(XtDisplay(XtParent(w)),XtWindow(XtParent(w)),
        vw->pix.gc,
        vw->pix.x, vw->pix.y, vw->pix.w, vw->pix.h);
 
    XtCallCallbackList(w,vw->pix.callbacks, (XtPointer)info);
}


static Boolean XoCvtStringToViewmode(dpy,args,num_args,fromVal,toVal,
	destruct)
     Display     *dpy;          /* unused */
     XrmValuePtr args;          /* unused */
     Cardinal    *num_args;     /* unused */
     XrmValuePtr fromVal;
     XrmValuePtr toVal;
     XtPointer   destruct;      /* unused */
{
	static int ret;
	ret = -1;

	if (fromVal->addr != NULL) {
		if (strcmp(fromVal->addr,"shape") == 0)
			ret = XoVarShapeMode;
		if (strcmp(fromVal->addr,"colorview") == 0)
			ret = XoVarShapeMode;
		if (strcmp(fromVal->addr,"boxview") == 0)
			ret = XoVarShapeMode;
		if (strcmp(fromVal->addr,"fillboxview") == 0)
			ret = XoVarShapeMode;
		if (strcmp(fromVal->addr,"colorboxview") == 0)
			ret = XoVarShapeMode;
		if (strcmp(fromVal->addr,"graph") == 0)
			ret = XoVarGraphMode;
		if (strcmp(fromVal->addr,"surface") == 0)
			ret = XoVarSurfaceMode;
		if (strcmp(fromVal->addr,"contour") == 0)
			ret = XoVarContourMode;
		if (ret >=0) {
			XoCvtDone(int,ret);
		}
	}
	toVal->addr = NULL;
	toVal->size = 0;
	XtDisplayStringConversionWarning(dpy,fromVal->addr,"XtRViewmode");
		return((Boolean) False);
}

void XoUpdateView(w)
 Widget w;
{
	Expose(w);
}

void XoProjectView(w)
 Widget w;
{
	Project(w);
	Expose(w);
}


#undef DRAWLINES
#undef DRAWSEGS
#undef DRAWPOINTS
#undef DRAWRECTS
#undef DRAWARROWS
#undef FILLRECTS
#undef FILLPOLY

#undef	TEXTDRAW
#undef	TEXTFILL
#undef	TEXTNODRAW
#undef XO_ARROWHEAD_LEN
