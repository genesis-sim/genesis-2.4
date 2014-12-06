static char rcsid[] = "$Id: sim_base.c,v 1.4 2005/07/01 10:03:08 svitak Exp $";

/*
** $Log: sim_base.c,v $
** Revision 1.4  2005/07/01 10:03:08  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.3  2005/06/27 19:00:57  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.2  2005/06/18 19:20:03  svitak
** Added Darwin to list of architectures requiring a.out.h.
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.10  2001/04/25 17:17:01  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.9  2000/09/21 19:38:58  mhucka
** Added long specifiers where needed in printf format strings.
**
** Revision 1.8  1997/07/22 17:31:34  venkat
** SimStartup() modified to initialize the command callback list and
** subsequently add the quitCallback() (defined in sim_quit.c) to it.
** This callback is invoked when the quit/exit command is executed
**
** Revision 1.7  1996/05/09 18:49:51  dhb
** Added call to initialize the element hash table.
**
 * Revision 1.6  1995/07/21  19:36:37  dhb
 * Hpux has problems including a.out.h.  Added hpux to Solaris
 * conditional exclusion of this inclusion :^)
 *
 * Revision 1.5  1995/02/17  22:11:59  dhb
 * Added call to AddCleanup() in SimStartup() so that the ElementReaper()
 * function is called to free previously deleted elements.
 *
 * Revision 1.4  1994/03/20  19:29:20  dhb
 * Changes for C code startups.  Added call to STARTUP_basic() in
 * SimStartup().  NOTE: I left in the FUNC_basic() call since other
 * setup stuff probably needs the symbol table info.  STARTUP_basic()
 * must follow these because it may depend on them.
 *
 * Revision 1.3  1993/09/17  18:20:59  dhb
 * Solaris compatability.
 *
 * Revision 1.2  1993/07/21  21:31:57  dhb
 * fixed rcsid variable type
 *
 * Revision 1.1  1992/10/27  19:32:56  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#if !defined(Solaris) && !defined(hpux) && !defined(Darwin)
#include <a.out.h>
#endif
#include "sim_ext.h"
#include "profile.h"
#include "shell_func_ext.h"

static int sim_flags = 0;
static Element *root_element;

void OK()
{
    if(CurrentScriptFp() == stdin && IsSilent()<1){
	printf("OK\n");
    }
}

void FAILED()
{
    if(CurrentScriptFp() == stdin && IsSilent()<1){
	printf("FAILED\n");
    }
}

void SetSimFlags(argc,argv)
int argc;
char **argv;
{
int bit;
int shift;

    if(argc < 2){
	printf("usage: %s bit# [on/off]\n",argv[0]);
	return;
    }
    shift = atoi(argv[1]);
    bit = 1 << shift;
    if(strcmp(argv[2],"on") == 0){
	sim_flags |= bit;
    } else 
    if(strcmp(argv[2],"off") == 0){
	sim_flags &= ~bit;
    } else
	printf("usage: %s bit# [on/off]\n",argv[0]);
}

void ValidElements()
{
    printf(" Valid elements are :\n");
}


void catchsig(sig)
int	sig;
{
    switch(sig){
	case SIGINT: 
	    SetBreakFlag();
	break;
    }
#ifdef MASSCOMP
    /*
    ** reset the signal interrupt handler
    */
    signal(SIGINT,catchsig);
#endif
}

void net_abort (sig)
int     sig;
{
    fprintf (stderr, "\nsignal %d received by %d ...\n", sig, getpid ());
    SetBreakFlag();
}


void DisplayProfile()
{
int	i;
    printf("\n");
    for(i=0;i<17;i++)
	printf("%3d : %-30s : %8.4f sec : %7.3f %% \n",
	i,GetPname(i),GetProfile(i),100*GetProfile(i)/GetProfile(0));
    printf("\n");
}

void define_region(reg,type,xl,yl,xh,yh)
struct region_type *reg;
short	type;
int xl,yl,xh,yh;
{
	reg->type = type; 
	reg->xl = xl;
	reg->yl = yl;
	reg->xh = xh;
	reg->yh = yh;
}

/*
** return the current time and date obtained from the system clock
*/
char	*date()
{
long   clock;
long   time ();
char   *ctime ();

    time (&clock);
    return(ctime(&clock));
}

Element *RootElement()
{
    return(root_element);
}

void StartupElements()
{
Element *element;

    /*
    ** setup the root element
    */
    root_element = Create("neutral","Root",NULL,NULL,0);
    Enable(root_element);
    SetWorkingElement(root_element);
    SetRecentElement(root_element);
    SetRecentConnection(NULL);

    /*
    ** set up the prototype element on the root
    */
    element = Create("neutral","proto",root_element,NULL,0);
    Block(element);
    /*
    ** set up the output element on the root
    */
    Create("neutral","output",root_element,NULL,0);

    SetClock(0,1.0);
}

void SimStartup()
{
extern int ElementReaper();
extern void InitJobs();
extern void InitSchedule();
extern void BasicActions();
extern void  STARTUP_basic();
extern void  DATA_basic();
extern void  FUNC_basic();

/* Callbacks that could execute on certain commands like quit */
extern void InitCommandCallbacks();
extern void AddCommandCallback();


    /*
    ** initialize the simulator memory allocator
    */
    init_smalloc();
    /*
    ** load in the basic symbol table information
    */
    DATA_basic();
    FUNC_basic();

    /*
    ** initialize the lists
    */
    InitJobs();
    InitSchedule();
    InitCommandCallbacks();

   /* 
	Set up command callbacks here by adding them 
	to the initialized command callbacks list
   */

    AddCommandCallback(quitCallback);

    /*
    ** load in the basic classes
    */
    ClassHashInit();
    BasicClasses();

    /*
    ** load in the basic actions
    */
    ActionHashInit();
    BasicActions();

    /*
    ** load in the basic objects
    */
    ObjectHashInit();
    BasicObjects();

    ElementHashInit();
    StartupElements();

    /*
    ** run the startup code
    */

    STARTUP_basic();

    /*
    ** Set up cleanup function to reap deleted elements
    */

    AddCleanup(ElementReaper);

}

