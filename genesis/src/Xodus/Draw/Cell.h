/* $Id: Cell.h,v 1.1.1.1 2005/06/14 04:38:32 svitak Exp $ */
/*
 * $Log: Cell.h,v $
 * Revision 1.1.1.1  2005/06/14 04:38:32  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.5  2000/06/12 04:28:16  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.4  1995/03/17 20:28:24  venkat
 * Definition of cell mode resource constants- Upi changes
 *
 * Revision 1.2  1994/01/13  19:35:36  bhalla
 * *** empty log message ***
 *
 * Revision 1.2  1994/01/13  19:35:36  bhalla
 * *** empty log message ***
 * */
#ifndef _Cell_h
#define _Cell_h

/************************************************************
  
  Cell Object.

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
 linewidth		  Linewidth int		1
 linestyle		  Linestyle int		1
 npts		      Npts		int		0
 frompts	      Npts		int		0
 topts		      Npts		int		0
 xpts		      Xpts		Pointer		NULL
 ypts		      Xpts		Pointer		NULL
*/

#ifndef XtNcolmin
#define XtNcolmin	"colmin"
#define XtNcolmax	"colmax"
#define XtNdiamin	"diamin"
#define XtNdiamax	"diamax"
#define XtNdiarange	"diarange"
#endif

#ifndef XtNxpts
#define XtNxpts			"xpts"
#define XtNypts			"ypts"
#define XtNzpts			"zpts"
#define XtCPts			"Pts"
#endif

#define XtNsoma			"soma"
#define XtCSoma			"Soma"

#ifndef XtNnpts
#define XtNnpts			"npts"
#define XtCNpts			"Npts"
#endif

#ifndef XtNfromlist
#define XtNfromlist			"fromlist"
#define XtNtolist			"tolist"
#define XtNshapelist		"shapelist"
#define XtNnlist			"nlist"
#define XtCFromlist			"Fromlist"
#endif

/*
#ifndef XtNparenthood
#define XtNparenthood			"parenthood"
#define XtCParenthood			"Parenthood"
#endif
*/

#ifndef XtNcolor
#define XtNcolorscale	"colorscale"
#define XtCColorscale	"Colorscale"
#define XtNcolor		"color"
#define XtNdia			"dia"
#define XtNnames		"names"
#define XtNncols		"ncols"
#endif

#ifndef XtCRange
#define XtCRange		"Range"
#endif

#ifndef XtNcellmode
#define XtNcellmode "cellmode"
#define XtCCellmode "Cellmode"
#define XtRCellmode "Cellmode"
#endif
#define XoCellEndCoordMode 0
#define XoCellMidCoordMode 1


typedef struct _CellClassRec*       CellObjectClass;
typedef struct _CellRec*            CellObject;
typedef struct _CellRec*            Cell;

extern WidgetClass cellObjectClass;

#define XO_CELL_PTS_INCREMENT 100
/* These defines are carefully set up to have appropriate bitwise
** effects. Do not mess with them unless you have figured it all out.
*/
#define XO_CELL_TO_DEND			0x01
#define XO_CELL_TO_TIP			0x02
#define XO_CELL_TO_SOMA			0x044
#define XO_CELL_ONLY_TO_SOMA	0x04
#define XO_CELL_FROM_DEND		0x010
#define XO_CELL_FROM_TIP		0x020
#define XO_CELL_FROM_SOMA		0x044

#define XO_CELL_DEND			0x01
#define XO_CELL_TIP				0x02
#define XO_CELL_SDEND			0x05
#define XO_CELL_STIP			0x06
#define XO_CELL_SOMA			0x04

#define XO_FROM_MASK			0x0F0
#define XO_TO_MASK				0x0F
#define XO_FROM_SHIFT			4
#endif
