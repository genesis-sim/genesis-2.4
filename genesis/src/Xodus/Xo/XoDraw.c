/* $Id: XoDraw.c,v 1.3 2005/07/20 20:01:57 svitak Exp $ */
/*
 * $Log: XoDraw.c,v $
 * Revision 1.3  2005/07/20 20:01:57  svitak
 * Added standard header files needed by some architectures.
 *
 * Revision 1.2  2005/07/01 10:02:50  svitak
 * Misc fixes to address compiler warnings, esp. providing explicit types
 * for all functions and cleaning up unused variables.
 *
 * Revision 1.1.1.1  2005/06/14 04:38:32  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.4  2000/06/12 04:28:20  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.3  1995/04/11 19:57:28  venkat
 * Removed extra mode arg from calls to XFillRectangles and XDrawSegments
 *
 * Revision 1.2  1995/03/21  21:54:12  venkat
 * Added function XoDrawLtAlignedString()
 *
 * Revision 1.1  1994/01/13  18:34:14  bhalla
 * Initial revision
 * */
#include <stdio.h>	/* printf() */
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>

#define XOX 0
#define XOPS 1

static int XoOutputMode = XOX;

void XoSetPostscript(display,window)
	Display	*display;
	Window	window;
{
	XoOutputMode = XOPS;
	if (!PreparePS(display,window,1.0))
	return;
}

void XoSetX()
{
	XoOutputMode = XOX;
	FinishPS();
}

void XoFillArc(display,drawable,gc,x,y,w,h,start,end)
	Display *display;
	Drawable drawable;
	GC gc;
	Position x,y;
	Dimension w,h;
	int	start,end;
{
	if (XoOutputMode == XOX)
		XFillArc(display,drawable,gc,x,y,w,h,start,end);
	else if (XoOutputMode == XOPS)
		printf("Postscript not yet implemented\n");
}

void XoDrawLine(display,drawable,gc,x1,y1,x2,y2)
	Display *display;
	Drawable drawable;
	GC gc;
	Position x1,y1,x2,y2;
{
	if (XoOutputMode == XOX)
		XDrawLine(display,drawable,gc,x1,y1,x2,y2);
	else if (XoOutputMode == XOPS)
		XPSDrawLine(display,drawable,gc,x1,y1,x2,y2);
}

void XoDrawLines(display,drawable,gc,points, npoints,mode)
	Display *display;
	Drawable drawable;
	GC gc;
	XPoint	*points;
	int	npoints;
	int	mode;
{
	int chunksize;
	int i,j;

	if (XoOutputMode == XOX) {
		chunksize = XMaxRequestSize(display)/sizeof(XPoint)/2 - 1;
		j = chunksize + 1;
		for(i = 0;i<npoints;i+=chunksize){
			if (j + i > npoints)
				j = npoints - i;
			XDrawLines(display,drawable,gc,points+i,j,mode);
		}
	} else if (XoOutputMode == XOPS)
		XPSDrawLines(display,drawable,gc,points, npoints,mode);
}

void XoDrawSegments(display,drawable,gc,segs, nsegs,mode)
	Display *display;
	Drawable drawable;
	GC gc;
	XSegment	*segs;
	int	nsegs;
	int	mode;
{
	int chunksize;
	int i,j;

	if (XoOutputMode == XOX) {
		chunksize = XMaxRequestSize(display)/sizeof(XSegment)/2 - 1;
		j = chunksize;
		for(i = 0;i<nsegs;i+=chunksize){
			if (j + i > nsegs)
				j = nsegs - i;
			XDrawSegments(display,drawable,gc,segs+i,j);
		}
	} else if (XoOutputMode == XOPS)
		XPSDrawSegments(display,drawable,gc,segs, nsegs,mode);
}

void XoDrawPoints(display,drawable,gc,points, npoints,mode)
	Display *display;
	Drawable drawable;
	GC gc;
	XPoint	*points;
	int	npoints;
	int	mode;
{
	int chunksize;
	int i,j;

	if (XoOutputMode == XOX) {
		chunksize = XMaxRequestSize(display)/sizeof(XPoint)/2 - 1;
		j = chunksize + 1;
		for(i = 0;i<npoints;i+=chunksize){
			if (j + i > npoints)
				j = npoints - i;
			XDrawPoints(display,drawable,gc,points+i,j,mode);
		}
	} else if (XoOutputMode == XOPS)
		XPSDrawPoints(display,drawable,gc,points, npoints,mode);
}

void XoFillPolygon(display,drawable,gc,points, npoints,mode)
	Display *display;
	Drawable drawable;
	GC gc;
	XPoint	*points;
	int	npoints;
	int	mode;
{
	int chunksize;
	int i,j;

	if (XoOutputMode == XOX) {
		chunksize = XMaxRequestSize(display)/sizeof(XPoint)/2 - 1;
		j = chunksize + 1;
		for(i = 0;i<npoints;i+=chunksize){
			if (j + i > npoints)
				j = npoints - i;
			XFillPolygon(display,drawable,gc,points+i,j,Complex,mode);
		}
	} else if (XoOutputMode == XOPS)
		XPSFillPolygon(display,drawable,gc,points, npoints,mode);
}

