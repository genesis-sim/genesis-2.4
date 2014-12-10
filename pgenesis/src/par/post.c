static char rcsid[] = "$Id: post.c,v 1.1.1.1 2005/06/14 04:38:39 svitak Exp $";

/*
 * $Log: post.c,v $
 * Revision 1.1.1.1  2005/06/14 04:38:39  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.1  1999/12/19 03:59:42  mhucka
 * Adding PGENESIS from GENESIS 2.1 CDROM release.
 *
 * Revision 1.12  1997/03/27 06:26:50  ngoddard
 * mods to scale up to larger networks
 *
 * Revision 1.11  1997/03/02 08:29:24  ngoddard
 * reset gets its own sync id and syncs before exit
 *
 * Revision 1.10  1997/02/28 05:24:51  ngoddard
 * support for lookahead optimisation, bug fixes
 *
 * Revision 1.9  1997/02/20 21:13:38  ngoddard
 * support for xpvm; using pvm_trecv for timeout ops; fixed bugs related to futures; provide user warnings when appears stalled; barrer and barrierall take a third argument: how long to wait before stall is assumed; beginnings of performance stats package
 *
 * Revision 1.8  1997/01/31 05:14:26  ngoddard
 * barriers don't complete asyncs now, minor bug fixes
 *
 * Revision 1.7  1997/01/21 22:20:25  ngoddard
 * paragon port
 *
 * Revision 1.6  1997/01/21 20:19:47  ngoddard
 * bug fixes for zones, implement string literal fields, first release of pgenesis script
 *
 * Revision 1.5  1996/08/13 21:25:05  ghood
 * General debugging to get parallel example scripts working.
 *
 * Revision 1.4  1996/06/21 16:10:45  ghood
 * General debugging
 *
 * Revision 1.3  1996/05/03 19:48:12  ghood
 * moving to biomed area
 *
 * Revision 1.2  1995/05/24  15:34:48  ngoddard
 * improved threads but still basic design flaw, also fixed comment leader in barrier tests
 *
 * Revision 1.1  1995/04/03  22:04:21  ngoddard
 * Initial revision
 *
 * Revision 1.1  1994/09/07  19:01:00  ngoddard
 * passive messaging works, src figures map
 *
 * Revision 1.5  1993/12/30  00:06:08  ngoddard
 * Release 12/14/93
 *
 * Revision 1.2  1993/10/15  22:51:51  ngoddard
 * added Id and Log keywords
 *
 *
*/

/* TODO add ADDMSGIN action to catch element copying */
/* TODO add DELETEMSGIN/OUT action to catch element deletion */

#include <stdio.h>
#include "par_ext.h"
#include "acct.h"

/*
** Postmaster : Handles parallel machine
*/
/* September 1994, Nigel H. Goddard */

