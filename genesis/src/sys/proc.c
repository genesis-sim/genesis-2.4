static char rcsid[] = "$Id: proc.c,v 1.2 2005/06/25 22:04:40 svitak Exp $";

/*
** $Log: proc.c,v $
** Revision 1.2  2005/06/25 22:04:40  svitak
** Added explicit return types to untyped functions. Fixed return statements.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.6  2001/04/01 19:27:25  mhucka
** Using stdlib.h instead of malloc.h is better.
**
** Revision 1.5  2000/09/21 19:43:46  mhucka
** Don't define TRUE/FALSE here, just include header.h
**
** Revision 1.4  2000/07/12 06:17:25  mhucka
** Added #include of unistd.h and malloc.h.
**
** Revision 1.3  1996/05/23 23:17:48  dhb
** t3d/e port
**
** Revision 1.2  1993/09/17  20:19:34  dhb
** Solaris compatability.
**
** Revision 1.1  1992/12/11  19:05:33  dhb
** Initial revision
**
*/

/* no need for this on CRAY */
#ifndef CRAY

#include <stdio.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include "header.h"

extern int errno;

/*
** proc.c
**
**	Handle process termination in a sane way.
*/


typedef struct _exit_node
  {
    int			en_status;
    int			en_pid;
    struct _exit_node	*en_next;
  } ExitNode;

static ExitNode	*ExitList = NULL;



#ifdef COMMENT
static ProcHandler(sig)

int	sig;

{	/* ProcHandler --- Handle process termination */

	ExitNode	*en;

	en = (ExitNode *) malloc(sizeof(ExitNode));
	if (en == NULL)
	  {
	    perror("Wait");
	    return(-1);
	  }

	en->en_pid = wait(&en->en_status);

	if (en->en_pid == -1)	/* no child or EINTR */
	    free(en);
	else
	  {
	    en->en_next = ExitList;
	    ExitList = en;
	  }

}	/* ProcHandler */
#endif


void ProcInit()

{	/* ProcInit --- Set up to catch process termination signal */

	ExitList = NULL;

}	/* ProcInit */



int CollectExitedProcs()

{	/* CollectExitedProcs --- Wait on any ZOMBIE procs */

	int		status;
	int		pid;
	ExitNode	*en;

/* mds3 changes */
#if defined(i860)
      while ((pid = wait(&status)) > 0)
#elif defined(Solaris)
      while ((pid = waitpid((pid_t)-1,&status,WNOHANG)) > 0)
#else
      while ((pid = wait3(&status, WNOHANG, NULL)) > 0)
#endif
	  {
	    en = (ExitNode *) malloc(sizeof(ExitNode));
	    if (en == NULL)
	      {
	        perror("CollectExitedProcs");
	        return(-1);
	      }

	    en->en_pid = pid;
	    en->en_status = status;

	    en->en_next = ExitList;
	    ExitList = en;
	  }

      return 0;

}	/* CollectExitedProcs */



int ExitedProc(pid, status)

int	pid;
int	*status;

{	/* ExitedProc --- Return TRUE if process "pid" has exited */

	ExitNode	*cur;

	CollectExitedProcs();

	cur = ExitList;
	while (cur != NULL)
	  {
	    if (cur->en_pid == pid)
	      {
		if (status != NULL)
		    *status = cur->en_status;

		return(TRUE);
	      }

	    cur = cur->en_next;
	  }

	return(FALSE);

}	/* ExitedProc */



void RmExitedProc(pid)

int	pid;

{	/* RmExitedProc --- Remove process "pid" from exited proc list */

	ExitNode	*cur, *prev;

	prev = NULL;
	cur = ExitList;
	while (cur != NULL)
	  {
	    if (cur->en_pid == pid)
	      {
		if (prev == NULL)
		    ExitList = cur->en_next;
		else
		    prev->en_next = cur->en_next;
		free(cur);

		break;
	      }

	    prev = cur;
	    cur = cur->en_next;
	  }

}	/* RmExitedProc */



#ifdef COMMENT
int Wait(status)

int	*status;

{	/* Wait --- wait for a terminated process */

	ExitNode	*en;

	en = (ExitNode *) malloc(sizeof(ExitNode));
	if (en == NULL)
	  {
	    perror("Wait");
	    return(-1);
	  }

	sighold(SIGCLD);
	
	en->en_pid = wait(&en->en_status);
	if (status != NULL)
	    *status = en->en_status;

	if (en->en_pid == -1)	/* no child or EINTR */
	    free(en);
	else
	  {
	    en->en_next = ExitList;
	    ExitList = en;
	  }
	sigrelse(SIGCLD);

1	return(en->en_pid);

}	/* Wait */
#endif



int WaitOn(pid, status)

int	pid;
int	*status;

{	/* WaitOn --- Atomic wait and check for process termination */

	while (pid > 0 && !ExitedProc(pid, status))
	    sleep(1);
	RmExitedProc(pid);

	return(pid);

}	/* WaitOn */

#endif /* CRAY */
