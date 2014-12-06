static char rcsid[] = "$Id: zbrent.c,v 1.2 2005/07/01 10:03:10 svitak Exp $";

/*
** $Log: zbrent.c,v $
** Revision 1.2  2005/07/01 10:03:10  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.1  1992/12/11 19:06:13  dhb
** Initial revision
**
*/

#include <math.h>

#define ITMAX 100
#define EPS 3.0e-8

float zbrent(func,x1,x2,tol,erflag)
float x1,x2,tol;
float (*func)();	/* ANSI: float (*func)(float); */
int		*erflag;	/* error condtion check for graceful death */
{
	int iter;
	float a=x1,b=x2,c = 0.0,d = 0.0,e = 0.0,min1,min2;
	float fa=(*func)(a),fb=(*func)(b),fc,p,q,r,s,tol1,xm;
	void nrerror();

	if (fb*fa > 0.0) {
		nrerror("Root must be bracketed in ZBRENT");
		*erflag = 1;
	}
	fc=fb;
	for (iter=1;iter<=ITMAX;iter++) {
		if (fb*fc > 0.0) {
			c=a;
			fc=fa;
			e=d=b-a;
		}
		if (fabs(fc) < fabs(fb)) {
			a=b;
			b=c;
			c=a;
			fa=fb;
			fb=fc;
			fc=fa;
		}
		tol1=2.0*EPS*fabs(b)+0.5*tol;
		xm=0.5*(c-b);
		if (fabs(xm) <= tol1 || fb == 0.0) return b;
		if (fabs(e) >= tol1 && fabs(fa) > fabs(fb)) {
			s=fb/fa;
			if (a == c) {
				p=2.0*xm*s;
				q=1.0-s;
			} else {
				q=fa/fc;
				r=fb/fc;
				p=s*(2.0*xm*q*(q-r)-(b-a)*(r-1.0));
				q=(q-1.0)*(r-1.0)*(s-1.0);
			}
			if (p > 0.0)  q = -q;
			p=fabs(p);
			min1=3.0*xm*q-fabs(tol1*q);
			min2=fabs(e*q);
			if (2.0*p < (min1 < min2 ? min1 : min2)) {
				e=d;
				d=p/q;
			} else {
				d=xm;
				e=d;
			}
		} else {
			d=xm;
			e=d;
		}
		a=b;
		fa=fb;
		if (fabs(d) > tol1)
			b += d;
		else
			b += (xm > 0.0 ? fabs(tol1) : -fabs(tol1));
		fb=(*func)(b);
	}
	nrerror("Maximum number of iterations exceeded in ZBRENT");
	*erflag = 1;
	return 0.0;
}

#undef ITMAX
#undef EPS
