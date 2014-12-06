/* $Id: Tree.c,v 1.3 2005/07/01 10:02:30 svitak Exp $ */
/*
 * $Log: Tree.c,v $
 * Revision 1.3  2005/07/01 10:02:30  svitak
 * Misc fixes to address compiler warnings, esp. providing explicit types
 * for all functions and cleaning up unused variables.
 *
 * Revision 1.2  2005/06/18 18:50:25  svitak
 * Typecasts to quiet compiler warnings. From OSX Panther patch.
 *
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.23  2000/06/12 04:28:18  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.22  2000/05/02 06:06:42  mhucka
 * Added type casts for certain function call arguments.
 *
 * Revision 1.21  1998/01/20 00:24:42  venkat
 * Included-stdio.h-for-prototyping-sprintf.-Caused-undefined-results-on-64-bit-Irix
 *
 * Revision 1.20  1996/06/27  23:39:53  venkat
 * Changed type of the colorscale resource and any local variables attempting
 * to get the list of colortable indices, from int to long. On the alpha these
 * types are different sizes and information is lost in the scale-down process
 * leading to unpredictable color index values and behaviour.
 *
 * Revision 1.19  1995/08/02  01:54:19  dhb
 * More tree picking changes from Upi Bhalla.
 *
 * Revision 1.18  1995/06/16  05:43:02  dhb
 * Merged in 1.15.1.1 changes.
 *
<<<<<<< 1.17
 * Revision 1.17  1995/06/12  17:13:26  venkat
 * More-Resource-Converter-clean-up-(fromVal->addr!=NULL)-is-checked-instead-of-checking-(fromVal->size)
 *
 * Revision 1.16  1995/06/02  19:46:02  venkat
 * Resource-converters-clean-up
 *
=======
 * Revision 1.15.1.1  1995/06/16  05:37:28  dhb
 * Fixes for click location problems.
 * from Upi Bhalla.
 *
 *
 * Upi Bhalla Mt Sinai June 7 95:
 * Changes to SelectDistance and related code for determining if
 * a tree was clicked, and if so, where. New version takes into
 * account the coordinates of shape icons on tree.
 *
>>>>>>> 1.15.1.1
 * Revision 1.15  1995/04/12  22:59:12  dhb
 * Changes from Upi appear to fix problem with initialization of
 * Tree cx and cy when no lable is present.
 *
 * Revision 1.14  1995/03/24  20:32:15  venkat
 * Used nint() macro globally and removed system specific
 * definitions of nint()
 *
 * Revision 1.13  1995/03/17  20:38:25  venkat
 * Used Linux and Ultrix specific definitions for nint()
 *
 * Revision 1.12  1995/01/23  02:59:36  bhalla
 * 1. Put in ifdef for defining nint in Linux.
 * 2. Put in ifdef for Linux to get the resource conversions to work
 *
 * Revision 1.11  1994/11/07  22:22:09  bhalla
 * Several changes:
 * 1. Added in code to draw multiple arrows in case of multiple msgs
 * between same to elements.
 * 2. Put in code to bypass truncated/invisible leaves.
 * 3. Added in extra check for invalid ptname on leaf.
 * 4. Made some improvements to the changing fonts story, but there are
 * still problems.
 *
 * Revision 1.10  1994/06/30  21:50:10  bhalla
 * Added a few lines so that the tree displays icons with the same
 * font as defined in the shape from which the icon was derived.
 *
 * Revision 1.9  1994/06/13  23:07:56  bhalla
 * Bugfix: check for non-NULL leaf->ptname
 *
 * Revision 1.8  1994/04/18  19:20:29  bhalla
 * Changed XO_TREE_MAX_SEL_DIST to 400
 *
 * Revision 1.7  1994/04/11  21:27:39  bhalla
 * 1. Put in code to handle specification of item color by ptlist
 * 2. Fixed bug where in highlight, item names were not being set
 * 	according to ptlist->ptname
 * 3. Put in XoCopyCursor in Select so that cursor changes work
 *
 * Revision 1.6  1994/03/30  17:23:30  bhalla
 * Changed the HIGHLIGHT1 mode so it now draws a rectangle around the
 * selected item.
 * Did assorted changes associated with cretion of the xtree_leaf_struct
 * to handle info about each item on tree
 *
 * Revision 1.5  1994/03/28  22:54:36  bhalla
 * Added fliparrow
 * Added hlmode code
 *
 * Revision 1.4  1994/03/22  15:31:07  bhalla
 * Added stuff for coloring msgarrows
 * Changed the selection code so it ignores blank areas on the screen
 * which nevertheless are surrounded by tree components
 *
 * Revision 1.3  1994/02/02  18:49:54  bhalla
 * XoProjectTree now does clear screen and expose. May also have sundry
 * changes related to vast expansion of xtree.c capabilities. Changed
 * the tree->tree.selected field to tree->pix.selected to use new general
 * pix subpart selection facilities.
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
#include "TreeP.h"

#define nint(x) (int)(x+0.5)

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
#define XO_TREE_ARROW_GAP 20
#define XO_TREE_ARROWHEAD_LEN 6
#define XO_TREE_ARROWHEAD_WID 2
#define XO_TREE_MAX_SEL_DIST 400


#define XO_TREE_MAX_TEXT_CHARS 50

#define offset(field) XtOffset(TreeObject, tree.field)
static XtResource resources[] = {
  {XtNtreemode, XtCTreemode, XtRTreemode, sizeof(int),
     offset(treemode), XtRString, "tree"},
  {XtNhlmode, XtCHlmode, XtRHlmode, sizeof(int),
     offset(hlmode), XtRString, "none"},
  {XtNnshapes, XtCNshapes, XtRInt, sizeof(int),
     offset(nshapes), XtRString, "0"},
  {XtNshapechange, XtCShapechange, XtRInt, sizeof(int),
     offset(shapechange), XtRString, "0"},
  {XtNshapes, XtCShapes, XtRPointer, sizeof(Widget *),
     offset(shapes), XtRImmediate, (XtPointer)NULL},
  {XtNcolorscale, XtCColorscale, XtRPointer, sizeof(unsigned long*),
     offset(colorscale), XtRImmediate, (XtPointer)NULL},
  {XtNncols, XtCNcols, XtRInt, sizeof(int),
     offset(ncols), XtRString, "0"},
  {XtNptlist, XtCPtlist, XtRPointer, sizeof(struct xtree_leaf_type *),
     offset(ptlist), XtRImmediate, (XtPointer)NULL},
  {XtNncoords, XtCNcoords, XtRInt, sizeof(int),
     offset(ncoords), XtRString, "0"},
  {XtNhighlight1, XtCForeground, XtRXoPixel, sizeof(Pixel),
     offset(highlight1), XtRString, "red"},
  {XtNhighlight2, XtCForeground, XtRXoPixel, sizeof(Pixel),
     offset(highlight2), XtRString, "gray"},
  {XtNfromlist, XtCFromlist, XtRPointer, sizeof(int*),
     offset(fromlist), XtRImmediate, (XtPointer)NULL},
  {XtNtolist, XtCTolist, XtRPointer, sizeof(int*),
     offset(tolist), XtRImmediate, (XtPointer)NULL},
  {XtNarrowflags, XtCArrowflags, XtRPointer, sizeof(int*),
     offset(arrowflags), XtRImmediate, (XtPointer)NULL},
  {XtNnlist, XtCNlist, XtRInt, sizeof(int),
     offset(nlist), XtRString, "0"},
};
#undef offset



/* methods */

