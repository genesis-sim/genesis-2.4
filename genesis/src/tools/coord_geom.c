static char rcsid[] = "$Id: coord_geom.c,v 1.2 2005/07/01 10:03:10 svitak Exp $";

/*
** $Log: coord_geom.c,v $
** Revision 1.2  2005/07/01 10:03:10  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:33  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.5  1997/07/18 03:02:36  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.4  1997/06/12 22:26:24  dhb
** Fixed typo in do_rotate()'s check that path names existing elements,
** == in conditional rather than =
**
** Revision 1.3  1993/02/24 01:32:17  dhb
** 1.4 to 2.0 command argument changes.
**
 * Revision 1.2  1992/10/29  19:06:29  dhb
 * Replaced explicit msgin looping with MSGLOOP macro.
 * Removed commented dead code.
 *
 * Revision 1.1  1992/10/29  19:04:10  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include <math.h>
#include "sim_ext.h"
#include "tools.h"

#define EGG_AVERAGE 1
#define EGG_ROOT 2
#define EGG_SHELL 3
#define BOX_AVERAGE 4
#define BOX_ROOT 5
#define BOX_SHELL 6
#ifndef EPSILON
#define EPSILON 1.0e-60
#endif
#define	RAXIAL	1
#define	AXIAL	2

#define dot_prd(x1,y1,z1,x2,y2,z2) (x1 * x2 + y1 * y2 + z1 * z2)

void constrain();
void sum_vectors();
void cross_prd();
void rotate3d();
void RotateAboutAxis();

void do_rancoord(argc,argv)
	int	argc;
	char	**argv;
{
	Element	*elm;

	int el_flag = 0;
	int ch_flag = 0;
	int	status;

	initopt(argc, argv, "element fraction -electrical -channel");
	while ((status = G_getopt(argc, argv)) == 1)
	  {
	    if (strcmp(G_optopt, "-electrical") == 0)
		el_flag = 1;
	    else if (strcmp(G_optopt, "-channel") == 0)
		ch_flag = 1;
	  }

	if (status < 0) {
		printoptusage(argc, argv);
		return;
	}

	if (!(elm= GetElement(optargv[1]))) {
		fprintf(stderr,"Element %s not found\n",optargv[1]);
		return;
	}
	rancoord(elm,Atof(optargv[2]),Atof(optargv[2]),Atof(optargv[2]),el_flag,ch_flag);
}

void do_rotate(argc,argv)
	int argc;
	char	**argv;
{
	Element	*elm;
	ElementList	el;
	Element	*elms[MAX_EL_SIZE];
	ElementList	*sel;
	float	ox = 0.0,oy = 0.0,oz = 0.0;
	float	ax = 0.0,ay = 0.0,az = 1.0;
	float	dx,dy,dz;
	int		translatekids = 0;
	int		fixkids = 0;
	int		i;
	float	theta;
	char	*str;
	int	status;

	el.element = elms;

	initopt(argc, argv, "path angle -x -y -z -axis x y z -center x y z -translatekids -fixkids");
	while ((status = G_getopt(argc, argv)) == 1)
	  {
	    if (strcmp(G_optopt, "-x") == 0)
	      {
		ax = 1.0; ay = 0.0 ; az = 0.0;
	      }
	    else if (strcmp(G_optopt, "-y") == 0)
	      {
		ax = 0.0; ay = 1.0 ; az = 0.0;
	      }
	    else if (strcmp(G_optopt, "-z") == 0)
	      {
		ax = 0.0; ay = 0.0 ; az = 1.0;
	      }
	    else if (strcmp(G_optopt, "-axis") == 0)
	      {
		ax = Atof(optargv[1]);
		ay = Atof(optargv[2]);
		az = Atof(optargv[3]);
	      }
	    else if (strcmp(G_optopt, "-center") == 0)
	      {
		ox = Atof(optargv[1]);
		oy = Atof(optargv[2]);
		oz = Atof(optargv[3]);
	      }
	    else if (strcmp(G_optopt, "-translatekids") == 0)
		translatekids = 1;
	    else if (strcmp(G_optopt, "-fixkids") == 0)
		fixkids = 1;
	  }

	if (status < 0) {
		printoptusage(argc, argv);
		printf("  The -x, -y, -z and -axis options select the axis of rotation. -x, -y and -z select \n");
		printf("a coordinate axis. The -axis option sets a vector for the rotation. \n");
		printf("Default is z axis\n");
		printf("-center sets the center of the rotation. Default is the origin\n");
		printf("By default, the transform applies to all children of theselected element\n");
		printf("-translatekids causes the children to be translated parallel to the parent,\n");
		printf("rather than rotated.  -fixkids prevents the children from being moved\n");
		return;
	}


	sel = WildcardGetElement(optargv[1]);

	/*
	if (!(elm = GetElement(optargv[1]))) {
		printf("Element '%s' not found\n",optargv[1]);
		return;
	}
	*/
	if (sel->nelements == 0) {
		printf("No elements found on path '%s'\n",optargv[1]);
		return;
	}
	theta = Atof(optargv[2]);

	if (fixkids) {
		RotateAboutAxis(sel,ox,oy,oz,ax,ay,az,theta);
	} else if (translatekids) {
		for (i = 0 ; i < sel->nelements ; i++) {
			elm = sel->element[i];
			find_all_dends(elm,1,&el);
			dx = elm->x; dy = elm->y; dz = elm->z;
			RotateAboutAxis(&el,ox,oy,oz,ax,ay,az,theta);
			dx = elm->x - dx; dy = elm->y - dy; dz = elm->z - dz;
			rel_position(elm,dx,dy,dz);
			elm->x -= dx; elm->y -= dy; elm->z -= dz;
		}
	} else {
		for (i = 0 ; i < sel->nelements ; i++) {
			elm = sel->element[i];
			find_all_dends(elm,1,&el);
			RotateAboutAxis(&el,ox,oy,oz,ax,ay,az,theta);
		}
	}
	FreeElementList(sel);
}

