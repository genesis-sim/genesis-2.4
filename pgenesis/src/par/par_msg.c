static char rcsid[] = "$Id: par_msg.c,v 1.2 2005/09/29 23:16:24 ghood Exp $";

/*
 * $Log: par_msg.c,v $
 * Revision 1.2  2005/09/29 23:16:24  ghood
 * updated for PGENESIS 2.3
 *
 * Revision 1.1.1.1  2005/06/14 04:38:39  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.1  1999/12/19 03:59:41  mhucka
 * Adding PGENESIS from GENESIS 2.1 CDROM release.
 *
 * Revision 1.19  1997/03/27 06:26:46  ngoddard
 * mods to scale up to larger networks
 *
 * Revision 1.18  1997/02/28 05:24:48  ngoddard
 * support for lookahead optimisation, bug fixes
 *
 * Revision 1.17  1997/02/20 21:13:35  ngoddard
 * support for xpvm; using pvm_trecv for timeout ops; fixed bugs related to futures; provide user warnings when appears stalled; barrer and barrierall take a third argument: how long to wait before stall is assumed; beginnings of performance stats package
 *
 * Revision 1.16  1997/01/31 05:14:21  ngoddard
 * barriers don't complete asyncs now, minor bug fixes
 *
 * Revision 1.15  1997/01/21 22:20:21  ngoddard
 * paragon port
 *
 * Revision 1.14  1997/01/21 20:19:44  ngoddard
 * bug fixes for zones, implement string literal fields, first release of pgenesis script
 *
 * Revision 1.13  1996/08/19 21:34:02  ghood
 * Made pvm calls type-consistent with their parameters.
 *
 * Revision 1.12  1996/08/13 21:25:02  ghood
 * General debugging to get parallel example scripts working.
 *
 * Revision 1.11  1996/06/21 16:10:41  ghood
 * General debugging
 *
 * Revision 1.10  1996/05/03 19:48:07  ghood
 * moving to biomed area
 *
 * Revision 1.9  1995/07/05  21:21:18  mckeon
 * Added skeleton for rvolumeconnect
 *
 * Revision 1.8  1995/06/09  18:54:29  ngoddard
 * various fixes for ANSI C, function prototypes, and Solaris
 *
 * Revision 1.7  1995/05/10  18:56:43  ngoddard
 * before making changes to allow ids on barriers
 *
 * Revision 1.6  1995/05/02  22:07:00  ngoddard
 * changed to use -execdir and -altsimrc
 *
 * Revision 1.5  1995/04/26  19:57:10  ngoddard
 * commenced allowing out-of-zone setups out of step
 *
 * Revision 1.4  1995/04/13  19:48:15  geigel
 * CompleteRemoteActiveMessage -- multiple destinations added to vector
 *
 * Revision 1.3  1995/04/12  19:57:57  ngoddard
 * Fixed a precedence problem (?) in CompleteRemoteActiveMessage
 * and removed the need for the \ put in in revision 1.2
 *
 * Revision 1.2  1995/04/04  18:07:05  ngoddard
 * Irix 4.0.5H doesn't like a broken line, added backslash to unbreak it
 *
 * Revision 1.1  1995/04/03  22:04:06  ngoddard
 * Initial revision
 *
 * Revision 1.1  1994/09/07  19:01:00  ngoddard
 * passive messaging works, src figures map
 *
 */

/* 
  TODO handle character strings (or any vector?)
  TODO turn on/off PASSIVE and ACTIVE messages before/after addmsg
  TODO make sure num_in_nodes, num_out_nodes are adjusted on errors
  */
#include <stdio.h>
#include <time.h>
#include "tvdefs.h"
#include "par_ext.h"
#include "pvm-defs.h"
#include "acct.h"

extern PFI GetxgEventLoop();

/*****************************************************************
 *	DELIVERY OF MESSAGES TO NON-LOCAL DESTINATIONS
 *****************************************************************/