void XoFillRectangles(display,drawable,gc,rects, nrects,mode)
	Display *display;
	Drawable drawable;
	GC gc;
	XRectangle	*rects;
	int	nrects;
	int	mode;
{
	int chunksize;
	int i,j;

	if (XoOutputMode == XOX) {
		chunksize = XMaxRequestSize(display)/sizeof(XPoint)/2 - 1;
		j = chunksize;
		for(i = 0;i<nrects;i+=chunksize){
			if (j + i > nrects)
				j = nrects - i;
			XFillRectangles(display,drawable,gc,rects+i,j);
		}
	} else if (XoOutputMode == XOPS)
		XPSFillRectangles(display,drawable,gc,rects, nrects,mode);
}

void XoDrawCenteredString(display,drawable,gc,font,x,y,string,len)
	Display *display;
	Drawable drawable;
	GC gc;
	XFontStruct *font;
	int x,y;
	char	*string;
	int		len;
{
	int ox,oy; /* offsets for x and y */
	if (XoOutputMode == XOX) {
		ox = XTextWidth(font,string,len)/2;
		oy = (font->max_bounds.ascent + font->max_bounds.descent)/2;
		XDrawString(display,drawable,gc,x-ox,y+oy,string,len);
	} else if (XoOutputMode == XOPS) {
		ox = XTextWidth(font,string,len)/2;
		oy = (font->max_bounds.ascent + font->max_bounds.descent)/2;
		XPSDrawString(display,drawable,gc,x-ox,y+oy,string,len);
	}
}

void XoFillCenteredString(display,drawable,gc,font,x,y,string,len)
	Display *display;
	Drawable drawable;
	GC gc;
	XFontStruct *font;
	int x,y;
	char	*string;
	int		len;
{
	int ox,oy; /* offsets for x and y */
	if (XoOutputMode == XOX) {
		ox = XTextWidth(font,string,len)/2;
		oy = (font->max_bounds.ascent + font->max_bounds.descent)/2;
		XDrawImageString(display,drawable,gc,x-ox,y+oy,string,len);
	} else if (XoOutputMode == XOPS) {
		ox = XTextWidth(font,string,len)/2;
		oy = (font->max_bounds.ascent + font->max_bounds.descent)/2;
		XPSDrawImageString(display,drawable,gc,x-ox,y+oy,string,len);
	}
}

void XoDrawRtAlignedString(display,drawable,gc,font,x,y,string,len)
	Display *display;
	Drawable drawable;
	GC gc;
	XFontStruct *font;
	int x,y;
	char	*string;
	int		len;
{
	int ox,oy; /* offsets for x and y */
	if (XoOutputMode == XOX) {
		ox = XTextWidth(font,string,len);
		oy = (font->max_bounds.ascent + font->max_bounds.descent)/2;
		XDrawString(display,drawable,gc,x-ox,y+oy,string,len);
	} else if (XoOutputMode == XOPS) {
		ox = XTextWidth(font,string,len);
		oy = (font->max_bounds.ascent + font->max_bounds.descent)/2;
		XPSDrawString(display,drawable,gc,x-ox,y+oy,string,len);
	}
}


void XoDrawLtAlignedString(display,drawable,gc,font,x,y,string,len)
	Display *display;
	Drawable drawable;
	GC gc;
	XFontStruct *font;
	int x,y;
	char	*string;
	int		len;
{
	int ox,oy; /* offsets for x and y */
	if (XoOutputMode == XOX) {
		ox = XTextWidth(font,string,len);
		oy = (font->max_bounds.ascent + font->max_bounds.descent)/2;
		XDrawString(display,drawable,gc,x,y+oy,string,len);
	} else if (XoOutputMode == XOPS) {
		ox = XTextWidth(font,string,len);
		oy = (font->max_bounds.ascent + font->max_bounds.descent)/2;
		XPSDrawString(display,drawable,gc,x,y+oy,string,len);
	}
}

void XoFillRtAlignedString(display,drawable,gc,font,x,y,string,len)
	Display *display;
	Drawable drawable;
	GC gc;
	XFontStruct *font;
	int x,y;
	char	*string;
	int		len;
{
	int ox,oy; /* offsets for x and y */
	if (XoOutputMode == XOX) {
		ox = XTextWidth(font,string,len);
		oy = (font->max_bounds.ascent + font->max_bounds.descent)/2;
		XDrawImageString(display,drawable,gc,x-ox,y+oy,string,len);
	} else if (XoOutputMode == XOPS) {
		ox = XTextWidth(font,string,len);
		oy = (font->max_bounds.ascent + font->max_bounds.descent)/2;
		XPSDrawImageString(display,drawable,gc,x-ox,y+oy,string,len);
	}
}

void XoDrawVertString(display,drawable,gc,font,x,y,string,len)
	Display *display;
	Drawable drawable;
	GC gc;
	XFontStruct *font;
	int x,y;
	char	*string;
	int		len;
{
	int ox,oy; /* offsets for x and y */
	int	dy;
	char	tstr[2];
	int i;

	tstr[1] = '\0';
	if (XoOutputMode == XOX) {
		ox = x - XTextWidth(font,string,1)/2;
		dy = font->max_bounds.ascent+font->max_bounds.descent;
		oy = y - ( len * dy ) / 2;
		for(i=0;i<len;i++,oy+=dy) {
			tstr[0] = string[i];
			XDrawString(display,drawable,gc,ox,oy,tstr,1);
		}
	} else if (XoOutputMode == XOPS) {
		ox = x - XTextWidth(font,string,1)/2;
		dy = font->max_bounds.ascent+font->max_bounds.descent;
		oy = y - ( len * dy ) / 2;
		for(i=0;i<len;i++,oy+=dy) {
			tstr[0] = string[i];
			XPSDrawString(display,drawable,gc,ox,oy,tstr,1);
		}
	}
}
