#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>

/* #include "Widg/Framed.h" */
#include "Xodumputils.h"

#include <stdio.h>

#define assigncolors(pixel, red, grn, blu) do { (pixel).r = (red); (pixel).g = (grn); (pixel).b=(blu); } while (0)
#define assignmaxcolorval(maxval, xcolor) (maxval) = (((maxval) >= (xcolor).red)? (maxval) : (((xcolor).red >= (xcolor).green) ? (xcolor).red : (((xcolor).green >= (xcolor).blue) ? (xcolor).green : (xcolor).blue)))

extern char *xocmdname;
 
Display *XgDisplay();
int XoGetUsedDepth();
Visual *XoGetUsedVisual();
Colormap XoGetUsedColormap();


void* XoGetXImage(w)
 Widget w;
{
  long 	     ignored;
  int        x, y, width, height, border, i;
  XImage     *ximage;
  Window parentW; 
/*
  Pixmap XoGetPixmapFromWidget();
  Widget XoGetFramedChild();
*/

  int screen = DefaultScreen(XgDisplay());
  Window rootW = DefaultRootWindow(XgDisplay());
  Window window = XtWindow(w);
 
	if (XtIsSubclass(w, topLevelShellWidgetClass))
		parentW = XtWindow(w);
	else 
		parentW = XtWindow(XtParent(w));

/*  Widget cw = XoGetFramedChild((FramedWidget)w); */

 /** Changes for pixmap dump  Mon Mar 25 16:22:22 PST 1996 **/
	
  if(!XGetGeometry(XgDisplay(), window, &ignored, 
	&x, &y, &width, &height, &border, &ignored)) {
	fprintf(stderr, "%s: unable to get window coordinates\n", xocmdname);
	return (NULL);
  } 


/*
  if(!XGetGeometry(XgDisplay(), XtWindow(cw), &ignored, 
	&x, &y, &width, &height, &border, &ignored)) {
	fprintf(stderr, "%s: unable to get window coordinates\n", xocmdname);
	return (NULL);
  }
*/
    

 i = DisplayWidth(XgDisplay(), screen);
      if (x + width > i)
        width = i - x;
 i = DisplayHeight(XgDisplay(), screen);
      if (y + height > i)
        height = i - y;

/*  fprintf(stderr, "The pixmap coords are x=%d,y=%d,w=%d,h=%d\n",x,y,width,height); */
 ximage = XGetImage(XgDisplay(), parentW, 
		x, y, width, height, AllPlanes,
            XoGetUsedDepth()==1 ? XYPixmap : ZPixmap);

/*
 ximage = XGetImage(XgDisplay(), XoGetPixmapFromWidget(cw), 
		x, y, width, height, AllPlanes,
            XoGetUsedDepth()==1 ? XYPixmap : ZPixmap);
*/

 return((void *)ximage);
}

static char** alloc2Darray(rows, cols, size)
 int rows, cols, size;
{
  char** its; 
  int i;
    its = (char**) malloc(rows * sizeof(char*) );
    if ( its == (char**) 0 ){
       XgError( "out of memory allocating an array" );
	return (its);
    }
    its[0] = (char*) malloc( rows * cols * size );
    if ( its[0] == (char*) 0 ){
       XgError( "out of memory allocating an array" );
	return((char **)0);
    }
    for ( i = 1; i < rows; ++i )
       its[i] = &(its[0][i * cols * size]);
    return its;
}

XgPixel** XoReadXgPixels(ximage, maxcolorvalP)
XImage *ximage;
pixval *maxcolorvalP;
{
 XgPixel **retpixels;
 XgPixel *pP;
 int Rows = ximage -> height;
 int Cols = ximage -> width;
 int Size = sizeof(XgPixel);
 int row,col;
 Colormap ourcmap = XoGetUsedColormap();
 Display *ourdisp = XgDisplay();
 XColor color;
 *maxcolorvalP = 0;

 retpixels = (XgPixel **) alloc2Darray(Rows, Cols, Size);
 if (retpixels == (XgPixel **)0) return(retpixels);
 for (row=0; row<Rows; row++){
	pP = retpixels[row];
	fprintf(stderr, ".");
  for (col=0; col < Cols; ++col, ++pP){
 	color.pixel = XGetPixel(ximage, col, row);
	XQueryColor(ourdisp, ourcmap, &color);	
	/*
	fprintf(stderr, "Column		Row \n");
	fprintf(stderr, "%d		%d  \n", col, row);
	fprintf(stderr, "color pixel index (%ld) \n",color.pixel); 
	fprintf(stderr, "rgb values are: (%d, %d, %d)\n",color.red, color.green, color.blue);
	fprintf(stderr, "--------------------------------------\n");
	system("sleep 5");
	*/
	assigncolors(*pP, color.red, color.green, color.blue);	
	assignmaxcolorval(*maxcolorvalP, color);
  }
 }	
 return (retpixels);
}

/** External func definitions to be filled in later **/

void XoDumpJPEG()
{
}

void XoDumpMPEG()
{
}
