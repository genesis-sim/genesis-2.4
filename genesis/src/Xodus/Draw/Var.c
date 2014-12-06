/* $Id: Var.c,v 1.3 2005/07/01 10:02:31 svitak Exp $ */
/*
 * $Log: Var.c,v $
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
 * Revision 1.17  2000/06/12 04:28:18  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.16  2000/05/02 06:06:42  mhucka
 * Added type casts for certain function call arguments.
 *
 * Revision 1.15  1998/01/20 00:24:42  venkat
 * Included-stdio.h-for-prototyping-sprintf.-Caused-undefined-results-on-64-bit-Irix
 *
 * Revision 1.14  1996/07/02  16:50:50  venkat
 * Fixed a memory leak for the pts array of the Var. It is now freed in roy
 * method for the widget since it is a private member and allocated locally in this
 * module
 *
 * Revision 1.13  1996/06/20  23:19:09  venkat
 * Changed type of the colorscale resource and any local variables attempting
 * to get the list of colortable indices, from int to long. On the alpha these
 * types are different sizes and information is lost in the scale-down process leading to
 * unpredictable color index values and behaviour.
 *
 * Revision 1.12  1995/12/06  01:11:22  venkat
 * Fixed bugs in the XoUpdateVar and XoProjectVar wrapper functions
 * to identify the passed argument as a widget and not consider it by
 * default as an int - Caused segmentation faults on the 64-bit alphas.
 *
 * Revision 1.11  1995/08/05  17:28:09  dhb
 * Changed CheckInterpMorph() to set the interpolated value in the
 * case of a zero value range between shapes to zero.
 *
 * Revision 1.10  1995/08/02  01:55:19  venkat
 * Code to check the interpolation range != 0 and if it is the case use the
 *
 * lower limit.
 *
 * Revision 1.9  1995/07/11  22:08:41  venkat
 * The do_var bit is set to 1 in the Expose method. Reexpose will therefore be
 * done whenever it is called.
 *
 * Revision 1.8  1995/06/16  05:47:45  dhb
 * Merged in 1.5.1.1 changes.
 *
<<<<<<< 1.7
 * Revision 1.7  1995/06/12  17:13:26  venkat
 * More-Resource-Converter-clean-up-(fromVal->addr!=NULL)-is-checked-instead-of-checking-(fromVal->size)
 *
 * Revision 1.6  1995/06/02  19:46:02  venkat
 * Resource-converters-clean-up
 *
=======
 * Revision 1.5.1.1  1995/06/16  05:38:18  dhb
 * Fixes for arithmetic errors in color lookups, by Upi Bhalla.
 *
>>>>>>> 1.5.1.1
 * Revision 1.5  1994/11/07  22:20:32  bhalla
 * Put in better calculation for default click location of var pix.
 * The SelectDistance routine still needs work
 *
 * Revision 1.4  1994/06/13  23:08:58  bhalla
 * Major bugfix involving valindex
 *
 * Revision 1.3  1994/03/22  15:31:59  bhalla
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
#include "Xo/Xo_ext.h"
#include "CoreDrawP.h"
#include "ShapeP.h"
#include "VarP.h"

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

#define XO_VAR_MAX_TEXT_CHARS 50

#define offset(field) XtOffset(VarObject, var.field)
static XtResource resources[] = {
  {XtNvarmode, XtCVarmode, XtRVarmode, sizeof(int),
     offset(varmode), XtRString, "shape"},
  {XtNnshapes, XtCNshapes, XtRInt, sizeof(int),
     offset(nshapes), XtRString, "0"},
  {XtNshapeflag, XtCShapeflag, XtRInt, sizeof(int),
     offset(shapeflag), XtRString, "0"},
  {XtNvarShapes, XtCVarShapes, XtRPointer, sizeof(varShape*),
     offset(shapes), XtRImmediate, (XtPointer)NULL},
  {XtNvarValues, XtCVarValues, XtRPointer, sizeof(double*),
     offset(var_values), XtRImmediate, (XtPointer)NULL},
  {XtNcolorscale, XtCColorscale, XtRPointer, sizeof(unsigned long*),
     offset(colorscale), XtRImmediate, (XtPointer)NULL},
  {XtNncols, XtCNcols, XtRInt, sizeof(int),
     offset(ncols), XtRString, "0"},
  {XtNvarColorVal, XtCVarSelect, XtRInt, sizeof(int),
     offset(color_val), XtRString, "1"},
  {XtNvarMorphVal, XtCVarSelect, XtRInt, sizeof(int),
     offset(morph_val), XtRString, "0"},
  {XtNvarXOffsetVal, XtCVarSelect, XtRInt, sizeof(int),
     offset(xoffset_val), XtRString, "0"},
  {XtNvarYOffsetVal, XtCVarSelect, XtRInt, sizeof(int),
     offset(yoffset_val), XtRString, "0"},
  {XtNvarZOffsetVal, XtCVarSelect, XtRInt, sizeof(int),
     offset(zoffset_val), XtRString, "0"},
  {XtNvarTextVal, XtCVarSelect, XtRInt, sizeof(int),
     offset(text_val), XtRString, "0"},
  {XtNvarTextColorVal, XtCVarSelect, XtRInt, sizeof(int),
     offset(textcolor_val), XtRString, "0"},
  {XtNvarLineThicknessVal, XtCVarSelect, XtRInt, sizeof(int),
     offset(linethickness_val), XtRString, "0"},
};
#undef offset


/* methods */

