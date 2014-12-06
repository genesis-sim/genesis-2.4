static char rcsid[] = "$Id: Xo_ext.h,v 1.1.1.1 2005/06/14 04:38:32 svitak Exp $";

/*
** $Log: Xo_ext.h,v $
** Revision 1.1.1.1  2005/06/14 04:38:32  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.4  2001/06/29 21:11:28  mhucka
** Added declaration for XoHaveCreatedGenesisCmap().
**
** Revision 1.3  2001/04/18 22:39:35  mhucka
** Miscellaneous portability fixes, mainly for SGI IRIX.
**
** Revision 1.2  2000/05/19 18:35:24  mhucka
** Added more definitions and cleaned up formatting.
**
** Revision 1.1  2000/05/02 06:10:57  mhucka
** Function declarations for use by callers.
**
*/

void         XgInitialize();
void         XgEventLoop();
int          XgEventStep();
XtAppContext XgAppContext();
Display      *XgDisplay();
Colormap     XoGetUsedColormap();
Visual*      XoGetUsedVisual();
int          XoGetUsedDepth();
Display      *XgDisplay();
Widget       XgCreateTopLevelShell();
unsigned long *XoColorscale();
int          XoHaveCreatedGenesisCmap();
