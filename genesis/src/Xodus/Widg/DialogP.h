/* $Id: DialogP.h,v 1.1.1.1 2005/06/14 04:38:33 svitak Exp $ */
/*
 * $Log: DialogP.h,v $
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.3  2000/06/12 04:28:18  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.2  1995/03/06 19:33:11  venkat
 * Modified to subclass from XoComposite
 *
 * Revision 1.1  1994/03/22  15:35:54  bhalla
 * Initial revision
 * */
#ifndef _DialogP_h
#define _DialogP_h

#include "Dialog.h"
#include "Widg/FrameP.h"


typedef struct {
    int empty;
} DialogClassPart;

typedef struct _DialogClassRec {
    CoreClassPart	core_class;
    CompositeClassPart  composite_class;
    XoCompositeClassPart	xocomposite_class;
    FrameClassPart	frame_class;
    DialogClassPart	dialog_class;
} DialogClassRec;

extern DialogClassRec dialogClassRec;

typedef struct {
	XtCallbackList	callbacks;
	char	*value;
	char	*label;
	XFontStruct	*font;
  /* private state */
  Widget                form;
  Widget                button;
  Widget                text_framed;
} DialogPart;

typedef struct _DialogRec {
    CorePart		core;
    CompositePart	composite;
    XoCompositePart	xocomposite;
    FramePart           frame;
    DialogPart	        dialog;
} DialogRec;

#endif /* _DialogP_h */
