static char rcsid[] = "$Id: evaluate.c,v 1.3 2005/07/20 20:01:59 svitak Exp $";

/*
** $Log: evaluate.c,v $
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
** Revision 1.1  1992/12/11 19:02:57  dhb
** Initial revision
**
*/

#include <math.h>
#include "hh_ext.h" 

/*
** Evaluate steady-state value for HH state variable at v
*/

#define EXPONENTIAL  	1
#define SIGMOID		2
#define LINOID		3

/* M.Nelson Caltech 8/88 */
void hh_evaluate(hh_state_var,v,
	  alphaFORM,alphaA,alphaB,alphaV0,
	  betaFORM,betaA,betaB,betaV0)
double *hh_state_var;
double v;

short alphaFORM;
float alphaA;
float alphaB;
float alphaV0;

short betaFORM;
float betaA;
float betaB;
float betaV0;

{
    double alpha = 0.0,beta = 0.0;

    if(debug > 1){
	printf("    hh_evaluate: alpha FORM = %d, A= %f B= %f V0 = %f\n",
	    alphaFORM,alphaA,alphaB,alphaV0);
	printf("    hh_evaluate:beta FORM = %d, A= %f B= %f V0 = %f\n",
	    betaFORM,betaA,betaB,betaV0);
    }

     switch(alphaFORM){
     case EXPONENTIAL:
	 alpha = alphaA*exp((v-alphaV0)/alphaB);
	 break;
     case SIGMOID:
	 alpha = alphaA/(exp((v-alphaV0)/alphaB)+1);
	 break;
     case LINOID:
	 if(v == alphaV0)
	     alpha = alphaA*alphaB;
	 else
	     alpha = alphaA*(v-alphaV0)/(exp((v-alphaV0)/alphaB)-1);
	 break;
    default:
	 printf("Warning - hh_update has no functional form for alpha\n");
	 break;
     }
     switch(betaFORM){
     case EXPONENTIAL:
	 beta = betaA*exp((v-betaV0)/betaB);
	 break;
     case SIGMOID:
	 beta = betaA/(exp((v-betaV0)/betaB)+1);
	 break;
     case LINOID:
	 if(v == betaV0)
	     beta = betaA*betaB;
	 else
	     beta = betaA*(v-betaV0)/(exp((v-betaV0)/betaB)-1);
	 break;
     Default:
	 break;
     }

    *hh_state_var = alpha/(alpha+beta);
}
