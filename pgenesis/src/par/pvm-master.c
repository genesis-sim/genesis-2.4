static char rcsid[] = "$Id: pvm-master.c,v 1.2 2005/09/29 23:22:02 ghood Exp $";

/*
 * $Log: pvm-master.c,v $
 * Revision 1.2  2005/09/29 23:22:02  ghood
 * updated for PGENESIS 2.3
 *
 * Revision 1.1.1.1  2005/06/14 04:38:39  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.1  1999/12/19 03:59:42  mhucka
 * Adding PGENESIS from GENESIS 2.1 CDROM release.
 *
 * Revision 1.24  1997/06/24 16:51:58  ghood
 * nxpgenesis now spawns nxpgenesis processes by default
 *
 * Revision 1.23  1997/02/28 05:24:52  ngoddard
 * support for lookahead optimisation, bug fixes
 *
 * Revision 1.22  1997/02/20 21:13:38  ngoddard
 * support for xpvm; using pvm_trecv for timeout ops; fixed bugs related to futures; provide user warnings when appears stalled; barrer and barrierall take a third argument: how long to wait before stall is assumed; beginnings of performance stats package
 *
 * Revision 1.21  1997/01/31 05:14:27  ngoddard
 * barriers don't complete asyncs now, minor bug fixes
 *
 * Revision 1.20  1997/01/21 22:20:25  ngoddard
 * paragon port
 *
 * Revision 1.19  1997/01/21 20:19:48  ngoddard
 * bug fixes for zones, implement string literal fields, first release of pgenesis script
 *
 * Revision 1.18  1996/08/28 22:19:36  ngoddard
 * typo
 *
 * Revision 1.17  1996/08/28 21:18:45  ghood
 * Now uses PGENESIS_DEBUG environment variable to determine whether
 * to spawn processes under debugger.
 *
 * Revision 1.16  1996/08/26 16:16:14  ngoddard
 * parnx-> p
 *
 * Revision 1.15  1996/08/19 21:34:02  ghood
 * Made pvm calls type-consistent with their parameters.
 *
 * Revision 1.14  1996/08/13 21:25:06  ghood
 * General debugging to get parallel example scripts working.
 *
 * Revision 1.13  1996/07/29 21:39:37  ngoddard
 * C90 compilation
 *
 * Revision 1.12  1996/06/21  16:10:46  ghood
 * General debugging
 *
 * Revision 1.11  1996/05/03 19:48:13  ghood
 * moving to biomed area
 *
 * Revision 1.10  1996/02/01  19:04:17  ngoddard
 * added exit status
 *
 * Revision 1.9  1995/11/09  17:25:58  ngoddard
 * some fixes for Solaris
 *
 * Revision 1.8  1995/10/18  22:27:30  ngoddard
 * fixed bug which issued warning even when NNODES was set on W/S
 *
 * Revision 1.7  1995/10/03  19:43:31  geigel
 * number of nodes for T3D fix
 *
 * Revision 1.6  1995/10/03  17:56:06  geigel
 * Updates for T3D port
 *
 * Revision 1.5  1995/06/09  18:54:32  ngoddard
 * various fixes for ANSI C, function prototypes, and Solaris
 *
 * Revision 1.4  1995/05/02  22:07:03  ngoddard
 * changed to use -execdir and -altsimrc
 *
 * Revision 1.3  1995/04/18  21:28:11  ngoddard
 * *** empty log message ***
 *
 * Revision 1.2  1995/04/11  04:26:24  ngoddard
 * Patch-1-test
 *
 * Revision 1.1  1995/04/03  22:04:23  ngoddard
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
**                     pvm-master.c :                          **
**                                                             **
****************************************************************/

#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/param.h>
#include <unistd.h>
#include "par_ext.h"
#include "pvm-defs.h"
#if USE_MPI
#include <mpi.h>
#endif

extern char *getenv();

#define NNODE_ENV "NNODES"

/*
 *   GetNumNodes - extracts the number of nodes to be used and places
 *                 this value in the postmaster structure.  Returns the
 *                 total number of nodes that were started (which is
 *                 always >= the number to be used)
 */