PostAction(pm,action)
     Postmaster * pm;
     Action 	*action;
{
  int nnodes;
  Implementation * ipm;
  int i,j;
  char	*field;
  MsgOut * msgout;
  static double last_simulation_time = -999999999.0;
  
  DBGMSG("Entering PostAction...\n");
  nnodes= pm->nnodes;
  ipm = pm->ipm;
  SELECT_ACTION(action)
    {
    case INIT:
      break;
    case PROCESS:
      if (pm->perfmon)
	Acct(PGENESIS_PROCESS);
      /*      ELog("Postmaster handling a process action"); */
      /* execute all the buffered raddmsg commands */
      DBGMSG("flushing buffered messages...\n");
      FlushBufferedMessageRequests(pm);
      /* a node should do a waiton all if it needs to have all remote
	 operations completed before doing a step */
      /* Request response that all recommands have completed */
      /* DBGMSG("requesting rcommands completed...\n"); */
      /* RequestRcommandsCompleted(pm); */
      /* wait until all the remote commands returning futures have completed */
      /*      DBGMSG("waiting futures resolved...\n"); */
      /*      WaitFuturesResolved(pm); */
      /* wait until all the remote commands without a future have completed */
      /* DBGMSG("waiting rcommands completed...\n"); */
      /* WaitRcommandsCompleted(pm); */
      
      /* check if time has advanced since the last PROCESS action */
      if (SimulationTime() != last_simulation_time)
	{
	  last_simulation_time = SimulationTime();
	  /* if required, make sure all nodes are ready to step */
	  if (pm->sync_before_step) {
	    /*	    ELog("sync_before_step"); */
	    DBGMSG("syncinc nodes...\n");
	    if (pm->perfmon)
	      Acct(PGENESIS_PROCESS_SYNC);
	    SyncZoneNodes(pm, PAR_STEP_SYNC, pm->msg_hang_time, 100000.);
	    if (pm->perfmon)
	      Acct(PGENESIS_PROCESS);
	  }
	  /* increment the parallel library's timestep counter */
	  SetParlibStep(GetParlibStep() + 1);
	  /*  sprintf(str, "Incrementing step count to %d", GetParlibStep());
	  ELog(str); */
	}

      if (pm->perfmon)
	Acct(PGENESIS_PROCESS_SNDREC);
#if LOOKAHEAD_DEBUG
      fprintf(stdout, "node %d doing process:sendandreceive\n",
	      pm->ipm->mynode);
      fflush(stdout);
#endif
      SendAndReceiveData(pm);
#if LOOKAHEAD_DEBUG
      fprintf(stdout, "node %d done process\n", pm->ipm->mynode);
      fflush(stdout);
#endif
      if (pm->perfmon)
	Acct(PGENESIS_PROCESS);

      DBGMSG("completed PROCESS phase...\n");
      break;
    case EVENT:
      if (pm->perfmon)
	Acct(PGENESIS_EVENT);
      {
	MsgIn * msg = (MsgIn*) action->data;
	double * time = (double *) action->argv;

	if (MSGACTIVE(msg))
	  NotePostmasterEvent(pm,msg,*time);
      }
      break;

    case RESET:
      DBGMSG("resetting...\n");
      ResetActiveMessageCounters(pm);
      SyncZoneNodes(pm, PAR_RESET_SYNC, pm->msg_hang_time, 100000.);
      /* execute all the buffered raddmsg commands */
      DBGMSG("flushing buffered messages...\n");
      FlushBufferedMessageRequests(pm);
      /* a node should do a waiton all if it needs to have all remote
	 operations completed before doing a step */
      /* Request response that all recommands have completed */
      /* DBGMSG("requesting rcommands completed...\n"); */
      /* RequestRcommandsCompleted(pm); */
      /* wait until all the remote commands returning futures have completed */
      /*      DBGMSG("waiting futures resolved...\n"); */
      /*      WaitFuturesResolved(pm); */
      /* wait until all the remote commands without a future have completed */
      /* DBGMSG("waiting rcommands completed...\n"); */
      /* WaitRcommandsCompleted(pm); */

#if LOOKAHEAD_DEBUG
      fprintf(stdout, "node %d doing reset:sendandreceive\n",
	      pm->ipm->mynode);
      fflush(stdout);
#endif
      for (i = 0; i < nnodes; i++)
	{
	  ipm->most_recent[i] = -10000000.; /* nothing received yet */
	  /* dt is minimum value allowed for lookahead */
	  ipm->remain_ahead[i] = ipm->lookahead[i] = ClockValue(0);
	}
      /* give reset a negative time so its data isn't confused with */
      /* the data from the first step */
      SimulationTime() -= ClockValue(0); 
      SendAndReceiveData(pm);
      SimulationTime() += ClockValue(0);
#if LOOKAHEAD_DEBUG
      fprintf(stdout, "node %d done reset\n", pm->ipm->mynode);
      fflush(stdout);
#endif

      /* another sync is needed so that no node can execute any remote
	 operations (e.g., step@all) while other nodes may still be in
	 the SendAndReceiveData loop which will execute the op */
      SyncZoneNodes(pm, PAR_RESET_SYNC, pm->msg_hang_time, 100000.);
      DBGMSG("resetted...\n");
      break;
      
    case ADDMSGOUT:
      msgout = (MsgOut *) (action->data);
      /* ASSUMPTION: user isn't adding msgs from postmaster */
      assert(ipm->add_msg != 0);
      ipm->add_msg = 0;		/* reset */
      if (MSGACTIVE(msgout)) {/* if active */
	AddStructToVectorItem(ipm->out_active[ipm->current_active_node],
			      ipm->current_active_index,
			      &msgout, sizeof(Msg *));
      }
      break;
      
    case CREATE :
      /* define and initialize the accounting categories */
      NewAcct( PGENESIS, "PGENESIS" );
      NewAcct( PGENESIS_EVENT, "PGENESIS_EVENT" );
      NewAcct( PGENESIS_PROCESS, "PGENESIS_PROCESS" );
      NewAcct( PGENESIS_PROCESS_SYNC, "PGENESIS_PROCESS_SYNC" );
      NewAcct( PGENESIS_PROCESS_SNDREC, "PGENESIS_PROCESS_SNDREC" );
      NewAcct( PGENESIS_PROCESS_SNDREC_SND, "PGENESIS_PROCESS_SNDREC_SND" );
      NewAcct( PGENESIS_PROCESS_SNDREC_REC, "PGENESIS_PROCESS_SNDREC_REC" );
      NewAcct( PGENESIS_PROCESS_SNDREC_GETFIELD, "PGENESIS_PROCESS_SNDREC_GETFIELD" );
      Acct(PGENESIS);
      pm->perfmon = 0;
      /* start workers, establish communication */
      /* TODO: make sure paron can be done again if PVM not running */
      if (!ParallelStartup(pm,action))
	{ /* failed setup */
	  DBGMSG("Failed to start parallel stuff. Aborting\n");
	  return 0;
	}
      break;
    case SET :
      return (0);
      if (action->argc != 2)
	return(0); /* do the normal set */
      field = action->argv[0];
      if (strcmp(field,"xmin")!= 0 &&
	  strcmp(field,"xmax")!= 0 &&
	  strcmp(field,"ymin")!= 0 &&
	  strcmp(field,"ymax")!= 0 &&
	  strcmp(field,"zmin")!= 0 &&
	  strcmp(field,"zmax")!= 0 &&
	  strcmp(field,"sync_before_step")!= 0
	  ) 
	{
	  printf("Field %s is unmodifiable. Value unchanged.\n",
		 field);
	  return(1); /* do not let the default routine modify it */
	}
      if (strcmp(field,"msg_hang_time")==0 ||
	  strcmp(field,"msg_ack_time")==0) 
	{
	  if (atof(action->argv[1]) <= 0.0) 
	    {
	      printf("value %s for %s is too small. Value unchanged.\n",
		     action->argv[1],field);
	      return(1); /* do not let default routine modify it*/
	    }
	}
      return(0); /* let the default routine modify it */
      break;
    case CHECK:
      break;
    case COPY:
      printf("Postmasters cannot be copied!\n");
      return(0);
      break;
    case DELETEMSGIN:
    case DELETEMSGOUT:
      printf("Deleting a message which connects via a postmaster is unsupported.\n");
      abort();
      break;
    default:
      printf("Unimplemented action (type %d, name %s)\n\trequested on postmaster.\n",
	     action->type, action->name);
      return(0);
      break;
    }
  DBGMSG("Exiting PostAction...\n");
  if (pm->perfmon)
    Acct(PGENESIS);
  return 1;
}

int do_print_perfstats(argc, argv)
     int argc;
     char ** argv;
{
  Postmaster * pm = GetPostmaster();
  if (pm == NULL)
    return(0);

  if (pm->perfmon)
    PrintAcct("pgenesis", pm->ipm->mynode);
  return 1;
}


    
