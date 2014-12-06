static char rcsid[] = "$Id: rate.c,v 1.2 2005/07/20 20:01:59 svitak Exp $";

/*
** $Log: rate.c,v $
** Revision 1.2  2005/07/20 20:01:59  svitak
** Added standard header files needed by some architectures.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.1  1992/12/11 19:02:59  dhb
** Initial revision
**
*/

#include <math.h>
#include "hh_ext.h" 
#define PRECISION 1e-3

/* M.Wilson Caltech 2/89 */
double GateState(gate,state,E, A1,B1,C1,D1,F1, A2,B2,C2,D2,F2, method,name)
Segment *gate;
double 	state;
double 	E;
float	A1,B1,C1,D1,F1;
float	A2,B2,C2,D2,F2;
int 	method;
char   	*name;
{
double alpha,beta;
double term1,term2;
double epsilon;

    /*
    ** calculate the alpha (open) rate variable
    */
    term1 = A1 + B1*E;
    term2 = exp((E+D1)/F1) + C1;
    if(A1 < 0){
	epsilon = -PRECISION*A1;
    } else {
	epsilon = PRECISION*A1;
    }
    /*
    ** check for singularity
    */
    if(term2 == 0.0){
	/*
	** beware of precision problems
	*/
	if(term1 <= epsilon && term1 >= -epsilon){
	    /*
	    ** use the limit
	    */
	    alpha = -B1*F1/C1;
	} else {
	    alpha = -B1*F1/C1;
	    ErrorMessage("GateState","alpha divide by zero.",gate);
	}
    } else {
	alpha = term1/term2;
    }
    /*
    ** calculate the beta (close) rate variable
    */
    term1 = A2 + B2*E;
    term2 = exp((E+D2)/F2) + C2;
    /*
    ** check for singularity
    */
    if(term2 == 0.0){
	if(term1 <= epsilon && term1 >= -epsilon){
	    beta = -B2*F2/C2;
	} else {
	    beta = -B2*F2/C2;
	    ErrorMessage("GateState","beta divide by zero.",gate);
	}
    } else {
	beta = term1/term2;
    }

    return(IntegrateMethod(method,gate,state,alpha,alpha+beta,
    Clockrate(gate),name));
}

/* M.Wilson Caltech 2/89 */
double GateSteadyState(gate,E, A1,B1,C1,D1,F1, A2,B2,C2,D2,F2)
Segment	*gate;
double 	E;
float	A1,B1,C1,D1,F1;
float	A2,B2,C2,D2,F2;
{
double alpha,beta;
double term1,term2;
double epsilon;

    /*
    ** calculate the alpha (open) rate variable
    */
    term1 = A1 + B1*E;
    term2 = exp((E+D1)/F1) + C1;
    if(A1 < 0){
	epsilon = -PRECISION*A1;
    } else {
	epsilon = PRECISION*A1;
    }
    /*
    ** check for singularity
    */
    if(term2 == 0.0){
	if(term1 <= epsilon && term1 >= -epsilon){
	    /*
	    ** use the limit
	    */
	    alpha = -B1*F1/C1;
	} else {
	    alpha = -B1*F1/C1;
	    ErrorMessage("GateSteadyState","alpha divide by zero.",gate);
	}
    } else {
	alpha = term1/term2;
    }
    /*
    ** calculate the beta (close) rate variable
    */
    term1 = A2 + B2*E;
    term2 = exp((E+D2)/F2) + C2;
    /*
    ** check for singularity
    */
    if(term2 == 0.0){
	if(term1 <= epsilon && term1 >= -epsilon){
	    /*
	    ** use the limit
	    */
	    beta = -B2*F2/C2;
	} else {
	    beta = -B2*F2/C2;
	    ErrorMessage("GateSteadyState","alpha divide by zero.",gate);
	}
    } else {
	beta = term1/term2;
    }
    return(alpha/(alpha+beta));
}

/* M.Wilson Caltech 2/89 */
double RateState(gate,E, A1,B1,C1,D1,F1)
Segment	*gate;
double 	E;
float	A1,B1,C1,D1,F1;
{
double alpha;
double term1,term2;
double epsilon;

    /*
    ** calculate the alpha (open) rate variable
    */
    term1 = A1 + B1*E;
    term2 = exp((E+D1)/F1) + C1;
    if(A1 < 0){
	epsilon = -PRECISION*A1;
    } else {
	epsilon = PRECISION*A1;
    }
    /*
    ** check for singularity
    */
    if(term2 == 0.0){
	if(term1 <= epsilon && term1 >= -epsilon){
	    /*
	    ** use the limit
	    */
	    alpha = -B1*F1/C1;
	} else {
	    alpha = -B1*F1/C1;
	    ErrorMessage("RateState","alpha divide by zero.",gate);
	}
    } else {
	alpha = term1/term2;
    }
    return(alpha);
}