void do_rel_position(argc,argv)
	int	argc;
	char	**argv;
{
	Element	*elm;

	initopt(argc, argv, "element dx dy dz");
	if (G_getopt(argc, argv) != 0)
	  {
	    printoptusage(argc, argv);
	    return;
	  }

	if (!(elm= GetElement(optargv[1]))) {
		fprintf(stderr,"Element %s not found\n",optargv[1]);
		return;
	}
	rel_position_dends(elm,Atof(optargv[2]),Atof(optargv[3]),Atof(optargv[4]));
}

void do_egg_constrain(argc,argv)
	int	argc;
	char	**argv;
{

	initopt(argc, argv, "path start x y z length width height");
	if (G_getopt(argc, argv) != 0)
	  {
	    printoptusage(argc, argv);
	    return;
	  }

	constrain(optargv[1],optargv[2],EGG_AVERAGE,
		Atof(optargv[3]), Atof(optargv[4]), Atof(optargv[5]),
		Atof(optargv[6]), Atof(optargv[7]), Atof(optargv[8]));
}


/*
** Path specifies the elements to be constrained
** Mode specifies how to constrain them : using an ellipsoid or
** a box
** Vectors 1 and 2 specify the bounds of the ellipsoid/box
*/
void constrain(path,start,mode,x1,y1,z1,x2,y2,z2)
	char	*path;
	char	*start;
	int		mode;
	float	x1,y1,z1,x2,y2,z2;
{
	ElementList	*el; /* nelements, **element */
	Element	*elm;
	float	x = 0, y = 0, z = 0;

	el = WildcardGetElement(path,0);
	elm = GetElement(start);

	switch(mode)  {
		case EGG_AVERAGE :
			/* Take the average direction (by summing all vectors)
			** and turn so that this faces to the middle of the volume
			*/
			sum_vectors(elm,&x,&y,&z);
			rotate3d(el,elm->x,elm->y,elm->z,x,y,z,x1,y1,z1);
			break;
		case EGG_ROOT :
			/* Take the vector specified by the start and its parent,
			** and turn so that this faces to the middle of the volume
			*/
			break;
		case EGG_SHELL :
			/* Check all vectors and move into the vol.
			*/
			break;
		case BOX_AVERAGE :
			break;
		case BOX_ROOT :
			break;
		case BOX_SHELL :
			break;
		default :
			break;
	}
}

void sum_vectors(parent,x,y,z)
	Element	*parent;
	float	*x,*y,*z;
{
	MsgIn	*msg;
	Element	*child;

	MSGLOOP(parent, msg) {
	    case RAXIAL:
		{
			child = msg->src;
			*x += child->x - parent->x;
			*y += child->y - parent->y;
			*z += child->z - parent->z;
			sum_vectors(child,x,y,z);
		}
		break;
	}
}
	

