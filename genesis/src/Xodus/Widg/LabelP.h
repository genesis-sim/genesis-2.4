/* $Id: LabelP.h,v 1.1.1.1 2005/06/14 04:38:33 svitak Exp $ */
/*
 * $Log: LabelP.h,v $
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.3  2000/06/12 04:28:19  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.2  1995/03/06 20:03:39  venkat
 * ClassPart and WidgetPart structures changed to subclass
 * directly from XoCore.
 *
 * Revision 1.1  1994/03/22  15:35:54  bhalla
 * Initial revision
 * */
/* 
 * LabelP.h - Private definitions for Label widget
 * 
 */

#ifndef _XoLabelP_h
#define _XoLabelP_h

/***********************************************************************
 *
 * Label Widget Private Data
 *
 ***********************************************************************/

#include "Label.h"
#include "XoCoreP.h"

#ifdef  XtRJustify
#undef  XtRJustify
#endif
#define XtRJustify    "XoJustify"

/* New fields for the Label widget class record */

typedef struct {int foo;} LabelClassPart;

/* Full class record declaration */
typedef struct _LabelClassRec {
    CoreClassPart	core_class;
    XoCoreClassPart	xocore_class;
    LabelClassPart	label_class;
} LabelClassRec;

extern LabelClassRec labelClassRec;

/* New fields for the Label widget record */
typedef struct {
    /* resources */
    XFontStruct	*font;
    char	*label;
    XoJustify	justify;
    Dimension	internal_width;
    Dimension	internal_height;
    Boolean	resize;

    /* private state */
    GC		gc;
    Position	label_x;
    Position	label_y;
    Dimension	label_width;
    Dimension	label_height;
    Dimension	label_len;	/* Length of the label in characters */
} LabelPart;


/****************************************************************
 *
 * Full instance record declaration
 *
 ****************************************************************/

typedef struct _LabelRec {
    CorePart	core;
    XoCorePart  xocore;
    LabelPart	label;
} LabelRec;

#endif /* _XoLabelP_h */