void NotePostmasterEvent(pm, msgin, time)
     Postmaster * pm;
     MsgIn * msgin;
     double time;
{
  unsigned int * pdstidx;
  int act_idx, dst_node, dst_idx, cnt, blk, ind;
  Implementation * ipm = pm->ipm;
  BufferManager * bfm = ipm->in_active_msg;
  unsigned int dstidx;
  assert(msgin->nslots == 1);
  assert(bfm != NULL);
  act_idx = MSGVALUE(msgin,0);

  /*INT_STRUCT_LOOP_HEAD(bfm, act_idx, dstidx) */
  InitIntStructLoop(bfm, act_idx);
  while (IterIntStructLoop(&dstidx))
    {
      dst_node = dstidx >> (8*sizeof(unsigned short));
      assert(dst_node >= 0);
      assert(dst_node < ipm->nnodes);
      dst_idx = dstidx - (dst_node << (8*sizeof(unsigned short)));
      if (((int) ipm->in_active_cnt[dst_node]+1) >
	  ((int) ipm->in_active_siz[dst_node])) {
	/* have to make the vector bigger */
	if (ipm->in_active[dst_node] != NULL) { /* realloc bigger */
	  ipm->in_active[dst_node] = (unsigned short *)
	    realloc(ipm->in_active[dst_node],
		    (ipm->in_active_siz[dst_node] += IN_ACT_BLK_SIZ)*sizeof(unsigned short));
	  ipm->in_active_time[dst_node] = (double *)
	    realloc(ipm->in_active_time[dst_node],
		    (ipm->in_active_siz[dst_node])*sizeof(double));
	} else {		/* malloc for first time */
	  ipm->in_active[dst_node] = (unsigned short *)
	    malloc((ipm->in_active_siz[dst_node] += IN_ACT_BLK_SIZ)*sizeof(unsigned short));
	  ipm->in_active_time[dst_node] = (double *)
	    malloc((ipm->in_active_siz[dst_node])*sizeof(double));
	}
      } /* if have to make vector bigger */
      ipm->in_active[dst_node][ipm->in_active_cnt[dst_node]] = dst_idx;
      ipm->in_active_time[dst_node][ipm->in_active_cnt[dst_node]++] = time;
    }
  /*    INT_STRUCT_LOOP_TAIL; */
}

void GetFieldValues(pm)
     Postmaster * pm;
{
  Implementation * ipm = pm->ipm;
  BufferManager * bfm = ipm->field_info;
  int blkno, idx;
  if (bfm == NULL)
    return;			/* no fields to get */
  for (blkno = 0; blkno < bfm->blkcnt; blkno++)
    if (bfm->key[blkno] != NULL)
      for (idx = 0; idx < bfm->siz; idx++)
	if (bfm->key[blkno][idx] != 0) {
	  Slot slot;
	  FieldInfo * fld = &(((FieldInfo *) (bfm->item[blkno]))[idx]);
	  slot.data = (char *) (bfm->key[blkno][idx]);
	  slot.func = ipm->access_func[fld->type];
	  fld->data = slot.func(&slot);
    }
}