int IsInEgg(elm,x1,y1,z1,x2,y2,z2)
	Element	*elm;
	float	x1,y1,z1,x2,y2,z2;
{
	float x,y,z,ex,ey,ez;

	ex = fabs((x1-x2)/2.0);
	ey = fabs((y1-y2)/2.0);
	ez = fabs((z1-z2)/2.0);

	if (ex < EPSILON || ey < EPSILON || ez < EPSILON) {
		fprintf(stderr,"Ellipsoid is too narrow\n");
		return(0);
	}

	x = elm->x - (x1 + x2)/2.0;
	y = elm->y - (y1 + y2)/2.0;
	z = elm->z - (z1 + z2)/2.0;

	if ((x/ex * x/ex + y/ey * y/ey + z/ez * z/ez) <= 1.0)
	/* Point is within ellipsoid */
		return(1);
	return(0);
}

int IsInBox(elm,x1,y1,z1,x2,y2,z2)
	Element	*elm;
	float	x1,y1,z1,x2,y2,z2;
{
	float temp;

	if (x1 > x2) {
		temp = x1 ; 
		x1 = x2 ;
		x2 = temp;
	}
	if (y1 > y2) {
		temp = y1 ; 
		y1 = y2 ;
		y2 = temp;
	}
	if (z1 > z2) {
		temp = z1 ; 
		z1 = z2 ;
		z2 = temp;
	}

	if (elm->x > x1 && elm->x < x2 &&
		elm->y > y1 && elm->y < y2 && 
		elm->z > z1 && elm->z < z2) {
	/* Point is within box */
		return(1);
	 }
	 return(0);
}

/*
** 
**	rotate3d(elmlist,ox,oy,oz,rx,ry,rz,px,py,pz)
** elmlist is the list of elements to be rotated
**	o is the center of rotaion
**	r is the original position
**	p is the destination direction
** The axis of rotation is normal to the plane defined by 
**	o,r,p. It is centered at o
** The rotation brings r into line with p and o
** All the elements in the elmlist are rotated according to the
** transform thus calculated.
*/
void rotate3d(elmlist,ox,oy,oz,rx,ry,rz,px,py,pz)
	ElementList	*elmlist;
	float	ox,oy,oz;
	float	rx,ry,rz;
	float	px,py,pz;
{
	Element	*elm;
	int		i;

	/* coords of elements being transformed */
	float x,y,z;

	/* Vectors for the axes of the new frame of reference */
	float	ix,iy,iz;
	float	jx,jy,jz;
	float	kx,ky,kz;

	/* matrix for intermediate transform */
	float m11,m12,m13;
	float m21,m22,m23;
	float m31,m32,m33;

	/* matrix for final transform */
	float t11,t12,t13;
	float t21,t22,t23;
	float t31,t32,t33;

	/* Length of original vector rel to origin */
	float dr;

	/* length of destination vector rel to origin */
	float dp;

	/* Yet another length */
	float len;

	/* original vector rel to origin */
	float drx,dry,drz;

	/* dest vector rel to origin */
	float dpx,dpy,dpz;

	/* sin and cos variables */
	float s,c;


	/* finding the vectors rel to the origin */
	drx = rx - ox;
	dry = ry - oy;
	drz = rz - oz;
	dr = sqrt(drx * drx + dry * dry + drz * drz);

	dpx = px - ox;
	dpy = py - oy;
	dpz = pz - oz;
	dp = sqrt(dpx * dpx + dpy * dpy + dpz * dpz);

	/* ensuring that the source and dest vectors are not parallel */
	c = dot_prd(drx,dry,drz,dpx,dpy,dpz) / (dr * dp);
	if (c > 0.9999) /* ridiculously near parallel */
	/* return original coords */
		return;

	/* Setting up the axes in the new frame */
	ix = drx/dr; iy = dry/dr ; iz = drz/dr;

	cross_prd(&kx,&ky,&kz,drx,dry,drz,dpx,dpy,dpz);
	len = sqrt(kx * kx + ky * ky + kz * kz);
	kx = kx/len ; ky = ky/len ; kz = kz/len;

	cross_prd(&jx,&jy,&jz,kx,ky,kz,ix,iy,iz);

	/* Doing rotation about z axis in new frame */
	/* First, finding sin and cos : */
	c = dot_prd(drx,dry,drz,dpx,dpy,dpz) / (dr * dp);
	s = len / (dr * dp);


	/* then, pre-multiplying the axes transform by the rot transf */
	m11=c * ix - s * jx; m12=c * iy - s * jy; m13=c * iz - s * jz;
	m21=s * ix + c * jx; m22=s * iy + c * jy; m23=s * iz + c * jz;
	m31=kx; m32 = ky; m33 = kz;

	/* Finally, pre-multiplying by the inverse transform to the
	** original frame. This is just the transpose of the first
	** transform matrix, since the columns were orthogonal */
	t11 = dot_prd(ix,jx,kx,m11,m21,m31);
	t12 = dot_prd(ix,jx,kx,m12,m22,m32);
	t13 = dot_prd(ix,jx,kx,m13,m23,m33);
	t21 = dot_prd(iy,jy,ky,m11,m21,m31);
	t22 = dot_prd(iy,jy,ky,m12,m22,m32);
	t23 = dot_prd(iy,jy,ky,m13,m23,m33);
	t31 = dot_prd(iz,jz,kz,m11,m21,m31);
	t32 = dot_prd(iz,jz,kz,m12,m22,m32);
	t33 = dot_prd(iz,jz,kz,m13,m23,m33);

	/* The transformation itself : easy, innit ? */
	for (i = 0 ; i < elmlist->nelements ; i++) {
		elm = elmlist->element[i];
		x = elm->x - ox ; y = elm->y - oy ; z = elm->z - oz;
		elm->x = dot_prd(t11,t12,t13,x,y,z) + ox;
		elm->y = dot_prd(t21,t22,t23,x,y,z) + oy;
		elm->z = dot_prd(t31,t32,t33,x,y,z) + oz;
	}
}

