static char rcsid[] = "$Id: sim_intgrt.c,v 1.3 2005/07/20 20:02:03 svitak Exp $";

/*
** $Log: sim_intgrt.c,v $
** Revision 1.3  2005/07/20 20:02:03  svitak
** Added standard header files needed by some architectures.
**
** Revision 1.2  2005/06/27 19:01:07  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.6  2001/04/25 17:17:02  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.4  2000/06/12 04:53:31  mhucka
** Fixed trivial errors in some printf format strings.
**
** Revision 1.3  1997/07/29 00:19:01  dhb
** Fixed calls to AB2() and AB3() with old buffer header address
** still being passed.
**
** Revision 1.2  1993/07/21 21:31:57  dhb
** fixed rcsid variable type
**
 * Revision 1.1  1992/10/27  20:09:35  dhb
 * Initial revision
 *
*/

/******************************************************************
**                                                               **   
**                 By Matt Wilson, 1987, Caltech                 **   	
**                                                               **   
**          Bugfixes by Bruce P. Graham, 22-5-91, JCSMR,ANU      **   
**                                                               **   
******************************************************************/
#include <math.h>
#include <stdio.h>
#include "shell_func_ext.h"
#include "sim_ext.h"

#define Trapezoid(y,A,B,dt) \
	(((y)*(1-(dt)*(B)/2.0)+(dt)*(A))/(1+(dt)*(B)/2.0))

#define INTEG_AB2() \
	value = y + ( \
	1.5 *dy \
	- 0.5 *PreviousEvent(buffer,0)->magnitude \
	) * dt

#define INTEG_AB3() \
	value = y + ( \
	23.0 *dy  \
	- 16.0 *PreviousEvent(buffer,0)->magnitude \
	+ 5.0 *PreviousEvent(buffer,1)->magnitude \
	) * dt/12.0

#define INTEG_AB4() \
	value = y + ( \
	55.0 *dy  \
	- 59.0 *PreviousEvent(buffer,0)->magnitude \
	+ 37.0 *PreviousEvent(buffer,1)->magnitude \
	- 9.0 *PreviousEvent(buffer,2)->magnitude \
	) * dt/24.0

#define INTEG_AB5() \
	value = y + ( \
	1901.0 *dy  \
	- 2774.0 *PreviousEvent(buffer,0)->magnitude \
	+ 2616.0 *PreviousEvent(buffer,1)->magnitude \
	- 1274.0 *PreviousEvent(buffer,2)->magnitude \
	+ 251.0 *PreviousEvent(buffer,3)->magnitude \
	) * dt/720.0

double DirectIntegrate(method,segment,state,dy,dt,name)
int	method;
Segment	*segment;
double	state;
double	dy;
double	dt;
char *name;
{
double result = 0;

    switch(method){
	case EEULER_INT:
	case GEAR_INT:
	case TRAPEZOIDAL_INT:
	default:
	    Error();
	    printf("integration method %d unavailable\n",method);
	    result = 0;
	    break;
	case AB2_INT:
	    /*
	    ** Adams-Bashforth 2 step
	    */
	    /* Attach buffer below segment, instead of to parent
	       (as per BPG 15-5-91)
	    result = AB2(segment->parent,&(segment->child),
	    */
	    result = AB2(segment->parent,
			    state,dy,dt,
			    name);
	    break;
	case AB3_INT:
	    /*
	    ** Adams-Bashforth 3 step
	    */
	    /* Attach buffer below segment, instead of to parent
	       (as per BPG 15-5-91)
	    result = AB3(segment->parent,&(segment->child),
	    */
	    result = AB3(segment->parent,
			    state,dy,dt,
			    name);
	    break;
	case FEULER_INT:
	    /*
	    ** Euler: a definite must
	    */
	    result = Euler(state,dy,dt);
	    break;
    }
    return(result);
}