/* I have no idea where Expose is getting defined
*/
#ifdef Expose
#undef Expose
#endif
static Boolean XoCvtStringToVarmode();
static void SelectDistance();
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


VarClassRec varClassRec = {
  { /* RectObj (Core) Fields */

    /* superclass         */    (WidgetClass) &pixClassRec,
    /* class_name         */    "Var",
    /* size               */    sizeof(VarRec),
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
    /* select */  	         	XoInheritSelect,
    /* unselect */ 	         	XoInheritUnSelect,
    /* motion */  	         	XoInheritMotion,
    /* hightlight */          	XoInheritHighlight,
    /* extension */             NULL
  },
  {
	/* var fields */
	/* make_compiler_happy */	0
  }
};

WidgetClass varObjectClass = (WidgetClass)&varClassRec;

static void ClassInitialize()
{
  XtSetTypeConverter (XtRString, XtRVarmode,
                      XoCvtStringToVarmode, NULL, 0 ,
                      XtCacheNone, NULL);
}

static void Initialize (req, new)
     Widget req, new;
{
  VarObject var = (VarObject) new;
  CoreDrawWidget dw = (CoreDrawWidget)XtParent(new);
  XGCValues values;
  XtGCMask mask = GCForeground | GCLineStyle | GCCapStyle | 
	GCJoinStyle | GCLineWidth;
  Display 	*disp = XtDisplay(XtParent(new));
  Drawable d = XtWindow(XtParent(new));
  values.foreground = XBlackPixel(disp,XDefaultScreen(disp));
  values.background = XWhitePixel(disp,XDefaultScreen(disp));
  values.line_width = 0;
  values.line_style = LineSolid;
  values.cap_style = CapNotLast;
  values.join_style = JoinMiter;

  /* Check that the initial values are reasonable */
	var->var.varmode = XoVarShapeMode;
	var->var.textcolor = XBlackPixel((Display *)XgDisplay(),0);
		
  var->pix.gc=XCreateGC(disp,d,mask,&values);
  mask = GCForeground;
  var->var.textgc=XCreateGC(disp,d,mask,&values);
  /* 20 is a reasonable initial value for the size of the pts cache */
  var->var.text = XtCalloc(XO_VAR_MAX_TEXT_CHARS,sizeof(char));
								/* The text field is permanently
								** allocated for doing numbers */
	/* Perhaps I should do an XtGetValues here ... */
	var->var.bg = dw->core.background_pixel;
  var->var.pts =
	(XPoint *) XtCalloc(20,sizeof(XPoint));
	var->var.alloced_pts = 20;
}


