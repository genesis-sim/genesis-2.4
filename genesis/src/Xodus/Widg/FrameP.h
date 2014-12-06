/* $Id: FrameP.h,v 1.1.1.1 2005/06/14 04:38:33 svitak Exp $ */
/*
 * $Log: FrameP.h,v $
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.3  2000/06/12 04:28:19  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.2  1995/03/06 19:46:01  venkat
 * Class object part structures modified to subclass directly from
 * XoComposite
 *
 * Revision 1.1  1994/03/22  15:35:54  bhalla
 * Initial revision
 * */
#ifndef _FrameP_h
#define _FrameP_h

#include "Frame.h"
/* include superclass private header file */
#include "XoComposiP.h" 
/* define unique representation types not found in <X11/StringDefs.h> */

#define XtRXoFrameElevation "XoFrameElevation"

typedef struct {
    int empty;
} FrameClassPart;

typedef struct _FrameClassRec {
  CoreClassPart	        core_class;
  CompositeClassPart    composite_class;
  XoCompositeClassPart	xocomposite_class;
  FrameClassPart	frame_class;
} FrameClassRec;

extern FrameClassRec frameClassRec;

typedef struct {
  /* resources */
  XoFrameElevation      elevation;
  Pixel                 highlight;
  Pixel                 shadow;
  Pixel 		fg;
  Pixel 		bg;
  /* private state */
  Pixel                 flat;
  GC                    flat_gc;
  GC                    highlight_gc;
  GC                    shadow_gc;
  Dimension             frame_width;
} FramePart;

typedef struct _FrameRec {
  CorePart	      core;
  CompositePart       composite;
  XoCompositePart     xocomposite;
  FramePart	      frame;
} FrameRec;

#endif /* _FrameP_h */
