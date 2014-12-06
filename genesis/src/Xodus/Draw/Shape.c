/* $Id: Shape.c,v 1.3 2005/07/01 10:02:30 svitak Exp $ */
/*
 * $Log: Shape.c,v $
 * Revision 1.3  2005/07/01 10:02:30  svitak
 * Misc fixes to address compiler warnings, esp. providing explicit types
 * for all functions and cleaning up unused variables.
 *
 * Revision 1.2  2005/06/18 18:50:25  svitak
 * Typecasts to quiet compiler warnings. From OSX Panther patch.
 *
 * Revision 1.1.1.1  2005/06/14 04:38:32  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.15  2000/10/09 22:59:32  mhucka
 * Removed needless declaration for sqrt().
 *
 * Revision 1.14  2000/06/12 04:13:24  mhucka
 * 1) Removed nested comments in RCS/CVS log lines, to quiet down the
 *    IRIX cc compiler.
 * 2) Added NOTREACHED comments where appropriate.
 *
 * Revision 1.13  2000/05/02 06:06:42  mhucka
 * Added type casts for certain function call arguments.
 *
 * Revision 1.12  1996/06/06 20:01:32  dhb
 * Paragon port.
 *
 * Revision 1.11  1995/07/08  03:09:20  dhb
 * Replaced XClearArea() with XoDrawForceExpose().
 *
 * Revision 1.10  1995/06/12  17:13:26  venkat
 * More-Resource-Converter-clean-up-(fromVal->addr!=NULL)-is-checked-instead-of-checking-(fromVal->size)
 *
 * Revision 1.9  1995/06/02  21:33:42  venkat
 * Commented DISPLAY debug statements
 *
 * Revision 1.8  1995/06/02  19:46:02  venkat
 * Resource-converters-clean-up
 *
 * Revision 1.7  1995/03/17  20:41:09  venkat
 * Added Linux specific resource conversions
 *
 * Revision 1.6  1994/06/13  23:07:27  bhalla
 * Fixed updating in SetValues
 *
 * Revision 1.5  1994/05/25  13:49:22  bhalla
 * bugfix for segment allocation and assignment in the expose.
 * In converting to drawmode, replaced strcmp with strncmps to allow some
 * forgiveness in drawmode names
 *
 * Revision 1.4  1994/04/25  18:01:13  bhalla
 * Added code to give a single point or line seg a finite size for
 * selection purposes
 *
 * Revision 1.3  1994/02/24  15:00:32  bhalla
 * Got rid of transfmode, which is handled by pixflags
 *
 * Revision 1.2  1994/01/13  19:35:36  bhalla
 * *** empty log message ***
 *
 * Revision 1.2  1994/01/13  19:35:36  bhalla
 * *** empty log message ***
 * */
#include <math.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include "Xo/XoDefs.h"
#include "Xo/Xo_ext.h"
#include "CoreDrawP.h"
#include "ShapeP.h"

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

#define XO_ARROWHEAD_LEN 5
#define XO_LINE_SEL_DIST 10
/*
#define DISPLAY_DEBUG
#ifdef DISPLAY_DEBUG
#include <stdio.h>
 static	char shapeaddr[15];
 char *XoGetReqdShape();
#endif
*/