void SendDataMessage(pm, dst_node)
     Postmaster * pm;
     int dst_node;		/* node number in zone */
{
  Implementation * ipm = pm->ipm;
  BufferManager * dbfm = ipm->in_data[dst_node];
  BufferManager * fbfm = ipm->field_info;
  int i, j;
  double cur_time = SimulationTime();
  /* Initialize the pvm message buffer */
  InitPvmMessage(pm);
  /* always pack zone node number */
  if (pvm_pkint(&(ipm->myznode), 1, 1) < 0)
    { PvmError(); }
  /* send the current step number if there are active messages */
  if ((ipm->exist_msg[dst_node] & OUT_ACTIVE) != 0)
    if (pvm_pkdouble(&(cur_time), 1, 1) < 0)
      { PvmError(); }
  assert(((ipm->check_msg[dst_node] | OUT_PASSIVE) ||
	  (ipm->exist_msg[dst_node] | OUT_ACTIVE)));
  if (ipm->check_msg[dst_node] & OUT_PASSIVE) {
    /* note sent so we don't do it again */
    ipm->check_msg[dst_node] &= ~OUT_PASSIVE;
    /* get the slot data for passive messages into the sending buffer */
    for (i = 0; i < dbfm->blkcnt; i++)
      if (dbfm->key[i] != NULL)	/* does block exist? */
	for (j = 0; j < dbfm->siz; j++)
	  if (dbfm->key[i][j] != 0) /* does item exist? */
	    ((double*)(dbfm->item[i]))[j] = *(DATA_ADDR(fbfm,dbfm->key[i][j]));
    /* pack the slot data for passive messages into the PVM buffer */
    /* TODO: T3D, Paragon should use matched pvm_psend (also workstation?) */
    for (i = 0; i < dbfm->blkcnt; i++)
      if (dbfm->key[i] != NULL)	/* does block exist? */
	if (pvm_pkdouble(dbfm->item[i], dbfm->siz, 1) < 0)
	  { PvmError(); }
  }
  if (ipm->check_msg[dst_node] & OUT_ACTIVE) {
    ipm->check_msg[dst_node] &= ~OUT_ACTIVE;
    /* if there could be active messages, send them */
    /* pack the active message count into the PVM buffer */
    if (pvm_pkushort(&(ipm->in_active_cnt[dst_node]), 1, 1) < 0)
      { PvmError(); }
    if (ipm->in_active_cnt[dst_node] > 0) {
      /* pack the active message indices into the PVM buffer */
      if (pvm_pkushort(ipm->in_active[dst_node],
		       ipm->in_active_cnt[dst_node], 1) < 0)
	{ PvmError(); }
      /* pack the active message times into the PVM buffer */
      /* TODO probably lots have the same time, send a code */
      if (pvm_pkdouble(ipm->in_active_time[dst_node],
		       ipm->in_active_cnt[dst_node],1) < 0)
	{ PvmError(); }
      /* reset the active message count */
      ipm->in_active_cnt[dst_node] = 0;
    }
  }
#if LOOKAHEAD_DEBUG
  fprintf(stdout, "node %d, sending data to %d\n", pm->ipm->mynode, dst_node);
  fflush(stdout);
#endif
  SendLocalDataMessage(pm, dst_node);
#if LOOKAHEAD_DEBUG
  fprintf(stdout, "node %d, sent data\n", pm->ipm->mynode);
  fflush(stdout);
#endif
}

int ExistingOutgoingNodes(pm)
     Postmaster * pm;
{
  int i = pm->ipm->nnodes, cnt = 0;
  unsigned char * chk = pm->ipm->exist_msg;
  while (i-- > 0)
    if (chk[i] & (OUT_ACTIVE | OUT_PASSIVE))
      cnt++;
  return cnt;
}

void ResetActiveMessageCounters(pm)
     Postmaster * pm;
{
  Implementation * ipm = pm->ipm;
  /* reset the outgoing active message count for all nodes */
  memset(ipm->in_active_cnt, 0, ipm->nnodes*sizeof(unsigned short));
}

/*****************************************************************
 *	RECEIPT OF MESSAGES FROM NON-LOCAL SOURCES
 *****************************************************************/

