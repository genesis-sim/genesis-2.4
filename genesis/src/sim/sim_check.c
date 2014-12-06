static char rcsid[] = "$Id: sim_check.c,v 1.3 2005/10/04 22:05:13 svitak Exp $";

/*
** $Log: sim_check.c,v $
** Revision 1.3  2005/10/04 22:05:13  svitak
** Merged branch tagged rel-2-3beta-fixes back to main trunk. All comments from
** the branch modifications should be included.
**
** Revision 1.2.2.1  2005/08/13 05:22:41  svitak
** Fixed extern decl of CHECK_Simulate() to match definition.
**
** Revision 1.2  2005/06/27 19:00:58  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.6  2001/04/25 17:17:01  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.5  1997/06/12 23:24:36  dhb
** Changed simulation_dt type from float to double to avoid
** roundoff errors in float to double compares when checking
** element clocks against the simulation dt.
**
** Revision 1.4  1994/06/15 20:58:46  dhb
** Moved checking of projections into the projection CHECK actions.
** (Now in axonlib.)
**
 * Revision 1.3  1993/12/23  02:23:54  dhb
 * Changed direct call to element action function to use CallActionFunc()
 * in order to support extended objects.
 *
 * Revision 1.2  1993/02/16  22:27:21  dhb
 * 1.4 to 2.0 command argument changes.
 *
 * 	CheckSimulation (check) required no changes.
 *
 * Fixed type error in rcsid variable declaration.
 *
 * Revision 1.1  1992/10/27  19:41:41  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include "sim_ext.h"
#include "shell_func_ext.h"

/*
** check the consistency of the time intervals for the 
** buffers projections and segments
*/
void CheckSimulation()
{
Element			*element;
ElementStack		*stk;
double 			simulation_dt;
Action			*action;
extern void              CHECK_Simulate();

    Reschedule();
    simulation_dt = ClockValue(0);
    if(simulation_dt <= 0.0){
	Error();
	printf("Invalid simulation step size of %f\n",simulation_dt);
    }
    /*
    ** go through each enabled element
    */
    stk = NewPutElementStack(RootElement());
    /*
    ** scan all elements except those that are disabled
    */
    action = GetAction("CHECK");
    while ((element = NewFastNextElement(1,stk))) {
	if(Clockrate(element) < simulation_dt){
	    Error();
	    printf("the interval for '%s' (%f) ",
	    Pathname(element),
	    Clockrate(element));
	    printf("is less than the simulation dt (%f)\n",
	    simulation_dt);
	}
	/*
	** does the object have a check action?
	*/
	CallActionFunc(element, action);
    }
    NewFreeElementStack(stk);
    CHECK_Simulate();
} 
