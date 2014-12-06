static char rcsid[] = "$Id: signal.c,v 1.2 2005/06/25 22:04:40 svitak Exp $";

/*
** $Log: signal.c,v $
** Revision 1.2  2005/06/25 22:04:40  svitak
** Added explicit return types to untyped functions. Fixed return statements.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.10  2001/04/25 17:17:04  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.9  2000/09/21 19:46:11  mhucka
** #include some files to get some function declarations.
**
** Revision 1.8  2000/07/12 06:18:29  mhucka
** Removed some unused variables in sig_msg().
**
** Revision 1.7  1999/12/31 08:32:38  mhucka
** 1) Added SIGTYPE macro for return type declarations.
** 2) Massive cleanup of code.
**
** Revision 1.6  1999/08/22 04:42:15  mhucka
** Various fixes, mostly for Red Hat Linux 6.0
**
** Revision 1.5  1997/07/18 20:05:56  dhb
** Changes from PSC: T3E support
**
** Revision 1.4  1995/06/13  20:51:32  dhb
** SetSignals() now causes SIGPIPE to be ignored.  ClearSignals() returns
** to the default SIGPIPE behavior.
**
** Revision 1.3  1995/03/21  01:07:58  venkat
** Added Linux ifdef for signal SIGFPE in SetSignals()
**
** Revision 1.2  1993/09/17  20:49:59  dhb
** Solaris compatability.
**
** Revision 1.1  1992/12/11  19:05:36  dhb
** Initial revision
**
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <signal.h>
#include <unistd.h>

#include "system_deps.h"
#include "shell_func_ext.h"

void SetSIGQUIT()
{
    signal(SIGQUIT, sig_msg_restore_context);
}

void SetSignals()
{
    signal(SIGINT, restore_context);
    SetSIGQUIT();
    signal(SIGQUIT, SIG_IGN);
    signal(SIGSEGV, sig_msg_restore_context);
    signal(SIGILL, sig_msg_restore_context);
#ifdef Linux
    signal(SIGFPE, sig_msg_restore_context);
#endif
    signal(SIGBUS, sig_msg_restore_context);
    signal(SIGTRAP, sig_msg_restore_context);
    signal(SIGPIPE, SIG_IGN);
}

void ClearSignals()
{
    signal(SIGINT, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);
    signal(SIGSEGV, SIG_DFL);
    signal(SIGILL, SIG_DFL);
    signal(SIGFPE, SIG_DFL);
    signal(SIGBUS, SIG_DFL);
    signal(SIGTRAP, SIG_DFL);
    signal(SIGPIPE, SIG_DFL);
}

void CoreRequest()
{
     extern int IsBatchMode();

     if (IsBatchMode())    /* added by Greg Hood, PSC, May 1996 */
	  abort(); 

     printf("Dump core (y/n)[n]? ");
     normal_tty();
     if (getchar() == 'y'){
	  signal(SIGQUIT, SIG_DFL);
	  kill(getpid(), SIGQUIT);
     }
     tset();
}

#ifdef Solaris
SIGTYPE
sig_msg(sig, code)
int sig, code;
#else
SIGTYPE
sig_msg(sig, code, scp)
int sig, code;
struct sigcontext *scp;
#endif
{
     switch (sig) {
     case SIGSEGV :
          printf("Segmentation violation.\n");
          CoreRequest();
          break;
     case SIGILL :
          printf("Illegal instruction; code = %d\n", code);
          signal(SIGILL, sig_msg_restore_context);
          CoreRequest();
          break;
     case SIGFPE :
          printf("Floating point exception; code = %d\n", code);
          CoreRequest();
          break;
     case SIGBUS :
          printf("Bus error.\n");
          CoreRequest();
          break;
     case SIGTRAP :
          printf("Trace trap.\n");
          CoreRequest();
          signal(SIGTRAP, sig_msg_restore_context);
          break;
     default:
          printf("Fatal error *** signal: %d; code %d.\n", sig, code);
          CoreRequest();
          break;
     }
     printf("Exit program (y/n)[n]? ");
     normal_tty();
     if (getchar() == 'y')
     {
#ifdef T3E
	  globalexit(0);
#else
	  exit(0);
#endif
     }
     tset();
}