/* returns 1 if a data message was processed, 0 otherwise */
int ReceiveDataMessage(pm, tim, todo)
     Postmaster * pm;
     float tim;			/* timeout in seconds */
     int * todo;		/* how many stalls remaining */
{
  Implementation * ipm = pm->ipm;
  BufferManager * dbfm;
  int src_node, explen, bufid, len, rtype, srcid, i;
  int mtag = LOCAL_DATA;	
  double cur_time = SimulationTime(), src_time;
  struct timeval curtim, timdone, timout;
  PFI xgEventLoop = GetxgEventLoop();
  float timo;

  timout.tv_sec = tim;
  timout.tv_usec = ((tim - timout.tv_sec) * 1000000);
  gettimeofday(&curtim, (struct timezone*)0);
  TVXADDY(&timdone, &timout, &curtim); /* termination time */

  while (TVXLTY(&curtim, &timdone))
    {
      TVXSUBY(&timout, &timdone, &curtim); /* compute remaining time */
      assert(timout.tv_sec >= 0 && timout.tv_usec >= 0);
      bufid = ReceiveSomeMessage(pm, -1, &timout);
      if (bufid > 0)		/* Found one */
	{
	  if (pvm_bufinfo (bufid, &len, &rtype, &srcid) < 0)
	    { printf ("Error in pvm_bufinfo in RecieveDataMessage\n");return; }
	  if (rtype == PAR_SETUP_MESSAGE)
	    {
	      if (ProcessSetupMessage(pm, bufid) < 0) /* handle it */
		{
		  fprintf(stdout, "failed to handle a setup message in ReceiveDataMessage\n");
		  fflush(stdout);
		}
	    }
	  else
	    {			/* found one, process it and return */
	      ProcessDataMessage(pm, len, rtype, srcid, todo);
	      return 1;
	    }
	}
      else
	return 0;		/* timed out */
      gettimeofday(&curtim, (struct timezone*)0);
    }
  return 0;			/* timed out */
}
  
