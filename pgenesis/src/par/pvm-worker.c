static char rcsid[] = "$Id: pvm-worker.c,v 1.2 2005/09/29 23:22:02 ghood Exp $";

/*
 * $Log: pvm-worker.c,v $
 * Revision 1.2  2005/09/29 23:22:02  ghood
 * updated for PGENESIS 2.3
 *
 * Revision 1.1.1.1  2005/06/14 04:38:39  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.1  1999/12/19 03:59:42  mhucka
 * Adding PGENESIS from GENESIS 2.1 CDROM release.
 *
 * Revision 1.7  1997/06/24 16:55:57  ghood
 * bug fix to no_tty initial value
 *
 * Revision 1.6  1996/06/21 16:10:47  ghood
 * General debugging
 *
 * Revision 1.5  1996/05/03 19:48:14  ghood
 * moving to biomed area
 *
 * Revision 1.4  1996/02/01 19:04:44  ngoddard
 * added exit status
 *
 * Revision 1.3  1995/10/03  17:56:06  geigel
 * Updates for T3D port
 *
 * Revision 1.2  1995/04/18  21:28:14  ngoddard
 * *** empty log message ***
 *
 * Revision 1.1  1995/04/03  22:04:28  ngoddard
 * Initial revision
 *
 * Revision 1.1  1994/09/07  19:01:00  ngoddard
 * passive messaging works, src figures map
 *
 * Revision 1.5  1993/12/30  00:06:14  ngoddard
 * Release 12/14/93
 *
 * Revision 1.2  1993/10/15  22:51:51  ngoddard
 * added Id and Log keywords
 *
 *
*/

/****************************************************************
**                                                             **
**                     pvm-worker.c :                          **
**                                                             **
****************************************************************/

#include "par_ext.h"
#include "pvm-defs.h"
#if USE_MPI
#include <mpi.h>
#endif

/* 
** If this process was started by the master, then it sends its parent
** (the master) its tid to tell the master it is alive.
** In any case, it then waits for the master to send the list of
** all the nodes.
*/

int WorkerStartup(pm, action, master_tid)
     Postmaster *pm;
     Action * action;
     int master_tid;
{
  int i, j, id;
  int array_len;
  int * tid_array, * others_tid_array;
  int flag, redirect_output, oldval;
  Implementation * ipm = pm->ipm;
  int no_tty = (action->argv[6][0] == '\0' &&
                getenv("PGENESIS_DEBUG") == 0);
  int mytid;
  int active;

#if USE_MPI
  if (MPI_Comm_rank(MPI_COMM_WORLD, &mytid) != MPI_SUCCESS)
    {
      printf ("Error: Could not determine MPI rank of this node\n");
      exit(1);
    }
#endif

#if USE_PVM
  mytid = atoi(action->argv[action->argc-1]);
  /* send the master my tid */
  InitPvmMessage(pm);
  pvm_pkint(&mytid, 1, 1);
  SendPvmReply(pm, master_tid, PAR_WORKER_ALIVE);
#endif

  /* Receive message determining whether you are active or not */
  pvm_recv (master_tid, PAR_ISACTIVE_MSG);
  pvm_upkint (&active, 1, 1); 

  if (active) {

    /* Receive nnodes and tid_array from master */
    pvm_recv (master_tid, PAR_NODEID_MSG);
    pvm_upkint (&(pm->nnodes), 1, 1); 
    /* create tid_array for slaves, filled by master when spawning */
    tid_array = ipm->tid_array =
      (int *) malloc (pm->nnodes * sizeof(int));  
    others_tid_array = ipm->others_tid_array =
      (int *) malloc ((pm->nnodes - 1) * sizeof(int));  
    pvm_upkint (ipm->tid_array, pm->nnodes, 1); 

    /* receive debug flag and flag indicating if output is to be
       redirected */
    pvm_upkint (&flag, 1, 1);
    pvm_upkint (&redirect_output, 1, 1);

    /* determine mynode and copy tids to others_tid_array */
    for (i = j = 0; i < pm->nnodes; i++)
      if (mytid == ipm->tid_array[i])
	pm->mynode = i; /* last half OBSELETE */
      else
	ipm->others_tid_array[j++] = tid_array[i];
    assert(i == pm->nnodes);
    assert(j == pm->nnodes - 1);

    /* set PVM options based on debug flag and redirect
       output flag */
    /* if the debug flag was just 4, we do not set this
       because it generate tons of useless libpvm messages  (ghood) */
    if (flag != 4)
      oldval = pvm_setopt (PvmDebugMask, flag);
    if (redirect_output) 
    oldval = pvm_setopt (PvmOutputTid, master_tid);

    if (no_tty) {
      /* DEPENDENCY: assumes iofunc is job 0 */
      RemoveJob(0);
    }

    if (flag & 0x4)
      printf("Worker %d started\n", mytid);
  }
  else {
    /* we are inactive, we might as well shut down */
    printf ("Processor %d is inactive and thus is exiting \n", mytid);
    pvm_exit();
    exit(0);
  }
}


