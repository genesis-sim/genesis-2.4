/* $Id: Button.h,v 1.1.1.1 2005/06/14 04:38:33 svitak Exp $ */
/*
 * $Log: Button.h,v $
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.5  2000/07/03 18:15:27  mhucka
 * Removed extra comma to eliminate compiler warnings.
 *
 * Revision 1.4  2000/06/12 04:28:18  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.3  1995/03/06 19:02:55  venkat
 * Modified for subclassing from XoComposite
 *
 * Revision 1.2  1994/03/22  15:35:54  bhalla
 * Added Id and Log to header
 * */
#ifndef _Button_h
#define _Button_h

/****************************************************************
 *
 * Button widget
 *
 ****************************************************************/

#include <Xo/StringDefs.h>
#include "XoComposit.h"

/* Resources:

 Name		     Class		RepType		Default Value
 ----		     -----		-------		-------------
 background	     Background		Pixel		XtDefaultBackground
 border		     BorderColor	Pixel		XtDefaultForeground
 borderWidth	     BorderWidth	Dimension	1
 destroyCallback     Callback		Pointer		NULL
 height		     Height		Dimension	0
 mappedWhenManaged   MappedWhenManaged	Boolean		True
 sensitive	     Sensitive		Boolean		True
 width		     Width		Dimension	0
 x		     Position		Position	0
 y		     Position		Position	0

 state               State              Boolean         off
 type                ButtonType         XoButtonType    XoPressButton

 onClass             ChildClass         WidgetClass     framedWidgetClass
 offClass            ChildClass         WidgetClass     framedWidgetClass
 
 onCallback          Callback           Pointer         NULL
 offCallback         Callback           Pointer         NULL
*/

/* define any special resource names here that are not in <X11/StringDefs.h> */

#define XtNtype           "type"
#define XtCType           "Type"

typedef enum {
  XoPressButton,
  XoHoldButton,
  XoToggleButton
} XoButtonType;

#define XtNstate          "state"
#define XtCState          "State"

#define XtNonClass        "onClass"
#define XtNoffClass       "offClass"
#define XtCChildClass     "ChildClass"

#ifndef XtNcallback
#define XtNcallback     "callback"
#endif

#define XtNonLabel     "onlabel"
#define XtNoffLabel    "offlabel"
#define XtNonBg     "onbg"
#define XtNoffBg    "offbg"
#define XtNonFg     "onfg"
#define XtNoffFg    "offfg"
#define XtNonFont     "onfont"
#define XtNoffFont    "offfont"

#ifndef XtNradioData
#define XtNradioData "radiodata"
#endif

#ifndef XtNradioGroup
#define XtNradioGroup "radiogroup"
#define XtCRadioGroup "RadioGroup"
#endif


/* declare specific ButtonWidget class and instance datatypes */

typedef struct _ButtonClassRec*	        ButtonWidgetClass;
typedef struct _ButtonRec*		ButtonWidget;

/* declare the class constant */

extern WidgetClass buttonWidgetClass;

#endif /* _Button_h */