int ProcessDataMessage(pm, len, rtype, srcid, todo)
     Postmaster * pm;
     int len, rtype, srcid;
     int * todo;
{
  Implementation * ipm = pm->ipm;
  BufferManager * dbfm;
  int src_node, explen, bufid, i;
  double cur_time = SimulationTime(), src_time;

  if (pvm_upkint(&src_node, 1, 1) < 0)
    PvmError();
#if LOOKAHEAD_DEBUG
  fprintf(stdout, "node %d (%x) reveived msg type %d from node %d (%x)\n", ipm->mynode, ipm->tid_array[ipm->mynode], rtype, src_node, srcid);
  fflush(stdout);
#endif
  assert((ipm->exist_msg[src_node] & IN_ACTIVE) || /* active must exist */
	 (ipm->check_msg[src_node] & IN_PASSIVE)); /* passive same step */
  if ((ipm->exist_msg[src_node] & IN_ACTIVE) != 0)
    {
      if (pvm_upkdouble(&src_time, 1, 1) < 0)
	PvmError();
      /* assert causality not violated */
      assert(cur_time - src_time <= ipm->lookahead[src_node]);
      /* but allow src to get arbitrarily far ahead */
      /* we could keep a pending list of pvm buffers and not */
      /* actually distribute the spike EVENTS until the latest */
      /* time possible.  This would trade off time/memory in */
      /* keeping stuff in PVM buffers vs. time/memory in */
      /* keeping it in the synchan object.  Probably PVM is */
      /* significantly more efficient, especially since */
      /* buffers are packed info.  But let's be simple for now. */
      /* We distribute spikes as soon as they arrive, no matter */
      /* how far in the future or past (subject to lookahead */
      /* limit) they originate. */
    }
  if (ipm->check_msg[src_node] & IN_PASSIVE)
    {
      /* note found so we won't accept again this step */
      ipm->check_msg[src_node] &= ~IN_PASSIVE;
      /* unstall if no active component, otherwise active will unstall */
      if (todo && !(ipm->exist_msg[src_node] & IN_ACTIVE))
	*todo -= 1;		/* unstall one node's data */
#if LOOKAHEAD_DEBUG
      fprintf(stdout, "parstep %d, node %d: received passive from %d\n", GetParlibStep(), ipm->mynode, src_node); 
      fflush(stdout);
#endif
      /* unpack the data into its rightful place */
      dbfm = ipm->out_data[src_node];
      assert(dbfm != NULL);
      for (i = 0; i < dbfm->blkcnt; i++)
	if (dbfm->key[i] != NULL) /* does block exist? */
	  if (pvm_upkdouble(dbfm->item[i], dbfm->siz, 1) < 0)
	    PvmError();
    }
  if ((ipm->exist_msg[src_node] & IN_ACTIVE) == 0)
    {
#if LOOKAHEAD_DEBUG
      fprintf(stdout, "node %d not expecting active message from node %d, exist_msg[%d] is %d\n", ipm->mynode, src_node, src_node, ipm->exist_msg[src_node]);
      fflush(stdout);
#endif
    }
  else
    {
      int i;
      unsigned short cnt;
      /* mustn't get data from the same timestep twice */
      assert(ipm->most_recent[src_node] < src_time);
      ipm->most_recent[src_node] = src_time;

      /* update lookahead info */
      if (todo)			/* note if we are unstalling a node */
	{
#if LOOKAHEAD_DEBUG
	  fprintf(stdout, "node %d todo before update is %d, remain_ahead[%d] is %lf\n", ipm->mynode, *todo, src_node, ipm->remain_ahead[src_node]);
	  fflush(stdout);
#endif
	  if (ipm->remain_ahead[src_node] < ClockValue(0))
	    {
	      *todo -= 1;
#if LOOKAHEAD_DEBUG
	      fprintf(stdout, "node %d: decremented todo, now %d\n", ipm->mynode, *todo);
	      fflush(stdout);
#endif
	    }
	}
      else
	{
#if LOOKAHEAD_DEBUG
	  fprintf(stdout, "node %d: todo is null\n", ipm->mynode);
	  fflush(stdout);
#endif
	}
      ipm->remain_ahead[src_node] = ipm->lookahead[src_node] -
	(cur_time - src_time);
#if LOOKAHEAD_DEBUG
      fprintf(stdout, "node %d: remain_ahead[%d] is %lf\n", ipm->mynode,
	      src_node, ipm->remain_ahead[src_node]);
      fflush(stdout);
#endif
      /* deliver active messages if there are any */
      assert(ipm->out_active[src_node] != NULL);
      /* unpack the active message count */
      if (pvm_upkushort(&cnt, 1, 1)< 0)
	PvmError();
#if LOOKAHEAD_DEBUG
      fprintf(stdout, "time %d, node %d: received active from %d time %d cnt %d, src_time %lf\n", (int) (cur_time/ClockValue(0)), ipm->mynode, src_node, (int) (src_time/ClockValue(0)), cnt, src_time); 
      fflush(stdout);
#endif
      if (cnt > 0)
	{
	  BufferManager * bfm = ipm->out_active[src_node];
	  short * pidx;
	  double * ptim;
	  while (((int) cnt) > ((int) ipm->out_active_siz))
	    {
	      if (ipm->out_active_idx != NULL)
		{
		  free(ipm->out_active_idx);
		  free(ipm->out_active_time);
		}
	      ipm->out_active_siz += OUT_ACT_BLK_SIZ;
	      ipm->out_active_idx = (short *)
		malloc(ipm->out_active_siz * sizeof(short));
	      ipm->out_active_time = (double *)
		malloc(ipm->out_active_siz * sizeof(double));
	    }
	  /* unpack the active message indices from the PVM buffer */
	  if (pvm_upkshort(ipm->out_active_idx, cnt, 1) < 0)
	    PvmError();
	  /* unpack the active message times from the PVM buffer */
	  /* TODO probably lots have the same time, send a code */
	  if (pvm_upkdouble(ipm->out_active_time, cnt, 1) < 0)
	    PvmError();
	  /* deliver the active messages */
	  for (i = 0, pidx = ipm->out_active_idx, ptim = ipm->out_active_time;
	       i < ((int) cnt); i++, pidx++, ptim++)
	    {
	      /* loop through the vector of Msgs calling CallEventAction */
	      Msg * msg;
/*	      MSG_STRUCT_LOOP_HEAD(bfm, *pidx, msg) */
	      InitMsgStructLoop(bfm, *pidx);
	      while (IterMsgStructLoop(&msg))
		{
		  assert(msg != NULL); 
		  CallEventActionWithTime(pm, msg, *ptim);
		}
/*	      MSG_STRUCT_LOOP_TAIL; */
	    }
	}
    }
}

