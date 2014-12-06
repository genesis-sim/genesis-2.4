/* $Id: XoGeometry.c,v 1.1.1.1 2005/06/14 04:38:32 svitak Exp $ */
/*
 * $Log: XoGeometry.c,v $
 * Revision 1.1.1.1  2005/06/14 04:38:32  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.15  2001/06/29 21:11:03  mhucka
 * Added extra parens inside conditionals and { } groupings to quiet certain
 * compiler warnings.
 *
 * Revision 1.14  2001/04/18 22:39:35  mhucka
 * Miscellaneous portability fixes, mainly for SGI IRIX.
 *
 * Revision 1.13  2000/07/03 18:14:17  mhucka
 * Added missing type casts.
 *
 * Revision 1.12  2000/06/12 04:28:20  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.11  1999/10/17 04:00:49  mhucka
 * Applied additional patches from Upi dated July 1998 that never seem to have
 * made it into DR 2.2.1.
 *
 * Revision 1.10  1998/07/15 06:22:59  dhb
 * Upi update
 *
 * Revision 1.9  1997/06/20 01:20:22  venkat
 * XoResolveReference() changed to query the being_destroyed flag of the
 * reference widget to indicate to the caller (layout code in FormLayout.c or
 * constraint code in Form.c) that the contraint resources need to be set back
 * to the provided default reference. This indication is done by setting the
 * passed int * to 1 if the being_destroyed flag is 1.
 *
 * Revision 1.8  1995/06/15  18:16:33  venkat
 * Some defensive (mem alloc) coding in XoCvtStringToString()
 *
 * Revision 1.7  1995/06/02  19:47:02  venkat
 * Resource-converters-clean-up
 *
 * Revision 1.6  1995/02/11  01:29:55  venkat
 * Changed XoStringFromGeometry() to return NULL
 * and set hgeom field as the height of the pixel
 * instead of the literal string XoNaturalGeometry.
 *
 * Revision 1.5  1995/02/10  23:37:23  venkat
 * Made changes to XoCvtStringToXoGeometry to avoid internal
 * string buffer overflow. Changed XoGeometryFromString() to
 * not display field syntax error if a space is provided for the
 * field
 *
 * Revision 1.4  1995/01/31  19:00:20  venkat
 * XoGetGeometryFromString() modified to display error messages
 * in case of bad geometry specifications
 *
 * Revision 1.3  1994/11/07  22:28:47  bhalla
 * more fun with conversions. I think dhb originated this fix to
 * the conversion for XoGeometryRec.
 *
 * Revision 1.2  1994/03/22  15:16:14  bhalla
 * Put in stuff to get rid of leading spaces in geometry strings. This may
 * not have been necessary, as the real problem turned out to be in
 * xo_cvt.c
 *
 * Revision 1.1  1994/01/13  18:34:14  bhalla
 * Initial revision
 * */
#include <X11/IntrinsicP.h>
#include <X11/ObjectP.h>
#include <X11/Object.h>
#include <Xo/Geometry.h>
#include <Xo/XoDefs.h>
#include <X11/StringDefs.h>
#include "../../sys/system_deps.h"

#include <stdio.h>		/* for debugging */


