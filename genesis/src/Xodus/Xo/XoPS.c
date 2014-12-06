/* $Id: XoPS.c,v 1.2 2005/07/01 10:02:51 svitak Exp $ */
/*
 * $Log: XoPS.c,v $
 * Revision 1.2  2005/07/01 10:02:51  svitak
 * Misc fixes to address compiler warnings, esp. providing explicit types
 * for all functions and cleaning up unused variables.
 *
 * Revision 1.1.1.1  2005/06/14 04:38:32  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.6  2000/06/12 04:28:20  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.5  1997/08/07 20:40:04  venkat
 * Randy Gobbel's (RG) optimization which defers the PSLineTo() call from
 * XPSDrawLines() for repeated colinear lines - cuts size of file significantly.
 *
 * Revision 1.4  1997/05/29 00:52:08  venkat
 * Implemented -usage and -help options
 *
 * Revision 1.3  1995/04/12  20:43:00  venkat
 * XQueryFont function calls changed to get the
 * correct second argument of the font id and not the graphics
 * context.
 *
 * Revision 1.2  1995/01/13  22:31:33  dhb
 * X11R6 patches from Upi.  Replaces references to GC structure internal
 * fields with function calls which use correct Xlib interface functions.
 *
 * Revision 1.1  1994/01/13  18:34:14  bhalla
 * Initial revision
 * */
#include <stdio.h>
#include <stdlib.h>	/* getenv */
#include <string.h>	/* strcmp, atoi */
#include <math.h>
#include <X11/Xlib.h>
#include <signal.h>

/* A set of postscript driver functions and variables */

static int	SCREEN_DPI = 100;
static FILE	*PSfp;
static char		*PSfilename;
static int	global_window_height;
static int	global_window_width;
static float	pix_scale;
static float	page_scale;
static int		inverse = 1;
static int		min_gray = 0;
static int		max_gray = 256;
static int		current_gray = -1;
static int		current_line = -1;
static int		fileout = 0;

static void XoPS_usage(/* int argc, char **argv */);

/* This function does 2 things. It provides the user with the
** current settings, when invoked without any args, and sets
** the appropriate args when called with them
*/

static void XoPS_usage(argc, argv) 
int argc;
char **argv;
{
        printf("usage: %s [-maxgray maxgray(0-256)][-mingray mingray][-inverse 0/1]\n",argv[0]);

        printf("[-dpi screendpi][-filename name][-printer][-help][-usage]\n");
        printf("status : maxgray = %d, mingray = %d, inverse = %d, dpi = %d\n",
            max_gray, min_gray, inverse, SCREEN_DPI);
        if(fileout) {
            printf("output to file '%s'\n", PSfilename);
        } else {
            printf("output to printer\n");
        }
}

/* do_psparms(argc,argv) */
int XoSetPSparms(argc,argv)
int argc;
char **argv;
{
int nxtarg;
	char	*CopyString();
        if(argc < 2){
		XoPS_usage(argc, argv);
        	return(0);
        }
        nxtarg = 0;
        while(++nxtarg < argc){
                if(strcmp(argv[nxtarg],"-maxgray") == 0){
						max_gray = atoi(argv[++nxtarg]);
                } else
                if(strcmp(argv[nxtarg],"-mingray") == 0){
						min_gray = atoi(argv[++nxtarg]);
                } else
                if(strcmp(argv[nxtarg],"-dpi") == 0){
						SCREEN_DPI = atoi(argv[++nxtarg]);
                } else
                if(strcmp(argv[nxtarg],"-filename") == 0){
                    PSfilename = CopyString(argv[++nxtarg]);
					fileout = 1;
                } else
                if(strcmp(argv[nxtarg],"-printer") == 0){
					fileout = 0;
                } else
                if(strcmp(argv[nxtarg],"-inverse") == 0){
                        inverse = atoi(argv[++nxtarg]);
                } else
                if(strcmp(argv[nxtarg],"-usage") == 0){
			XoPS_usage(argc, argv);
                } else
                if(strcmp(argv[nxtarg],"-help") == 0){
			XoPS_usage(argc, argv);
                }
        }
	return 0;
}

float ComputeGray(pixel)
int pixel;
{
    if(inverse){
	return(1.0-(float)(pixel - min_gray)/(max_gray -min_gray));
    }
    return((float)(pixel - min_gray)/(max_gray -min_gray));
}

