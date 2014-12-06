#ifndef _Xodumputils_h
#define _Xodumputils_h


typedef unsigned short pixval;

typedef struct _XgPixel{
	 pixval r;	
	 pixval g;
	 pixval b;
}XgPixel;



/** Genesis external routines and definitions **/
#define FILEFMTS 3
#define GIF 0
#define JPEG 1
#define MPEG 2 
 extern void XoDumpGIF(/*Window, String filename */);
 extern void XoDumpMPEG();
 extern void XoDumpJPEG();

/*  Useful library routines */

/* Generic routine to get an image from a widget*/
 extern void* XoGetXImage(/*Widget */); 
/* Generic routine to get the 2-D pixel array from an ximage */
 extern XgPixel** XoReadXgPixels(/* XImage*, int *maxcolorval */); 

#endif