double IntegrateMethod(method,segment,state,A,B,dt,name)
int	method;
Segment	*segment;
double	state;
double	A;
double	B;
double	dt;
char *name;
{
double D;
double result;

    switch(method){
	case EEULER_INT:
	    /*
	    ** the default is the exponential integration form
	    */
	    D = exp(-B*dt);
	    result = state*D + (A/B)*(1-D);
	    break;
	case GEAR_INT:
	    /*
	    ** Gear 2nd order
	    */
	    /* Attach buffer below segment, instead of to parent (BPG 15-5-91)
	    result = Gear(segment->parent,&(segment->child),*/
	    result = Gear(segment,
			    state,A,B,dt,name);
	    break;
	case AB2_INT:
	    /*
	    ** Adams-Bashforth 2 step
	    */
	    /* Attach buffer below segment, instead of to parent (BPG 15-5-91)
	    result = AB2(segment->parent,&(segment->child),*/
	    result = AB2(segment, state,A - state*B,dt, name);
	    break;
	case AB3_INT:
	    /*
	    ** Adams-Bashforth 3 step
	    */
	    /* Attach buffer below segment, instead of to parent (BPG 15-5-91)
	    result = AB3(segment->parent,&(segment->child),*/
	    result = AB3(segment, state,A - state*B,dt, name);
	    break;
	case TRAPEZOIDAL_INT:
	    /*
	    ** Trapezoidal
	    */
	    result = Trapezoid(state,A,B,dt);
	    break;
	/* Why not include the 4 and 5 step methods ? (BPG 22-5-91) */
	case AB4_INT:
	    /*
	    ** Adams-Bashforth 4 step
	    */
	    /* Attach buffer below segment, instead of to parent (BPG 15-5-91)
	    result = AB4(segment->parent,&(segment->child),*/
	    result = AB4(segment, state,A - state*B,dt, name);
	    break;
	case AB5_INT:
	    /*
	    ** Adams-Bashforth 5 step
	    */
	    /* Attach buffer below segment, instead of to parent (BPG 15-5-91)
	    result = AB5(segment->parent,&(segment->child),*/
	    result = AB5(segment, state,A - state*B,dt, name);
	    break;
	/* The oldest and greatest ! (BPG 24-5-91) */
	case RK_INT:
	    /*
	    ** Runge-Kutta 4 step
	    */
	    result = Runge_Kutta(state,A,B,dt);
	    break;
	case FEULER_INT:
	    /*
	    ** Euler: a definite must
	    */
	    result = Euler(state,A - state*B,dt);
	    break;
	/* A predictor-corrector method (BPG 22-5-91) */
	case EPC_INT:
	    /*
	    ** Euler predictor - modified Euler corrector
		(from Parnas and Segev, J.Physiol. (1979) 295:323-343)
	    */
	    result = Euler_Predictor_Corrector(state,A,B,dt);
	    break;
	default :
		/* 
		** default to good old Exp Euler
		*/
	    D = exp(-B*dt);
	    result = state*D + (A/B)*(1-D);
		break;
    }
    return(result);
}


#define NSTEPS 2

double Gear(element,y,alpha,beta,dt,y_buffername)
Element *element;
double y;
double alpha;
double beta;
double dt;
char *y_buffername;
{
Buffer	*buffer;
double		value;
Element *FindElement();

/*
** Completely empirical bugfix by Upi Bhalla, Caltech, 7 Jul 1991
*/
	dt *= 1.5;

    /*
    ** try to get the buffer by name
    */
    /* look for buffer amongst the children (BPG 15-5-91)
    if((buffer = (Buffer *)FindElement(element,y_buffername)) == NULL){*/
    if((buffer = (Buffer *)FindElement(element->child,y_buffername)) == NULL){
	/*
	** if it fails then create the buffer needed
	*/
	buffer=(Buffer *)Create("passive_buffer",y_buffername,element,NULL,0);
	Block(buffer);
    }
    /*
    ** check the size to make sure it is the correct size
    */
    if(buffer->size < NSTEPS){
	/*
	** if it is too small then expand it
	*/
	ExpandBuffer(buffer, NSTEPS - buffer->size); 
    }
    /*
    ** check the buffer to make sure it is full before trying
    ** to get events from it
    */
    if(PreviousEvent(buffer,0)){
	/*
	** do the 2nd order Gear
	*/
	value = (4.0*y - PreviousEvent(buffer,0)->magnitude + 2.0*dt*alpha) /
		(3.0*(1 + 2.0*dt*beta/3.0));

    } else {
	/*
	** use Trapezoidal to start up
	*/
	value = Trapezoid(y,alpha,beta,dt);
    }
    /*
    ** add the current state to the buffer
    */
    /*make compatible with actual function (BPG 15-5-91) 
    PutEvent(buffer, value, NULL, WRAP);*/
    PutEvent(buffer, value, WRAP);
    return(value);
}

#undef NSTEPS

#define NSTEPS 2

