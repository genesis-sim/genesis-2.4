static char* rcsid = "$Id: DrawUtil.c,v 1.1.1.1 2005/06/14 04:38:33 svitak Exp $";

/*
** $Log: DrawUtil.c,v $
** Revision 1.1.1.1  2005/06/14 04:38:33  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.1  1995/07/17 20:40:09  dhb
** Initial revision
**
**
** This code moved from its original location in "Draw.c".
*/

#include <math.h>

double XoLength(v)
	float *v;
{
	double r = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];

	if (r > 0)
		return(sqrt(r));
	return(0.0);
}

double XoVLength(x,y,z)
	float x,y,z;
{
	double r = x * x + y * y + z * z;

	if (r > 0)
		return(sqrt(r));
	return(0.0);
}

void XoVNormalize(x,y,z)
	float *x,*y,*z;
{
	double len = XoVLength(*x,*y,*z);
	if (len > 0) {
	    *x /= len;
	    *y /= len;
	    *z /= len;
	}
}

/* This function generates a unit cross product */
void XoUnitCross(x0,x1,x2,y0,y1,y2,z0,z1,z2)
	float x0,x1,x2;
	float y0,y1,y2;
	float *z0,*z1,*z2;
{
	double len;
	float tz[3];

	tz[0] = x1 * y2 - y1 * x2;
	tz[1] = x2 * y0 - y2 * x0;
	tz[2] = x0 * y1 - y0 * x1;

	if ((len = XoLength(tz)) > 0) {
		*z0 = tz[0]/len;
		*z1 = tz[1]/len;
		*z2 = tz[2]/len;
	} else {
		*z0 = 0;
		*z1 = 0;
		*z2 = 1;
	}
}