#define offset(field) XtOffset(ShapeObject, shape.field)
static XtResource resources[] = {
  {XtNdrawmode, XtCMode, XtRDrawmode, sizeof(int),
     offset(drawmode), XtRString, "DrawLines"},
  {XtNnpts, XtCNpts, XtRInt, sizeof(int),
     offset(npts), XtRString, "0"},
  {XtNxpts, XtCPts, XtRPointer, sizeof(double*),
     offset(xpts), XtRImmediate, (XtPointer)NULL},
  {XtNypts, XtCPts, XtRPointer, sizeof(double*),
     offset(ypts), XtRImmediate, (XtPointer)NULL},
  {XtNzpts, XtCPts, XtRPointer, sizeof(double*),
     offset(zpts), XtRImmediate, (XtPointer)NULL},
  {XtNlinewidth, XtCLineWidth, XtRInt, sizeof(int),
     offset(linewidth), XtRString, "1"},
  {XtNlinestyle, XtCMode, XtRLinestyle, sizeof(int),
     offset(linestyle), XtRString, "LineSolid"},
  {XtNcapstyle, XtCMode, XtRCapstyle, sizeof(int),
     offset(capstyle), XtRString, "CapNotLast"},
  {XtNjoinstyle, XtCMode, XtRJoinstyle, sizeof(int),
     offset(joinstyle), XtRString, "JoinMiter"},
  {XtNtext, XtCString, XtRString, sizeof(String),
     offset(text), XtRString, NULL},
  {XtNtextcolor, XtCForeground, XtRXoPixel, sizeof(Pixel),
     offset(textcolor), XtRString, "XtDefaultForeground"},
  {XtNtextmode, XtCMode, XtRTextmode, sizeof(int),
     offset(textmode), XtRString, "draw"},
  {XtNtextfont, XtCFont, XtRFontStruct, sizeof(XFontStruct *),
     offset(textfont), XtRString, "XtDefaultFont"},
};
#undef offset


/* methods */

/* I have no idea where Expose is getting defined
*/
#ifdef Expose
#undef Expose
#endif
static Boolean XoCvtStringToDrawmode();
static Boolean XoCvtStringToLinestyle();
static Boolean XoCvtStringToCapstyle();
static Boolean XoCvtStringToJoinstyle();
static Boolean XoCvtStringToTextmode();
static void SelectDistance();
static void ClassInitialize();
static void ResetGC();
static void ResetTextGC();
static void Expose();
static void Initialize();
static Boolean SetValues();
static void Destroy();
static void Project();
static int get_perp_dist();
static XtGeometryResult QueryGeometry();

/* class record definition */


ShapeClassRec shapeClassRec = {
  { /* RectObj (Core) Fields */

    /* superclass         */    (WidgetClass) &pixClassRec,
    /* class_name         */    "Shape",
    /* size               */    sizeof(ShapeRec),
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
	/* shape fields */
	/* make_compiler_happy */	0
  }
};

WidgetClass shapeObjectClass = (WidgetClass)&shapeClassRec;

static void ResetGC (shape)
     ShapeObject shape;
{
  /* Set the values in the GC */
  XGCValues values;
  XtGCMask mask = GCForeground | GCLineStyle | GCCapStyle | 
	GCJoinStyle | GCLineWidth;
  
  if (shape->pix.gc != NULL)
    XtReleaseGC (XtParent((Widget)shape), shape->pix.gc);

  values.foreground = shape->pix.fg;
  values.line_style = shape->shape.linestyle;
  values.line_width = shape->shape.linewidth;
  values.cap_style = shape->shape.capstyle;
  values.join_style = shape->shape.joinstyle;
  shape->pix.gc = XtGetGC(XtParent((Widget)shape), mask, &values);
}

static void ResetTextGC (shape)
     ShapeObject shape;
{
  /* Set the values in the GC */
  XGCValues values;
  XtGCMask mask = GCForeground | GCFont;
  
  if (shape->shape.textgc != NULL)
    XtReleaseGC (XtParent((Widget)shape), shape->shape.textgc);

  values.foreground = shape->shape.textcolor;
  values.font = shape->shape.textfont->fid;
  shape->shape.textgc = XtGetGC(XtParent((Widget)shape), mask, &values);
}

static void ClassInitialize()
{
  XtSetTypeConverter (XtRString, XtRDrawmode,
                      XoCvtStringToDrawmode, NULL, 0 ,
                      XtCacheNone, NULL);
  XtSetTypeConverter (XtRString, XtRTextmode,
                      XoCvtStringToTextmode, NULL, 0 ,
                      XtCacheNone, NULL);
  XtSetTypeConverter (XtRString, XtRLinestyle,
                      XoCvtStringToLinestyle, NULL, 0 ,
                      XtCacheNone, NULL);
  XtSetTypeConverter (XtRString, XtRCapstyle,
                      XoCvtStringToCapstyle, NULL, 0 ,
                      XtCacheNone, NULL);
  XtSetTypeConverter (XtRString, XtRJoinstyle,
                      XoCvtStringToJoinstyle, NULL, 0 ,
                      XtCacheNone, NULL);
}