void PSSetPixel(display,pixel)
Display *display;
unsigned long pixel;
{

    if(pixel != current_gray){
	/*
	XColor color;
	int intensity;
	color.pixel = pixel;
	XQueryColor(display,DefaultColormap(display,XDefaultScreen(display)),&color);
	intensity = (color.red + color.green + color.blue)/3;
	*/
	current_gray = pixel;
	if(pixel == XBlackPixel(display,XDefaultScreen(display))){
	    fprintf(PSfp,"%d setgray\n",inverse);
	} else 
	if(pixel == XWhitePixel(display,XDefaultScreen(display))){
	    fprintf(PSfp,"%d setgray\n",!inverse);
	} else {
	    fprintf(PSfp,"%f setgray\n",ComputeGray(pixel));
	}
    }
}

void PSSetLineWidth(width)
int width;
{
    if(width == 0) width = 1;
    if(current_line != width){ 
	current_line = width;
	fprintf(PSfp,"%f setlinewidth\n",current_line/pix_scale);
    }
}

void PSClosefill()
{
    fprintf(PSfp,"closepath fill\n");
}

void PSNewpath()
{
    fprintf(PSfp,"newpath\n");
}

void PSStroke()
{
    fprintf(PSfp,"stroke\n");
}

void PSMoveto(x,y)
int x,y;
{
    fprintf(PSfp,"%d %d moveto\n", x, global_window_height - y);
}

void PSLineto(x,y)
int x,y;
{
    fprintf(PSfp,"%d %d lineto\n", x, global_window_height - y);
}

void PSShow(s)
char *s;
{
    fprintf(PSfp,"(%s) show\n",s);
}

void PSFont(height)
int height;
{
    fprintf(PSfp,"/Helvetica findfont %d scalefont setfont\n",height);
}

unsigned long XoGetGCForeground(display,gc)
	Display *display;
	GC gc;
{
	XGCValues    val;

	XGetGCValues(display,gc,GCForeground,&val);
	return val.foreground;
}

int XoGetGCLineWidth(display,gc)
	Display *display;
	GC gc;
{
	XGCValues    val;

	XGetGCValues(display,gc,GCLineWidth,&val);
	return val.line_width;
}

void XPSDrawText(display,drawable,context,x,y,s)
Display 	*display;
Drawable	drawable;
GC		context;
int 		x, y;
char 		*s;
{
XFontStruct *finfo;
int	height;

	/*
	** get the current foreground color from the graphics context
	*/


	PSSetPixel(display,XoGetGCForeground(display,context));
/*	finfo  = XQueryFont(display,context->gid); */
	finfo  = XQueryFont(display,XGContextFromGC(context));
	height = finfo->ascent + finfo->descent;
	PSFont(height);
	PSMoveto(x,y);
	PSShow(s);
	XFreeFontInfo(NULL,finfo,0);
}

void XPSDrawLine (display, drawable, context, x1, y1, x2, y2)
Display	*display;
Drawable drawable;
GC 	context;
int 	x1,y1,x2,y2;
{

	/*
	** get the current foreground color from the graphics context
	*/
	PSSetPixel(display,XoGetGCForeground(display,context));
	PSSetLineWidth(XoGetGCLineWidth(display,context));
	PSNewpath();
	PSMoveto(x1,y1);
	PSLineto(x2,y2);
	PSStroke();
}

#define MAXCHUNK 10000