/* I have no idea where Expose is getting defined
*/
#ifdef Expose
#undef Expose
#endif
static Boolean XoCvtStringToTreemode();
static Boolean XoCvtStringToHlmode();
static void SelectDistance();
static void Select();
static void UnSelect();
static void Highlight();
static void ClassInitialize();
static void Expose();
static void DrawPoints();
static void Initialize();
static Boolean SetValues();
static void Destroy();
static void Project();
static XtGeometryResult QueryGeometry();

/* class record definition */


TreeClassRec treeClassRec = {
  { /* RectObj (Core) Fields */

    /* superclass         */    (WidgetClass) &pixClassRec,
    /* class_name         */    "Tree",
    /* size               */    sizeof(TreeRec),
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
    /* unselect */ 	         	UnSelect,
    /* motion */  	         	XoInheritMotion,
    /* highlight */          	Highlight,
    /* extension */             NULL
  },
  {
	/* tree fields */
	/* make_compiler_happy */	0
  }
};

WidgetClass treeObjectClass = (WidgetClass)&treeClassRec;

static void ClassInitialize()
{
  XtSetTypeConverter (XtRString, XtRTreemode,
                      XoCvtStringToTreemode, NULL, 0 ,
                      XtCacheNone, NULL);
  XtSetTypeConverter (XtRString, XtRHlmode,
                      XoCvtStringToHlmode, NULL, 0 ,
                      XtCacheNone, NULL);
}

static void Initialize (req, new)
     Widget req, new;
{
  TreeObject tree = (TreeObject) new;
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
	tree->tree.treemode = XoTreeTreeMode;
	tree->tree.textcolor = XBlackPixel((Display *)XgDisplay(),0);
		
  tree->pix.gc=XCreateGC(disp,d,mask,&values);
  mask = GCForeground;
  tree->tree.textgc=XCreateGC(disp,d,mask,&values);
  /* 20 is a reasonable initial value for the size of the pts cache */
  tree->tree.text = XtCalloc(XO_TREE_MAX_TEXT_CHARS,sizeof(char));
								/* The text field is permanently
								** allocated for doing numbers */
	/* Perhaps I should do an XtGetValues here ... */
	tree->tree.bg = dw->core.background_pixel;
  tree->tree.pts =
	(XPoint *) XtCalloc(20,sizeof(XPoint));
	tree->tree.alloced_pts = 20;
  tree->tree.ncoords = 20;
  tree->tree.coords = 
	(XPoint *) XtCalloc(20,sizeof(XPoint));
}


static Boolean SetValues (curw, reqw, neww)
     Widget curw, reqw, neww;
{
  TreeObject curs = (TreeObject) curw;
  TreeObject news = (TreeObject) neww;
  ShapeObject sw;
  Boolean ret = False;
  int	max_npts = 0;
  int	i;

	/* if shapes or nshapes change then the number of points needs
	** recalculation and reallocation */
  if ( news->tree.nshapes != curs->tree.nshapes ||
  	news->tree.shapes != curs->tree.shapes ||
  	news->tree.shapechange != curs->tree.shapechange) {
	for(i = 0 ; i < news->tree.nshapes; i++) {
		sw = (ShapeObject)(news->tree.shapes[i]);
		if (max_npts < sw->shape.npts) 
			max_npts = sw->shape.npts;
	}
	if (max_npts > news->tree.alloced_pts) {
		news->tree.alloced_pts = max_npts + 20;
		news->tree.pts =
			(XPoint *)XtRealloc((char *) news->tree.pts,
				news->tree.alloced_pts * sizeof(XPoint));
	}
	news->tree.shapechange = 0;
  }
  if (news->tree.ncoords > curs->tree.ncoords) {
		news->tree.coords =
			(XPoint *)XtRealloc((char *) news->tree.coords,
				news->tree.ncoords * sizeof(XPoint));
  }

  /* These are the items which do need reprojection */
  if (news->tree.treemode != curs->tree.treemode ||
  	news->pix.tx != curs->pix.tx ||
  	news->pix.ty != curs->pix.ty ||
  	news->pix.tz != curs->pix.tz ||
  	news->tree.ptlist != curs->tree.ptlist ||
  	news->tree.ncoords != curs->tree.ncoords ||
  	news->tree.nshapes != curs->tree.nshapes ||
  	news->tree.hlmode != curs->tree.hlmode ||
  	news->tree.shapes != curs->tree.shapes) {
		Project(news);
		ret = True;
	}
	if (ret && (news->pix.pixflags & XO_UPDATE_ALL_ON_SET))
		return(True);
	else 
		return(False);
}