static void Initialize (req, new)
     Widget req, new;
{
  ShapeObject shape = (ShapeObject) new;

  /* Check that the initial values are reasonable */
	if (shape->shape.linewidth < 0 || shape->shape.linewidth > 10) 
		shape->shape.linewidth = 0;
	if (shape->shape.linestyle != LineSolid &&
		shape->shape.linestyle != LineOnOffDash &&
		shape->shape.linestyle != LineDoubleDash)
		shape->shape.linestyle = LineSolid;
	if (shape->shape.capstyle != CapNotLast &&
		shape->shape.capstyle != CapButt &&
		shape->shape.capstyle != CapRound &&
		shape->shape.capstyle != CapProjecting)
		shape->shape.capstyle = CapNotLast;
	if (shape->shape.joinstyle != JoinMiter &&
		shape->shape.joinstyle != JoinRound &&
		shape->shape.joinstyle != JoinBevel)
		shape->shape.joinstyle = JoinMiter;
	if (shape->shape.textmode != TEXTDRAW &&
		shape->shape.textmode != TEXTFILL &&
		shape->shape.textmode != TEXTNODRAW)
		shape->shape.textmode = TEXTDRAW;

	shape->shape.textcolor = XBlackPixel((Display *)XgDisplay(),0);
		
  shape->pix.gc=NULL;
  ResetGC(shape);
  shape->shape.textgc=NULL;
  ResetTextGC(shape);
  /* 20 is a reasonable initial value for the size of the pts cache */
  if (shape->shape.text == NULL)
	shape->shape.text = XtNewString("");
  else 
	shape->shape.text = XtNewString(shape->shape.text);
  shape->shape.pts =
	(XPoint *) XtCalloc(20,sizeof(XPoint));
	shape->shape.alloced_pts = 20;
}


static Boolean SetValues (curw, reqw, neww)
     Widget curw, reqw, neww;
{
  ShapeObject curs = (ShapeObject) curw;
  ShapeObject news = (ShapeObject) neww;
  Boolean ret = False;
  Boolean clrdraw = False;

  /* Assorted things which affect the GC and need clearing of the draw*/
  if (news->pix.fg != curs->pix.fg ||
	news->shape.linestyle != curs->shape.linestyle ||
	news->shape.linewidth != curs->shape.linewidth ||
	news->shape.capstyle != curs->shape.capstyle ||
	news->shape.joinstyle != curs->shape.joinstyle) {
    ResetGC(news);
	clrdraw = True;
  }

  /* Assorted things which affect the text GC */
  if (news->shape.textcolor != curs->shape.textcolor ||
	news->shape.textfont != curs->shape.textfont) {
  	ResetTextGC(news);
	ret = True;
  }

  /* These are the items which do not affect the projection,
  ** but do need redrawing */
  if (news->shape.drawmode != curs->shape.drawmode ||
	news->shape.linestyle != curs->shape.linestyle ||
	news->shape.linewidth != curs->shape.linewidth ||
	news->shape.capstyle != curs->shape.capstyle ||
	news->shape.joinstyle != curs->shape.joinstyle ||
	news->shape.textcolor != curs->shape.textcolor
	)
	ret = True;

  /* This is a special case which needs memory reallocation */
  if (news->shape.text != curs->shape.text) {
  	if (news->shape.text != NULL) {
		if (curs->shape.text != NULL) XtFree((char *) curs->shape.text);
		news->shape.text = XtNewString(news->shape.text);
		ret = True;
	}
  }
  /* These are the items which do need reprojection, and
  redrawing of the entire draw */
  if (
  	news->pix.tx != curs->pix.tx ||
  	news->pix.ty != curs->pix.ty ||
  	news->pix.tz != curs->pix.tz ||
  	news->shape.npts != curs->shape.npts ||
  	news->shape.xpts != curs->shape.xpts ||
  	news->shape.ypts != curs->shape.ypts ||
  	news->shape.zpts != curs->shape.zpts) {
		if (news->shape.npts != curs->shape.npts) {
			if (news->shape.npts < 1) {
  				news->shape.npts = curs->shape.npts;
			} else if (news->shape.npts > curs->shape.alloced_pts) {
			/* 20 is a random extra amount so that future changes
			** have a fair chance of fitting too */
				news->shape.alloced_pts = news->shape.npts + 20;
				news->shape.pts =
					(XPoint *)XtRealloc((char *) news->shape.pts,
						news->shape.alloced_pts * sizeof(XPoint));
			}
		}
		/*
		ResizeDraws(XtParent(neww));
		*/
		Project(news);
		clrdraw = True;
	}
	/* These items also need redrawing of the entire draw. */
  if (
	news->shape.textfont != curs->shape.textfont ||
  	news->shape.drawmode != curs->shape.drawmode)
	clrdraw = True;
	
	if (clrdraw) {
		ret = False;
		XoDrawForceExpose(XtParent(neww));
	}

	if (ret && !(news->pix.pixflags & XO_UPDATE_SELF_ON_SET_NOT))
		return(True);
	else 
		return(False);
}

  