static int GetNumNodes(post, action, abort_flag)
     Post * post;
     Action * action;
     int *abort_flag;
{
  char *  nnodes;
  int num_nodes;
  int id;

#if USE_MPI
  if (MPI_Comm_size(MPI_COMM_WORLD, &num_nodes) != MPI_SUCCESS)
    {
      printf ("Error: Could not determine total number of MPI nodes\n");
      post->nnodes = 1;
      (*abort_flag) = 1;
      return(0);
    }
  if (MPI_Comm_rank(MPI_COMM_WORLD, &id) != MPI_SUCCESS)
    {
      printf ("Error: Could not determine MPI rank of this node\n");
      post->nnodes = 1;
      (*abort_flag) = 1;
      return(0);
    }
  post->mynode = id;

  if (action->argv[3][0] != '\0')
    { 
      /* number of nodes given on paron statement */
      post->nnodes = atoi(action->argv[3]);

      /* check if we have enough PEs to accomodate */
      if (num_nodes < post->nnodes)
	{
	  /* too few PEs, default to 1 and flag to abort */
	  printf ("Error: %d nodes requested but only %d PEs are running.\n", 
		  post->nnodes, num_nodes);
	  post->nnodes = 1;
	  (*abort_flag) = 1;
	}
      else if (num_nodes > post->nnodes)
	{
	  /* too many PEs, some are wasted */
	  printf ("Warning: %d nodes requested -- %d PEs are running, excess of %d unused PEs\n", post->nnodes, num_nodes, num_nodes - post->nnodes);
	}
      return(num_nodes);
    }

  /* by default use all of the nodes in the MPI environment */
  printf("Warning: number of nodes not specified in paron statement, defaulting to %d.\n", num_nodes);
  post->nnodes = num_nodes;
  return(num_nodes);
#endif

#if USE_PVM
  /* if number of nodes is specified in environment, use that */
  nnodes = getenv(NNODE_ENV);  
  num_nodes = (nnodes && (*nnodes != '\0')) ?  atoi(nnodes) : 0;

  if (action->argv[3][0] != '\0')
    { 
      /* number of nodes given on paron statement */
      post->nnodes = atoi(action->argv[3]);

      if (post->nnodes <= 0)
	{
	  printf ("Error: invalid number (%d) of nodes requested.\n", 
		  post->nnodes);
	  post->nnodes = 1;
	  (*abort_flag) = 1;
	  return(0);
	}

      /* if we don't know how many nodes will be running, set it according
         to the number of nodes requested */
      if (num_nodes == 0)
	num_nodes = post->nnodes;

      /* check if we have enough PEs to accomodate */
      if (num_nodes < post->nnodes)
	{
	  /* too few PEs, default to 1 and flag to abort */
	  printf ("Error: %d nodes requested but only %d PEs are running.\n", 
		  post->nnodes, num_nodes);
	  post->nnodes = 1;
	  (*abort_flag) = 1;
	}
      else if (num_nodes > post->nnodes)
	{
	  /* too many PEs, some are wasted */
	  printf ("Warning: %d nodes requested -- %d PEs are running, excess of %d unused PEs\n", post->nnodes, num_nodes, num_nodes - post->nnodes);
	}
      return(num_nodes);
    }

  if (num_nodes == 0)
    {
      printf ("Error: Could not determine number of PVM nodes to use\n");
      post->nnodes = 1;
      (*abort_flag) = 1;
      return(0);
    }

  printf("Warning: number of nodes not specified, defaulting to %d.\n",
	 num_nodes);
  post->nnodes = num_nodes;
  return(num_nodes);
#endif
}


static int CheckWorkersAlive(pm, n_act_nodes)
     Postmaster *pm;
     int n_act_nodes;	
{
  double	end_time;
  int		n_good_nodes;
  double	pgen_clock();
  
  n_good_nodes = 1;
  end_time = pgen_clock() + pm->msg_hang_time;
  while (pgen_clock() < end_time && n_good_nodes < n_act_nodes) 
    {
      struct timeval timout;
      timout.tv_sec = pm->msg_hang_time/n_act_nodes;
      timout.tv_usec = 0;

      if (pvm_trecv(-1, PAR_WORKER_ALIVE, &timout) > 0)
	pvm_upkint (&(pm->ipm->tid_array[n_good_nodes++]), 1, 1);
    }
  return n_good_nodes;
}


/*
 *  FillArgv - Fills the argv array that gets sent to spawn child
 *             GENESIS processes.  Note that this does not get called
 *             for the T3D version as the T3D version does no spawning.
 *
 *             See comments in MasterStartup for action->agrv to spawn
 *             argv translation.
 */
