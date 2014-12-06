/* $Id: CrossHairP.h,v 1.1.1.1 2005/06/14 04:38:33 svitak Exp $ */
/*
 * $Log: CrossHairP.h,v $
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.4  2000/06/12 04:28:16  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.3  1994/12/06 00:17:22  dhb
 * Nov 8 1994 changes from Upi Bhalla
 *
 * Revision 1.2  1994/01/13  19:35:36  bhalla
 * *** empty log message ***
 *
 * Revision 1.2  1994/01/13  19:35:36  bhalla
 * *** empty log message ***
 * */
#ifndef _CrossHairP_h
#define _CrossHairP_h

#include "CrossHair.h"
#include "PixP.h" /* include superclass private header file */

/*
** Relevant fields :
** The crosshair pix has no fields, but we need to use a dummy.
** The other fields are inherited from the pix class
*/

typedef struct _CrossHairClassPart {
	int make_compiler_happy;
} CrossHairClassPart;

typedef struct _CrossHairClassRec {
  RectObjClassPart	rect_class;
  PixClassPart		pix_class;
  CrossHairClassPart		crosshair_class;
} CrossHairClassRec;

extern CrossHairClassRec crosshairClassRec;

typedef struct {
	/* resources */
	int	make_compiler_happy;
} CrossHairPart;

typedef struct _CrossHairRec {
  ObjectPart		object;
  RectObjPart	rectangle;
  PixPart		pix;
  CrossHairPart		crosshair;
} CrossHairRec;


#endif /* _CrossHairP_h */
