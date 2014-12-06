/* $Id: XoCursor.c,v 1.2 2005/07/01 10:02:50 svitak Exp $ */
/*
 * $Log: XoCursor.c,v $
 * Revision 1.2  2005/07/01 10:02:50  svitak
 * Misc fixes to address compiler warnings, esp. providing explicit types
 * for all functions and cleaning up unused variables.
 *
 * Revision 1.1.1.1  2005/06/14 04:38:32  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.2  2000/06/12 04:28:20  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.1  1994/01/13 18:34:14  bhalla
 * Initial revision
 * */
#include <stdio.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>

#define XO_DRAG_CURSOR_PIXMAP_SIZE 50
static Cursor DefaultCursor;
static Cursor DragCursor;
static Pixmap XoDragCursorPixmap;
static XColor	white,black;

/* This file handles the cursor related stuff */

void XoInitCursor()
{
	Display	*disp,*XgDisplay();
	static Pixmap XoDefaultCursorPixmap;
	int screen = XDefaultScreen(XgDisplay());
	Colormap cmap = DefaultColormap(XgDisplay(),0);
	static char 	XoDefaultCursorFile[] = "genesis.bmp";
	static char 	XoDragCursorFile[] = "gendrag.bmp";
	char	*bmap_name,*FindFileOnPath();
	int		xhot,yhot;
	unsigned int wid,ht;

	disp = XgDisplay();
	bmap_name = FindFileOnPath(XoDefaultCursorFile);
	if (!bmap_name) {
		DefaultCursor = XCreateFontCursor(disp,0);
		return;
	}

	white.pixel = WhitePixel(disp,screen);
	white.red = white.green = white.blue = 65535;
	black.pixel = WhitePixel(disp,screen);
	black.red = black.green = black.blue = 0;

	if (XAllocColor(disp,cmap,&white) == 0) {
			fprintf(stderr,"Cannot allocate white color\n");
			return;
	}
	if (XAllocColor(disp,cmap,&black) == 0) {
			fprintf(stderr,"Cannot allocate black color\n");
			return;
	}

	if (XReadBitmapFile(
		disp,XDefaultRootWindow(disp),
		bmap_name,
		&wid,&ht,
		&XoDefaultCursorPixmap,
		&xhot,&yhot) != 0) {
		fprintf(stderr,"Cannot read bitmap file %s\n",
			bmap_name);
		DefaultCursor = XCreateFontCursor(disp,0);
		return;
	}

	DefaultCursor = XCreatePixmapCursor(disp,XoDefaultCursorPixmap,
		XoDefaultCursorPixmap,
		&black,&white,
		(xhot > 0) ? xhot : 0 ,(yhot > 0) ? yhot : 0 );

	bmap_name = FindFileOnPath(XoDragCursorFile);
	if (!bmap_name) {
		DragCursor = XCreateFontCursor(disp,1);
		return;
	}
	if (XReadBitmapFile(
		disp,XDefaultRootWindow(disp),
		bmap_name,
		&wid,&ht,
		&XoDragCursorPixmap,
		&xhot,&yhot) != 0) {
		fprintf(stderr,"Cannot read bitmap file %s\n",
			bmap_name);
		DragCursor = XCreateFontCursor(disp,1);
		return;
	}

	DragCursor = XCreatePixmapCursor(disp,XoDragCursorPixmap,
		XoDragCursorPixmap,
		&black,&white,
		(xhot > 0) ? xhot : 0 ,(yhot > 0) ? yhot : 0 );
}

Cursor XoDefaultCursor()
{
	return(DefaultCursor);
}

Cursor XoDragCursor()
{
	return(DragCursor);
}

void XoCopyCursor(disp,src,gc,x,y,w,h)
	Display	*disp;
	Drawable	src;
	GC	gc;
	int	x,y;
	unsigned int w,h;
{
	XDefineCursor(disp,src,DragCursor);
}

void XoRestoreCursor(disp,src)
	Display	*disp;
	Drawable	src;
{
	XDefineCursor(disp,src,DefaultCursor);
}