/* buffer_head redundant (BPG 22-5-91)
double AB2(element,buffer_head,y,dy,dt,dy_buffername)*/
double AB2(element,y,dy,dt,dy_buffername)
Element *element;
/*Buffer **buffer_head;	(BPG 22-5-91) */
double y;
double dy;
double dt;
char *dy_buffername;
{
Buffer	*buffer;
double		value;
Element *FindElement();

    /*
    ** try to get the buffer by name
    */
    /* look for buffer amongst the children (BPG 15-5-91)
    if((buffer = (Buffer *)FindElement(element,dy_buffername)) == NULL){*/
    if((buffer = (Buffer *)FindElement(element->child,dy_buffername)) == NULL){
	/*
	** if it fails then create the buffer needed
	*/
	/* some debug messages (BPG 15-5-91) */
	if (debug > 1) {
	  printf("IntegrateMethod: Adams-Bashforth 2 step: ");
	  printf("creating buffer\n");
	}
	buffer=(Buffer *)Create("passive_buffer",dy_buffername,element,NULL,0);
	Block(buffer);
    }
    /*
    ** check the size to make sure it is the correct size
    */
    if(buffer->size < NSTEPS){
	/*
	** if it is too small then expand it
	*/
	ExpandBuffer(buffer, NSTEPS - buffer->size); 
    }
    /*
    ** check the buffer to make sure it is full before trying
    ** to get events from it
    */
    if(PreviousEvent(buffer,0)){
	/*
	** do the 2 step Adams Bashforth integration
	*/
	INTEG_AB2();
    } else {
	/*
	** use Eulers to start up
	*/
	value = Euler(y,dy,dt);
    }
    /*
    ** add the current derivative to the buffer
    */
    /*make compatible with actual function (BPG 15-5-91) 
    PutEvent(buffer, dy, NULL, WRAP);*/
    PutEvent(buffer, dy, WRAP);
    return(value);
}

#undef NSTEPS

#define NSTEPS 3

/* buffer_head redundant (BPG 22-5-91)
double AB3(element,buffer_head,y,dy,dt,dy_buffername)*/
double AB3(element,y,dy,dt,dy_buffername)
Element *element;
/*Buffer **buffer_head;	(BPG 22-5-91) */
double y;
double dy;
double dt;
char *dy_buffername;
{
Buffer	*buffer;
double		value;
Element *FindElement();

    /*
    ** try to get the buffer by name
    */
    /* look for buffer amongst the children (BPG 15-5-91)
    if((buffer = (Buffer *)FindElement(element,dy_buffername)) == NULL){*/
    if((buffer = (Buffer *)FindElement(element->child,dy_buffername)) == NULL){
	/*
	** if it fails then create the buffer needed
	*/
	buffer=(Buffer *)Create("passive_buffer",dy_buffername,element,NULL,0);
	Block(buffer);
    }
    /*
    ** check the size to make sure it is the correct size
    */
    if(buffer->size < NSTEPS){
	/*
	** if it is too small then expand it
	*/
	ExpandBuffer(buffer, NSTEPS - buffer->size); 
    }
    /*
    ** check the buffer to make sure it is full before trying
    ** to get events from it
    */
    if(PreviousEvent(buffer,1)){
	/*
	** do the 3 step Adams Bashforth integration
	*/
	INTEG_AB3();
    } else 
    /*
    ** try the next step down
    */
    if(PreviousEvent(buffer,0)){
	/*
	** do the 2 step Adams Bashforth integration
	*/
	INTEG_AB2();
    } else {
	/*
	** use Eulers to start up
	*/
	value = Euler(y,dy,dt);
    }
    /*
    ** add the current derivative to the buffer
    */
    /*make compatible with actual function (BPG 15-5-91) 
    PutEvent(buffer, dy, NULL, WRAP);*/
    PutEvent(buffer, dy, WRAP);
    return(value);
}

#undef NSTEPS

#define NSTEPS 4

/* buffer_head redundant (BPG 22-5-91)
double AB4(element,buffer_head,y,dy,dt,dy_buffername)*/
double AB4(element,y,dy,dt,dy_buffername)
Element *element;
/*Buffer **buffer_head;	(BPG 22-5-91) */
double y;
double dy;
double dt;
char *dy_buffername;
{
Buffer	*buffer;
double		value;
Element *FindElement();

    /*
    ** try to get the buffer by name
    */
    /* look for buffer amongst the children (BPG 15-5-91)
    if((buffer = (Buffer *)FindElement(element,dy_buffername)) == NULL){*/
    if((buffer = (Buffer *)FindElement(element->child,dy_buffername)) == NULL){
	/*
	** if it fails then create the buffer needed
	*/
	buffer=(Buffer *)Create("passive_buffer",dy_buffername,element,NULL,0);
	Block(buffer);
    }
    /*
    ** check the size to make sure it is the correct size
    */
    if(buffer->size < NSTEPS){
	/*
	** if it is too small then expand it
	*/
	ExpandBuffer(buffer, NSTEPS - buffer->size); 
    }
    /*
    ** check the buffer to make sure it is full before trying
    ** to get events from it
    */
    if(PreviousEvent(buffer,2)){
	/*
	** do the 4 step Adams Bashforth integration
	*/
	INTEG_AB4();
    } else
    /*
    ** try the next step down
    */
    if(PreviousEvent(buffer,1)){
	/*
	** do the 3 step Adams Bashforth integration
	*/
	INTEG_AB3();
    } else 
    if(PreviousEvent(buffer,0)){
	/*
	** do the 2 step Adams Bashforth integration
	*/
	INTEG_AB2();
    } else {
	/*
	** use Eulers to start up
	*/
	value = Euler(y,dy,dt);
    }
    /*
    ** add the current derivative to the buffer
    */
    /*make compatible with actual function (BPG 15-5-91) 
    PutEvent(buffer, dy, NULL, WRAP);*/
    PutEvent(buffer, dy, WRAP);
    return(value);
}