/*********************************************************************
  Converters for Xodus geometry types.


  XoCvtStringToRequestType (args, num_args, fromVal, toVal)

  converts the string passed into the function in the XrmValue
  `*fromVal' and into an XoRequestType.  The following strings are
  recognized (comparison is case insenstive):
              "absolute"     ->     XoAbsoluteRequest
	      ""             ->     XoAbsoluteRequest
	      "relative"     ->     XoRelativeRequest
	      "%"            ->     XoRelativeRequest
  otherwise a warning is issued and a NULL address is returned.


  XoCvtStringToReference (args, num_args, fromVal, toVal)
     static XtConvertArgRec parentCvtArgs[] = {
       {XtBaseOffset, (caddr_t)XtOffset(Widget, core.parent), sizeof(Widget)},
     };
  converts the string passed in fromVal to a XoReference.  The
  XoReference type is either a Widget or one of the special defined
  values XoReferToParent or XoReferToLastChild.  The conversion
  proceeds as:
       (case insensitive)
              ""             ->     XoReferToParent
	      "null"         ->     XoReferToParent
	      "parent"       ->     XoReferToParent
	      "last"         ->     XoReferToLastChild
       (case sensitive)
              sibling name   ->     sibling Widget
  otherwise a warning is issued and a NULL address is returned.  The 
  for a sibling widget is performed by XtNameToWidget().


  XoCvtReferenceToWidget (args, num_args, fromVal, toVal)
     static XtConvertArgRec parentCvtArgs[] = {
       {XtBaseOffset, (caddr_t)XtOffset(Widget, core.parent), sizeof(Widget)},
     };
  converts an XoReference to a Widget.  In most cases this is a null
  operation.  The only conversion necessary is for the special values
  XoReferToParent and XoReferToLastChild.


  XoCvtStringToReferenceEdge (args, num_args, fromVal, toVal)

  converts the string passed into the function in the XrmValue
  `*fromVal' and into an XoReferenceEdge.  The following strings are
  recognized (comparison is case insenstive):
              "left"         ->     XoReferToLeft
              "right"        ->     XoReferToRight
	      "top"          ->     XoReferToTop
	      "bottom"       ->     XoReferToBottom
	      "width"        ->     XoReferToWidth
	      "height"       ->     XoReferToHeight
  otherwise a warning is issued and a NULL address is returned.


  XoCvtStringToXoGeometry (args, num_args, fromVal, toVal)
    static XtConvertArgRec widgetCvtArgs[] = {
      {XtBaseOffset, (caddr_t) XtOffset(Widget, core.self), sizeof (Widget)},
    };
  converts a string of the form "nn%:ref.edge" to a XoGeometryRec
  structure.  The "nn" represents a decimal integer that is placed in
  the request field.  The rest of the string is optional, although the
  ':' must be present for either a ref or edge to be recognized, and
  the '.' must indicate an edge.  If present, the % indicates that the
  requestType should be XoRelativeRequest.  The ref and edge strings
  are respectively passed to XoCvtStringToReference and
  XoCvtStringToReferenceEdge.

**********************************************************************/

static XrmQuark
  XtQnullstring,
  XtQabsolute, XtQrelative, XtQpercent,
  XtQnull, XtQparent, XtQlast,
  XtQleft, XtQright, XtQtop, XtQbottom, XtQwidth, XtQheight;

static void XoRegisterRequestTypeQuarks ()
{
  XtQnullstring = XrmStringToQuark("");
  XtQabsolute   = XrmStringToQuark("absolute");
  XtQpercent    = XrmStringToQuark("%");
  XtQrelative   = XrmStringToQuark("relative");
}

static void XoRegisterReferenceQuarks ()
{
  XtQnullstring = XrmStringToQuark("");
  XtQnull       = XrmStringToQuark("null");
  XtQparent     = XrmStringToQuark("parent");
  XtQlast       = XrmStringToQuark("last");
}

static void XoRegisterReferenceEdgeQuarks ()
{
  XtQleft       = XrmStringToQuark("left");
  XtQright      = XrmStringToQuark("right"); 
  XtQtop        = XrmStringToQuark("top");
  XtQbottom     = XrmStringToQuark("bottom");
  XtQwidth      = XrmStringToQuark("width");
  XtQheight     = XrmStringToQuark("height");
}

/* ARGSUSED */
Boolean XoCvtStringToString(dpy, args, num_args, fromVal, toVal, destruct)
     Display     *dpy;		/* unused */
     XrmValuePtr args;		/* unused */
     Cardinal    *num_args;	/* unused */
     XrmValuePtr fromVal;
     XrmValuePtr toVal;
     XtPointer   destruct;	/* unused */
{
	static char localstr[500]; 
  if(fromVal->addr != NULL){
	if(fromVal->size >= 500){
		strncpy(localstr, fromVal->addr, 500);		
		localstr[499]='\0';
	}
	else 
		strcpy(localstr, fromVal->addr);
   if(toVal->addr != NULL){
	if(toVal->size < strlen(localstr)+1){
	 toVal->size = strlen(localstr)+1;
	 return(False);
	}
	else 
  	 strcpy(toVal->addr,localstr);
   }
   else
	toVal->addr = localstr;

   toVal->size = strlen(localstr)+1; 
   return (True); 
 }
 toVal->addr = NULL;
 toVal->size = 0;
 XtDisplayStringConversionWarning(dpy,fromVal->addr,"XtRString");
 return((Boolean) False);
}


