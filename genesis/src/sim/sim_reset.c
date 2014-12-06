static char rcsid[] = "$Id: sim_reset.c,v 1.2 2005/06/27 19:01:09 svitak Exp $";

/*
** $Log: sim_reset.c,v $
** Revision 1.2  2005/06/27 19:01:09  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.6  2001/04/25 17:17:03  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.5  1997/07/18 02:58:25  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.4  1994/09/16 20:27:00  dhb
** Moved RESET code for projection connections into projection RESET
** action.
**
** The ResetElements() function which appears to be old code, also performed
** the same function.  Removed it there too.  Actually, the code here was in
** error, calling the CHECK action instead of RESET.
**
 * Revision 1.3  1993/06/29  18:53:23  dhb
 * Extended objects (incomplete)
 *
 * Revision 1.2  1993/02/12  23:32:11  dhb
 * 1.4 to 2.0 command arg changes:
 * 	do_reset (reset) no changes necessary
 * 	do_reset_simstate (not addfunced) no changes
 * 	do_reset_elements (not addfunced, but looks like a way to call
 * 		the RESET action for a set of elements) added calls to
 * 		getopt routines
 *
 * Revision 1.1  1992/10/27  20:25:39  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include "shell_func_ext.h"
#include "sim_ext.h"

void SimReset()
{
PFI			func;
Element			*element;
ElementStack		*stk;
Action			*action;

    SetCurrentStep(0);
    simulation_time = 0;
    StepStatus();
    SetBreakFlag();
    ResetErrors();
    Reschedule();
/*
    CheckSimulation();
*/
    ResetTmin();

    action = GetAction("RESET");

    /*
    ** reset each element
    */
    stk = NewPutElementStack(RootElement());
    while((element=NewFastNextElement(3,stk)) != NULL){
	/*
	** get the element reset function
	*/
	if((func = GetActionFunc(element->object,RESET,NULL,NULL)) != NULL){
	    if (!IsHsolved(element)) {
			CallActionFunc(element, action);
		}
	}
    }
    NewFreeElementStack(stk);
}

void ResetTmin()
{
ElementStack	*stk;
Element	*element;

    stk = NewPutElementStack(RootElement());
    /*
    ** go through all elements 
	**
	** Bugfix by Upi Bhalla 16 Jul 1991 :
	** flag here is set to 0 to cause proper
	** reset for passive buffers involved in higher order integ methods
    */
    while ((element = NewNextElement(2,BUFFER_ELEMENT,stk))) {
	/*
	** clear the tmin of the buffers on this element
	*/
	((Buffer *)element)->tmin = 0;
	/*
	** Upi Bhalla 18 Jul 1991
	** Another bugfix : completely reset the passive buffers.
	*/
	if (strcmp(element->object->name,"passive_buffer") == 0)
		ClearBuffer(element);
    }
    NewFreeElementStack(stk);
}

void ResetSimState()
{
    SetCurrentStep(0);
    simulation_time = 0;
    StepStatus();
    SetBreakFlag();
    ResetErrors();
    Reschedule();
    ResetTmin();

}

void ResetElements(list)
ElementList *list;
{
PFI			func;
Element			*element;
Action			*action;
int			i;

    action = GetAction("RESET");

    /*
    ** reset each element
    */
    for(i=0;i<list->nelements;i++){
	element = list->element[i];
	/*
	** get the element reset function
	*/
	if((func = GetActionFunc(element->object,RESET,NULL,NULL)) != NULL){
	    CallActionFunc(element, action);
	}
    }
}

void do_reset()
{
    SimReset(); 
}

void do_reset_simstate()
{ 
    ResetSimState(); 
}

void do_reset_elements(argc,argv) 
int argc;
char **argv;
{ 
ElementList *list;

    initopt(argc, argv, "path");
    if (G_getopt(argc, argv) < 0)
      {
	printoptusage(argc, argv);
	return;
      }

    list = WildcardGetElement(optargv[1],1);
    ResetElements(list);
    FreeElementList(list);
}
