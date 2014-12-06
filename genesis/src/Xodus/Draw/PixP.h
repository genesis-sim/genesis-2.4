/* $Id: PixP.h,v 1.1.1.1 2005/06/14 04:38:32 svitak Exp $ */
/*
 * $Log: PixP.h,v $
 * Revision 1.1.1.1  2005/06/14 04:38:32  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.4  2000/06/12 04:28:17  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.3  1994/02/02 18:49:54  bhalla
 * Added pix.selected field for use in complex pixes with subparts that
 * can be selected
 *
 * Revision 1.2  1994/01/13  19:35:36  bhalla
 * *** empty log message ***
 * */
#ifndef _PixP_h
#define _PixP_h

#include "Pix.h"

/*
** Relevant fields :
** The core fields for any pix are :
** float x,y,w,h : these are fractions of the parent widget and specify
** the click-sensitive region and the rectangle that the pix gadget
** occupies. It is not necessary to restrict the pix to within this
** region, but it may be a good idea. These are private fields.
** fg: Foreground color. This is a public field
** gc : for graphics - there can be additional gcs too. This is private
** callbacks This is private
*/

typedef struct _PixClassPart {
	/* put any new class info (possibly new methods) here */
	void (*project)(); /* handles pix-to-screen conversion,
		       **usually using parent TransformPoint method*/
	void (*undraw)(); /* This is used for refresh of pixes which
			** need to clear the pixels previously drawn into.
			** it can take the values:
			** XoNoUndraw - No undraw needed. This is the default
			** XoUndrawByClearing() - Need to clear entire draw window
			** XoUndrawProc() - the pixes own undraw method.
			*/

	void (*select_distance)(); /* Finds the dist between the point
			select_x,select_y,select_z, and the pix */

	void (*select)();
	/* Invoked by the parent when the pix is selected. The
	** default function calls the pix callbacklist */

	void (*unselect)();
	/* Invoked by the parent when the select event is completed.
	** The inherted function does nothing */

	void (*motion)(); /* Handles motion events in the selected pix.
					** The inherited function XoNoMotion does nothing.*/
	void (*highlight)(); /* Does highlighting/unhighlighting.
						** the inherited default draws an XOR
						** rectangle */
	XtPointer extension;
} PixClassPart;

typedef struct _PixClassRec {
  RectObjClassPart	rect_class;
  PixClassPart		pix_class;
} PixClassRec;

#define XoInheritProject		_XtInherit
#define XoInheritUndraw			_XtInherit
#define XoInheritSelectDistance	_XtInherit
#define XoInheritSelect			_XtInherit
#define XoInheritUnSelect		_XtInherit
#define XoInheritMotion			_XtInherit
#define XoInheritHighlight		_XtInherit

extern PixClassRec pixClassRec;

typedef struct {
  /* resources */

  /* position resources to specify geometry of object in actual space */

  float		tx;
  float		ty;
  float		tz;

  /* graphics resources from which to set GC */
  Pixel			fg;

  /* A flag for determining many of the pix options */
  int			pixflags; /* need at least 17 bits for this */
  /* Private pix fields */
  /* These are not core fields */
  /* Bounding box */
  Position		x;
  Position		y;
  Dimension		w;
  Dimension		h;
  Position		cx;	/* These are screen coords for center of pix */
  Position		cy;
  float			cz; /* This is the depth info for pix, used in
					** deciding drawing order for hidden pixes */
  /* GC for drawing */
  GC			gc;
  /* GC for doing highlight. This will usually do an XOR in the
  ** foreground color,
  ** so as to keep the number of distinct GCs down and promote
  ** sharing, but I put it here to enable variations */
  GC			hlgc;
  /* callbacks */
  XtCallbackList	callbacks;

  /* Used to identify a subpart of a pix that has been selected **
  ** Not all pixes use this field, but it is involved in the core **
  ** actions relating to selection so it must be placed here */
  int	selected;
} PixPart;

typedef struct _PixRec {
  ObjectPart		object;
  RectObjPart	rectangle;
  PixPart		pix;
} PixRec;


#endif /* _PixP_h */
