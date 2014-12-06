/* $Id: StringDefs.h,v 1.1.1.1 2005/06/14 04:38:33 svitak Exp $ */
/*
 * $Log: StringDefs.h,v $
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.2  2000/06/12 04:28:19  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.1  1994/03/22 15:35:54  bhalla
 * Initial revision
 * */

#ifndef _Xo_Child_Resource
#define _Xo_Child_Resource

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#define XtNchildResource "child."
#define XtNonResource    "on."
#define XtNoffResource   "off."

String XoPrefixResourceName();
String XoChildResource();
String XoOnResource();
String XoOffResource();

#endif