/* ARGSUSED */
Boolean XoCvtStringToRequestType(dpy, args, num_args, fromVal, toVal, destruct)
     Display     *dpy;		/* unused */
     XrmValuePtr args;		/* unused */
     Cardinal    *num_args;	/* unused */
     XrmValuePtr fromVal;
     XrmValuePtr toVal;
     XtPointer   destruct;	/* unused */
{
  static XoRequestType requestType;
  XrmQuark q;
  char lowerName[1000];
  
  XoLowerName (lowerName, (char*)fromVal->addr);
  q = XrmStringToQuark(lowerName);
  if (q == XtQnullstring || q == XtQabsolute )
    requestType = XoAbsoluteRequest;
  else if (q == XtQpercent || q == XtQrelative )
    requestType = XoRelativeRequest;
  else {
    XtStringConversionWarning(fromVal->addr, "requestType");
    toVal->size = sizeof(XoRequestType);
    return ((Boolean)False);
  }
	XoCvtDone(XoRequestType,requestType);
}


/* ARGSUSED */
Boolean XoCvtStringToReference(dpy, args, num_args, fromVal, toVal, destruct)
     Display     *dpy;		/* unused */
     XrmValuePtr args;
     Cardinal    *num_args;
     XrmValuePtr fromVal;
     XrmValuePtr toVal;
     XtPointer   destruct;	/* unused */
{
  static XoReference reference;
  XrmQuark q;
  char *name, lowerName[1000];

  if (*num_args != 1)
    XtErrorMsg("wrongParameters", "cvtStringToReference", "xoToolkitError",
	       "StringToReference conversion needs parent arg", NULL, 0);

  /* this may not be needed , see the bottom ov the routine  ** 
  if (toVal->addr != NULL && toVal->size < sizeof(XoReference)) {
	toVal->size = sizeof(XoReference);
	return False;
  }
  toVal->size = sizeof (XoReference);
  */
  
  XoLowerName (lowerName, name = (char*)fromVal->addr);
  q = XrmStringToQuark(lowerName);
  if (q == XtQnullstring || q == XtQnull || q == XtQparent )
    reference = XoReferToParent;
  else if (q == XtQlast )
    reference = XoReferToLastChild;
  else {
    if ( ! (reference = XtNameToWidget( *(Widget*)args[0].addr, name)) ) {
      XtStringConversionWarning(fromVal->addr, "xoReference");
      toVal->size = sizeof(XoReference);
      return ((Boolean)False);
    }      
  }
	XoCvtDone(XoReference,reference);
}



Boolean XoCvtReferenceToWidget (dpy, args, num_args, fromVal, toVal, destruct)
     Display     *dpy;
     XrmValuePtr args;
     Cardinal    *num_args;
     XrmValuePtr fromVal;
     XrmValuePtr toVal;
     XtPointer   destruct;
{
  static Widget widget;
  Widget child  = *(Widget *)args[0].addr;
  Widget parent = *(Widget *)args[1].addr;
  Widget last_child = parent;

  if (toVal->addr != NULL && toVal->size < sizeof(Widget)) {
	toVal->size = sizeof(Widget);
	return False;
  }
  toVal->size = sizeof (Widget);
  if (XtIsComposite (parent)) {
    CompositeWidget cw = (CompositeWidget) parent;
    Widget *firstborn = cw->composite.children,
           *childP;
    int    num_children = cw->composite.num_children;

    for (childP = firstborn; childP - firstborn < num_children; childP++)
      if (*childP == child) break;
    
    last_child = childP == firstborn ? parent : *--childP;
  }

  switch ((ADDR) *(XoReference *)fromVal->addr) {
  case (ADDR) XoReferToParent:  
    widget = parent;  break;
  case (ADDR) XoReferToLastChild:  
    widget = last_child;  break;
  default:
    widget = *(Widget *)fromVal->addr;
  }
  XoCvtDone(Widget, widget);
}