static void Expose (w)
     Widget w;
{
  ShapeObject sw = (ShapeObject) w;
  XPoint	*points;
  XSegment	*segs;
  XRectangle	*rects;
  int		nsegs;
  int		i,j;
  int		dx,dy;
  int		rx,ry;
  int		len;

  if (sw->pix.pixflags & XO_VISIBLE_NOT) return;

	/* Need to do a check on the mode before doing this */
  if (sw->shape.npts > 0) {
		points=sw->shape.pts;
			
  	switch(sw->shape.drawmode) {
		case DRAWLINES:
/*
#ifdef DISPLAY_DEBUG
	sprintf(shapeaddr,"%x",sw);
	if(strcmp(XoGetReqdShape(),shapeaddr)==0){
	 fprintf(stderr, "The number of points for this shape are %d\n",
				sw->shape.npts);
	 if(sw->shape.npts>=4)
	 fprintf(stderr, "Points 2,3 and 4 are (%d,%d),(%d,%d) and (%d,%d)\n",
points[1].x,points[1].y,points[2].x,points[2].y,points[3].x,points[3].y); 
	}
#endif
*/
  			XoDrawLines(XtDisplay(XtParent(w)), XtWindow(XtParent(w)),
			sw->pix.gc, points,sw->shape.npts,CoordModeOrigin); 
			break;
		case DRAWSEGS:
			nsegs = sw->shape.npts/2;
			if (nsegs <= 0) break;
			segs = (XSegment *)
				XtCalloc(nsegs,sizeof(XSegment));
			for(i=0, j=0 ; j<nsegs ;j++, i++) {
				segs[j].x1 = points[i].x;
				segs[j].y1 = points[i].y;
				i++;
				segs[j].x2 = points[i].x;
				segs[j].y2 = points[i].y;
			}
  			XoDrawSegments(XtDisplay(XtParent(w)),XtWindow(XtParent(w)),
			sw->pix.gc, segs,nsegs,CoordModeOrigin); 
			XtFree((char *) segs);
			break;
		case DRAWPOINTS:
  			XoDrawPoints(XtDisplay(XtParent(w)), XtWindow(XtParent(w)),
			sw->pix.gc, points,sw->shape.npts,CoordModeOrigin); 
			break;
		case DRAWARROWS:
			nsegs = sw->shape.npts / 2;
			segs = (XSegment *)
				XtCalloc(3 * nsegs,sizeof(XSegment));
			for(i=0, j=0; j<nsegs ; i++, j++) {
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
				sw->pix.gc, segs,3 * nsegs,CoordModeOrigin); 
			XtFree((char *) segs);
			break;
		case FILLRECTS:
			nsegs = sw->shape.npts/2;
			rects = (XRectangle *)
				XtCalloc(nsegs,sizeof(XRectangle));
			for(i=0, j=0 ; j<nsegs ; j++, i+=2) {
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
				sw->pix.gc, rects,nsegs,CoordModeOrigin); 
			XtFree((char *) rects);
			break;
		case FILLPOLY:
  			XoFillPolygon(XtDisplay(XtParent(w)), XtWindow(XtParent(w)),
			sw->pix.gc, points,sw->shape.npts,CoordModeOrigin); 
			break;
		default:
			break;
  	}
  }
  if (sw->shape.text != NULL && (int)strlen(sw->shape.text) > 0) {
  	switch(sw->shape.textmode) {
		case TEXTDRAW:
			XoDrawCenteredString(XtDisplay(XtParent(w)),
				XtWindow(XtParent(w)), sw->shape.textgc,
				sw->shape.textfont,
				sw->pix.cx,sw->pix.cy,
				sw->shape.text,strlen(sw->shape.text));
			break;
		case TEXTFILL:
			XoFillCenteredString(XtDisplay(XtParent(w)),
				XtWindow(XtParent(w)), sw->shape.textgc,
				sw->shape.textfont,
				sw->pix.cx,sw->pix.cy,
				sw->shape.text,strlen(sw->shape.text));
			break;
		case TEXTNODRAW:
			/* do not draw anything */
			break;
		default:
			break;
  	}
  }
  if (sw->pix.pixflags & XO_IS_SELECTED) { /* do the highlight */
	sw->pix.pixflags &= ~XO_IS_SELECTED;
	(((PixObjectClass)
		(sw->object.widget_class))->pix_class.highlight)(sw,NULL);
  }
}