void cross_prd(x1,y1,z1,x2,y2,z2,x3,y3,z3)
	float *x1,*y1,*z1,x2,y2,z2,x3,y3,z3;
{
	*x1 = y2 * z3 - y3 * z2;
	*y1 = z2 * x3 - z3 * x2;
	*z1 = x2 * y3 - x3 * y2;
}

/*
** 
**	RotateInPlane(elmlist,ox,oy,oz,rx,ry,rz,px,py,pz)
** elmlist is the list of elements to be rotated
**	o is the center of rotaion
**	r is the original vector : angle is calculated from this.
**	p is the third vector defining the plane of rotation.
**	theta is the angle of rotation relative to r
** The axis of rotation is normal to the plane defined by 
**	o,r,p. It is centered at o
** All the elements in the elmlist are rotated according to the
** transform thus calculated.
*/
void RotateInPlane(elmlist,ox,oy,oz,rx,ry,rz,px,py,pz,theta)
	ElementList	*elmlist;
	float	ox,oy,oz;
	float	rx,ry,rz;
	float	px,py,pz;
	float	theta;
{
	Element	*elm;
	int		i;

	/* coords of elements being transformed */
	float x,y,z;

	/* Vectors for the axes of the new frame of reference */
	float	ix,iy,iz;
	float	jx,jy,jz;
	float	kx,ky,kz;

	/* matrix for intermediate transform */
	float m11,m12,m13;
	float m21,m22,m23;
	float m31,m32,m33;

	/* matrix for final transform */
	float t11,t12,t13;
	float t21,t22,t23;
	float t31,t32,t33;

	/* Length of original vector rel to origin */
	float dr;

	/* length of destination vector rel to origin */
	float dp;

	/* Yet another length */
	float len;

	/* original vector rel to origin */
	float drx,dry,drz;

	/* dest vector rel to origin */
	float dpx,dpy,dpz;

	/* sin and cos variables */
	float s,c;


	/* finding the vectors rel to the origin */
	drx = rx - ox;
	dry = ry - oy;
	drz = rz - oz;
	dr = sqrt(drx * drx + dry * dry + drz * drz);

	dpx = px - ox;
	dpy = py - oy;
	dpz = pz - oz;
	dp = sqrt(dpx * dpx + dpy * dpy + dpz * dpz);

	/* ensuring that the original and plane vectors are not parallel */
	c = dot_prd(drx,dry,drz,dpx,dpy,dpz) / (dr * dp);
	if (c > 0.9999) /* ridiculously near parallel */
	/* return original coords */
		return;

	/* Setting up the axes in the new frame */
	ix = drx/dr; iy = dry/dr ; iz = drz/dr;

	cross_prd(&kx,&ky,&kz,drx,dry,drz,dpx,dpy,dpz);
	len = sqrt(kx * kx + ky * ky + kz * kz);
	kx = kx/len ; ky = ky/len ; kz = kz/len;

	cross_prd(&jx,&jy,&jz,kx,ky,kz,ix,iy,iz);

	/* Doing rotation about z axis in new frame */
	/* First, finding sin and cos : */
	c = cos(theta);
	s = sin(theta);

	/* then, pre-multiplying the axes transform by the rot transf */
	m11=c * ix - s * jx; m12=c * iy - s * jy; m13=c * iz - s * jz;
	m21=s * ix + c * jx; m22=s * iy + c * jy; m23=s * iz + c * jz;
	m31=kx; m32 = ky; m33 = kz;

	/* Finally, pre-multiplying by the inverse transform to the
	** original frame. This is just the transpose of the first
	** transform matrix, since the columns were orthogonal */
	t11 = dot_prd(ix,jx,kx,m11,m21,m31);
	t12 = dot_prd(ix,jx,kx,m12,m22,m32);
	t13 = dot_prd(ix,jx,kx,m13,m23,m33);
	t21 = dot_prd(iy,jy,ky,m11,m21,m31);
	t22 = dot_prd(iy,jy,ky,m12,m22,m32);
	t23 = dot_prd(iy,jy,ky,m13,m23,m33);
	t31 = dot_prd(iz,jz,kz,m11,m21,m31);
	t32 = dot_prd(iz,jz,kz,m12,m22,m32);
	t33 = dot_prd(iz,jz,kz,m13,m23,m33);

	/* The transformation itself : easy, innit ? */
	for (i = 0 ; i < elmlist->nelements ; i++) {
		elm = elmlist->element[i];
		x = elm->x - ox ; y = elm->y - oy ; z = elm->z - oz;
		elm->x = dot_prd(t11,t12,t13,x,y,z) + ox;
		elm->y = dot_prd(t21,t22,t23,x,y,z) + oy;
		elm->z = dot_prd(t31,t32,t33,x,y,z) + oz;
	}
}

