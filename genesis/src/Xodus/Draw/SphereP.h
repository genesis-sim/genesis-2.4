/* $Id: SphereP.h,v 1.1.1.1 2005/06/14 04:38:32 svitak Exp $ */
/*
 * $Log: SphereP.h,v $
 * Revision 1.1.1.1  2005/06/14 04:38:32  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.4  2000/06/12 04:28:18  mhucka
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
#ifndef _SphereP_h
#define _SphereP_h

#include "Sphere.h"
#include "PixP.h" /* include superclass private header file */

/*
** Relevant fields :
** The sphere pix only has a radius field R. The rest are inherited
** from the pix class
*/

typedef struct _SphereClassPart {
	int make_compiler_happy;
} SphereClassPart;

typedef struct _SphereClassRec {
  RectObjClassPart	rect_class;
  PixClassPart		pix_class;
  SphereClassPart		sphere_class;
} SphereClassRec;

extern SphereClassRec sphereClassRec;

typedef struct {
	/* resources */
	float r;
	/* private */
	Dimension screenr;
} SpherePart;

typedef struct _SphereRec {
  ObjectPart		object;
  RectObjPart	rectangle;
  PixPart		pix;
  SpherePart		sphere;
} SphereRec;


#endif /* _SphereP_h */
