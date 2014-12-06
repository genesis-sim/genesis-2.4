static char rcsid[] = "$Id: sim_tform.c,v 1.3 2005/06/29 17:16:30 svitak Exp $";

/*
** $Log: sim_tform.c,v $
** Revision 1.3  2005/06/29 17:16:30  svitak
** Oops. profile.h is needed.
**
** Revision 1.2  2005/06/27 19:01:12  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:29  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.3  2001/04/25 17:17:03  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.2  1993/07/21 21:32:47  dhb
** fixed rcsid variable type
**
 * Revision 1.1  1992/10/27  20:35:16  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include "sim_ext.h"

void SetProjectionScale(projection_parm,scale)
struct projection_parm_type *projection_parm;
float	*scale;
{
	projection_parm->matrix[0][0] *= scale[0];
	projection_parm->matrix[1][1] *= scale[1];
	projection_parm->matrix[2][2] *= scale[2];
}

void SetProjectionTranslate(projection_parm,translate)
struct projection_parm_type *projection_parm;
float	*translate;
{
	projection_parm->translate[0] += translate[0];
	projection_parm->translate[1] += translate[1];
	projection_parm->translate[2] += translate[2];
}

void SetProjectionRotate(projection_parm,rotate_z)
struct projection_parm_type *projection_parm;
float	rotate_z;
{
}

/*
* ProjectionCoordTransform_2D
*
* transforms src coordinates into dst coordinates for a particular projection
* assuming no components in the z direction
* this restriction is a requirement of the relative distribution
* algorithms which assume equivalent projection characteristics
* independent of the absolute src coordinates
* the absolute distribution algorithms can precalculate
* arbitrarily complex 3 dimensional relationships on a per-connection
* basis and thus do not need to adhere to this restriction.
* Of course they are slower and require more storage and therefore 
* can only be effectively used in the sparsely connected case
*/
void ProjectionCoordTransform_2D(projection_parm,x,y,tx,ty)
struct projection_parm_type *projection_parm;
float	x,y,*tx,*ty;
{

	/*
	* apply the the 2-D transformation matrix for this projection
	* to the coordinates
	*/
	*tx = projection_parm->matrix[0][0]*x + projection_parm->matrix[0][1]*y +
		projection_parm->translate[0];
	*ty = projection_parm->matrix[1][0]*x + projection_parm->matrix[1][1]*y + 
		projection_parm->translate[1];

}

