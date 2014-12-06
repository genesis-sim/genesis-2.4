/* $Id: XoUtil.c,v 1.1.1.1 2005/06/14 04:38:32 svitak Exp $ */
/* $log$ */
/* 
 * Miscellaneous functions used in more than one place in the Widget set.
 * This file should accompany any Widget that is lifted from the set into
 * a new context.
 */

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/CoreP.h>
#define  XK_LATIN1
#include <X11/keysymdef.h>

/*
  void XoLowerName(dst, src)
  char *dst, *src;
  
  copy the string src into the array dst, lowering the case of all Latin 
  alphabetic characters.  This function is identical to (and replaces) 
  XmuCopyISOLatin1Lowered().
*/
  

void XoLowerName(dst, src)
    char *dst, *src;
{
    register unsigned char *dest, *source;
 
    for (dest = (unsigned char *)dst, source = (unsigned char *)src;
         *source;
         source++, dest++)
    {
        if ((*source >= XK_A) && (*source <= XK_Z))
            *dest = *source + (XK_a - XK_A);
        else if ((*source >= XK_Agrave) && (*source <= XK_Odiaeresis))
            *dest = *source + (XK_agrave - XK_Agrave);
        else if ((*source >= XK_Ooblique) && (*source <= XK_Thorn))
            *dest = *source + (XK_oslash - XK_Ooblique);
        else
            *dest = *source;
    }
    *dest = '\0';
}





/* 
  Boolean XoSubclassIsSubclass (object_class, test_class)
    WidgetClass object_class, test_class;

  check to see if object_class is a (potentially nth generation) subclass
  of test_class
*/
  
Boolean XoSubclassIsSubclass (object_class, test_class)
     WidgetClass object_class, test_class;
{
  if (object_class == coreWidgetClass)
    return False;
  if (object_class == test_class) 
    return True;
  return 
    XoSubclassIsSubclass (object_class->core_class.superclass, test_class);
}


/*
  Pixel XoPixelFromString (w, name)
    Widget w;
    String name;

  convert name to a pixel in color map associated with object w.
*/

Pixel XoPixelFromString (w, name)
    Widget w;
    String name;
{
  XrmValue in, out;
  
  in.addr = name;
  out.size = 0;

  if (! XtConvertAndStore (w, XtRString, &in, XtRPixel, &out) )
    return 0;

  return *out.addr;
}

