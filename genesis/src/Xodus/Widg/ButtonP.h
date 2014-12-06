/* $Id: ButtonP.h,v 1.1.1.1 2005/06/14 04:38:33 svitak Exp $ */
/*
 * $Log: ButtonP.h,v $
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.4  2000/06/12 04:28:18  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.3  1995/03/06 19:03:16  venkat
 * Modified to be subclassed form XoComposite
 *
 * Revision 1.2  1994/03/22  15:35:54  bhalla
 * Jasonized code, no changes I can see
 * */
#ifndef _ButtonP_h
#define _ButtonP_h

#include "Button.h"


#include "XoComposiP.h"

/* define unique representation types not found in <X11/StringDefs.h> */

#ifndef XtRWidgetClass
#define XtRWidgetClass "WidgetClass"
#endif
#define XtRButtonType  "XoButtonType"

typedef struct {
    int empty;
} ButtonClassPart;

typedef struct _ButtonClassRec {
    CoreClassPart	core_class;
    CompositeClassPart  composite_class;
    XoCompositeClassPart xocomposite_class;
    ButtonClassPart	button_class;
} ButtonClassRec;

extern ButtonClassRec buttonClassRec;

typedef struct {
    /* resources */
    XoButtonType        type;
    Boolean             state;
	String				onlabel;
	Pixel				onbg;
	Pixel				onfg;
	XFontStruct			*onfont;
	String				offlabel;
	Pixel				offbg;
	Pixel				offfg;
	XFontStruct			*offfont;
    XtCallbackList      callbacks;
	XtPointer			radiodata;
	Widget				radiogroup;
    /* private state */
    LabelWidget              on;
    LabelWidget              off;
} ButtonPart;

typedef struct _ButtonRec {
    CorePart		core;
    CompositePart       composite;
    XoCompositePart	xocomposite;
    ButtonPart	        button;
} ButtonRec;

#endif /* _ButtonP_h */