static void Destroy(w)
	Widget w;
{
  ShapeObject sw = (ShapeObject) w;
  XtReleaseGC(XtParent(w),sw->pix.gc);
  if (sw->shape.text)
	XtFree(sw->shape.text);
  if (sw->shape.pts)
  	XtFree((char *)(sw->shape.pts));
}

/* We dont really need this since the parent widget has complete
** control over the gadget layout */
static XtGeometryResult QueryGeometry(w,intended,preferred)
	Widget w;
	XtWidgetGeometry *intended, *preferred;
{
	ShapeObject sw = (ShapeObject)w;
	preferred->x = sw->pix.x;
	preferred->y = sw->pix.y;
	preferred->width = sw->pix.w;
	preferred->height = sw->pix.h;
	if (intended->request_mode & (CWX | CWY | CWWidth | CWHeight)) {
		if (preferred->x == intended->x &&  
			preferred->y == intended->y &&  
			preferred->width == intended->width &&  
			preferred->height == intended->height)
			return(XtGeometryYes);
	} else {
		if (preferred->x == sw->pix.x &&  
			preferred->y == sw->pix.y &&  
			preferred->width == sw->pix.w &&  
			preferred->height == sw->pix.h)
			return(XtGeometryYes);
	}
	return(XtGeometryAlmost);
}




