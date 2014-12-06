/* $Id: XoChildRes.c,v 1.1.1.1 2005/06/14 04:38:32 svitak Exp $ */
/*
 * $Log: XoChildRes.c,v $
 * Revision 1.1.1.1  2005/06/14 04:38:32  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.2  2000/06/12 04:28:20  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.1  1994/01/13 18:34:14  bhalla
 * Initial revision
 * */
#include <Xo/StringDefs.h>
#include <string.h>

String XoPrefixResourceName (prefix, name)
     String prefix, name;
{
  String ret;

  ret = (String) XtMalloc (strlen (prefix) + strlen (name) + 1);
  strcpy (ret, prefix);
  strcat (ret, name);
  return ret;
}


String XoChildResource (name)
     String name;
{
  return XoPrefixResourceName (XtNchildResource, name);
}
  
String XoOnResource (name)
     String name;
{
  return XoPrefixResourceName (XtNonResource, name);
}
  
String XoOffResource (name)
     String name;
{
  return XoPrefixResourceName (XtNoffResource, name);
}
  