#undef NSTEPS

#define NSTEPS 5

/* buffer_head redundant (BPG 22-5-91)
double AB5(element,buffer_head,y,dy,dt,dy_buffername)*/
double AB5(element,y,dy,dt,dy_buffername)
Element *element;
/*Buffer **buffer_head;	(BPG 22-5-91) */
double y;
double dy;
double dt;
char *dy_buffername;
{
Buffer	*buffer;
double		value;
Element *FindElement();

    /*
    ** try to get the buffer by name
    */
    /* look for buffer amongst the children (BPG 15-5-91)
    if((buffer = (Buffer *)FindElement(element,dy_buffername)) == NULL){*/
    if((buffer = (Buffer *)FindElement(element->child,dy_buffername)) == NULL){
	/*
	** if it fails then create the buffer needed
	*/
	buffer=(Buffer *)Create("passive_buffer",dy_buffername,element,NULL,0);
	Block(buffer);
    }
    /*
    ** check the size to make sure it is the correct size
    */
    if(buffer->size < NSTEPS){
	/*
	** if it is too small then expand it
	*/
	ExpandBuffer(buffer, NSTEPS - buffer->size); 
    }
    /*
    ** check the buffer to make sure it is full before trying
    ** to get events from it
    */
    if(PreviousEvent(buffer,3)){
	/*
	** do the 5 step Adams Bashforth integration
	*/
	INTEG_AB5();
    } else 
    /*
    ** try the next step down
    */
    if(PreviousEvent(buffer,2)){
	/*
	** do the 4 step Adams Bashforth integration
	*/
	INTEG_AB4();
    } else 
    if(PreviousEvent(buffer,1)){
	/*
	** do the 3 step Adams Bashforth integration
	*/
	INTEG_AB3();
    } else 
    if(PreviousEvent(buffer,0)){
	/*
	** do the 2 step Adams Bashforth integration
	*/
	INTEG_AB2();
    } else {
	/*
	** use Eulers to start up
	*/
	value = Euler(y,dy,dt);
    }
    /*
    ** add the current derivative to the buffer
    */
    /*make compatible with actual function (BPG 15-5-91) 
    PutEvent(buffer, dy, NULL, WRAP);*/
    PutEvent(buffer, dy, WRAP);
    return(value);
}

#undef NSTEPS



/* A predictor - corrector method (BPG 22-5-91) 
   Predictor: Euler's method
   Corrector: modified Euler's method
   (doesn't yet work)
*/
#define EULER_EPS 1e-06

double Euler_Predictor_Corrector(y,A,B,dt)
double y;
double A;
double B;
double dt;
{
double value;
double new_value;
double dy;

    /*
    ** Predictor - Euler's method
    */
    dy = A - B * dt;	/* current derivative */
    new_value = Euler(y,dy,dt);	/* new y value */

    /*
    ** Corrector - modified Euler's method
    */
    do {
      value = new_value;
      new_value = Euler(y,(dy+A-B*value)/2,dt);	/* modified Euler */
    } while (fabs((new_value-value)/new_value) >= EULER_EPS);

    return(new_value);

}



/* Runge-Kutta 4-step method (BPG 22-5-91) 
*/
double Runge_Kutta(y,A,B,dt)
double y;
double A;
double B;
double dt;
{
    double k1, k2, k3, k4, value;

    k1 = (A - B*y) * dt;
    k2 = (A - B*(y + k1/2)) * dt;
    k3 = (A - B*(y + k2/2)) * dt;
    k4 = (A - B*(y + k3)) * dt;

    value = y + (k1 + 2*k2 + 2*k3 + k4) / 6;

    return(value);

}
