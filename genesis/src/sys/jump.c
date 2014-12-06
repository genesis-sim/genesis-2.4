static char rcsid[] = "$Id: jump.c,v 1.4 2005/10/04 22:05:13 svitak Exp $";

/*
** $Log: jump.c,v $
** Revision 1.4  2005/10/04 22:05:13  svitak
** Merged branch tagged rel-2-3beta-fixes back to main trunk. All comments from
** the branch modifications should be included.
**
** Revision 1.3.2.1  2005/08/13 05:23:27  svitak
** Fixed extern decl of sig_msg to match definition.
**
** Revision 1.3  2005/07/07 19:15:14  svitak
** Changed return type of TraceScript from int to void. These calls are within
** #ifdefs and will probably need to be removed if and when those statements are
** reached.
**
** Revision 1.2  2005/06/17 20:19:35  svitak
** Added Cygwin to the list of systems that need struct sigcontext
** defined.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.13  2001/04/25 17:17:04  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.11  2000/09/21 19:47:16  mhucka
** Added declaration for TraceScript().
**
** Revision 1.10  2000/07/12 06:16:25  mhucka
** Added #include of strings.h and removed some unusued variables
** in sig_msg_restore_context().
**
** Revision 1.9  1999/12/31 08:31:40  mhucka
** Added return type declarations.
**
** Revision 1.8  1998/07/21 22:16:59  dhb
** Fixed reference to __GLIBC__ (was looking at __GLIBC).
**
** Revision 1.7  1997/08/08 19:26:29  dhb
** Added check for __GLIBC__ (the new glibc c library) and define
** a sigcontext structure for this case.
**
** Revision 1.6  1996/06/06 20:53:55  dhb
** merged in 1.4.1.1 changes.
**
** Revision 1.5  1996/05/23  23:17:48  dhb
** t3d/e port
**
** Revision 1.4.1.1  1996/06/06  20:06:33  dhb
** Paragon port.
**
** Revision 1.4  1995/02/22  19:11:10  dhb
** Linux support.
**
** Revision 1.3  1993/10/05  20:14:47  dhb
** Fixed mistake in application of Solaris patches.
**
** Revision 1.2  1993/09/17  20:13:08  dhb
** Solaris compatability.
**
** Revision 1.1  1992/12/11  19:05:30  dhb
** Initial revision
**
*/

#include <math.h>
#include <stdio.h>
#include <signal.h>
#include <setjmp.h>
#include <strings.h>

#include "system_deps.h"

#if (defined(i860) && !defined(paragon)) || (defined(Linux) && !defined(__GLIBC__)) || defined(CRAY) || defined(Cygwin) || defined(Darwin)
struct sigcontext
{
  int xyzzy;
};
#endif

extern jmp_buf	main_context;

#ifndef Solaris
struct sigcontext sig_context;

SIGTYPE
save_context(sig,code,scp)
int		sig,code;
struct sigcontext *scp;
{
	/*
	** save the context for later return
	*/
	BCOPY(scp,&sig_context,sizeof(struct sigcontext));
}
#endif

#ifdef Solaris
SIGTYPE
restore_context(sig,code)
int		sig,code;
{
	/*
	** restore the earlier context
	lprintf("\ninterrupted\n");
	*/
	printf("\n");
	longjmp(main_context,1);
}
#else
SIGTYPE
restore_context(sig,code,scp)
int		sig,code;
struct sigcontext *scp;
{
	/*
	** restore the earlier context
	lprintf("\ninterrupted\n");
	*/
	printf("\n");
	longjmp(main_context,1);
}
#endif

#ifdef DEPENDENT
SIGTYPE
sig_msg_restore_context(sig,code,scp)
int		sig,code;
struct sigcontext *scp;
{
char *file;
char *FindSrcLine();
int line;
extern SIGTYPE sig_msg();

	file = FindSrcLine(scp->sc_pc,&line);
	if(file != NULL){
		printf("\nerror in %s at line %d\n",file,line);
	} else {
		printf("\nerror at pc = %d\n",scp->sc_pc);
	}
	sig_msg(sig,code,scp);
	restore_context(sig,code,scp);
}
#elif defined(Solaris)
SIGTYPE
sig_msg_restore_context(sig,code)
int		sig,code;
{
    char *file;
    int line;
    extern SIGTYPE sig_msg();
    extern void TraceScript();

    TraceScript();
    sig_msg(sig,code);
    restore_context(sig,code);
}
#else
SIGTYPE
sig_msg_restore_context(sig,code,scp)
int		sig,code;
struct sigcontext *scp;
{
    extern SIGTYPE sig_msg();
    extern void TraceScript();

    TraceScript();
    sig_msg(sig,code,scp);
    restore_context(sig,code,scp);
}
#endif