static Boolean SetValues (curw, reqw, neww)
     Widget curw, reqw, neww;
{
  VarObject curs = (VarObject) curw;
  VarObject news = (VarObject) neww;
  ShapeObject sw;
  Boolean ret = False;
  int	max_npts = 0;
  int	i;

	/* if shapes or nshapes change then the number of points needs
	** recalculation and reallocation */
  if ( news->var.nshapes != curs->var.nshapes ||
  	news->var.shapes != curs->var.shapes ||
  	news->var.shapeflag != curs->var.shapeflag) {
	for(i = 0 ; i < news->var.nshapes; i++) {
		sw = (ShapeObject)(news->var.shapes[i].shape);
		if (max_npts < sw->shape.npts) 
			max_npts = sw->shape.npts;
	}
	if (max_npts > news->var.alloced_pts) {
		news->var.alloced_pts = max_npts + 20;
		news->var.pts =
			(XPoint *)XtRealloc((char *) news->var.pts,
				news->var.alloced_pts * sizeof(XPoint));
	}
	news->var.shapeflag = 0;
  }

  /* These are the items which do need reprojection */
  if (news->var.varmode != curs->var.varmode ||
  	news->pix.tx != curs->pix.tx ||
  	news->pix.ty != curs->pix.ty ||
  	news->pix.tz != curs->pix.tz ||
  	news->var.nshapes != curs->var.nshapes ||
  	news->var.shapes != curs->var.shapes ||
  	news->var.var_values != curs->var.var_values ||
  	news->var.xoffset_val != curs->var.xoffset_val ||
  	news->var.yoffset_val != curs->var.yoffset_val ||
  	news->var.zoffset_val != curs->var.zoffset_val ||
  	news->var.linethickness_val != curs->var.linethickness_val ||
  	news->var.morph_val != curs->var.morph_val) {
		Project(news);
		ret = True;
	}
	if (ret && (news->pix.pixflags & XO_UPDATE_ALL_ON_SET))
		return(True);
	else 
		return(False);
}

static int CheckInterpColor(vw,new_interps)
  VarObject vw;
  double *new_interps;
{
	int i;
	double val;
	double hival,loval,retval = 0.0,range;
  	unsigned long		*colscale = vw->var.colorscale;
  	int		col;
	int		valindex = vw->var.color_val - 1;

  	if (valindex >= 0 && valindex < XoVarNValues){
		val = vw->var.var_values[valindex];
		for(i = 1; i < vw->var.nshapes; i++) {
			hival = vw->var.shapes[i].value_limit[valindex];
			loval = vw->var.shapes[i-1].value_limit[valindex];
			range = hival - loval;
			if (range != 0.0){
			 if ((val <= hival && val >= loval) || 
				(val >= hival && val <= loval)) {
				retval = vw->var.shapes[i-1].fgindex +
					(val - loval) *
					(double)(vw->var.shapes[i].fgindex -
						vw->var.shapes[i-1].fgindex) /
					(hival - loval);
				break;
			 }
			}
			else{
				if(val == loval){
					retval = vw->var.shapes[i-1].fgindex;
					break;
				}
			}
		}
		if (retval < 0.0) retval = 0.0;
		if (retval >= vw->var.ncols) retval = vw->var.ncols -1;
		col = retval;
		/*
		if (col < 0) col = 0;
		if (col >= vw->var.ncols) col = vw->var.ncols -1;
		*/
		if (vw->var.ncols > 0 && colscale[col] != vw->pix.fg) {
			vw->pix.fg = colscale[col];
			new_interps[XoVarColorVal] = vw->pix.fg;
			/*
  			XSetForeground(XtDisplay(XtParent((Widget)vw)),
				vw->pix.gc,colscale[col]);
			*/
			
			return(1);
		}
	}
	return(0);
}