void FillArgv (action, flag, argv)
     Action *action;
     int flag;
     char *argv[];
{
#define TMPWDSIZ 1024
      extern char * TopLevelNamedScriptFileName();
      extern PFI xgEventLoop;
      static char tmpwd[TMPWDSIZ];	/* holds current working directory */
      char nicebuf[32];		/* current process's nice value */
      int narg = 1;
      int mpri;
      
      /* set executable name */
      argv[0] = ((action->argv[7][0] == '\0') ?
                 (xgEventLoop != NULL ? "pgenesis" : "nxpgenesis") :
                 action->argv[7]);

      /* set arguments for executable, starting at narg == 1 */
      if (flag == 0 || action->argv[11][0] != '\0')
	{
	  /* no tty */
	  argv[narg++] = "-batch";
	  argv[narg++] = "-notty";
	}

      argv[narg++] = "-altsimrc";
      argv[narg++] = ((action->argv[4][0] == '\0') ?
		 ".psimrc" : action->argv[4]);
      argv[narg++] = "-silent";
      argv[narg++] = ((action->argv[5][0] == '\0') ?
		      (flag == 0 ? "3" : "0") : action->argv[5]);
      argv[narg++] = "-execdir";
      argv[narg++] = ((action->argv[9][0] == '\0') ? /* worker's directory */
		 getcwd(tmpwd,TMPWDSIZ) : action->argv[9]);
      if (action->argv[10][0] == '\0') { /* default nice level */
	mpri = getpriority(PRIO_PROCESS, 0); /* master process priority */
	if (mpri > 0) {		/* not default, pass on to workers */
	  sprintf(nicebuf,"%d", mpri);
	  argv[narg++] = "-nice";
	  argv[narg++] = nicebuf;
	}
      } else { /* explicit nice level */
	argv[narg++] = "-nice";
	argv[narg++] = action->argv[10];
      }
      /* this must come last */
      argv[narg++] = ((action->argv[2][0] == '\0') ? /* worker script file */
		 TopLevelNamedScriptFileName() : action->argv[2]);
      argv[narg++] = NULL;		/* terminator */
  
}

