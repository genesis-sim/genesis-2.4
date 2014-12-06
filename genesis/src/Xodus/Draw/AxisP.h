/* $Id: AxisP.h,v 1.1.1.1 2005/06/14 04:38:33 svitak Exp $ */
/*
 * $Log: AxisP.h,v $
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.6  2000/06/12 04:28:16  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.5  1995/03/22 18:57:07  venkat
 * Coalesced the two resources into a single unitsjustify
 * string resource member for backwards comaptibility and extensibility
 *
 * Revision 1.3  1994/12/06  00:17:22  dhb
 * Nov 8 1994 changes from Upi Bhalla
 *
 * Revision 1.2  1994/01/13  19:35:36  bhalla
 * *** empty log message ***
 *
 * Revision 1.2  1994/01/13  19:35:36  bhalla
 * *** empty log message ***
 * */
#ifndef _AxisP_h
#define _AxisP_h

#include "Axis.h"
#include "PixP.h" /* include superclass private header file */

/*
** Relevant fields :
** The core fields for any axis are :
** float x,y,w,h : these are fractions of the parent widget and specify
** the click-sensitive region and the rectangle that the axis gadget
** occupies. It is not necessary to restrict the axis to within this
** region, but it may be a good idea. These are private fields.
** fg: Foreground color. This is a public field
** gc : for graphics - there can be additional gcs too. This is private
** callbacks This is private
*/

typedef struct _AxisClassPart {
	int make_compiler_happy;
} AxisClassPart;

typedef struct _AxisClassRec {
  RectObjClassPart	rect_class;
  PixClassPart		pix_class;
  AxisClassPart		axis_class;
} AxisClassRec;

extern AxisClassRec axisClassRec;

typedef struct {
	/* resources */
	int		linewidth;
	int 	linestyle;
	String	units;
	String  unitsjustify;
	String	title;
	float	unitoffset;
	float	titleoffset;
	float	labeloffset; /* dist in draw coords of labels from axis */
	int		rotatelabels; /* Flag for rotating labels of axis */
	Pixel	textcolor;
	int		textmode; /* draw, fill, absent, radraw, rafill */
	float	ticklength; /* sign determines if in,out or absent */
	int		tickmode; /* every 5, every 10, auto.... */
	float	axx,axy,axz; /* vector defining axis */
	float	tickx,ticky,tickz; /* vector defining tick marks */
	float	axmin,axmax;	/* axis range in plot coords */
	float	labmin,labmax;	/* label range in plot coords */
	float	axlen;		/* length of axis in draw widget coords */
	XtCallbackList	refresh_callbacks;
	XFontStruct	*textfont;
	/* Private fields */
	XSegment *ticks;
	char	**labels;
	int		*labelx;
	int		*labely;
	int		unitsx;
	int		unitsy;
	int		titlex;
	int		titley;
	int		nticks;
	int		nlabels; /* first tick to mark with label */
	int		clickpt; /* which label was clicked on */
	int		alloced_ticks;
	int		alloced_labels;
	int		roundrange;
	GC		textgc;
} AxisPart;

typedef struct _AxisRec {
  ObjectPart		object;
  RectObjPart	rectangle;
  PixPart		pix;
  AxisPart		axis;
} AxisRec;


#endif /* _AxisP_h */