int DataTransmissionCompleted(pm)
     Postmaster * pm;
{
  int i = pm->ipm->nnodes;
  unsigned char * chk = pm->ipm->check_msg;
  double * rmn = pm->ipm->remain_ahead;
  while (i-- > 0)
    if (chk[i] & (IN_PASSIVE | OUT_PASSIVE | OUT_ACTIVE))
      return 0;
  for (i = pm->ipm->nnodes-1, chk = pm->ipm->exist_msg; i >= 0; i--)
    if ((chk[i] & IN_ACTIVE) && rmn[i] < 0.)
      return 0;
  return 1;
}

int ExistingIncomingNodes(pm)
     Postmaster * pm;
{
  int i = pm->ipm->nnodes, cnt = 0;
  unsigned char * chk = pm->ipm->exist_msg;
  while (i-- > 0)
    if (chk[i] & (IN_ACTIVE | IN_PASSIVE))
      cnt++;
  return cnt;
}

/*****************************************************************
 *	LOOKAHEAD ROUTINES
 *****************************************************************/

/*
For implementing lookahead: node maintains <lookahead> vector of how
far it can get ahead of each sending node, and sends lagging nodes a
request for update if needed.  This vector is filled either by user
supplied information or by examining all the synapses in the network -
it is the theoretical lookahead.

Possible strategies:

1) Send NULL messages on every timestep so every destination node can
be sure how far ahead it is of its source nodes.  Advantage: simple,
enables maximum lookahead.  Disadvantage: lots of NULL messages.

2) When destination is within some epsilon of its lookahead wrt a
source, it sends a request for update to the source.  The trick is to
adjust epsilon so that it is as small as possible (maximizing
lookahead) while making it extremely likely that the response from the
source will have arrived before the destination reaches its lookahead
limit wrt that source.  Advantage: potentially much less overhead.
Disadvantages: 1) adjusting epsilon must be per node and no good way
to be sure of avoiding a stall 2) in bad cases we need twice as many
null messages (request/response).

3) hybrid: start with NULL messages.  If destination finds it is
nowhere near its lookahead limit for some source, it can tell the
source to (a) not send NULLs in future, compute epsilons and poll when
necessary, or (b) send NULLs every N steps (or N after a non-NULL)
where N is the difference between the theoretical maximum lookahead
and the maximum lookahead achieved so far wrt that source.  In either
case we may need to adjust epsilon or N.  Strategy should be to avoid
stalling at all costs.  So, if a stall occurs, exponentially decrease
N or increase epsilon, then linearly do the reverse to increase
effective lookahead.  Maybe could adjust N/epsilon down/up if
lookahead wrt source is approaching max, to try to avoid stalling
without the drastic step of exponential reduction of lookahead.

Implementation strategy:

1) is probably not at all bad on most platforms, especially LANS.  The
bandwidth cost is minimal.  The cpu time used to send/receive NULLs
shouldn't be significant for low numbers of nodes or low node-node
connectivity.  So let's do (1) and see how well it works.  We can add
3(b) with adaptive N pretty easily later.

Should consult conservative PDES work on problem of adjusting N/epsilon.

*/