Widget XoResolveReference (child, reference, isDestroyed, defRef)
     Widget child;
     XoReference reference;
     int *isDestroyed;
     XoReference defRef;
{
  XrmValue in, out;
  static Widget refWidg;


  if ((reference != NULL) && ((ADDR)reference != (ADDR)XoReferToLastChild)
	&& ((ADDR)reference != (ADDR)XoReferToParent) 
	&& ((Widget)reference)->core.being_destroyed) {
	*isDestroyed = 1;
	in.addr  = (caddr_t) &defRef; 
  } else {
	*isDestroyed = 0;
	in.addr = (caddr_t) &reference;
  }
  out.addr = NULL;
  if (XtConvertAndStore (child, XtRReference, &in, XtRWidget, &out)) {
	refWidg = *(Widget *)out.addr;
	if (refWidg->core.being_destroyed) 
		return child->core.parent;
	return refWidg;
  }
  else 
	return (Widget) NULL;
}
  

  

/* ARGSUSED */
Boolean
  XoCvtStringToReferenceEdge(dpy, args, num_args, fromVal, toVal, destruct)
     Display     *dpy;		/* unused */
     XrmValuePtr args;		/* unused */
     Cardinal    *num_args;	/* unused */
     XrmValuePtr fromVal;
     XrmValuePtr toVal;
     XtPointer   destruct;	/* unused */
{
  static XoReferenceEdge referenceEdge;
  XrmQuark q;
  char lowerName[1000];
  
  XoLowerName (lowerName, (char*)fromVal->addr);
  q = XrmStringToQuark(lowerName);
  if (q == XtQleft)
    referenceEdge = XoReferToLeft;
  else if (q == XtQright)
    referenceEdge = XoReferToRight;
  else if (q == XtQtop)
    referenceEdge = XoReferToTop;
  else if (q == XtQbottom)
    referenceEdge = XoReferToBottom;
  else if (q == XtQwidth)
    referenceEdge = XoReferToWidth;
  else if (q == XtQheight)
    referenceEdge = XoReferToHeight;
  else {
    XtStringConversionWarning(fromVal->addr, "ReferenceEdge");
    toVal->size = sizeof(XoReferenceEdge);
    return ((Boolean)False);
  }
  XoCvtDone(XoReferenceEdge,referenceEdge);
}


#include <string.h>

#define error()                                                   \
        {                                                         \
	  XtStringConversionWarning(fromVal->addr, "XoGeometry"); \
	  toVal->size = 0;                                        \
          toVal->addr = (caddr_t) NULL;                           \
          return False;                                           \
        }


