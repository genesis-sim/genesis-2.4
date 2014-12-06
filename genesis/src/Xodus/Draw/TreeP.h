/* $Id: TreeP.h,v 1.1.1.1 2005/06/14 04:38:33 svitak Exp $ */
/*
 * $Log: TreeP.h,v $
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.5  2000/06/12 04:28:18  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.4  1996/06/27 23:46:56  venkat
 * Changed the type of the colorscale resource to long (unsigned).
 *
 * Revision 1.3  1994/03/30  17:25:24  bhalla
 * Did assorted changes associated with cretion of the xtree_leaf_struct
 * to handle info about each item on tree
 *
 * Revision 1.2  1994/01/13  19:35:36  bhalla
 * *** empty log message ***
 *
 * Revision 1.2  1994/01/13  19:35:36  bhalla
 * *** empty log message ***
 * */
#ifndef _TreeP_h
#define _TreeP_h

#include "Tree.h"
#include "PixP.h" /* include superclass private header file */

/*
** Relevant fields :
** The core fields for any tree are :
** float x,y,w,h : these are fractions of the parent widget and specify
** the click-sensitive region and the rectangle that the tree gadget
** occupies. It is not necessary to restrict the tree to within this
** region, but it may be a good idea. These are private fields.
** fg: Foreground color. This is a public field
** gc : for graphics - there can be additional gcs too. This is private
** callbacks This is private
*/

typedef struct _TreeClassPart {
	int make_compiler_happy;
} TreeClassPart;

typedef struct _TreeClassRec {
  RectObjClassPart	rect_class;
  PixClassPart		pix_class;
  TreeClassPart		tree_class;
} TreeClassRec;

extern TreeClassRec treeClassRec;

typedef struct {
	/* Resources */
	Widget		*shapes; /* array of the shape ptrs */
	int			nshapes;
	int			shapechange; /* to warn of changes to shapes */
	int			treemode; /* tree, custom, geometry */
	unsigned long*		colorscale;	/* color lookup table */
	int			ncols;

	/* Private fields, duplicating many of the Shape fields */
	Pixel	bg;
	int		npts;
	int		drawmode;
	String	text;
	Pixel	textcolor;
	int		textmode;
	XFontStruct *textfont;
	XPoint	*pts;
	int		alloced_pts;
	GC		textgc;
	Position	cx;
	Position	cy;

	/* fields distinct from the Var fields */
	struct xtree_leaf_type *ptlist; /* coords etc of each leaf */
	int		ncoords; /* This is the number of pts in xpts etc */
	XPoint	*coords; /* These are the projected coords of each var */
	/* private fields */
	int		selected;
	Pixel	highlight1;
	Pixel	highlight2;
	int		hlmode;
	int		*fromlist;
	int		*tolist;
	int		*arrowflags;
	int		nlist;
} TreePart;

typedef struct _TreeRec {
  ObjectPart		object;
  RectObjPart	rectangle;
  PixPart		pix;
  TreePart		tree;
} TreeRec;


#endif /* _TreeP_h */
