/* $Id: Geometry.h,v 1.1.1.1 2005/06/14 04:38:32 svitak Exp $ */
/*
 * $Log: Geometry.h,v $
 * Revision 1.1.1.1  2005/06/14 04:38:32  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.2  2000/06/12 04:28:20  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.1  1994/01/13 18:34:14  bhalla
 * Initial revision
 * */
/* Header File for Xodus Geometry handling */


#ifndef XoGeometry_h
#define XoGeometry_h

#include     <X11/Intrinsic.h>

#define XtRGeometryRequest "int"

#define XoNaturalGeometry  ((1 << (sizeof(int)*8 - 2)) - 10)
/* Strange value is to avoid collisions with possible valid inputs and
   with XoUnsetParameter, etc. used internally by Form widget */

#define XtRRequestType     "XoRequestType"
#define XtRReference       "XoReference"
#ifndef XtRWidget
#define XtRWidget          "Widget"
#endif
#define XtRReferenceEdge   "XoReferenceEdge"
#define XtRXoGeometry      "XoGeometryRec"

typedef enum {
  XoAbsoluteRequest,
  XoRelativeRequest
} XoRequestType;

typedef Widget XoReference;
#define XoReferToParent    (XoReference) 0
#define XoReferToLastChild (XoReference) 1

typedef enum {
  XoReferToDefault,
  XoReferToLeft,
  XoReferToRight,
  XoReferToTop,
  XoReferToBottom,
  XoReferToWidth,
  XoReferToHeight
} XoReferenceEdge;

typedef struct {
  int             request;
  XoRequestType   requestType;
  XoReference     reference;
  XoReferenceEdge referenceEdge;
} XoGeometryRec, *XoGeometryPtr;

/* Converters */

Boolean XoCvtStringToRequestType ( /* args, num_args, fromVal, toVal */ );
/*     XrmValuePtr args;	   
       Cardinal    *num_args;
       XrmValuePtr fromVal;
       XrmValuePtr toVal;    */

Boolean  XoCvtStringToReference ( /* args, num_args, fromVal, toVal */ );
/*     XrmValuePtr args;       
       Cardinal    *num_args;    
       XrmValuePtr fromVal;
       XrmValuePtr toVal;    */

Boolean XoCvtReferenceToWidget ( /* args, num_args, fromVal, toVal */ );
/*     XrmValuePtr args;       
       Cardinal    *num_args;    
       XrmValuePtr fromVal;
       XrmValuePtr toVal;    */

Widget XoResolveReference  ( /* child, reference */ );
/*     Widget child;
       XoReference reference; */
       
Boolean XoCvtStringToReferenceEdge ( /* args, num_args, fromVal, toVal */ );
/*     XrmValuePtr args;       
       Cardinal    *num_args;    
       XrmValuePtr fromVal;
       XrmValuePtr toVal;    */

Boolean XoCvtStringToGeometry ( /* args, num_args, fromVal, toVal */ );
/*     XrmValuePtr args;	   
       Cardinal    *num_args;
       XrmValuePtr fromVal;
       XrmValuePtr toVal;    */

/* Initialize geometry stuff.  Called by XoForm and XoShell */
void          XoGeomInitialize  ();
Boolean       XoGeometryIsUnset ( /* XoGeomtryPtr */ );
XoGeometryPtr XoGeometryUnset   ( /* XoGeometryPtr */ );
XoGeometryPtr XoGeometryMakeUnsetRecord();


#endif


