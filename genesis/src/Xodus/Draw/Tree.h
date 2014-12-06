/* $Id: Tree.h,v 1.1.1.1 2005/06/14 04:38:33 svitak Exp $ */
/*
 * $Log: Tree.h,v $
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.4  2000/06/12 04:28:18  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.3  1994/03/30 17:25:07  bhalla
 * Did assorted changes associated with cretion of the xtree_leaf_struct
 * to handle info about each item on tree
 *
 * Revision 1.2  1994/01/13  19:35:36  bhalla
 * *** empty log message ***
 *
 * Revision 1.2  1994/01/13  19:35:36  bhalla
 * *** empty log message ***
 * */
#ifndef _Tree_h
#define _Tree_h

/************************************************************
  
  Tree Object.

*/


/* Resources: inherited from Pix ..

 Name                Class              RepType         Default Value
 ----                -----              -------         -------------
 destroyCallback     Callback           Pointer         NULL
 selectCallback      Callback           Pointer         NULL
 sensitive           Sensitive          Boolean         True

 x                   Position           Position        0
 y                   Position           Position        0
width               Width              Dimension       0
 height              Height             Dimension       0

 pixX	     Position		Position	0
 pixY	     Position		Position	0
 pixZ	     Position		Position	0

 fg		     Foreground         Pixel		BlackPixel

 Own resources
 mode		      Mode		String		"DrawLine"
 npts		      Npts		int		0
 xpts		      Xpts		Pointer		NULL
 ypts		      Xpts		Pointer		NULL
 zpts		      Xpts		Pointer		NULL
*/

#include "TreeLeaf.h"

#define XoTreeTreeMode 1
#define XoTreeCustomMode 2
#define XoTreeGeometryMode 3

#define XoTreeHlNone 0
#define XoTreeHlOne 1
#define XoTreeHlMulti 2

#define XO_TREE_HIGHLIGHT1 0x01
#define XO_TREE_HIGHLIGHT2 0x02
#define XO_TREE_SELECTED	0x04
#define XO_TREE_TRUNCATE	0x08
#define XO_TREE_TRUNCATED	0x010
#define XO_TREE_DISABLE		0x020
#define XO_TREE_INVISIBLE	0x040
#define XO_TREE_UNHIGHLIGHT1 0x80

#define XO_TREE_CHILD_ARROW1 0
#define XO_TREE_CHILD_ARROW2 1
#define XO_TREE_MSGARROW_OFFSET 10

#ifndef XtNcolorscale
#define	XtNcolorscale	"colorscale"
#define	XtNncols	"ncols"
#define XtCColorscale	"colorscale"
#define XtCNcols	"Ncols"
#endif


#ifndef XtNshapes
#define XtNshapes	"shapes"
#define XtCShapes	"Shapes"
#endif

#ifndef XtNnshapes
#define XtNnshapes		"nshapes"
#define XtCNshapes			"Nshapes"
#endif

#ifndef XtNshapechange
#define XtNshapechange	"shapechange"
#define XtCShapechange		"Shapechange"
#endif

#ifndef XtNptlist
#define XtNptlist "ptlist"
#define XtCPtlist "Ptlist"
#endif

/*
#ifndef XtNshapeflags
#define XtNshapeflags	"shapeflags"
#define XtCShapeflags	"Shapeflags"
#endif


#ifndef XtNshapeindex
#define XtNshapeindex		"shapeindex"
#define XtCShapeindex		"Shapeindex"
#endif
*/

#ifndef XtNtreemode
#define XtNtreemode		"treemode"
#define XtCTreemode			"Treemode"
#define XtRTreemode		"Treemode"
#endif

#ifndef XtNncoords
#define XtNncoords	"ncoords"
#define XtCNcoords	"Ncoords"
#endif

/*
#ifndef XtNxpts
#define XtNxpts	"xpts"
#endif

#ifndef XtCXpts
#define XtCXpts "Xpts"
#endif

#ifndef XtNypts
#define XtNypts	"ypts"
#endif

#ifndef XtCYpts
#define XtCYpts "Ypts"
#endif

#ifndef XtNzpts
#define XtNzpts	"zpts"
#endif

#ifndef XtCZpts
#define XtCZpts "Zpts"
#endif

#ifndef XtNptnames
#define XtNptnames "ptnames"
#define XtCPtnames "Ptnames"
#endif
*/

#ifndef XtNhighlight1
#define XtNhighlight1 "highlight1"
#endif

#ifndef XtNhighlight2
#define XtNhighlight2 "highlight2"
#endif

#ifndef XtNhlmode
#define XtNhlmode "hlmode"
#endif

#ifndef XtCHlmode
#define XtCHlmode "Hlmode"
#endif

#ifndef XtRHlmode
#define XtRHlmode "Hlmode"
#endif

#ifndef XtNfromlist
#define XtNfromlist "fromlist"
#define XtCFromlist "Fromlist"
#endif

#ifndef XtNtolist
#define XtNtolist "tolist"
#define XtCTolist "Tolist"
#endif

#ifndef XtNarrowflags
#define XtNarrowflags "arrowflags"
#define XtCArrowflags "Arrowflags"
#endif

#ifndef XtNnlist
#define XtNnlist "nlist"
#define XtCNlist "Nlist"
#endif



typedef struct _TreeClassRec*       TreeObjectClass;
typedef struct _TreeRec*            TreeObject;
typedef struct _TreeRec*            Tree;

extern WidgetClass treeObjectClass;

#endif
