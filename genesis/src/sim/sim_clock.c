static char rcsid[] = "$Id: sim_clock.c,v 1.2 2005/06/27 19:00:58 svitak Exp $";

/*
** $Log: sim_clock.c,v $
** Revision 1.2  2005/06/27 19:00:58  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.9  2001/04/25 17:17:01  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.8  2001/04/18 22:39:38  mhucka
** Miscellaneous portability fixes, mainly for SGI IRIX.
**
** Revision 1.7  2000/09/07 05:17:27  mhucka
** Moved definition of clock_value from sim.h to here.
**
** Revision 1.6  1997/07/18 02:58:25  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.5  1996/10/05 17:21:36  dhb
** Added USECLOCK action
**
 * Revision 1.4  1996/06/24  14:16:40  dhb
 * Changed SetClock() and do_set_clock() to use double rather than
 * float for the clock value.
 *
 * Revision 1.3  1995/01/24  18:39:57  dhb
 * Added argument validity checks to do_set_clock() and SetClock()
 * functions.
 *
 * Revision 1.2  1993/02/15  21:54:16  dhb
 * 1.4 to 2.0 command argument changes.
 *
 * 	do_set_clock (setclock) and do_element_clock (useclock) changed to
 * 	use GENESIS getopt routines.
 *
 * Revision 1.1  1992/10/27  19:42:50  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include "shell_func_ext.h"
#include "sim_ext.h"

short			nactive_clocks;
short			clock_set[NCLOCKS];
short			clock_active[NCLOCKS];
double			clock_value[NCLOCKS];

void CheckClocks()
{
int i;
double simulation_dt;

    simulation_dt = clock_value[0];
    BZERO(clock_active,sizeof(short)*NCLOCKS);
    for(i=0;i<nactive_clocks;i++){
	/* Hack to bypass the divide-by-zero case which seems to be here */
	if ((1.99999999*clock_value[i]) > simulation_dt) {
	if(i == 0 || CheckInterval(clock_value[i],simulation_dt)){
	    clock_active[i] = 1;
	} else {
	    clock_active[i] = 0;
	}
	} else {
	    clock_active[i] = 1;
	}
    }
}

void CheckClocksSet()
{
int i;

    BZERO(clock_set,sizeof(short)*NCLOCKS);
    nactive_clocks = 0;
    for(i=0;i<NCLOCKS;i++){
	if(clock_value[i]>0){
	    clock_set[i] = i;
	    nactive_clocks++;
	}
    }
}

void ShowClocks()
{
int i;
    /*
    ** print all non-zero clock values
    */
    printf("\nACTIVE CLOCKS\n");
    printf("-------------\n");
    for(i=0;i<NCLOCKS;i++){
	if(clock_value[i] != 0) {
	    printf("[%d] : %g\n",
	    i,
	    clock_value[i]);
	}
    }
    printf("\n");
}

void AssignElementClock(element,clock)
Element 	*element;
int		clock;
{
    Action	action;

    /* Abort clock change if USECLOCK action fails */
    action.name = "USECLOCK";
    action.type = USECLOCK;
    action.data = (char*) &clock;
    action.argc = 0;
    action.argv = NULL;
    if (CallActionFunc(element, &action) == 0)
	return;

    /*
    ** clear the old clock value
    */
    element->flags = ((element->flags) & ~CLOCKMASK);
    /*
    ** and assign the new one
    */
    element->flags = ((element->flags) | (clock & CLOCKMASK));
}

void SetClock(n,val)
int n;
double val;
{
    if (n < 0 || n >= NCLOCKS)
      {
	printf("SetClock: bad clock number %d\n", n);
	return;
      }

    clock_value[n] = val;
    CheckClocksSet();
}

void do_set_clock(argc,argv)
int argc;
char **argv;
{
int clock;
double value;

    initopt(argc, argv, "clock value");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return;
      }

    clock = atoi(optargv[1]);
    value = Atod(optargv[2]);

    if (clock < 0 || clock >= NCLOCKS)
      {
	printf("%s: bad clock number %d, clocks are numbered 0 to %d\n",
		argv[0], clock, NCLOCKS);
	return;
      }

    if (value < 0.0)
      {
	printf("%s: bad clock value %g, clock values are >= 0\n",
		argv[0], value);
	return;
      }

    if(IsSilent()<1 && clock_value[clock] != 0.0 &&
    clock_value[clock] != value){
	printf("changing clock [%d] from %g to %g\n",
	clock,
	clock_value[clock],
	value);
    }
    SetClock(clock,value);
}

void do_element_clock(argc,argv)
int argc;
char **argv;
{
int clock;
char *path;
ElementList *list;
int	i;

    initopt(argc, argv, "path clock");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return;
      }

    path = optargv[1];
    clock = atoi(optargv[2]);
    list = WildcardGetElement(path,0);
    for(i=0;i<list->nelements;i++){
	AssignElementClock(list->element[i],clock);
    }
    FreeElementList(list);
}
