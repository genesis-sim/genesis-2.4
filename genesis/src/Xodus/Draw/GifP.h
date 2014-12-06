/* $Id: GifP.h,v 1.1.1.1 2005/06/14 04:38:32 svitak Exp $ */
/*
 * $Log: GifP.h,v $
 * Revision 1.1.1.1  2005/06/14 04:38:32  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.2  2000/06/12 04:28:17  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.1  1994/02/02 18:49:54  bhalla
 * Initial revision
 * */
#ifndef _GifP_h
#define _GifP_h

#include "Gif.h"
#include "PixP.h" /* include superclass private header file */

/*
** Relevant fields :
** The sphere pix only has a radius field R. The rest are inherited
** from the pix class
*/

typedef struct _GifClassPart {
	int make_compiler_happy;
} GifClassPart;

typedef struct _GifClassRec {
  RectObjClassPart	rect_class;
  PixClassPart		pix_class;
  GifClassPart		gif_class;
} GifClassRec;

extern GifClassRec gifClassRec;

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
	XImage	*image;

} GifPart;

typedef struct _GifRec {
  ObjectPart		object;
  RectObjPart	rectangle;
  PixPart		pix;
  GifPart		gif;
} GifRec;


#endif /* _GifP_h */
