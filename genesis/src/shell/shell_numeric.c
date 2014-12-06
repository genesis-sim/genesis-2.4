static char rcsid[] = "$Id: shell_numeric.c,v 1.3 2005/07/20 20:02:03 svitak Exp $";

/*
** $Log: shell_numeric.c,v $
** Revision 1.3  2005/07/20 20:02:03  svitak
** Added standard header files needed by some architectures.
**
** Revision 1.2  2005/06/26 08:25:37  svitak
** Provided explicit types for untyped funtions. Fixed return statements to
** match return type.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.1  1992/12/11 19:04:39  dhb
** Initial revision
**
*/

#include <math.h>
#include "shell_ext.h"

float rangauss(mean,var)
float	mean,var;
{
static int saved = FALSE;
static float gsave;
static float oldmean,oldvar;
float 	fac;
float	r;
float	x,y;

    /*
    ** check for a saved deviate with the same mean and var
    */
    if(saved && (oldmean == mean) && (oldvar == var)){
	saved = FALSE;
	return(gsave);
    } else {
	/*
	** get a random number in the unit circle
	*/
	do {
	    x=frandom(-1,1); 
	    y=frandom(-1,1);
	    r = x*x + y*y;
	} while((r>=1.0) || (r<=0.0));
	fac = sqrt(-2.0*var*log(r)/r);
	/*
	** Box-Muller transformation to get 2 normal deviates
	*/
	gsave = x*fac+mean;		/* save this one */
	oldmean = mean;
	oldvar = var;
	saved = TRUE;
	return(y*fac+mean);		/* return the other */
    } 
}