int MasterStartup(pm,action)
     Postmaster *pm;
     Action *action;
{
  int     i, j, id;
  char *  nnodes;
  int * tid_array, *others_tid_array, *inactive_tid_array;
  int redirect_output = 0;
  int n_total_nodes;
  int active;
  int abort_flag = 0;
  int flag = 0;
  char *argv[12];		/* arguments for workers */
  
  /* action->argv is as follows */
  /* [0] - "post" */
  /* [1] - "/post" */
  /* [2] - start-file-name, "" implies TopLevel */
  /* [3] - num nodes, "" implies get from environment or pvm */
  /* [4] - .simrc file to use, "" implies .psimrc */
  /* [5] - silence level, "" imples 3 */
  /* [6] - spawn value, 4 implies debug, 8 implies trace (see PVM) */
  /* [7] - worker executable name, "" implies "pgenesis" */
  /* [8] - worker output file, "" implies none */
  /* [9] - worker execution directory, "" implies cwd on master */
  /* [10] - worker priority (nice) level, "" implies default priority */
  /* [11] - worker batch mode, "" implies no batch mode */

  /* determine number of nodes to use */
  n_total_nodes = GetNumNodes(pm, action, &abort_flag);

  /* create tid_array for slaves, filled by master when spawning */
  tid_array = pm->ipm->tid_array =
    (int *) malloc (n_total_nodes * sizeof(int));  
  others_tid_array = pm->ipm->others_tid_array = tid_array + 1;
  inactive_tid_array = others_tid_array + pm->nnodes - 1;

  pm->mynode = 0;

  /* master's tid is always in tid_array[0], workers rearrange tid_array */
  tid_array[0] = atoi(action->argv[action->argc-1]);
  
#if USE_MPI
  /* cmc load the tid array */
  for (i = 0; i < n_total_nodes; ++i)
    tid_array[i] = i;

  if (n_total_nodes <= 1)
    {
      printf ("Running on one node.\n");
      fflush (stdout);
      return;
    }

  if (pm->nnodes > 1)
    {
      /* Send message to nodes indicating that they are active */
      active = 1;
      InitPvmMessage(pm);
      pvm_pkint (&active, 1, 1);
      SendStartupMessage(pm, others_tid_array, pm->nnodes-1,
			 PAR_ISACTIVE_MSG);
      
      /* Broadcast nnodes and tid_array to slave tasks */
      /* Also send debug flag and output file flag */
      InitPvmMessage(pm);
      pvm_pkint (&(pm->nnodes), 1, 1); /* send total nodes */
      pvm_pkint (tid_array, pm->nnodes, 1); /* send tids */
      pvm_pkint (&flag, 1, 1);  /* send debug flag */
      pvm_pkint (&redirect_output, 1, 1); /* output redirect */
      SendStartupMessage (pm, others_tid_array, pm->nnodes-1,
			  PAR_NODEID_MSG);
    }
  
  /* send messages to the inactive nodes essentially telling
     them to shut down */
  if (n_total_nodes > pm->nnodes)
    {
      active = 0;
      InitPvmMessage(pm);
      pvm_pkint (&active, 1, 1);
      SendStartupMessage (pm, inactive_tid_array,
			  n_total_nodes - pm->nnodes,
			  PAR_ISACTIVE_MSG);
    }

  /* if we're slated to abort, then do it now. */
  if (abort_flag)
    {
      printf ("Parallel GENESIS aborting!\n");
      pvm_exit();
      exit(1);  /* EXIT_FAILURE */
    }
#endif
  
#if USE_PVM
  if (n_total_nodes > 1)   /* if more than one node */
    {
      int flag = 0;
      char * argv[12];		/* arguments for workers */

      /* determine debug flag */
      if (getenv("PGENESIS_DEBUG"))
	flag = 0x4;
      if (action->argv[6][0] != '\0')	      /* paron flag overrides
						 environment variable */
	flag = atoi(action->argv[6]) & 0xc;

      /* create and fill your argv array to be used for spawning */
      FillArgv (action, flag, argv);

      /* notify the start of the spawn */
      if (IsSilent() < 2) {
	printf ("Spawning GENESIS");
	if (flag != 0)
	  printf(" (debug = %x)", flag);
	printf(" on %d nodes using:\n\t", (pm->nnodes - 1));
	for (id = 0; argv[id] != NULL; id++)
	  printf ("%s ", argv[id]);
	printf ("\n");
	fflush (stdout);
      }

      /* set the worker output file, if any */
      /* pvm3.3.4 required */
      if (action->argv[8][0] != '\0') {
	FILE * wkrout = fopen(action->argv[8], "w");
	if (wkrout != NULL) {
	  pvm_catchout(wkrout);
	  redirect_output = 1;
	}
	else
	  printf("unable to open file %s, worker output defaulting\n",
		 action->argv[8]);
      }

      /* spawn slaves and add tids to tid_array after master */
      id = pvm_spawn (argv[0],argv+1,flag,NULL,(pm->nnodes-1),tid_array+1);
      if (id < pm->nnodes - 1)
	{
	  printf("WARNING - some Genesis nodes failed to spawn\n");
	  for (i = 1; i < pm->nnodes; ++i)
	    if (tid_array[i] < 0) 
	      {
		printf("\tReason: %d", tid_array[i]);
		if (tid_array[i] == PvmNoFile)
		  printf(" (executable file not found)");
		printf("\n");
	      }
	  ParError(SPAWN_FAILURE, -1);
	}

      if (id > 0) {

	/* announce the actual spwaning of nodes. */
	if (IsSilent() < 2) {
	  printf ("spawn of %d node%s initiated... ", id,
		  ((id > 1) ? "s" : ""));
	  fflush (stdout);
	}

	/* collect alive messages from workers */
	pm->msg_hang_time = (float) HANG_TIME;
	if ((i = CheckWorkersAlive(pm, n_total_nodes)) != n_total_nodes) {
	  ParError(SPAWN_FAILURE, -1);
	  pm->nnodes = i;
	}

	/* announce completion of spawn process */
	if (IsSilent() < 2) {
	  printf ("... %d completed spawn, running with %d node%s.\n",
		  pm->nnodes-1, pm->nnodes, ((pm->nnodes > 1) ? "s" : ""));
	  fflush (stdout);
	}

	if (n_total_nodes > 1)   /* check again for nodes before broadcasting */
	  {

	    if (pm->nnodes > 1) {
	      /* Send message to nodes indicating that they are active */
	      active = 1;
	      InitPvmMessage(pm);
	      pvm_pkint (&active, 1, 1);
	      SendStartupMessage(pm, others_tid_array, pm->nnodes-1,
				 PAR_ISACTIVE_MSG);
	    
	      /* Broadcast nnodes and tid_array to slave tasks */
	      /* Also send debug flag and output file flag */
	      InitPvmMessage(pm);
	      pvm_pkint (&(pm->nnodes), 1, 1); /* send total nodes */
	      pvm_pkint (tid_array, pm->nnodes, 1); /* send tids */
	      pvm_pkint (&flag, 1, 1);  /* send debug flag */
	      pvm_pkint (&redirect_output, 1, 1); /* output redirect */
	      SendStartupMessage (pm, others_tid_array, pm->nnodes-1,
				  PAR_NODEID_MSG);
	    }

	    /* send messages to the inactive nodes essentially telling
	       them to shut down */
	    if (n_total_nodes > pm->nnodes) {
	      active = 0;
	      InitPvmMessage(pm);
	      pvm_pkint (&active, 1, 1);
	      SendStartupMessage (pm, inactive_tid_array,
				  n_total_nodes - pm->nnodes, PAR_ISACTIVE_MSG);
	    }

	    /* if we're slated to abort, then do it now. */
	    if (abort_flag) {
	      printf ("Parallel GENESIS aborting!\n");
	      pvm_exit();
	      exit(1);  /* EXIT_FAILURE */
	    }
	  }

	return;
      }
    }
#endif
}