static void Expose (w)
     Widget w;
{
  TreeObject tw = (TreeObject) w;
  ShapeObject sw;
  XPoint	*points = tw->tree.pts;
  XSegment	*segs;
  int		max_nsegs;
  int		nsegs;
  int		i,j,k;
  int		ox,oy;
  Position	x1,y1;
  int	coordno;
  char	*str;
  int		lastmsgindex = -1;
  Pixel		treefg = tw->pix.fg;
	int lastaf = -1;
	XPoint mpt[5]; /* used in msgarrows */

  if (tw->pix.pixflags & XO_VISIBLE_NOT) return;
  if (tw->tree.nshapes <= 0) return;
  if (tw->tree.ncoords <= 0) return;

  for (coordno = 0 ; coordno < tw->tree.ncoords; coordno++) {
	struct xtree_leaf_type *leaf = tw->tree.ptlist + coordno;
	if (leaf->shapeflags &
		(XO_TREE_TRUNCATED | XO_TREE_INVISIBLE))
		continue;
	if (leaf->shapeindex < 0 || leaf->shapeindex > tw->tree.nshapes) {
		/* this is an error, and should never happen */
		printf("Warning Tree.c: looking up undefined shape %d\n",
			leaf->shapeindex);
		continue;
	}
	sw = (ShapeObject)tw->tree.shapes[leaf->shapeindex];
	ox = tw->tree.coords[coordno].x;
	oy = tw->tree.coords[coordno].y;
	for(i=0; i < sw->shape.npts; i++) {
		points[i].x = sw->shape.pts[i].x + ox;
		points[i].y = sw->shape.pts[i].y + oy;
    }
/* Copy over the other standard values */
	tw->tree.drawmode = sw->shape.drawmode;
	tw->tree.cx	= sw->pix.cx + ox;
	tw->tree.cy	= sw->pix.cy + oy;
	tw->tree.npts = sw->shape.npts;
	/* Do assorted things related to text display */
	str = leaf->ptname;
	if (str && *str != '\0') {
		strncpy(tw->tree.text,str,XO_TREE_MAX_TEXT_CHARS);
		/* just to be safe */
		tw->tree.text[XO_TREE_MAX_TEXT_CHARS -1] = '\0';

		/* setting up the text color */
		/* It is OK to hard-set the textgc since it is independent */
		if (leaf->textfg == -1) {
			if (tw->tree.textcolor != sw->shape.textcolor) {
				XSetForeground(XtDisplay(XtParent(w)),tw->tree.textgc,
					sw->shape.textcolor);
				tw->tree.textcolor = sw->shape.textcolor;
			}
		} else {
			XSetForeground(XtDisplay(XtParent(w)),tw->tree.textgc,
				leaf->textfg);
			tw->tree.textcolor = leaf->textfg;
		}
		if (tw->tree.textfont != sw->shape.textfont) {
			tw->tree.textfont = sw->shape.textfont;
			XSetFont(XtDisplay(XtParent(w)),tw->tree.textgc,
				sw->shape.textfont->fid);
		}
		tw->tree.textmode = sw->shape.textmode;
	} else {
		tw->tree.textmode = TEXTNODRAW;
	}
	if (leaf->fg == -1)
		tw->pix.fg = sw->pix.fg;
	else
		tw->pix.fg = leaf->fg;
	XSetForeground(XtDisplay(XtParent(w)),tw->pix.gc,tw->pix.fg);
	/* Doing the draw */
	DrawPoints(w);
	if (leaf->shapeflags & XO_TREE_TRUNCATE) {
		/* in this case we offset the original shape a bit, and
		** redraw it in different colors */
		for(i=0; i < sw->shape.npts; i++) {
			points[i].x += 2;
			points[i].y += 2;
    	}
		XSetForeground(XtDisplay(XtParent(w)),tw->pix.gc,
			tw->tree.highlight2);
		DrawPoints(w);
		/* and then we finally redraw it again in the original color */
		for(i=0; i < sw->shape.npts; i++) {
			points[i].x += 2;
			points[i].y += 2;
    	}
		XSetForeground(XtDisplay(XtParent(w)),tw->pix.gc,tw->pix.fg);
		DrawPoints(w);
	}
  }
  /* Do the arrow and tree linking drawing here based on the coords
  ** array. */
  max_nsegs = 0;
  for (i = 0; i < tw->tree.nlist; i++) {
	if (tw->tree.arrowflags[i] == XO_TREE_CHILD_ARROW1)
		max_nsegs++;
	else if (tw->tree.arrowflags[i] < 0)
		max_nsegs+=2;
  }
  if (max_nsegs > 0) {
	segs = (XSegment *)
		XtCalloc(max_nsegs,sizeof(XSegment));
  } else {
	segs = NULL;
  }

  	nsegs = 0;
  	for (i = 0; i < tw->tree.nlist; i++) {
		int af = tw->tree.arrowflags[i];

		if (af == XO_TREE_CHILD_ARROW1) {
			segs[nsegs].x1 = tw->tree.coords[tw->tree.tolist[i]].x;
			segs[nsegs].y1 = tw->tree.coords[tw->tree.tolist[i]].y;
			segs[nsegs].x2 = tw->tree.coords[tw->tree.fromlist[i]].x;
			segs[nsegs].y2 = tw->tree.coords[tw->tree.fromlist[i]].y;
			nsegs++;
		} else if (af < 0) {
			j = -af -1;
			if (j >= 0 && j < tw->tree.ncoords) {
				k = tw->tree.fromlist[i];
				x1 = (tw->tree.coords[j].x - tw->tree.coords[k].x)/2;
				y1 = (tw->tree.coords[j].y - tw->tree.coords[k].y)/2;
	
				segs[nsegs].x2 =
					tw->tree.coords[tw->tree.tolist[i]].x - x1;
				segs[nsegs].y2 = 
					tw->tree.coords[tw->tree.tolist[i]].y - y1;
				segs[nsegs].x1 = tw->tree.coords[tw->tree.tolist[i]].x;
				segs[nsegs].y1 = tw->tree.coords[tw->tree.tolist[i]].y;
				nsegs++;
				segs[nsegs].x1 = tw->tree.coords[j].x - x1;
				segs[nsegs].y1 = tw->tree.coords[j].y - y1;
				segs[nsegs].x2 = segs[nsegs-1].x2;
				segs[nsegs].y2 = segs[nsegs-1].y2;
				nsegs++;
			}
		} else if (af == XO_TREE_CHILD_ARROW2) { 
		} else if (af >= XO_TREE_MSGARROW_OFFSET) {
			/* Assume they are msg arrows. */
			short xa = tw->tree.coords[tw->tree.fromlist[i]].x;
			short ya = tw->tree.coords[tw->tree.fromlist[i]].y;
			short xb = tw->tree.coords[tw->tree.tolist[i]].x;
			short yb = tw->tree.coords[tw->tree.tolist[i]].y;

			double dx = xb - xa;
			double dy = yb - ya;
			double len;

			if (dx == 0 && dy == 0) continue;

			len = sqrt(dx * dx + dy * dy);

			if (len < XO_TREE_ARROW_GAP * 3) continue;

			/* Check if the last arrow was between the same two
			** elements */
			if (i > 0 && lastmsgindex >=0 && 
				tw->tree.fromlist[i] ==
				tw->tree.fromlist[lastmsgindex] &&
				tw->tree.tolist[i] == tw->tree.tolist[lastmsgindex]) {
				int dindex = i - lastmsgindex;
				/* set it up so dindex alternates -ve and +ve */
				dindex = (dindex %2) ? -2 * (dindex + 1) : dindex * 2;
				/* offset the coords by a few pixels perpendicular to
				** the line */
				xa += dindex * dy / len;
				xb += dindex * dy / len;
				ya -= dindex * dx / len;
				yb -= dindex * dx / len;
			} else {
				lastmsgindex = i;
			}

			if (lastaf != af) {
				int fgaf = af - XO_TREE_MSGARROW_OFFSET;
  				XSetForeground(XtDisplay(XtParent(w)),tw->pix.gc,
					fgaf);
			}
			dx /= len;
			dy /= len;

			mpt[0].x = nint(xa + dx * XO_TREE_ARROW_GAP);
			mpt[0].y = nint(ya + dy * XO_TREE_ARROW_GAP);
			mpt[1].x = nint(xb - dx * XO_TREE_ARROW_GAP);
			mpt[1].y = nint(yb - dy * XO_TREE_ARROW_GAP);
			mpt[2].x = mpt[1].x - nint(dx * XO_TREE_ARROWHEAD_LEN +
				dy * XO_TREE_ARROWHEAD_WID);
			mpt[2].y = mpt[1].y - nint(dy * XO_TREE_ARROWHEAD_LEN -
				dx * XO_TREE_ARROWHEAD_WID);
			mpt[3].x = mpt[1].x - nint(dx * XO_TREE_ARROWHEAD_LEN -
				dy * XO_TREE_ARROWHEAD_WID);
			mpt[3].y = mpt[1].y - nint(dy * XO_TREE_ARROWHEAD_LEN +
				dx * XO_TREE_ARROWHEAD_WID);
			mpt[4].x = mpt[1].x;
			mpt[4].y = mpt[1].y;
  			XoDrawLines(XtDisplay(XtParent(w)),XtWindow(XtParent(w)),
				tw->pix.gc, mpt,5,CoordModeOrigin); 
		}
		lastaf = af;
  	}
	tw->pix.fg = treefg;
  	XSetForeground(XtDisplay(XtParent(w)),tw->pix.gc,
		tw->pix.fg);
	if (segs && max_nsegs > 0) {
  		XoDrawSegments(XtDisplay(XtParent(w)),XtWindow(XtParent(w)),
			tw->pix.gc, segs,nsegs,CoordModeOrigin); 
  		XtFree((char *) segs);
	}
}