Boolean XoCvtStringToXoGeometry (dpy, args, num_args, fromVal, toVal, destruct)
     Display     *dpy;		/* unused */
     XrmValuePtr args;
     Cardinal    *num_args;
     XrmValuePtr fromVal;
     XrmValuePtr toVal;
     XtPointer   destruct;	/* unused */
{
  static XoGeometryRec geometryRec;
  char geometryString[1000], *s, *t;
  int scanned;
  XrmValue in, out;
  Widget object = *(Widget*)args[0].addr;

  /* Fill in return values in case of premature return.  If an error occurs
     then toVal will be zeroed out */
  geometryRec.reference = XoReferToParent;
  geometryRec.referenceEdge = XoReferToDefault;
  if (toVal->addr != NULL && toVal->size < sizeof(XoGeometryRec)) {
	toVal->size = sizeof(XoGeometryRec);
	return(False);
  }
  /*
  toVal->addr = (caddr_t) &geometryRec;
  */

  toVal->size = sizeof (XoGeometryRec);

  /* Upi fix: get rid of leading spaces */
  for(s = fromVal->addr; *s && (*s == ' ' || *s == '	'); s++);
  if (!*s || strlen(s) < 1)
	return(False);

  /* just in case, copy input string to writable space */
  t = strncpy (geometryString, s, 1000);
  geometryString[999]='\0';
  if ( ! sscanf (t, "%d%n", &geometryRec.request, &scanned))
    error();
  t += scanned;

  if ((s = strchr (t, ':')))
        *(s++) = '\0';
  in.addr = t;
  out.addr = (caddr_t) &geometryRec.requestType; 
  out.size = sizeof (XoRequestType);
  if ( ! XtConvertAndStore (object, XtRString, &in, XtRRequestType, &out) )
    error();
  t = s;

  if (!s) {
  	if (toVal->addr == NULL) toVal->addr = (caddr_t) &geometryRec;
  	else BCOPY(&geometryRec,toVal->addr,toVal->size);
	return True;
  }

  if ((s = strchr (t, '.'))) *(s++) = '\0';
  in.addr = t;
  out.addr = (caddr_t) &geometryRec.reference; 
  out.size = sizeof (XoReference);
  if ( ! XtConvertAndStore (object, XtRString, &in, XtRReference, &out) )
    error();
  t = s;
  
  if (!s) {
  	if (toVal->addr == NULL) toVal->addr = (caddr_t) &geometryRec;
  	else BCOPY(&geometryRec,toVal->addr,toVal->size);
	return True;
  }

  in.addr = t;
  out.addr = (caddr_t) &geometryRec.referenceEdge; 
  out.size = sizeof (XoReferenceEdge);
  if ( ! XtConvertAndStore (object, XtRString, &in, XtRReferenceEdge, &out) )
    error();
  XoCvtDone(XoGeometryRec,geometryRec);
}


Boolean XoCvtXoGeometryToString(dpy,args,num_args,fromVal,toVal,destruct)
     Display     *dpy;          /* unused */
     XrmValuePtr args;          /* unused */
     Cardinal    *num_args;     /* unused */
     XrmValuePtr fromVal;
     XrmValuePtr toVal;
     XtPointer   destruct;      /* unused */
{
	 static char string[500];
     XoGeometryPtr geometry = (XoGeometryPtr)fromVal->addr;

  sprintf(string,"%d",geometry->request);

  if (geometry->requestType == XoRelativeRequest) {
	strcat(string,"%");
  }
  if (geometry->reference == XoReferToParent) {
  }  else if (geometry->reference == XoReferToLastChild) {
	strcat(string,":last");
  } else {
	strcat(string,":");
	strcat(string,geometry->reference->core.name);
  }
  switch(geometry->referenceEdge) {
	case	XoReferToLeft:
		strcat(string,".left");
		break;
	case	XoReferToRight:
		strcat(string,".right");
		break;
	case	XoReferToTop:
		strcat(string,".top");
		break;
	case	XoReferToBottom:
		strcat(string,".bottom");
		break;
	case	XoReferToWidth:
		strcat(string,".width");
		break;
	case	XoReferToHeight:
		strcat(string,".height");
		break;
	case	XoReferToDefault:
	default:
		break;
  }
      if(toVal->addr != NULL){	
	if (toVal->size < strlen(string) + 1) {
		toVal->size = strlen(string) + 1;
		return((Boolean) False);
	} else {
		strcpy((char *)toVal->addr,string);
	}
      }
      else
	toVal->addr = string;
	
	toVal->size = strlen(string)+1;
	return(True);
}