int CompleteStallingData(pm)
     Postmaster * pm;
{
  Implementation * ipm = pm->ipm;
  /* count number with no lookahead remaining */
  int todo, i, recd = 0;
#if LOOKAHEAD_DEBUG
  fprintf(stdout, "node %d: entering completestallingdata\n", ipm->mynode);
  fflush(stdout);
#endif
  for (i = todo = 0; i < ipm->nznodes; i++)
    if ((ipm->check_msg[i] & IN_PASSIVE) ||
	((ipm->exist_msg[i] & IN_ACTIVE) &&
	 ipm->remain_ahead[i] < ClockValue(0)))
      {
#if LOOKAHEAD_DEBUG
	fprintf(stdout, "node %d: expect active from %d\n", ipm->mynode, i);
	fflush(stdout);
#endif
	todo++;
      }
  /* loop until count is zero */
  while (todo)			/* don't really need to inc recd */
    {
#if LOOKAHEAD_DEBUG
      fprintf(stdout, "node %d: %d stalled still ...\n", ipm->mynode, todo);
      fflush(stdout);
#endif
      recd += ReceiveDataMessage(pm, pm->pvm_hang_time, &todo);
#if LOOKAHEAD_DEBUG
      fprintf(stdout, "node %d: %d stalled still ...\n", ipm->mynode, todo);
      fflush(stdout);
#endif
    }
  /* instead of this it could be smart to save PVM buffers with data */
  /* that doesn't need to be distributed.  We distribute it when it */
  /* becomes necessary OR when we find we have to stall.  Saves memory */
  /* and time because synchan uses more of both than PVM does */
  /* and may reduce overall stall time because the requisite data may */
  /* arrive while we're doing the work of distributing previously */
  /* buffered data.  Note PVM buffering is a big win over serial GENESIS */ 
  /* if a model expects lots of spikes to be queueing at a synchan - i.e. */
  /* if on average the number of spikes generated presynaptically during */
  /* the average axonal delay is high.  PVM buffering is buffering at */
  /* source rather than destination. */
#if LOOKAHEAD_DEBUG
  fprintf(stdout, "node %d: exiting completestallingdata\n", ipm->mynode);
  fflush(stdout);
#endif
  return recd;
}

void SendAndReceiveData(pm)
     Postmaster * pm;
{
  int nnodes;
  Implementation * ipm;
  int dst_node, sent, recd;
  int i,j;
  double dt = ClockValue(0);
  
  nnodes= pm->nnodes;
  ipm = pm->ipm;

  DBGMSG("getting field values...\n");
  /* gather the field values to be sent */
  if (pm->perfmon)
    Acct(PGENESIS_PROCESS_SNDREC_GETFIELD);
  GetFieldValues(pm);
  if (pm->perfmon)
    Acct(PGENESIS_PROCESS_SNDREC);
  /* destination by destination, send the data, */
  /* then try to recieve some data */
  /* from another node.  The variable <sent> holds the number of */
  /* destinations sent to less the number recieved from. */
  /* The order of destinations is held in the vector dst_order which */
  /* is computed in the CREATE action. */
      
  /* copy the spec of who sends/gets messages to checking buffer */
  bcopy(ipm->exist_msg, ipm->check_msg, nnodes);
  assert(ipm->num_in_nodes == ExistingOutgoingNodes(pm));
  assert(ipm->num_out_nodes == ExistingIncomingNodes(pm));
      
  /* decrement counts of how many steps we have left before stalling */
  for (i = 0; i < ipm->nznodes; i++)
    if ((ipm->exist_msg[i] & IN_ACTIVE) != 0)
      {/* remain_ahead values must be >= dt on entry to this loop */
	assert(ipm->remain_ahead[i] >= dt);
	ipm->remain_ahead[i] -= dt;
      }
  /* we must receive a message from any node for which remain_ahead is */
  /* now less than or equal dt, see call to CompleteStallingData below */

  /* for each destination node, send and interleave a recieve */
  for (sent = recd = dst_node = 0; dst_node < ipm->nznodes; dst_node++) {
    if ((ipm->check_msg[dst_node] & (OUT_ACTIVE | OUT_PASSIVE)) != 0)
      {
	/* send the data to the destination, using zone node */
	if (pm->perfmon)
	  Acct(PGENESIS_PROCESS_SNDREC_SND);
	SendDataMessage(pm, dst_node);
	if (pm->perfmon)
	  Acct(PGENESIS_PROCESS_SNDREC);
	sent++;
	    
	/* try to receive data from another node */
	if (recd < ipm->num_out_nodes)
	  {
	    if (pm->perfmon)
	      Acct(PGENESIS_PROCESS_SNDREC_REC);
	    for (i = 0, j = sent-recd; i < j; i++)
	      if (ReceiveDataMessage(pm, 0., NULL) != 0)
		recd ++;	/* inc and try again */
	      else
		break;		/* forget it */
	    if (pm->perfmon)
	      Acct(PGENESIS_PROCESS_SNDREC);
	  }	    
	if (sent == ipm->num_in_nodes)
	  break;
      }
  }
  DBGMSG("entering receive data loop...\n");
  /* recieve remaining incoming data */
  if (pm->perfmon)
    Acct(PGENESIS_PROCESS_SNDREC_REC);
  recd += CompleteStallingData(pm);
  assert(DataTransmissionCompleted(pm));
  if (pm->perfmon)
    Acct(PGENESIS_PROCESS_SNDREC);
  DBGMSG("exited receive data loop...\n");
}