static void Project (w)
     Widget w;
{
  int x,y,z;
  int ox,oy,oz;
  ShapeObject sw = (ShapeObject) w;
  CoreDrawWidgetClass class = (CoreDrawWidgetClass)XtClass(XtParent(w));
  int parentwid= ((CoreDrawWidget)XtParent(w))->core.width;
  int parentht= ((CoreDrawWidget)XtParent(w))->core.height;
  int	*px,*py,*pz;
  int	i;
  XPoint	*points;
  int	maxx=-5000,maxy=-5000,minx=5000,miny=5000;
  void	(*tpt)();
  void	(*tpts)();

  if (sw->pix.pixflags & XO_VISIBLE_NOT) return;
  if (sw->pix.pixflags & XO_RESIZABLE_NOT){ /*Freeze size and position*/
		return;
  }
  if (sw->pix.pixflags & XO_PIXELCOORDS) { /* transform using pixels*/
		tpts = PixelTransformPoints;
  } else { /* use standard transformations */
		tpts = class->coredraw_class.transformpts;
  }
  if (sw->pix.pixflags & XO_PIXELOFFSET) { /* transform using pixels*/
		tpt = PixelTransformPoint;
  } else { /* use standard transformations */
		tpt = class->coredraw_class.transformpt;
  }

  if (sw->pix.pixflags & XO_PCTCOORDS) { /* transform using % of win */
		tpts = PctTransformPoints;
  }
  if (sw->pix.pixflags & XO_PCTOFFSET) { /* transform using % of win */
		tpt = PctTransformPoint;
  }

  /* Calculate offset on screen using parents TransformPoint routine */
  (tpt)(XtParent(w),
	sw->pix.tx,sw->pix.ty, sw->pix.tz,&x,&y,&z);
	sw->pix.cx = x; sw->pix.cy = y; sw->pix.cz=z;

  if (sw->shape.npts > 0) {
	
	  px = (int *) XtCalloc(sw->shape.npts,sizeof(int));
	  py = (int *) XtCalloc(sw->shape.npts,sizeof(int));
	  pz = (int *) XtCalloc(sw->shape.npts,sizeof(int));
	  points = sw->shape.pts;
	  
	  /* Calculate origin on screen using parents
	  ** TransformPoint routine, only if it is usual transforms */
		if (!(sw->pix.pixflags & (XO_PIXELCOORDS | XO_PCTCOORDS))) {
  		 	(tpt)(XtParent(w),
				(float)0.0,(float)0.0,(float)0.0, &ox,&oy,&oz);
	
			/* Figure out required offset for points being drawn */
			x-= ox;
			y-= oy;
			z-= oz;
		} else {
			y -= parentht; /*needed because both transforms include it*/
		}
	
	  (tpts)(XtParent(w),
			sw->shape.xpts,sw->shape.ypts, sw->shape.zpts,
				px,py,pz,sw->shape.npts);
		for(i=0;i<sw->shape.npts;i++) {
			points[i].x = x+px[i];
			points[i].y = y+py[i];
			/* Do the sorting of depth info here */
			/* points[i].z = z+pz[i]; */
			if (maxx < px[i])
				maxx = px[i];
			if (minx > px[i])
				minx = px[i];
			if (maxy < py[i])
				maxy = py[i];
			if (miny > py[i])
				miny = py[i];
		}
		maxx += x;
		minx += x;
		maxy += y;
		miny += y;
		/* This case occurs for single points or flat line segs */
		if (maxx == minx) {
			maxx += XO_LINE_SEL_DIST/2;
			minx -= XO_LINE_SEL_DIST/2;
		}
		if (maxy == miny) {
			maxy += XO_LINE_SEL_DIST/2;
			miny -= XO_LINE_SEL_DIST/2;
		}
	  XtFree((char *) px);
	  XtFree((char *) py);
	  XtFree((char *) pz);
  } else if (sw->shape.text != NULL && (int)strlen(sw->shape.text) > 0){
	maxx = x + XTextWidth(sw->shape.textfont,sw->shape.text,
			strlen(sw->shape.text)) / 2;
	minx = x + x - maxx;
	maxy = y + (sw->shape.textfont->max_bounds.ascent +
		sw->shape.textfont->max_bounds.descent)/2;
	miny = y + y - maxy;
  } else {
	maxx = minx = x;
	maxy = miny = y;
  }
  /* Set the rectangle geometry fields */
  if (x > parentwid * 2 || x < -parentwid * 2 || 
	y > parentht * 2 || y < -parentht * 2) {
	sw->pix.w = sw->pix.h = 0;
	sw->pix.x = x;
	sw->pix.y = y;
  } else {
  	sw->pix.x = minx;
  	sw->pix.y = miny;
  	sw->pix.w = maxx-minx;
  	sw->pix.h = maxy-miny;
  }
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
	ShapeObject sw = (ShapeObject)w;
	int i,besti;
	int	r,bestr;
  	XPoint	*pts;

	/* use XO_MAX_CLICK_DIST */
	if (sw->pix.pixflags & XO_CLICKABLE_NOT) {
		*dist = XO_MAX_SEL_DIST;
		return;
	}
	if (sw->shape.npts <= 0) {
		ans = (int)sqrt((double)sqr(x - sw->pix.x) +  
						(double)sqr(y - sw->pix.y));
		*dist = (Dimension) (ans > 0 ? ans : 0);
		return;
	}
	pts = sw->shape.pts;

	bestr = sqr(x - pts[0].x) + sqr(y - pts[0].y);
	besti = 0;
	switch(sw->shape.drawmode) {
		case DRAWPOINTS:
			/* Find the dist to the nearest point */
			for(i=1;i<sw->shape.npts;i++) {
				r = sqr(x - pts[i].x) + sqr(y - pts[i].y);
				if (r < bestr) {
					bestr = r;
					besti = i;
				}
			}
			break;
		case DRAWLINES:
			/* Find the perp dist to the nearest line segment */
			for(i=1;i<sw->shape.npts;i++) {
				r = get_perp_dist(x,y,pts,i,i-1);
				if (r < bestr) {
					bestr = r;
					besti = i;
				}
			}
			break;
		case DRAWSEGS:
		case DRAWARROWS:
			/* Find the perp dist to the nearest line segment */
			for(i=1;i<sw->shape.npts;i+=2) {
				r = get_perp_dist(x,y,pts,i,i-1);
				if (r < bestr) {
					bestr = r;
					besti = i;
				}
			}
			break;
		case DRAWRECTS:
		case FILLRECTS:
			/* Find if point lies inside a rectangle */
			for(i=1;i<sw->shape.npts;i+=2) {
				if (((x < pts[i].x && x > pts[i-1].x) ||
					(x < pts[i-1].x && x > pts[i].x)) &&
					((y < pts[i].y && y > pts[i-1].y) ||
					(y < pts[i-1].y && y > pts[i].y))) {
					*dist = 1;
					sw->pix.cz = sw->shape.zpts[i];
					return;
				}
			}
			break;
		case FILLPOLY:
			/* Find if point lies inside shape */
			ans = (int)sqrt((double)sqr(x - sw->pix.x) +  
							(double)sqr(y - sw->pix.y));
			*dist = (Dimension) (ans > 0 ? ans : 0);
			return;
			/* NOTREACHED */
			break;
	}
	bestr = sqrt((double)bestr);
	/* This is where we do stuff with the select mask */
	if (bestr < XO_LINE_SEL_DIST) { /* should #define this */
		*dist=bestr;
		sw->pix.cz = sw->shape.zpts[besti];
	} else {
		*dist = XO_MAX_SEL_DIST;
	}
}
#undef sqr