static void DrawPoints(w)
	Widget w;
{
  TreeObject tw = (TreeObject) w;
  XPoint	*points;
  XSegment	*segs;
  XRectangle	*rects;
  int		nsegs;
  int		i,j;
  int		dx,dy;
  int		rx,ry;
  int		len;

  if (tw->tree.npts > 0) {
		points=tw->tree.pts;
  	switch(tw->tree.drawmode) {
		case DRAWLINES:
  			XoDrawLines(XtDisplay(XtParent(w)), XtWindow(XtParent(w)),
			tw->pix.gc, points,tw->tree.npts,CoordModeOrigin); 
			break;
		case DRAWSEGS:
			segs = (XSegment *)
				XtCalloc(tw->tree.npts/2,sizeof(XSegment));
			for(i=0;i<tw->tree.npts;i++) {
				j=i/2;
				segs[j].x1 = points[i].x;
				segs[j].y1 = points[i].y;
				i++;
				segs[j].x2 = points[i].x;
				segs[j].y2 = points[i].y;
			}
  			XoDrawSegments(XtDisplay(XtParent(w)),XtWindow(XtParent(w)),
			tw->pix.gc, segs,tw->tree.npts/2,CoordModeOrigin); 
			XtFree((char *) segs);
			break;
		case DRAWPOINTS:
  			XoDrawPoints(XtDisplay(XtParent(w)), XtWindow(XtParent(w)),
			tw->pix.gc, points,tw->tree.npts,CoordModeOrigin); 
			break;
		case DRAWARROWS:
			nsegs = tw->tree.npts / 2;
			segs = (XSegment *)
				XtCalloc(3 * nsegs,sizeof(XSegment));
			for(i=0;i<tw->tree.npts - 1;i++) {
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
				tw->pix.gc, segs,3 * nsegs,CoordModeOrigin); 
			XtFree((char *) segs);
			break;
		case FILLRECTS:
			rects = (XRectangle *)
				XtCalloc(tw->tree.npts/2,sizeof(XRectangle));
			for(i=0;i<tw->tree.npts -1 ;i+=2) {
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
				tw->pix.gc, rects,tw->tree.npts/2,CoordModeOrigin); 
			XtFree((char *) rects);
			break;
		case FILLPOLY:
  			XoFillPolygon(XtDisplay(XtParent(w)), XtWindow(XtParent(w)),
			tw->pix.gc, points,tw->tree.npts,CoordModeOrigin); 
			break;
		default:
			break;
  	}
  }
  if (tw->tree.text != NULL && (int)strlen(tw->tree.text) > 0) {
  	switch(tw->tree.textmode) {
		case TEXTDRAW:
			XoDrawCenteredString(XtDisplay(XtParent(w)),
				XtWindow(XtParent(w)), tw->tree.textgc,
				tw->tree.textfont,
				tw->tree.cx,tw->tree.cy,
				tw->tree.text,strlen(tw->tree.text));
			break;
		case TEXTFILL:
			XoFillCenteredString(XtDisplay(XtParent(w)),
				XtWindow(XtParent(w)), tw->tree.textgc,
				tw->tree.textfont,
				tw->tree.cx,tw->tree.cy,
				tw->tree.text,strlen(tw->tree.text));
			break;
		case TEXTNODRAW:
			/* do not draw anything */
			break;
		default:
			break;
  	}
  }
  if (tw->pix.pixflags & XO_IS_SELECTED) { /* do the highlight */
	tw->pix.pixflags &= ~XO_IS_SELECTED;
	(((PixObjectClass)
		(tw->object.widget_class))->pix_class.highlight)(tw,NULL);
  }
}

