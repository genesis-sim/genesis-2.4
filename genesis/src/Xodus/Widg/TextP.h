/* $Id: TextP.h,v 1.1.1.1 2005/06/14 04:38:33 svitak Exp $ */
/*
 * $Log: TextP.h,v $
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.3  2000/06/12 04:28:19  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.2  1995/03/06 20:09:24  venkat
 * Changed class and widget part structures to
 * subclass XoCore
 *
 * Revision 1.1  1994/03/22  15:35:54  bhalla
 * Initial revision
 * */
#ifndef _TextP_h
#define _TextP_h

#include "Text.h"
/* include superclass private header file */
#include "Widg/LabelP.h"

typedef struct {
    int empty;
} TextClassPart;

typedef struct _TextClassRec {
    CoreClassPart	core_class;
    XoCoreClassPart	xocore_class;
    LabelClassPart      label_class;
    TextClassPart	text_class;
} TextClassRec;

extern TextClassRec textClassRec;

typedef struct {
  /* resources */
  int                   point;		/* I think point determines the location of the cursor */
  char *                initial_text;
  XtCallbackList		callbacks;
  /* Is the text in the widget editable? */
  Boolean editable;

  /* Text widget creation size */
  int initialHeight;

#ifdef NOTUSED
  /* File name to use for text */
  String	filename;
#endif

  /* private state */

  /* Contains the text currently being edited in the text widget */
  char *                text;

  /* Contains the number of characters in the text widget */
  int                   text_size;

  /* Contains the amount of memory allocated so far in the text buffer of the
   * the text widget.  Memory is always allocated in 4 byte chunks and as all the
   * 4 bytes in the chunk are used another 4 byte chunk is allocated.
   */
  int                   text_memory;

  /* Vertical offset to start drawing text. */
  int textStartDrawOffset;

  /* Mouse X and Y used to compare with current X & Y values to determine amount
   * to scroll.
   */
  int mouseStartX, mouseStartY;

  /* Determine if a click in the scroll bar has occurred to allow scrolling. */
  Boolean scrollingAccessible;

  /* Determine the actual width and height of the text inside the text widget */
  int actualTextWidth, actualTextHeight;

  /* Real height of the scroll bar taking into account space for the slider.
   * See SetScrollBarHeight for details.
   */
  int barHeight;

  /* Center Y location of the scroll bar slider */
  int barLocation;

  /* Half of the size of the slider of the scroll bar. */
  int halfSliderHeight;

  /* Determine if the text is longer than the widget to allow the scroll bar to appear */
  Boolean showScrollBar;
  int cursorY;

} TextPart;

typedef struct _TextRec {
    CorePart		core;
    XoCorePart		xocore;
    LabelPart           label;
    TextPart	        text;
} TextRec;

#endif /* _TextP_h */