void XoGeomInitialize()
{
  static XtConvertArgRec 
    widgetCvtArgs[] = {
      {XtBaseOffset, (caddr_t)XtOffset(Widget, core.self), sizeof(Widget)}
    },
    parentCvtArgs[] = {
      {XtBaseOffset, (caddr_t)XtOffset(Widget, core.parent), sizeof(Widget)}
    },
    referenceCvtArgs[] = {
      {XtBaseOffset, (caddr_t)XtOffset(Widget, core.self), sizeof(Widget)},
      {XtBaseOffset, (caddr_t)XtOffset(Widget, core.parent), sizeof(Widget)}
    };
  
  static int first_time = 1;

  if (first_time) {
    first_time = 0;
    XoRegisterRequestTypeQuarks ();
    XoRegisterReferenceQuarks ();
    XoRegisterReferenceEdgeQuarks ();

    XtSetTypeConverter(XtRString, XtRString,
		       (XtTypeConverter) XoCvtStringToString, 
		       NULL, 0, 
		       XtCacheNone, NULL );

    XtSetTypeConverter(XtRString, XtRRequestType,
		       (XtTypeConverter) XoCvtStringToRequestType, 
		       NULL, 0, 
		       XtCacheNone, NULL );
    XtSetTypeConverter(XtRString, XtRReference,
		       (XtTypeConverter) XoCvtStringToReference,
		       parentCvtArgs, XtNumber(parentCvtArgs), 
		       XtCacheNone, NULL );
    XtSetTypeConverter(XtRReference, XtRWidget,
		       (XtTypeConverter) XoCvtReferenceToWidget,
		       referenceCvtArgs, XtNumber(referenceCvtArgs), 
		       XtCacheNone, NULL );
    XtSetTypeConverter(XtRString, XtRReferenceEdge,
		       (XtTypeConverter) XoCvtStringToReferenceEdge,
		       NULL, 0,
		       XtCacheNone, NULL );
    XtSetTypeConverter(XtRString, XtRXoGeometry,
		       (XtTypeConverter) XoCvtStringToXoGeometry,
		       widgetCvtArgs, XtNumber(widgetCvtArgs),
		       XtCacheNone, NULL );
	/* For some reason the system cannot find these converters */
    XtSetTypeConverter(XtRXoGeometry, XtRString,
		       (XtTypeConverter) XoCvtXoGeometryToString,
		       NULL,0,
		       XtCacheNone, NULL );
  }
}

#define XoGeometryUnsetRequest ((1 << 15) - 1)

XoGeometryPtr XoGeometryUnset ( geom )
     XoGeometryPtr geom;
{
  geom->request = XoGeometryUnsetRequest;
  return geom;
}

Boolean XoGeometryIsUnset ( geom )
     XoGeometryPtr geom;
{
  return geom->request == XoGeometryUnsetRequest;                      
}

XoGeometryPtr XoGeometryMakeUnsetRecord ()
{
  XoGeometryPtr new;

  new = (XoGeometryPtr) XtNew (XoGeometryRec);
  XoGeometryUnset (new);
  return new;
}


XoGeometryPtr XoGeometryFromString (child, string, geometry)
	 Widget child;
     String string;
     XoGeometryPtr geometry;
{
  XrmValue in, out;

  in.addr = (caddr_t) string;
  out.addr = (caddr_t) geometry;
  out.size = sizeof(XoGeometryRec);

  if (XtConvertAndStore (child, XtRString, &in, XtRXoGeometry, &out))
  	return geometry;
  else {
      if(strcmp(string,"")!=0){
	printf("Failed to convert geometry string %s\n",string);
	printf("Using Default for the field\n");
      } 
	return geometry;
  }

}


char *XoStringFromGeometry(geometry)
     XoGeometryPtr geometry;
{
	static char string[500];

  if (XoGeometryIsUnset(geometry)) {
	return(NULL);
  }
  if(geometry->request==XoNaturalGeometry) return NULL;
  sprintf(string,"%d",geometry->request);

  if (geometry->requestType == XoRelativeRequest) {
	strcat(string,"%");
  }
    if (geometry->reference == XoReferToParent) {
  	}  else if (geometry->reference == XoReferToLastChild) {
		strcat(string,":last");
  	} else {
		if (geometry->reference->core.name) {
			strcat(string,":");
			strcat(string,geometry->reference->core.name);
		}
  	}
  switch(geometry->referenceEdge) {
	case	XoReferToLeft:
		strcat(string,".left");
		break;
	case	XoReferToRight:
		strcat(string,".right");
		break;
	case	XoReferToTop:
		strcat(string,".top");
		break;
	case	XoReferToBottom:
		strcat(string,".bottom");
		break;
	case	XoReferToWidth:
		strcat(string,".width");
		break;
	case	XoReferToHeight:
		strcat(string,".height");
		break;
	case	XoReferToDefault:
	default:
		break;
  }
	return(string);
}