static int get_perp_dist(x,y,pts,i,j)
	int x,y;
	XPoint *pts;
	int i,j;
{
	int xab,yab;
	int rabsq;
	int	xpa,ypa;
	int qx,qy;
	int	xpq,ypq;
	int paba;
	float k;

	xab = pts[j].x - pts[i].x;
	yab = pts[j].y - pts[i].y;
	rabsq = xab * xab + yab * yab;
	if (rabsq == 0) return(100000);

	xpa = x - pts[i].x;
	ypa = y - pts[i].y;

	paba = xab * xpa + yab * ypa;

	k = (float)paba/(float)rabsq;
	if (k < 0 || k > 1) return (100000);

	qx = pts[i].x + xab * k;
	qy = pts[i].y + yab * k;

	xpq = x - qx;
	ypq = y - qy;
	return(xpq * xpq + ypq * ypq);
}

static Boolean XoCvtStringToDrawmode(dpy,args,num_args,fromVal,toVal,
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
		if (strncmp(fromVal->addr,"DrawP",5) == 0)
			ret = DRAWPOINTS;
		if (strncmp(fromVal->addr,"DrawL",5) == 0)
			ret = DRAWLINES;
		if (strncmp(fromVal->addr,"DrawR",5) == 0)
			ret = DRAWRECTS;
		if (strncmp(fromVal->addr,"DrawS",5) == 0)
			ret = DRAWSEGS;
		if (strncmp(fromVal->addr,"DrawA",5) == 0)
			ret = DRAWARROWS;
		if (strncmp(fromVal->addr,"FillR",5) == 0)
			ret = FILLRECTS;
		if (strncmp(fromVal->addr,"FillP",5) == 0)
			ret = FILLPOLY;
		if (ret >=0) {
			XoCvtDone(int,ret);
		}
	}
	toVal->addr = NULL;
	toVal->size = 0;
	XtDisplayStringConversionWarning(dpy,fromVal->addr,"XtRDrawmode");
		return((Boolean) False);
}