void XPSDrawLines(display,drawable,context,points,npoints,mode)
Display *display;
Drawable drawable;
GC context;
XPoint   *points;
int     npoints;
int     mode;
{
int	i;
int	pcount;
XPoint  start, end;
	if(npoints <= 0 ){
	    return;
	}
	PSNewpath();
	PSSetPixel(display,XoGetGCForeground(display,context));
	PSSetLineWidth(XoGetGCLineWidth(display,context));
	end = start = points[0];
	PSMoveto(start.x, start.y);
	pcount = 0;
	for(i=1;i<npoints;i++){
	  /* This code used to produce zillions of extremely short or even
	     zero-length lines.  The following if statement consolidates
	     repeated colinear vertical and horizontal lines.  The essence of
	     it is: if we're in line with the current direction of the line
	     being drawn, and going the same direction, just update the
	     endpoint--so the PSLineto call is deferred until the line goes a
	     different direction.  This could be optimized even further by
	     dealing with cases where a bunch of lines go opposite directions,
	     but in the same place.  The simple optimization below cuts down the
	     size of the Postscript files from my simulations by more than
	     95%.  -RG
	  */
	  if (
	      (((start.x == end.x) && (points[i].x == start.x)) &&
	       (((end.y >= start.y) && (points[i].y >= end.y)) ||
		((end.y <= start.y) && (points[i].y <= end.y)))) ||
	      (((start.y == end.y) && (points[i].y == start.y)) &&
	       (((end.x >= start.x) && (points[i].x >= end.x)) ||
		((end.x <= start.x) && (points[i].x <= end.x))))) {
	    end = points[i];
	  } else {
	    PSLineto(end.x, end.y);
	    PSLineto(points[i].x,points[i].y);
	    end = start = points[i];
	    /*
	    * break it up into managable cuhnks
	    */
	    if(pcount > 200){
		PSStroke();
		PSNewpath();
		PSMoveto(points[i].x,points[i].y);
		pcount = 0;
	    }
	    pcount++;
	  }
	}
	PSLineto(end.x, end.y);
	PSStroke();
}

void XPSDrawRectangle(display,drawable,context,x,y,w,h)
Display *display;
Drawable drawable;
GC context;
int     x,y,w,h;
{
	PSSetPixel(display,XoGetGCForeground(display,context));
	PSSetLineWidth(XoGetGCLineWidth(display,context));
	PSNewpath();
	PSMoveto(x,y);
	PSLineto(x+w,y);
	PSLineto(x+w,y+h);
	PSLineto(x,y+h);
	PSLineto(x,y);
	PSStroke();
}

void XPSFillRectangle(display,drawable,context,x,y,w,h)
Display *display;
Drawable drawable;
GC context;
int     x,y,w,h;
{
	PSSetPixel(display,XoGetGCForeground(display,context));
	PSNewpath();
	PSMoveto(x,y);
	PSLineto(x+w,y);
	PSLineto(x+w,y+h);
	PSLineto(x,y+h);
	PSClosefill();
}

void XPSFillPolygon(display,drawable,context,points,npoints,shape,mode)
Display *display;
Drawable drawable;
GC context;
XPoint   *points;
int     npoints;
int     shape;
int     mode;
{
int	i;
int	pcount;

	if(npoints <= 0){
	    return;
	}
	PSNewpath();
	PSSetPixel(display,XoGetGCForeground(display,context));
	PSMoveto(points[0].x,points[0].y);
	pcount = 0;
	for(i=1;i<npoints;i++){
	    PSLineto(points[i].x,points[i].y);
	    /*
	    * break it up into managable cuhnks
	    */
	    if(pcount > 200){
		PSClosefill();
		PSMoveto(points[i].x,points[i].y);
		pcount = 0;
	    }
	    pcount++;
	}
	PSClosefill();
}

void PSHeader(info,requested_scale)
XWindowAttributes	*info;
float		requested_scale;
{
float	scale,scalex,scaley;

    fprintf(PSfp,"%%!\n");
    fprintf(PSfp,"initgraphics\n");

    global_window_width = info->width;
    global_window_height = info->height;

    scalex = 8.5/global_window_width;
    scaley = 11.0/global_window_height;
    /*
    * use the min scale
    * but only use 90 percent so that it doesnt completely fill the page
    */
    scale = requested_scale*((scalex < scaley) ? scalex : scaley);
    /*
    * convert to dots/pixel
    */
    pix_scale = scale*SCREEN_DPI;
    /*
    * convert to points/pixel
    */
    page_scale = scale*72;
    fprintf(PSfp,"%f %f scale\n",page_scale,page_scale);
    fprintf(PSfp,"%f %f translate\n",
    72*(scalex-scale)*global_window_width/2.0,
    72*(scaley-scale)*global_window_height/2.0);
}

void PSTrailer()
{
    fprintf(PSfp,"showpage ");
}