static int CheckInterpMorph(vw,new_interps,shape_num)
  VarObject vw;
  double *new_interps;
  int	*shape_num;
{
	int i;
	double val;
	double hival,loval,range;
	int		valindex = vw->var.morph_val - 1;

  	if (valindex >= 0 && valindex < XoVarNValues){
		val = vw->var.var_values[valindex];
		for(i = 1; i < vw->var.nshapes; i++) {
			hival = vw->var.shapes[i].value_limit[valindex];
			loval = vw->var.shapes[i-1].value_limit[valindex];
			range = hival - loval;
			if (range != 0.0){
			 if ((val <= hival && val >= loval) || 
				(val >= hival && val <= loval)) {
				new_interps[XoVarMorphVal] =
					(val - loval)/(hival - loval);
				*shape_num = i -1;
				return(1);
			 }
			}
			else{
				if(val == loval){
					new_interps[XoVarMorphVal] = 0.0;
					*shape_num = i-1;
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
static int CheckInterpXOffset(vw,new_interps)
  VarObject vw;
  double *new_interps;
{
	int i;
	double val;
	double hival,loval,range;
	int		valindex = vw->var.xoffset_val - 1;

  	if (valindex >= 0 && valindex < XoVarNValues){
		val = vw->var.var_values[valindex];
		for(i = 1; i < vw->var.nshapes; i++) {
			hival = vw->var.shapes[i].value_limit[valindex];
			loval = vw->var.shapes[i-1].value_limit[valindex];
			range = hival - loval;
			if (range != 0.0){
			 if ((val <= hival && val >= loval) || 
				(val >= hival && val <= loval)) {
				new_interps[XoVarXOffsetVal] = vw->var.shapes[i-1].xox +
					(val-loval) * 
					(vw->var.shapes[i].xox - vw->var.shapes[i-1].xox) /
					(hival - loval);
				new_interps[XoVarYOffsetVal] = vw->var.shapes[i-1].xoy +
					(val-loval) * 
					(vw->var.shapes[i].xoy - vw->var.shapes[i-1].xoy) /
					(hival - loval);
				return(1);
			 }
			}
			else{
				if(val == loval){
				 new_interps[XoVarXOffsetVal] = vw->var.shapes[i-1].xox;
				 new_interps[XoVarYOffsetVal] = vw->var.shapes[i-1].xoy;
				 return(1);
				}
			}
		}
	}
	return(0);
}

static int CheckInterpYOffset(vw,new_interps)
  VarObject vw;
  double *new_interps;
{
	int i;
	double val;
	double hival,loval,range;
	int		valindex = vw->var.yoffset_val - 1;

  	if (valindex >= 0 && valindex < XoVarNValues){
		val = vw->var.var_values[valindex];
		for(i = 1; i < vw->var.nshapes; i++) {
			hival = vw->var.shapes[i].value_limit[valindex];
			loval = vw->var.shapes[i-1].value_limit[valindex];
			range = hival - loval;
			if (range != 0.0){
			 if ((val <= hival && val >= loval) || 
				(val >= hival && val <= loval)) {
				new_interps[XoVarXOffsetVal] +=
					vw->var.shapes[i-1].yox + (val-loval) * 
					(vw->var.shapes[i].yox - vw->var.shapes[i-1].yox) /
					(hival - loval);
				new_interps[XoVarYOffsetVal] += 
					vw->var.shapes[i-1].yoy + (val-loval) * 
					(vw->var.shapes[i].yoy - vw->var.shapes[i-1].yoy) /
					(hival - loval);
				return(1);
			 }
			}
			else{
				if (val == loval){
				 new_interps[XoVarXOffsetVal] += vw->var.shapes[i-1].yox; 
				 new_interps[XoVarYOffsetVal] += vw->var.shapes[i-1].yoy; 
				 return(1);
				}
			}
		}
	}
	return(0);
}

static int CheckInterpZOffset(vw,new_interps)
  VarObject vw;
  double *new_interps;
{
	int i;
	double val;
	double hival,loval,range;
	int		valindex = vw->var.zoffset_val - 1;

  	if (valindex >= 0 && valindex < XoVarNValues){
		val = vw->var.var_values[valindex];
		for(i = 1; i < vw->var.nshapes; i++) {
			hival = vw->var.shapes[i].value_limit[valindex];
			loval = vw->var.shapes[i-1].value_limit[valindex];
			range = hival - loval;
			if (range != 0.0){
			 if ((val <= hival && val >= loval) || 
				(val >= hival && val <= loval)) {
				new_interps[XoVarXOffsetVal] +=
					vw->var.shapes[i-1].zox + (val-loval) * 
					(vw->var.shapes[i].zox - vw->var.shapes[i-1].zox) /
					(hival - loval);
				new_interps[XoVarYOffsetVal] +=
					vw->var.shapes[i-1].zoy + (val-loval) * 
					(vw->var.shapes[i].zoy - vw->var.shapes[i-1].zoy) /
					(hival - loval);
				return(1);
			 }
			}
			else{
				if(val == loval){
				 new_interps[XoVarXOffsetVal] += vw->var.shapes[i-1].zox;
				 new_interps[XoVarYOffsetVal] += vw->var.shapes[i-1].zoy; 
				 return(1);
				}
			}
		}
	}
	return(0);
}

static int CheckInterpText(vw,new_interps)
  VarObject vw;
  double *new_interps;
{
  	if (vw->var.text_val > 0 && vw->var.text_val <= XoVarNValues) {
		new_interps[XoVarTextVal] =
			vw->var.var_values[vw->var.text_val-1];
		return(1);
	}
	return(0);
}

static int CheckInterpTextColor(vw,new_interps)
  VarObject vw;
  double *new_interps;
{
	int i;
	double val;
	double hival,loval,retval = 0.0,range;
  	unsigned long		*colscale = vw->var.colorscale;
  	int		col;
	int		valindex = vw->var.textcolor_val - 1;

  	if (valindex >= 0 && valindex < XoVarNValues){
		val = vw->var.var_values[valindex];
		for(i = 1; i < vw->var.nshapes; i++) {
			hival = vw->var.shapes[i].value_limit[valindex];
			loval = vw->var.shapes[i-1].value_limit[valindex];
			range = hival -  loval;
			if (range != 0.0){
			 if ((val <= hival && val >= loval) || 
				(val >= hival && val <= loval)) {
				retval = vw->var.shapes[i-1].textcolorindex +
					(val - loval) *
					(double)(vw->var.shapes[i].textcolorindex -
						vw->var.shapes[i-1].textcolorindex) /
					(hival - loval);
				break;
			 }
			}
			else{
				if(val == loval){
				 retval = vw->var.shapes[i-1].textcolorindex;
				 break;
				}
			}
		}
		if (retval < 0.0) retval = 0.0;
		if (retval >= vw->var.ncols) retval = vw->var.ncols -1;
		col = retval;
		/*
		if (col < 0) col = 0;
		if (col >= vw->var.ncols) col = vw->var.ncols -1;
		*/
		if (vw->var.ncols > 0 && colscale[col] != vw->var.textcolor) {
			vw->var.textcolor = colscale[col];
			new_interps[XoVarTextColorVal] = vw->var.textcolor;
			return(1);
		}
	}
	return(0);
}

static int CheckInterpLineThickness(vw,new_interps)
  VarObject vw;
  double *new_interps;
{
	return(0);
}

static int DoMorph(vw,new_interps,shape_num)
  VarObject vw;
  double *new_interps;
  int	shape_num;
{
	double val = new_interps[XoVarMorphVal];
	XPoint *points = vw->var.pts;
	int		maxnpts = vw->var.alloced_pts;
	XPoint *lopts,*hipts;
	int		lonpts,hinpts;
	int		i,j,k;
	ShapeObject sw;

	if (shape_num >= vw->var.nshapes -1) return(0);
	sw = (ShapeObject)vw->var.shapes[shape_num].shape;
	lopts = sw->shape.pts;
	lonpts = sw->shape.npts;
	sw = (ShapeObject)vw->var.shapes[shape_num + 1].shape;
	hipts = sw->shape.pts;
	hinpts = sw->shape.npts;

	if (maxnpts < hinpts || maxnpts < lonpts) {
		printf("Error in Var.c DoMorph: number of allocated points < points in shape\n");
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
	vw->var.npts = maxnpts;
	return(1);
}

static int DoOffset(vw,new_interps,was_morphed)
  VarObject vw;
  double *new_interps;
  int	was_morphed;
{
	XPoint *points = vw->var.pts;
	int		maxnpts = vw->var.npts;
	int		ox = new_interps[XoVarXOffsetVal];
	int		oy = new_interps[XoVarYOffsetVal];
	int		i;

	if (vw->var.nshapes == 0) return(0);
	if (was_morphed) {
		if (maxnpts <= 0) return(0);
	
		/* doing the offsets */
		for(i = 0 ; i < maxnpts; i++) {
			points[i].x += ox;
			points[i].y += oy;
		}
		return(1);
	} else {
		ShapeObject sw = (ShapeObject)vw->var.shapes[0].shape;
		XPoint *origpts = sw->shape.pts;
		maxnpts = sw->shape.npts;
		if (maxnpts <= 0) return(0);
		if (maxnpts > vw->var.alloced_pts) {
			printf("Error in Var.c DoOffset: number of allocated points < points in shape\n");
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
  VarObject vw;
  double *new_interps;
{
	if (vw->var.textmode != TEXTNODRAW) {
		sprintf(vw->var.text,"%f",new_interps[XoVarTextVal]);
		return(1);
	}
	return(0);
}


static int DoLineThickness(vw,new_interps)
  VarObject vw;
  double *new_interps;
{
	return(0);
}


static void Expose (w)
     Widget w;
{
  VarObject vw = (VarObject) w;
  int		do_var = 0;
  int		do_undraw = 0;
  static double	new_interps[XoVarNVars];
			/* carries the result of
			** the interpolations, except for the Morph in which
			** it has the fractional value at which to do the
			** point interpolation */

  int	new_shape_num; /* identifies the first of 2 shapes to interp */
  Pixel	next_fg;
  Pixel next_textfg;

  if (vw->pix.pixflags & XO_VISIBLE_NOT) return;
  if (vw->var.nshapes <= 0) return;

  /* does color interpolation and setting if needed */
  if (CheckInterpColor(vw,new_interps))
	do_var |= (1 << XoVarColorVal);

  if (CheckInterpMorph(vw,new_interps,&new_shape_num))
	do_var |= (1 << XoVarMorphVal);
  /* initialize the X,Y,Z offsets in the new_interps to zero */
  new_interps[XoVarXOffsetVal] = new_interps[XoVarYOffsetVal] =
	new_interps[XoVarZOffsetVal] = 0.0;
  if (CheckInterpXOffset(vw,new_interps))
	do_var |= (1 << XoVarXOffsetVal);
  if (CheckInterpYOffset(vw,new_interps))
	do_var |= (1 << XoVarYOffsetVal);
  if (CheckInterpZOffset(vw,new_interps))
	do_var |= (1 << XoVarZOffsetVal);
  if (CheckInterpText(vw,new_interps))
	do_var |= (1 << XoVarTextVal);
  if (CheckInterpTextColor(vw,new_interps))
	do_var |= (1 << XoVarTextColorVal);
  if (CheckInterpLineThickness(vw,new_interps))
	do_var |= (1 << XoVarLineThicknessVal);

	/* Check for undraw */
	do_undraw = do_var & (
		(1 << XoVarMorphVal) |
		(1 << XoVarXOffsetVal) |
		(1 << XoVarYOffsetVal) |
		(1 << XoVarZOffsetVal) |
		(1 << XoVarTextVal) |
		(1 << XoVarLineThicknessVal));
	if (do_undraw) { /* doing the undraw - basically redraw in bg */
		if (do_var & (1 << XoVarColorVal))
			next_fg = new_interps[XoVarColorVal];
		else
			next_fg = vw->pix.fg;
  		XSetForeground(XtDisplay(XtParent(w)),vw->pix.gc,
			vw->var.bg);

		if (do_var & (1 << XoVarTextColorVal))
			next_textfg = new_interps[XoVarTextColorVal];
		else
			next_textfg = vw->var.textcolor;
  		XSetForeground(XtDisplay(XtParent(w)),vw->var.textgc,
			vw->var.bg);

		/* Doing the undraw */
		DrawPoints(vw);

  		XSetForeground(XtDisplay(XtParent(w)),vw->pix.gc,
			next_fg);
  		XSetForeground(XtDisplay(XtParent(w)),vw->var.textgc,
			next_textfg);
	} else {
		if (do_var & (1 << XoVarColorVal))
  			XSetForeground(XtDisplay(XtParent(w)),vw->pix.gc,
				(Pixel)new_interps[XoVarColorVal]);
		if (do_var & (1 << XoVarTextColorVal))
  			XSetForeground(XtDisplay(XtParent(w)),vw->var.textgc,
				(Pixel)new_interps[XoVarTextColorVal]);
	}

	/* Check for any draw */
	/** Draw the points if expose is called, regardless of do_var **/	
/* 	if (do_var) {   */
		/* the fg has already been done while setting fgs above */

  /* The morphing function fills the points array. */
		if (do_var & (1 << XoVarMorphVal))
			DoMorph(vw,new_interps,new_shape_num);

		/* This function handles all the offsets */
		if (do_var & ((1 << XoVarXOffsetVal) | (1 << XoVarYOffsetVal) |
				(1 << XoVarZOffsetVal))) {
  /* If there is no morphing, the pts array is filled from shape 0. */
			DoOffset(vw,new_interps,(do_var & (1 << XoVarMorphVal)));
		}
		if (do_var & (1 << XoVarTextVal))
			DoText(vw,new_interps);

		/* the textcolor has already been done */

		if (do_var & (1 << XoVarLineThicknessVal))
			DoLineThickness(vw,new_interps);
		DrawPoints(vw);
	/* } */
}

static void DrawPoints(w)
	Widget w;
{
  VarObject vw = (VarObject) w;
  XPoint	*points;
  XSegment	*segs;
  XRectangle	*rects;
  int		nsegs;
  int		i,j;
  int		dx,dy;
  int		rx,ry;
  int		len;

  if (vw->var.npts > 0) {
		points=vw->var.pts;
  	switch(vw->var.drawmode) {
		case DRAWLINES:
  			XoDrawLines(XtDisplay(XtParent(w)), XtWindow(XtParent(w)),
			vw->pix.gc, points,vw->var.npts,CoordModeOrigin); 
			break;
		case DRAWSEGS:
			segs = (XSegment *)
				XtCalloc(vw->var.npts/2,sizeof(XSegment));
			for(i=0;i<vw->var.npts;i++) {
				j=i/2;
				segs[j].x1 = points[i].x;
				segs[j].y1 = points[i].y;
				i++;
				segs[j].x2 = points[i].x;
				segs[j].y2 = points[i].y;
			}
  			XoDrawSegments(XtDisplay(XtParent(w)),XtWindow(XtParent(w)),
			vw->pix.gc, segs,vw->var.npts/2,CoordModeOrigin); 
			XtFree((char *) segs);
			break;
		case DRAWPOINTS:
  			XoDrawPoints(XtDisplay(XtParent(w)), XtWindow(XtParent(w)),
			vw->pix.gc, points,vw->var.npts,CoordModeOrigin); 
			break;
		case DRAWARROWS:
			nsegs = vw->var.npts / 2;
			segs = (XSegment *)
				XtCalloc(3 * nsegs,sizeof(XSegment));
			for(i=0;i<vw->var.npts - 1;i++) {
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
				XtCalloc(vw->var.npts/2,sizeof(XRectangle));
			for(i=0;i<vw->var.npts -1 ;i+=2) {
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
				vw->pix.gc, rects,vw->var.npts/2,CoordModeOrigin); 
			XtFree((char *) rects);
			break;
		case FILLPOLY:
  			XoFillPolygon(XtDisplay(XtParent(w)), XtWindow(XtParent(w)),
			vw->pix.gc, points,vw->var.npts,CoordModeOrigin); 
			break;
		default:
			break;
  	}
  }
  if (vw->var.text != NULL && (int)strlen(vw->var.text) > 0) {
  	switch(vw->var.textmode) {
		case TEXTDRAW:
			XoDrawCenteredString(XtDisplay(XtParent(w)),
				XtWindow(XtParent(w)), vw->var.textgc,
				vw->var.textfont,
				vw->pix.cx,vw->pix.cy,
				vw->var.text,strlen(vw->var.text));
			break;
		case TEXTFILL:
			XoFillCenteredString(XtDisplay(XtParent(w)),
				XtWindow(XtParent(w)), vw->var.textgc,
				vw->var.textfont,
				vw->pix.cx,vw->pix.cy,
				vw->var.text,strlen(vw->var.text));
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
  VarObject vw = (VarObject) w;
  XtReleaseGC(XtParent(w),vw->pix.gc);
  XtReleaseGC(XtParent(w),vw->var.textgc);
  if (vw->var.text != NULL)
	XtFree(vw->var.text);
  if (vw->var.pts != NULL)
  	XtFree((char *)(vw->var.pts));
}

/* We dont really need this since the parent widget has complete
** control over the gadget layout */
static XtGeometryResult QueryGeometry(w,intended,preferred)
	Widget w;
	XtWidgetGeometry *intended, *preferred;
{
	VarObject vw = (VarObject)w;
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
  VarObject vw = (VarObject) w;
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

  if (vw->var.nshapes == 0 || vw->var.shapes == NULL) return;
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
	/* used for default click location of var pix */
	vw->pix.x = x - XO_LINE_SEL_DIST;
	vw->pix.y = y - XO_LINE_SEL_DIST;
	/* vw->pix.z = z - XO_LINE_SEL_DIST; */
	vw->pix.w = 2 * XO_LINE_SEL_DIST;
	vw->pix.h = 2 * XO_LINE_SEL_DIST;

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
	
/* Apply the transforms individualy to each varShape */
  for (i = 0 ; i < vw->var.nshapes; i++) {
	vshape = &(vw->var.shapes[i]);
	if (vshape != NULL) { /* this check is probably not needed */
  		sw = (ShapeObject)vshape->shape;
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
			for(j=0;j<sw->shape.npts;j++) {
				points[j].x = x+px[j];
				points[j].y = y+py[j];
			}
	  		XtFree((char *) px);
	  		XtFree((char *) py);
	  		XtFree((char *) pz);
		}
	}
  }
/* Copy over the pts for shape 0 to the pts array in case morphing
** is not being used */
	sw = (ShapeObject)(vw->var.shapes[0].shape);
	points = sw->shape.pts;
	npts = ((ShapeObject)(vw->var.shapes[0].shape))->shape.npts;
	if (npts > vw->var.alloced_pts) {
		printf("Error:  Project in Var.c: npts allocation error\n");
	}
	for(i = 0 ; i < npts; i++) {
		vw->var.pts[i] = points[i];
	}
	vw->var.npts = npts;
/* Copy over the other standard values */
	vw->var.drawmode = sw->shape.drawmode;
	strncpy(vw->var.text,sw->shape.text,XO_VAR_MAX_TEXT_CHARS);
	/* just to be safe */
	vw->var.text[XO_VAR_MAX_TEXT_CHARS -1] = '\0';
	vw->var.textcolor = sw->shape.textcolor;
	vw->var.textfont = sw->shape.textfont;
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
/* This routine should really behave more like the SelectDistance
** from the Shape, since tx etc will not always be valid when the
** var is going all over the place.
*/
static void SelectDistance (w,x,y,dist)
     Widget w;
	 Position x,y;
	 Dimension *dist;
{
	int ans;
	VarObject vw = (VarObject)w;

	/* use XO_MAX_CLICK_DIST */
	if (vw->pix.pixflags & XO_CLICKABLE_NOT) {
		*dist = XO_MAX_SEL_DIST;
		return;
	}
		ans = (int)sqrt((double)sqr(x - vw->pix.x) +  
						(double)sqr(y - vw->pix.y));
		*dist = (Dimension) (ans > 0 ? ans : 0);
		return;
}
#undef sqr

static Boolean XoCvtStringToVarmode(dpy,args,num_args,fromVal,toVal,
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
	XtDisplayStringConversionWarning(dpy,fromVal->addr,"XtRVarmode");
		return((Boolean) False);
}

void XoUpdateVar(w)
Widget w;
{
	Expose(w);
}

void XoProjectVar(w)
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
