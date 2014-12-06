/* $Id: TopLevel.c,v 1.4 2005/07/12 06:11:32 svitak Exp $ */
/*
 * $Log: TopLevel.c,v $
 * Revision 1.4  2005/07/12 06:11:32  svitak
 * Removed malloc.h since it's included from stdlib.h. Added stdio before
 * stdlib if needed since some OSs require it.
 *
 * Revision 1.3  2005/07/01 10:02:50  svitak
 * Misc fixes to address compiler warnings, esp. providing explicit types
 * for all functions and cleaning up unused variables.
 *
 * Revision 1.2  2005/06/18 17:17:55  svitak
 * Added some typecasts for clean OSX Panther compiles.
 *
 * Revision 1.1.1.1  2005/06/14 04:38:32  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.16  2001/06/29 21:10:03  mhucka
 * Set the default colorscale to be "hot".
 *
 * Revision 1.15  2001/04/18 22:39:34  mhucka
 * Miscellaneous portability fixes, mainly for SGI IRIX.
 *
 * Revision 1.14  2000/06/12 04:28:20  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.13  2000/05/19 20:02:28  mhucka
 * No, I'm wrong.  You really do need to comment out the XtNborderPixmap setting
 * under Solaris.  Otherwise it's not able to create the window.
 *
 * Revision 1.12  2000/05/19 18:34:56  mhucka
 * 1) Apparently the use of XtNborderPixmap isn't the problem, so I uncommented
 *    it back.
 * 2) Removed unused variables.
 *
 * Revision 1.11  2000/05/02 06:17:39  mhucka
 * 1) ShellList was never initialized, yet it was used by
 *    XgCreateTopLevelShell().  This was probably not a good idea, so I changed
 *    the call in XgCreateTopLevelShell() to use NULL instead.
 * 2) Changed parameters in the call to XtVaAppCreateShell() to work around
 *    problems with some X servers, like Sun OpenWindows.  The symptom was that
 *    creating an xform would produce an X protocol error about BadMatch.  The
 *    fix was to avoid setting XtNborderPixmap.  I'm not sure what repercussions
 *    that will have, but for now things to seem to work as before under Linux
 *    and Solaris 2.7 (minus the problem).
 *
 * Revision 1.10  1995/06/12 17:03:30  venkat
 * Made the Pixmap resources as "None" instead of NULL. This does not CopyFromParent
 * but uses the corresponding Pixel resources instead. These resources are set at startup
 * as fallback resources for the application.
 *
 * Revision 1.9  1995/05/19  22:20:26  venkat
 * Some more color specific debugging statements for interim
 * solution finding effort for the XCreateWindow/BadMatch error in
 * SGI/IRIX 5.3: Problem surfaced in BETA2-0-P5
 *
 * Revision 1.8  1995/05/17  23:40:47  venkat
 * The width and height are set for the purpose of debugging.
 * This is because we need the shell's color based resources as well as
 * the corresponding window attribs. In order to realize the shell we need at least
 * a non-zero width/height shell
 *
 * Revision 1.7  1995/04/12  18:21:02  venkat
 * Added interface function XoWMPositionHack() which deals with the
 * toplevel shell's positioning interactions with the window manager.
 *
 * Revision 1.6  1995/04/05  19:05:17  venkat
 * Added WMShell resource XtNinput with value set to
 * true while creating application shell widget for non-pointer
 * -following keyboard focus window managers such as fvwm. This
 * hint is ignored by pointer-following kb focus window managers
 * such as twm and so is generalized this version works with all
 * window managers.
 *
 * Revision 1.5  1995/04/04  01:05:32  venkat
 * The Shell's color specific resources including
 * pixmaps are set in the call to XtVaAppCreateShell().
 *
 * Revision 1.4  1995/02/24  20:25:01  venkat
 * Changed name to XoGetUsedColormap()
 *
 * Revision 1.3  1995/02/18  00:37:23  venkat
 * Included the XtNcolormap resource in creating the
 * top level shell.
 *
 * Revision 1.2  1994/03/22  15:13:58  bhalla
 * Attempts to handle incorrect widget placement under twm. Later moved to
 * widg/xform.c
 *
 * Revision 1.1  1994/01/13  18:34:14  bhalla
 * Initial revision
 * */
/*
 * Routines for the maintenance of top level shell widgets.
 */
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/Xatom.h>
#include <stdio.h>
#include <stdlib.h>
#include "StringDefs.h"
#include "Xo/Xo_ext.h"


typedef struct _ShellElement {
  Widget shell;
  struct _ShellElement *next;
} ShellElement;


Widget XgCreateTopLevelShell (name)
	char * name;
{
  ShellElement *new;
#ifdef BROKEN 
  XWMHints	wm_hints;
#endif

  new = (ShellElement *) malloc (sizeof (ShellElement));
  new->next  = NULL;
  /* This version gives each shell the correct name but makes
  ** it hard to assign resources :
  **
  new->shell = XtVaAppCreateShell(name, "Genesis", topLevelShellWidgetClass,
				  XgDisplay(),
				  XtNmappedWhenManaged,False,
				  NULL);
  */

  /* This version calls everything 'genesis' but resources are easier
  ** to specify
	
	Venkat:- The pixmap resources need not be set but it doesnt hurt to put it in there
		 In some mysterious way the pixmaps were being set to invalid values
		 if the default visual was not used 
	2000-05-01 mhucka: taking it out.  It causes genesis to fail under Sun CDE.

  */
  new->shell = XtVaAppCreateShell("genesis", "Genesis", topLevelShellWidgetClass,
				  (Display *)XgDisplay(),
				  XtNmappedWhenManaged, False,
				  XtNallowShellResize, True,
				  XtNinput, True,
				  XtNcolormap, XoGetUsedColormap(),
				  XtNvisual, XoGetUsedVisual(),
				  XtNdepth, XoGetUsedDepth(),
				  XtNbackgroundPixmap, None,
/*				  XtNborderPixmap, None, */
				  XtNx, 0,
				  XtNy, 0,
				  XtNwidth, 300,
				  XtNheight, 300,
				  NULL);
#ifdef BROKEN 
  XSetIconName(XgDisplay(), XtWindow(new->shell), name);
  wm_hints.flags = InputHint;
  wm_hints.input = True;
  XSetWMHints(XgDisplay(), XtWindow(new->shell), &wm_hints);
#endif

  /* Set up a default GENESIS colormap, if we haven't already. */
  if (!XoHaveCreatedGenesisCmap())
      XoMakeCmap("hot", FALSE);

  return new->shell;
}

void XoWMPositionHack(toplevelshell,xpos,ypos)
 Widget   toplevelshell;
 Position xpos;
 Position ypos;
{
  XSizeHints hints;

  hints.flags = USPosition;
  hints.x = xpos;
  hints.y = ypos; 
  XSetSizeHints((Display *)XgDisplay(),XtWindow(toplevelshell),&hints,
				XA_WM_NORMAL_HINTS);	
}