int PreparePS(display,window,scale)
Display	*display;
Window 	window;
float	scale;
{
char command[80];
XWindowAttributes	info;
char *printer;

    XGetWindowAttributes(display,window,&info);
    if(fileout){
	PSfp = fopen(PSfilename,"w");
    } else {
	if((printer = (char *)getenv("PRINTER")) == NULL){
	    printer = "lw";
	}
	sprintf(command,"lpr -h -P%s",printer);
	PSfp = popen(command,"w");
	if (!PSfp) {
		fprintf(stderr,"could not open pipe to printer '%s'\n",printer);
		return(0);
	}
    }
    PSHeader(&info,scale);
	return(1);
}

void FinishPS()
{
    PSTrailer();
    current_line = -1;
    current_gray = -1;
    if(fileout){
	fclose(PSfp);
    } else {
	pclose(PSfp);
    }
}

/*
** sample :
**
** your code
** PreparePS(display,window);
** refresh_window(window);
** FinishPS();
**
*/

void XPSDrawString (display, w, gc, x, y, str, len)
	Display	*display;
	Window w;
	GC	gc;
	int x, y;
	char *str;               /* NOT necessarily null-terminated */
	int len;                 /* string length */
{
XFontStruct *finfo;
int	i;
int	height, width;
char string[200];

		for(i=0;i<len;i++){
			string[i] = str[i];
		}
		string[len] = '\0';
	/*
	** get the current foreground color from the graphics context
	*/
	PSSetPixel(display,XoGetGCForeground(display,gc));
/*	finfo = XQueryFont(display, gc->gid); */
	finfo  = XQueryFont(display,XGContextFromGC(gc));
	height = finfo->max_bounds.ascent + finfo->max_bounds.descent;
	width = finfo->max_bounds.rbearing - finfo->min_bounds.lbearing;
		fprintf(PSfp,"/Helvetica findfont %d scalefont setfont\n",
		height);
		fprintf(PSfp,"%d %d moveto\n",x+width,
		global_window_height - (y /* + height */));
		fprintf(PSfp,"(%s) show\n",str);

	XFreeFontInfo(NULL,finfo,0);
}

void XPSDrawImageString(display,window,gc,x,y,str, len)
	Display *display;
	Window	window;
	GC	gc;
	int	x,y;
	char	*str;
	int	len;
{
	XPSDrawString (display, window, gc, x, y, str, len);
}

void XPSDrawPoint(display, window, gc, x, y)
	Display *display;
	Window	window;
	GC	gc;
	int	x,y;
{
		XPSDrawLine(display,window,gc,x,y,x,y);
}

void XPSDrawPoints(display, window, gc, pts, npts, mode)

	Display *display;
	Window	window;
	GC	gc;
	XPoint	*pts;
	int	npts;
	int	mode;
{
	int	i;
	static	int	lastx, lasty;
	    lastx = lasty = 0;
	    for (i = 0; i <npts; i++) {
		    if (mode == CoordModePrevious) {
			    XPSDrawPoint(display,window, gc, pts[i].x+lastx,
				     pts[i].y+lasty);
			    lastx =pts[i].x;
			    lasty =pts[i].y;
		    } else {
			    XPSDrawPoint(display,window, gc, pts[i].x, pts[i].y);
		    }
	    }
}

void XPSDrawSegments(display,window,gc,segs,nsegs)
	Display *display;
	Window	window;
	GC	gc;
	XSegment	*segs;
	int	nsegs;
{
	int	i;
		for (i =0 ; i < nsegs; i++)
			XPSDrawLine(display,window,gc,segs[i].x1,
				segs[i].y1, segs[i].x2,
				segs[i].y2);
}

void XPSDrawRectangles(display,window,gc,rects,nrects)
	Display *display;
	Window	window;
	GC	gc;
	XRectangle	*rects;
	int	nrects;
{
	int	i;
		for (i =0; i < nrects; i++)
			XPSDrawRectangle(display,window,gc,
				rects[i].x, rects[i].y, 
				rects[i].width, rects[i].height);
		
}

void XPSFillRectangles(display,window,gc,rects,nrects)
	Display *display;
	Window	window;
	GC	gc;
	XRectangle	*rects;
	int	nrects;
{
	int	i;
		for (i =0; i < nrects; i++)
			XPSFillRectangle(display,window,gc,
				rects[i].x, rects[i].y, 
				rects[i].width, rects[i].height);
		
}