/*
** 
**	RotateAboutAxis(elmlist,ox,oy,oz,kx,ky,kz,theta)
** elmlist is the list of elements to be rotated
**	o is the center of rotaion
**	k is the axis vector
**	theta is the angle of rotation.
** All the elements in the elmlist are rotated according to the
** transform thus calculated.
*/
void RotateAboutAxis(elmlist,ox,oy,oz,kx,ky,kz,theta)
	ElementList	*elmlist;
	float	ox,oy,oz;
	float	kx,ky,kz;
	float	theta;
{
	Element	*elm;
	int		i;

	/* coords of elements being transformed */
	float x,y,z;

	/* Vectors for the axes of the new frame of reference */
	float	ix,iy,iz;
	float	jx,jy,jz;

	/* matrix for intermediate transform */
	float m11,m12,m13;
	float m21,m22,m23;
	float m31,m32,m33;

	/* matrix for final transform */
	float t11,t12,t13;
	float t21,t22,t23;
	float t31,t32,t33;

	/* Yet another length */
	float len;

	/* sin and cos variables */
	float s,c;

	/* Normalising the axis vector */
	len = sqrt(kx * kx + ky * ky + kz * kz);
	kx = kx/len ; ky = ky/len ; kz = kz/len;

	/* finding vectors for the other axes.
	** Trick : to get a vector nonparallel to k, we just rotate 
	** its components. No particular directions are needed.
	*/
	cross_prd(&jx,&jy,&jz,kx,ky,kz,ky,kz,kx);
	cross_prd(&ix,&iy,&iz,jx,jy,jz,kx,ky,kz);

	/* Doing rotation about z axis in new frame */
	/* First, finding sin and cos : */
	c = cos(theta);
	s = sin(theta);

	/* then, pre-multiplying the axes transform by the rot transf */
	m11=c * ix - s * jx; m12=c * iy - s * jy; m13=c * iz - s * jz;
	m21=s * ix + c * jx; m22=s * iy + c * jy; m23=s * iz + c * jz;
	m31=kx; m32 = ky; m33 = kz;

	/* Finally, pre-multiplying by the inverse transform to the
	** original frame. This is just the transpose of the first
	** transform matrix, since the columns were orthogonal */
	t11 = dot_prd(ix,jx,kx,m11,m21,m31);
	t12 = dot_prd(ix,jx,kx,m12,m22,m32);
	t13 = dot_prd(ix,jx,kx,m13,m23,m33);
	t21 = dot_prd(iy,jy,ky,m11,m21,m31);
	t22 = dot_prd(iy,jy,ky,m12,m22,m32);
	t23 = dot_prd(iy,jy,ky,m13,m23,m33);
	t31 = dot_prd(iz,jz,kz,m11,m21,m31);
	t32 = dot_prd(iz,jz,kz,m12,m22,m32);
	t33 = dot_prd(iz,jz,kz,m13,m23,m33);

	/* The transformation itself : easy, innit ? */
	for (i = 0 ; i < elmlist->nelements ; i++) {
		elm = elmlist->element[i];
		x = elm->x - ox ; y = elm->y - oy ; z = elm->z - oz;
		elm->x = dot_prd(t11,t12,t13,x,y,z) + ox;
		elm->y = dot_prd(t21,t22,t23,x,y,z) + oy;
		elm->z = dot_prd(t31,t32,t33,x,y,z) + oz;
	}
}
