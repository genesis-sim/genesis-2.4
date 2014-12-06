/* $Id: ImageP.h,v 1.1.1.1 2005/06/14 04:38:33 svitak Exp $ */
/*
 * $Log: ImageP.h,v $
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.4  2000/10/06 04:48:51  mhucka
 * Added a new field to ImagePart in order to be able to handle X graphics
 * contexts properly.  See the associated changes in Image.c.
 *
 * Revision 1.3  2000/06/12 04:28:19  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.2  1995/01/20 01:14:53  venkat
 *  <X11/copyright.h> not included
 *
 * Revision 1.1  1994/03/22  15:35:54  bhalla
 * Initial revision
 * */
/** Not available in some systems **/
/* #include <X11/copyright.h> */ 

/* Xodus : ImageP.h,v 1.0,  12 Nov 1989, Upi Bhalla */
/* Copyright	California Institute of Technology	1989 */

/*
** This widget for displaying gif format images
*/

#ifndef _ImageP_h
#define _ImageP_h

#include "Image.h"

/* include superclass private header file */
#include <X11/CoreP.h>
#include <X11/Composite.h>

typedef struct {
    int empty;
} ImageClassPart;

typedef struct _ImageClassRec {
    CoreClassPart	core_class;
    CompositeClassPart composite_class;
    ImageClassPart	image_class;
} ImageClassRec;

extern ImageClassRec imageClassRec;

typedef struct {
    /* resources */
	char	*filename;
	char	*fileformat;
	char	*storemode;
	char	*value;
	Boolean	rescaleable;
	int		srcx,srcy;
	int		dstx,dsty;
	int		wx,wy;
	XtCallbackList callbacks;
    /* private state */
	XImage	*image;
        GC       gc;
} ImagePart;

typedef struct _ImageRec {
    CorePart		core;
	CompositePart	composite;
    ImagePart	image;
} ImageRec;

#endif 
