static char rcsid[] = "$Id: update.c,v 1.3 2005/07/20 20:01:59 svitak Exp $";

/*
** $Log: update.c,v $
** Revision 1.3  2005/07/20 20:01:59  svitak
** Added standard header files needed by some architectures.
**
** Revision 1.2  2005/06/27 19:00:38  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.4  2001/06/29 21:14:31  mhucka
** Initialized some variables that gcc thought might be used before being
** initialized.
**
** Revision 1.3  2000/09/21 19:30:49  mhucka
** An sprintf format arg in xoCallbackFn() didn't specify "long" as the
** type of the arg.
**
** Revision 1.2  1995/06/16 05:36:12  dhb
** Fixes for exp() overflows from Upi Bhalla.
**
** Upi Bhalla Mt. Sinai June 7 1995. Added checks for the
** exponential operations to avoid arithmetic errors when the
** arguments are too big.
**
** Revision 1.1  1992/12/11  19:03:00  dhb
** Initial revision
**
*/

#include <math.h>
#include "hh_ext.h" 

/*
** Update function for non-linear HH type conductances
*/

#define EXPONENTIAL  	1
#define SIGMOID		2
#define LINOID		3

/* M.Nelson Caltech 8/88 */
void hh_update(segment,hh_state_var,name,method,v,
	  alphaFORM,alphaA,alphaB,alphaV0,
	  betaFORM,betaA,betaB,betaV0)
Segment *segment;
double *hh_state_var;
char   *name;
int method;
register double v;

short alphaFORM;
float alphaA;
float alphaB;
float alphaV0;

short betaFORM;
float betaA;
float betaB;
float betaV0;

{
register double alpha = 0.0, beta = 0.0;
double A,B;
double dt;

    dt = Clockrate(segment);

    if(debug > 1){
	printf("    hh_update: alpha FORM = %d, A= %f B= %f V0 = %f\n",
	    alphaFORM,alphaA,alphaB,alphaV0);
	printf("    hh_update:beta FORM = %d, A= %f B= %f V0 = %f\n",
	    betaFORM,betaA,betaB,betaV0);
    }

     switch(alphaFORM){
     case EXPONENTIAL:
	     /* This is horrible, because it will cause numerical
	     ** errors, if the test is true. However, the alternative
	     ** is a core dump.
	     */
	     if ((alpha = (v-alphaV0)/alphaB) > 100.0)
	     	alpha = alphaA;
	     else 
	     	alpha = alphaA*(exp(alpha));
	 /* alpha = alphaA*exp((v-alphaV0)/alphaB); */
	 break;
     case SIGMOID:
	     if ((alpha = (v-alphaV0)/alphaB) > 100.0)
	     	alpha = 0;
	     else 
	     	alpha = alphaA/(exp(alpha)+1);
	 /* alpha = alphaA/(exp((v-alphaV0)/alphaB)+1); */
	 break;
     case LINOID:
	 if(v == alphaV0) {
	     alpha = alphaA*alphaB;
	 } else {
     	/*
     	alpha = alphaA*(v-alphaV0)/(exp((v-alphaV0)/alphaB)-1);
     	*/
	     if ((alpha = (v-alphaV0)/alphaB) > 100.0)
	     	alpha = 0;
	     else 
	     	alpha = alphaA*(v-alphaV0)/(exp(alpha)-1);
	 }
	 break;
    default:
	 printf("Warning - hh_update has no functional form for alpha\n");
	 break;
     }
     switch(betaFORM){
     case EXPONENTIAL:
	     /* This is horrible, because it will cause numerical
	     ** errors, if the test is true. However, the alternative
	     ** is a core dump.
	     */
	     if ((beta = (v-betaV0)/betaB) > 100.0)
	     	beta = betaA;
	     else 
	     	beta = betaA*(exp(beta));
	 /* beta = betaA*exp((v-betaV0)/betaB); */
	 break;
     case SIGMOID:
	     if ((beta = (v-betaV0)/betaB) > 100.0)
	     	beta = 0;
	     else 
	     	beta = betaA/(exp(beta)+1);
	 /* beta = betaA/(exp((v-betaV0)/betaB)+1); */
	 break;
     case LINOID:
	 if(v == betaV0) {
	     beta = betaA*betaB;
	 } else {
     	/*
     	beta = betaA*(v-betaV0)/(exp((v-betaV0)/betaB)-1);
     	*/
	     if ((beta = (v-betaV0)/betaB) > 100.0)
	     	beta = 0;
	     else 
	     	beta = betaA*(v-betaV0)/(exp(beta)-1);
	 }
	 break;
     default:
	 break;
     }

    A = alpha;
    B = alpha+beta;

    if(debug > 1){
	printf("\t hh_update: ");
	printf("%s %s %d value=%f A=%f B=%f dt=%f \n",
	    segment->name,name,method,*hh_state_var,A,B,dt);
	}
    *hh_state_var = 
    IntegrateMethod(method,segment,*hh_state_var,A,B,dt,name);

}