int do_set_lookahead(argc, argv)
     int argc;
     char ** argv;

{
  Postmaster * pm = GetPostmaster();
  int i;
  double la;
  int node;
  double dt = ClockValue(0);
  
  if (pm)
    {
      Implementation * ipm = pm->ipm;
      switch (argc)
	{
	case 1:
#if LOOKAHEAD_DEBUG
	  fprintf(stdout, "lookahead computation not implemented yet\n");
	  fflush(stdout);
#endif
	  break;
	case 2:			/* global for all nodes */
	  sscanf(argv[1], "%lf", &la);
	  if (la < dt)
	    fprintf(stdout, "minimum lookahead is %g\n", dt);
	  else
	    for (i = 0; i < pm->ipm->nznodes; i++)
	      if ((ipm->exist_msg[i] & IN_ACTIVE) != 0)
		{
		  ipm->lookahead[node] = ipm->remain_ahead[node] = la;
		}
	  break;
	case 3:			/* per node */
	  sscanf(argv[1], "%d", &node);
	  sscanf(argv[2], "%lf", &la);
	  if (la < dt)
	    fprintf(stdout, "minimum lookahead is %f\n", dt);
	  else
	    if (node < 0 || node >= pm->ipm->nznodes)
	      fprintf(stdout,
		      "node number %d out of range for zone [%d - %d]\n",
		      node, 0, pm->ipm->nznodes);
	    else
	      if ((ipm->exist_msg[node] & IN_ACTIVE) != 0)
		{
		  ipm->lookahead[node] = ipm->remain_ahead[node] = la;
		}
	  break;
	default:
#if LOOKAHEAD_DEBUG
          fprintf(stdout, "use lookahead with one global argument\n");
	  fflush(stdout);
#endif
	  ;
	}
    }
  return 0;
}

int do_show_lookahead(argc, argv)
     int argc;
     char ** argv;

{
  Postmaster * pm = GetPostmaster();
  int i;
  double la;
  int node;
  double dt = ClockValue(0);
  
  if (pm)
    {
      Implementation * ipm = pm->ipm;
      for (i = 0; i < pm->ipm->nznodes; i++)
	if ((ipm->exist_msg[i] & IN_ACTIVE) != 0)
	    fprintf(stdout, "node %d: lookahead = %g, remaining = %g\n",
		    i, ipm->lookahead[i], ipm->remain_ahead[i]);
	else
	    fprintf(stdout, "node %d: no incoming active messages\n", i);
    }
}

float do_get_lookahead(argc, argv)
     int argc;
     char ** argv;

{
  Postmaster * pm = GetPostmaster();
  int i;
  double la;
  int node;
  double dt = ClockValue(0);
  
  if (argc != 2)
    fprintf(stdout, "usage: getlookahead <node number>\n");
  else
    if (pm)
      {
	Implementation * ipm = pm->ipm;
	sscanf(argv[1], "%d", &node);
	if ((ipm->exist_msg[node] & IN_ACTIVE) != 0)
	  return ((float) (ipm->lookahead[node]));
	else
	  return 0.;
      }
}