static Boolean XoCvtStringToLinestyle(dpy,args,num_args,fromVal,toVal,
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
		if (strcmp(fromVal->addr,"LineSolid") == 0)
			ret = LineSolid;
		if (strcmp(fromVal->addr,"LineOnOffDash") == 0)
			ret = LineOnOffDash;
		if (strcmp(fromVal->addr,"LineDoubleDash") == 0)
			ret = LineDoubleDash;
		if (ret >=0) {
			XoCvtDone(int,ret);
		}
	}
	toVal->addr = NULL;
	toVal->size = 0;
	XtDisplayStringConversionWarning(dpy,fromVal->addr,"XtRLinestyle");
		return((Boolean) False);
}

static Boolean XoCvtStringToCapstyle(dpy,args,num_args,fromVal,toVal,
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
		if (strcmp(fromVal->addr,"CapNotLast") == 0)
			ret = CapNotLast;
		if (strcmp(fromVal->addr,"CapButt") == 0)
			ret = CapButt;
		if (strcmp(fromVal->addr,"CapRound") == 0)
			ret = CapRound;
		if (strcmp(fromVal->addr,"CapProjecting") == 0)
			ret = CapProjecting;
		if (ret >=0) {
			XoCvtDone(int,ret);
		}
	}
	toVal->addr = NULL;
	toVal->size = 0;
	XtDisplayStringConversionWarning(dpy,fromVal->addr,"XtRCapstyle");
		return((Boolean) False);
}

static Boolean XoCvtStringToJoinstyle(dpy,args,num_args,fromVal,toVal,
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
		if (strcmp(fromVal->addr,"JoinMiter") == 0)
			ret = JoinMiter;
		if (strcmp(fromVal->addr,"JoinRound") == 0)
			ret = JoinRound;
		if (strcmp(fromVal->addr,"JoinBevel") == 0)
			ret = JoinBevel;
		if (ret >=0) {
			XoCvtDone(int,ret);
		}
	}
	toVal->addr = NULL;
	toVal->size = 0;
	XtDisplayStringConversionWarning(dpy,fromVal->addr,"XtRJoinstyle");
		return((Boolean) False);
}

static Boolean XoCvtStringToTextmode(dpy,args,num_args,fromVal,toVal,
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
		if (strcmp(fromVal->addr,"draw") == 0)
			ret = TEXTDRAW;
		if (strcmp(fromVal->addr,"fill") == 0)
			ret = TEXTFILL;
		if (strcmp(fromVal->addr,"nodraw") == 0)
			ret = TEXTNODRAW;
		if (ret >=0) {
			XoCvtDone(int,ret);
		}
	}
	toVal->addr = NULL;
	toVal->size = 0;
	XtDisplayStringConversionWarning(dpy,fromVal->addr,"XtRTextmode");
		return((Boolean) False);
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