static void Destroy(w)
	Widget w;
{
  TreeObject tw = (TreeObject) w;

  XtReleaseGC(XtParent(w),tw->pix.gc);
  XtReleaseGC(XtParent(w),tw->tree.textgc);
  if (tw->tree.text)
	XtFree(tw->tree.text);
  if (tw->tree.alloced_pts > 0 && tw->tree.pts)
  	XtFree((char *)(tw->tree.pts));
  if (tw->tree.ncoords > 0 && tw->tree.coords)
  	XtFree((char *)(tw->tree.coords));
}

/* We dont really need this since the parent widget has complete
** control over the gadget layout */
static XtGeometryResult QueryGeometry(w,intended,preferred)
	Widget w;
	XtWidgetGeometry *intended, *preferred;
{
	TreeObject tw = (TreeObject)w;
	preferred->x = tw->pix.x;
	preferred->y = tw->pix.y;
	preferred->width = tw->pix.w;
	preferred->height = tw->pix.h;
	if (intended->request_mode & (CWX | CWY | CWWidth | CWHeight)) {
		if (preferred->x == intended->x &&  
			preferred->y == intended->y &&  
			preferred->width == intended->width &&  
			preferred->height == intended->height)
			return(XtGeometryYes);
	} else {
		if (preferred->x == tw->pix.x &&  
			preferred->y == tw->pix.y &&  
			preferred->width == tw->pix.w &&  
			preferred->height == tw->pix.h)
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
  TreeObject tw = (TreeObject) w;
  CoreDrawWidgetClass class = (CoreDrawWidgetClass)XtClass(XtParent(w));
  int parentht= ((CoreDrawWidget)XtParent(w))->core.height;
  int	*px,*py,*pz;
  int	i,j;
  int		npts;
  XPoint	*points;
  void	(*tpt)();
  void	(*tpts)();
  ShapeObject	sw;
  double *xpts;
  double *ypts;
  double *zpts;
  int		maxx,maxy, minx,miny;
  struct xtree_leaf_type *leaf;

  if (tw->tree.ncoords == 0 || tw->tree.ptlist == NULL ||
	tw->tree.nshapes == 0 || tw->tree.shapes == NULL) return;
  if (tw->pix.pixflags & XO_VISIBLE_NOT) return;
  if (tw->pix.pixflags & XO_RESIZABLE_NOT){ /*Freeze size and position*/
		return;
  }
  if (tw->pix.pixflags & XO_PIXELCOORDS) { /* transform using pixels*/
		tpts = PixelTransformPoints;
  } else { /* use standard transformations */
		tpts = class->coredraw_class.transformpts;
  }
  if (tw->pix.pixflags & XO_PIXELOFFSET) { /* transform using pixels*/
		tpt = PixelTransformPoint;
  } else { /* use standard transformations */
		tpt = class->coredraw_class.transformpt;
  }

  if (tw->pix.pixflags & XO_PCTCOORDS) { /* transform using % of win */
		tpts = PctTransformPoints;
  }
  if (tw->pix.pixflags & XO_PCTOFFSET) { /* transform using % of win */
		tpt = PctTransformPoint;
  }

  /* Calculate offset on screen using parents TransformPoint routine */
  (tpt)(XtParent(w),
	tw->pix.tx,tw->pix.ty, tw->pix.tz,&x,&y,&z);
	tw->pix.cx = x; tw->pix.cy = y; tw->pix.cz=z;

  	/* Calculate origin on screen using parents
  	** TransformPoint routine, only if it is usual transforms */
	if (!(tw->pix.pixflags & (XO_PIXELCOORDS | XO_PCTCOORDS))) {
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

/* Apply the transforms to all the tree coords */
  	px = (int *) XtCalloc(tw->tree.ncoords,sizeof(int));
  	py = (int *) XtCalloc(tw->tree.ncoords,sizeof(int));
  	pz = (int *) XtCalloc(tw->tree.ncoords,sizeof(int));
/* Apply the offsets */
  	xpts = (double *) XtCalloc(tw->tree.ncoords,sizeof(double));
  	ypts = (double *) XtCalloc(tw->tree.ncoords,sizeof(double));
  	zpts = (double *) XtCalloc(tw->tree.ncoords,sizeof(double));
	for(j=0;j<tw->tree.ncoords;j++) {
		leaf = tw->tree.ptlist + j;
		xpts[j] = tw->pix.tx + leaf->xpts;
		ypts[j] = tw->pix.ty + leaf->ypts;
		zpts[j] = tw->pix.tz + leaf->zpts;
	}
/* do the transform */
	(tpts)(XtParent(w),
		xpts, ypts, zpts,
		px,py,pz,tw->tree.ncoords);
/* store the transformed coords */
	points = (XPoint *)tw->tree.coords;
	/* The ox and oy are subtracted here */
	if (!(tw->pix.pixflags & (XO_PIXELCOORDS | XO_PCTCOORDS))) {
		for(j=0;j<tw->tree.ncoords;j++) {
			points[j].x = px[j];
			points[j].y = py[j];
			/*
			points[j].x = px[j] - ox;
			points[j].y = py[j] - oy;
			*/
		}
	} else {
		for(j=0;j<tw->tree.ncoords;j++) {
			points[j].x = px[j];
			/*points[j].y = py[j] - parentht; */
			points[j].y = py[j];
		}
	}
/* Find the bounds to use for the xtree pix */
/* These bounds should be rather generous, because the 
** shapes can protrude outside them. */
	maxx = minx =  px[0];
	maxy = miny =  py[0];
	for(j=1;j<tw->tree.ncoords;j++) {
		if (maxx < px[j]) maxx = px[j];
		if (maxy < py[j]) maxy = py[j];
		if (minx > px[j]) minx = px[j];
		if (miny > py[j]) miny = py[j];
	}
	tw->pix.x = minx - 100;
	tw->pix.y = miny - 100;
	tw->pix.w = maxx - minx + 200;
	tw->pix.h = maxy - miny + 200;

	XtFree((char *) px);
	XtFree((char *) py);
	XtFree((char *) pz);
	/* Free the xpts etc */
	XtFree((char *) xpts);
	XtFree((char *) ypts);
	XtFree((char *) zpts);

/* Apply the transforms individually to each shape */
  for (i = 0 ; i < tw->tree.nshapes; i++) {
  		sw = (ShapeObject)(tw->tree.shapes[i]);
  		/* Calculate offset on screen using parents
		TransformPoint routine */
  		(tpt)(XtParent(w),
			sw->pix.tx,sw->pix.ty,sw->pix.tz,&cx,&cy,&cz);

		if (tw->pix.pixflags & (XO_PIXELCOORDS | XO_PCTCOORDS)) {
			x = cx; y = cy - parentht - parentht;
			sw->pix.cx = cx; sw->pix.cy = cy - parentht;
		} else {
			sw->pix.cx = cx - ox; sw->pix.cy = cy - oy;
			x = cx - ox; y = cy - oy;
		}

  		if (sw->shape.npts > 0) {
  			void	(*shape_tpts)();
  			int tx = x;
  			int ty = y;
	  		px = (int *) XtCalloc(sw->shape.npts,sizeof(int));
	  		py = (int *) XtCalloc(sw->shape.npts,sizeof(int));
	  		pz = (int *) XtCalloc(sw->shape.npts,sizeof(int));
	  		/* Figure out which version of the transforms to use.
	  		** The default is to use the same as the tree, but
	  		** this may have to be changed
	  		*/
	  		if (sw->pix.pixflags & XO_PIXELCOORDS) {
	  			shape_tpts = PixelTransformPoints;
	  			ty -= parentht;
	  		} else if (sw->pix.pixflags & XO_PCTCOORDS) {
	  			shape_tpts = PctTransformPoints;
	  			ty -= parentht;
	  		} else {
	  			shape_tpts = tpts;
	  			/* We need to put in these adjustments
	  			** for the xshape transforms. They
	  			** are not needed for the PIXEL or PCTCOORDS
	  			*/
	  			tx -= ox;
	  			ty -= oy;
	  		}
		
	  		(shape_tpts)(XtParent(w),
				sw->shape.xpts,sw->shape.ypts, sw->shape.zpts,
				px,py,pz,sw->shape.npts);

	  		points = (XPoint *)sw->shape.pts;
			for(j=0;j<sw->shape.npts;j++) {
				points[j].x = tx + px[j];
				points[j].y = ty + py[j];
			}
	  		XtFree((char *) px);
	  		XtFree((char *) py);
	  		XtFree((char *) pz);
		}
  }
/* Copy over the pts for shape 0 to the pts array in case morphing
** is not being used */
	sw = (ShapeObject)(tw->tree.shapes[0]);
	points = sw->shape.pts;
	npts = ((ShapeObject)(tw->tree.shapes[0]))->shape.npts;
	if (npts > tw->tree.alloced_pts) {
		printf("Error:  Project in Tree.c: npts allocation error\n");
	}
	for(i = 0 ; i < npts; i++) {
		tw->tree.pts[i] = points[i];
	}
	tw->tree.npts = npts;
/* Copy over the other standard values */
	tw->tree.drawmode = sw->shape.drawmode;
	strncpy(tw->tree.text,sw->shape.text,XO_TREE_MAX_TEXT_CHARS);
	/* just to be safe */
	tw->tree.text[XO_TREE_MAX_TEXT_CHARS -1] = '\0';
	tw->tree.textcolor = sw->shape.textcolor;
	tw->tree.textfont = sw->shape.textfont;
/*	tw->pix.fg = sw->pix.fg; */
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
	TreeObject tw = (TreeObject)w;
	ShapeObject sw;
	int i,j,besti;
	int	bestr;
	float	tx,ty;

	/* use XO_MAX_CLICK_DIST */
	if (tw->pix.pixflags & XO_CLICKABLE_NOT) {
		*dist = XO_MAX_SEL_DIST;
		return;
	}

	bestr = sqr(XO_MAX_SEL_DIST);
	besti = -1;
	for(i = 0 ; i < tw->tree.ncoords; i++) {
		/* Bypass all leaves which are truncated or invisible. */
		if (tw->tree.ptlist[i].shapeflags &
			(XO_TREE_TRUNCATED | XO_TREE_INVISIBLE))
			continue;

		/* find the distance to each of the shape coords */
		/* Here we find 3 distances: the nearest pt on the xshape=maxr,
		** to the 'average' of the points on the xshape (ans),
		** and the distance from the 'average' pt to the near pt = rj.
		** Our select-dist is maxr + ans - rj
		** In addition, we also find the distance to the text
		** center, if any, and use that if it is closer
		*/
		sw = (ShapeObject)tw->tree.shapes[tw->tree.ptlist[i].shapeindex];
		if (sw->shape.npts > 0) {
			int mx = 0,my = 0;
			int maxr = 1e6;
			int maxj = -1;
			int rj = 1e6;
			for(j=0; j < sw->shape.npts; j++) {
				tx = tw->tree.coords[i].x + sw->shape.pts[j].x - x;
				ty = tw->tree.coords[i].y + sw->shape.pts[j].y - y;
				ans = tx * tx + ty * ty;
				mx += tx;
				my += ty;
				if (ans < maxr) {
					maxr = ans;
					maxj = j;
				}
    		}
			tx = mx / sw->shape.npts;
			ty = my / sw->shape.npts;
			ans = tx * tx + ty * ty; /* dist to center of shape pts */
			ans = (int)sqrt((double)ans);
			maxr = (int)sqrt((double)maxr);
			tx = tw->tree.coords[i].x + sw->shape.pts[maxj].x - x;
			ty = tw->tree.coords[i].y + sw->shape.pts[maxj].y - y;
			rj = tx * tx + ty * ty; /* dist from center to nearest pt*/
			rj = (int)sqrt((double)rj);
			j = (maxr + ans - rj) * (maxr + ans - rj);
			if (bestr > j) {
				bestr = j;
				besti = i;
			}
			/* need to put in a check for the textmode here, and
			** if it is doing text, find out how far from the text
			** the click site was */
    	}
		/* Find the distance to the shape position */
		tx = tw->tree.coords[i].x - x;
		ty = tw->tree.coords[i].y - y;
		ans = tx * tx + ty * ty;
		if (ans < bestr) {
			bestr = ans;
			besti = i;
    	}
	}
	/*
	** We put this in because we do not want the tree to claim
	** control over blank areas of the screen which nevertheless
	** are surrounded by tree components
	** We should really do this using the size of the shape pix
	** used for the icon.
	*/
	if (bestr > XO_TREE_MAX_SEL_DIST) {
		bestr = sqr(XO_MAX_SEL_DIST);
		besti = -1;
	}
	ans = (int)sqrt((double)bestr);
					
	*dist = (Dimension)(ans > 0 ? ans : 0);
	tw->pix.selected = besti;
	if (besti >= 0 && besti < tw->tree.ncoords)
		tw->pix.cz = tw->tree.ptlist[besti].zpts;
	else
		tw->pix.cz = 0;
	return;
}
#undef sqr

static void Select(w,event,info)
	Widget w;
	XButtonEvent	*event;
	XoEventInfo		*info;
{
	TreeObject	tw = (TreeObject)w;
	PixObjectClass class = (PixObjectClass) tw->object.widget_class;
	static int ret;
	/* static char	ret[20]; */

	if (tw->pix.selected < 0 || tw->pix.selected >= tw->tree.ncoords)
		return;

	tw->tree.ptlist[tw->pix.selected].shapeflags |= XO_TREE_HIGHLIGHT1;

	if (tw->tree.hlmode == XoTreeHlOne || 
		tw->tree.hlmode == XoTreeHlMulti)
	/* here we just flip the state of the current item */
		tw->tree.ptlist[tw->pix.selected].shapeflags ^=
			XO_TREE_HIGHLIGHT2;

	(class->pix_class.highlight)(w,event);

	ret = tw->pix.selected;
	/* sprintf(ret,"%d",tw->pix.selected); */
	/* info->ret = (XtPointer)ret; */
	info->ret = (XtPointer)(&ret);

	/*
	** The following original code makes no sense, since pix.w and pix.h
	** are unsigned integers.  I'm removing the tests.
	** wid = (tw->pix.w < 0) ? 0 : tw->pix.w;
	** h = (tw->pix.h < 0) ? 0 : tw->pix.h;
	*/
	XoCopyCursor(XtDisplay(XtParent(w)),XtWindow(XtParent(w)),
		tw->pix.gc,
		tw->pix.x, tw->pix.y, tw->pix.w, tw->pix.h);

	XtCallCallbackList(w,tw->pix.callbacks, (XtPointer)info);
}

static void UnSelect(w,event,info)
	Widget w;
	XButtonEvent	*event;
	XoEventInfo		*info;
{
	TreeObject	tw = (TreeObject)w;
	PixObjectClass class = (PixObjectClass) tw->object.widget_class;

	XoRestoreCursor(XtDisplay(XtParent(w)),XtWindow(XtParent(w)));


	if (tw->pix.selected < 0 || tw->pix.selected >= tw->tree.ncoords)
		return;

	tw->tree.ptlist[tw->pix.selected].shapeflags &= ~XO_TREE_HIGHLIGHT1;
   tw->tree.ptlist[tw->pix.selected].shapeflags |= XO_TREE_UNHIGHLIGHT1;
	(class->pix_class.highlight)(w,event);
}

static void Highlight(w,event)
	Widget w;
	XButtonEvent	*event;
{
	TreeObject	tw = (TreeObject)w;
	ShapeObject	sw;
	int	ox,oy;
	XPoint	 *points = tw->tree.pts;
	int	colorflag;
	int i;
	Pixel treefg = tw->pix.fg;
	int minx, maxx, miny, maxy;
	struct xtree_leaf_type *leaf;

	if (tw->pix.selected < 0 || tw->pix.selected >= tw->tree.ncoords)
		return;

	leaf = tw->tree.ptlist + tw->pix.selected;
	sw = (ShapeObject)
		tw->tree.shapes[leaf->shapeindex];
	ox = tw->tree.coords[tw->pix.selected].x;
	oy = tw->tree.coords[tw->pix.selected].y;
	if (sw->shape.npts > 0) {
		maxx = minx = sw->shape.pts[0].x + ox;
		maxy = miny = sw->shape.pts[0].y + oy;
	} else {
		minx = ox - 10;
		maxx = ox + 10;
		miny = oy - 10;
		maxy = oy + 10;
	}
	for(i=0; i < sw->shape.npts; i++) {
		points[i].x = sw->shape.pts[i].x + ox;
		points[i].y = sw->shape.pts[i].y + oy;
		if (points[i].x > maxx) maxx = points[i].x;
		else if (points[i].x < minx) minx = points[i].x;
		if (points[i].y > maxy) maxy = points[i].y;
		else if (points[i].y < miny) miny = points[i].y;
    }
	minx--;maxx++; miny--; maxy++;
/* Copy over the other standard values */
	tw->tree.drawmode = sw->shape.drawmode;
	if (leaf->ptname && *(leaf->ptname) != '\0') {
		strncpy(tw->tree.text,leaf->ptname,XO_TREE_MAX_TEXT_CHARS);
		/* just to be safe */
		tw->tree.text[XO_TREE_MAX_TEXT_CHARS -1] = '\0';
	} else {
		tw->tree.text = "";
	}
	tw->tree.textcolor = sw->shape.textcolor;

	/* setting up the text color */
	if (leaf->textfg == -1) {
		if (tw->tree.textcolor != sw->shape.textcolor) {
			XSetForeground(XtDisplay(XtParent(w)),tw->tree.textgc,
				sw->shape.textcolor);
			tw->tree.textcolor = sw->shape.textcolor;
		}
	} else {
			XSetForeground(XtDisplay(XtParent(w)),tw->tree.textgc,
				leaf->textfg);
			tw->tree.textcolor = leaf->textfg;
	}
	if (tw->tree.textfont != sw->shape.textfont) {
		tw->tree.textfont = sw->shape.textfont;
		XSetFont(XtDisplay(XtParent(w)),tw->tree.textgc,
			sw->shape.textfont->fid);
	}
	tw->tree.textmode = sw->shape.textmode;
	tw->tree.cx	= sw->pix.cx + ox;
	tw->tree.cy	= sw->pix.cy + oy;

	tw->tree.npts = sw->shape.npts;
	if (leaf->fg == -1)
		tw->pix.fg = sw->pix.fg;
	else 
		tw->pix.fg = leaf->fg;
	colorflag = leaf->shapeflags &
	   (XO_TREE_HIGHLIGHT1 | XO_TREE_HIGHLIGHT2 | XO_TREE_UNHIGHLIGHT1);
	if (colorflag & XO_TREE_HIGHLIGHT1) {
		tw->pix.fg = tw->tree.highlight1;
		XSetForeground(XtDisplay(XtParent(w)),tw->pix.gc, tw->pix.fg);
		XDrawRectangle(XtDisplay(XtParent(w)),XtWindow(XtParent(w)),
			tw->pix.gc,
			minx, miny, (Dimension)(maxx-minx), (Dimension)(maxy-miny));
		tw->pix.fg = sw->pix.fg;
	}
	if (colorflag & XO_TREE_UNHIGHLIGHT1) {
		/* Get rid of the unhighlight flag */
		tw->tree.ptlist[tw->pix.selected].shapeflags &=
			~XO_TREE_UNHIGHLIGHT1;
		/* Draw the rectangle over in the background color */
		tw->pix.fg = tw->tree.bg;
		XSetForeground(XtDisplay(XtParent(w)),tw->pix.gc, tw->pix.fg);
		XDrawRectangle(XtDisplay(XtParent(w)),XtWindow(XtParent(w)),
			tw->pix.gc,
			minx, miny, (Dimension)(maxx-minx), (Dimension)(maxy-miny));
		if (leaf->fg == -1)
			tw->pix.fg = sw->pix.fg;
		else 
			tw->pix.fg = leaf->fg;
	}
	if (colorflag & XO_TREE_HIGHLIGHT2)
		tw->pix.fg = tw->tree.highlight2;
	XSetForeground(XtDisplay(XtParent(w)),tw->pix.gc,
				tw->pix.fg);

	/* Doing the draw */
	DrawPoints(w);
	tw->pix.fg = treefg;
}

static Boolean XoCvtStringToTreemode(dpy,args,num_args,fromVal,toVal,
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
		if (strcmp(fromVal->addr,"tree") == 0)
			ret = XoTreeTreeMode;
		if (strcmp(fromVal->addr,"graph") == 0)
			ret = XoTreeCustomMode;
		if (strcmp(fromVal->addr,"surface") == 0)
			ret = XoTreeGeometryMode;
		if (ret >=0) {
			XoCvtDone(int,ret);
		}
	}
	toVal->addr = NULL;
	toVal->size = 0;
	XtDisplayStringConversionWarning(dpy,fromVal->addr,"XtRTreemode");
		return((Boolean) False);
}

static Boolean XoCvtStringToHlmode(dpy,args,num_args,fromVal,toVal,
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
		if (strcmp(fromVal->addr,"none") == 0)
			ret = XoTreeHlNone;
		if (strcmp(fromVal->addr,"one") == 0)
			ret = XoTreeHlOne;
		if (strcmp(fromVal->addr,"multi") == 0)
			ret = XoTreeHlMulti;
		if (ret >=0) {
			XoCvtDone(int,ret);
		}
	}
	toVal->addr = NULL;
	toVal->size = 0;
	XtDisplayStringConversionWarning(dpy,fromVal->addr,"XtRHlmode");
		return((Boolean) False);
}

void XoUpdateTree(w)
	Widget w;
{
	Expose(w);
}

void XoProjectTree(w)
	Widget w;
{
	Project(w);
	XClearArea(XtDisplay(XtParent(w)),
		XtWindow(XtParent(w)),0,0,0,0,True);
	/* Expose(w); */
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
#undef XO_TREE_MAX_SEL_DIST
