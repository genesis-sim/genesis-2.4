static char rcsid[] = "$Id: rsync.c,v 1.2 2005/09/29 23:23:27 ghood Exp $";

/*
 * $Log: rsync.c,v $
 * Revision 1.2  2005/09/29 23:23:27  ghood
 * replaced dclock with pgen_clock since dclock was clashing with a system routine on the XT3
 *
 * Revision 1.1.1.1  2005/06/14 04:38:39  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.1  1999/12/19 03:59:43  mhucka
 * Adding PGENESIS from GENESIS 2.1 CDROM release.
 *
 * Revision 1.15  1997/05/19 17:59:34  ghood
 * Made barriers notify users (using "...") while waiting.
 *
 * Revision 1.14  1997/02/20 21:13:41  ngoddard
 * support for xpvm; using pvm_trecv for timeout ops; fixed bugs related to futures; provide user warnings when appears stalled; barrer and barrierall take a third argument: how long to wait before stall is assumed; beginnings of performance stats package
 *
 * Revision 1.13  1997/01/31 05:14:30  ngoddard
 * barriers don't complete asyncs now, minor bug fixes
 *
 * Revision 1.12  1996/08/19 21:34:02  ghood
 * Made pvm calls type-consistent with their parameters.
 *
 * Revision 1.11  1996/06/21 16:10:49  ghood
 * General debugging
 *
 * Revision 1.10  1995/06/26 19:29:56  ngoddard
 * fixed definitions of SyncZeroZone and note_sync_request
 * removed references to WaitSyncRequests
 *
 * Revision 1.9  1995/06/09  18:54:37  ngoddard
 * various fixes for ANSI C, function prototypes, and Solaris
 *
 * Revision 1.8  1995/06/06  19:49:11  ngoddard
 * zone masters weren't sending out global sync reply
 * now done by handler for zone sync reply
 *
 * Revision 1.7  1995/06/06  03:48:51  ngoddard
 * barrier handler sends response now
 *
 * Revision 1.6  1995/05/25  17:14:18  ngoddard
 * added function definition for compilation without prototypes
 *
 * Revision 1.5  1995/05/24  15:34:55  ngoddard
 * improved threads but still basic design flaw, also fixed comment leader in barrier tests
 *
 * Revision 1.4  1995/05/10  18:56:54  ngoddard
 * before making changes to allow ids on barriers
 *
 * Revision 1.3  1995/04/27  05:13:47  ngoddard
 * fixed bugs relating to in/outzone setup messages, passes zero, remote, addmsg tests
 *
 * Revision 1.2  1995/04/26  19:57:19  ngoddard
 * commenced allowing out-of-zone setups out of step
 *
 * Revision 1.1  1995/04/03  22:04:38  ngoddard
 * Initial revision
 *
 *
 */

#include "par_ext.h"
#include "pvm-defs.h"
#include "time.h"

/*
  
  Non-zero nodes in each zone only participate in local (zone-wide)
  syncs.  Each sends a request to the zone's zero node.  When the zero
  node decides the sync is complete, it sends back a sync reply and all
  nodes continue.  Local sync is implemented by sending the reply back
  as soon as every node has made a request.  Global (all zones) sync is
  implemented by sending a global sync request to the zero node of the
  zero zone, waiting for the global sync reply, and then sending the
  local sync reply back to all the zone's nodes.
  
  */

#if FUNC_PROT
/* private function prototypes to increase compile-time error detection */
static int WaitSync( Postmaster * pm, double sync_time, double note_time,
		    int req_typ, unsigned int sync_id);
static int wait_sync( Postmaster * pm, unsigned int * fptr,
		     unsigned int sync_fl, unsigned int sync_id,
		     double sync_time, double note_time, unsigned int * cptr,
		     int err1, int err2, char * buf);
static int WaitSyncReply( Postmaster * pm, double sync_time, double note_time,
			 int sync_src, int sync_dst, unsigned int syncid,
			 int req_typ);
static void note_sync_request(Postmaster * pm, int rep_typ,
			      unsigned int sync_id, int src, short * pcnt,
			      int clim, int * tids, int tidcnt,
			      unsigned int * fptr, char * buf);
static int SyncZeroZone(Postmaster * pm, unsigned int sync_id,
			double sync_time, double note_time,
			int req_typ, int rep_typ, int err1,
			unsigned int * fptr, short * cptr,
			int clim, int * tids, int tidcnt);
#else
static int WaitSync();
static int wait_sync();
static int WaitSyncReply();
static void note_sync_request();
static int SyncZeroZone();
#endif

FILE *event_log = NULL;

extern double pgen_clock();

static int WaitSync(pm, sync_time, note_time, req_typ, sync_id)
     Postmaster * pm;
     double sync_time, note_time;
     int req_typ;
     unsigned int sync_id;
{
  unsigned int sync_fl = 1 << sync_id;
  double end_time;
  Implementation * ipm = pm->ipm;
  unsigned int * fptr;		/* flag to check and reset */
  unsigned int * cptr;		/* sync_id flags */
  int cnt;			/* value for counter to reach */
  int err1, err2;
  int inzone = req_typ != PAR_ZONE_SYNC_REQ; /* others are in the zone */
  int temp;
  char buf[128];

  /*  ELog("Entering WaitSync"); */

  /* assert preconditions */
  assert(pm != NULL);
  assert(sync_time >= 0);
  assert(0 < sync_id && 32 > sync_id);
  assert(req_typ == PAR_SYNC_REQ ||
	 req_typ == PAR_GLOBAL_SYNC_REQ ||
	 req_typ == PAR_ZONE_SYNC_REQ);

  switch (req_typ) {
  case PAR_SYNC_REQ:
    fptr = &(ipm->wait_on_sync);
    cptr = ipm->synced;
    err1 = SYNC_HUNG;
    err2 = DUPLICATE_SYNC_IDS;
    sprintf(buf, "waiting for nodes to sync in barrier %d", sync_id);
    break;
  case PAR_GLOBAL_SYNC_REQ:
    fptr = &(ipm->wait_on_global_sync);
    cptr = ipm->global_synced;
    err1 = GLOBAL_SYNC_HUNG;
    err2 = DUPLICATE_GLOBAL_SYNC_IDS; 
    sprintf(buf, "waiting for nodes to sync in barrierall %d", sync_id);
   break;
  case PAR_ZONE_SYNC_REQ:
    fptr = &(ipm->wait_on_zone_sync);
    cptr = ipm->zone_synced;
    err1 = ZONE_SYNC_HUNG;
    err2 = DUPLICATE_ZONE_SYNC_IDS;
    sprintf(buf, "waiting for zones to sync in barrierall %d", sync_id);
    break;
  }
  temp = wait_sync(pm, fptr, sync_fl, sync_id, sync_time,
		   note_time, cptr, err1, err2, buf);
  /*   ELog("Leaving WaitSync"); */
  return (temp);
}

static int wait_sync(pm, fptr, sync_fl, sync_id, sync_time, 
		     note_time, synced, err1, err2, buf)
     Postmaster * pm;
     unsigned int * fptr;
     unsigned int sync_fl;
     unsigned int sync_id;
     double sync_time, note_time;
     unsigned int * synced;
     int err1, err2;
     char * buf;
{
  double end_time;

  /*   ELog("Entering wait_sync"); */
  if ((*fptr) & sync_fl) {
    ParError(err2, -1);
    return 0;
  }

  *fptr |= sync_fl;			/* note we are in a barrier */

  /* clear messages until all the guys have already synced with us */
  /* handlesyncreq sets synced & sync_fl to zero when everyone has synced */
  CLR_SETUP_COND_TIMEOUT(pm, sync_time, note_time, ((*synced) & sync_fl), buf);
/*
  end_time = pgen_clock() + sync_time;
  while (pgen_clock() < end_time && ((*synced) & sync_fl))
    ClearSetupMessages(pm, 1);
*/

  /* act as if we passed, even if we actually timed out, so others continue */
  *fptr &= ~sync_fl;			/* note we passed the barrier */

  /* did the barrier succeed? */
  if ((*synced) & sync_fl) { /* must have timed out */
    ParError(err1, -1);
    return 0;
  }
  /*   ELog("Leaving wait_sync"); */
  return 1;
}

SendSyncReply(pm, rep_typ, sync_id, tids, tidcnt)
     Postmaster * pm;
     int rep_typ;
     unsigned int sync_id;
     int * tids;
     int tidcnt;
{
  Implementation * ipm = pm->ipm;
  int inzone = rep_typ != PAR_ZONE_SYNC_REPLY; /* others are in the zone */

  /*   ELog("Entering SendSyncReply"); */
  /* assert preconditions */
  assert(tids != NULL);
  assert(tidcnt > 0);
  assert(pm != NULL);
  assert(0 < sync_id && 32 > sync_id);
  assert(rep_typ == PAR_SYNC_REPLY ||
	 rep_typ == PAR_GLOBAL_SYNC_REPLY ||
	 rep_typ == PAR_ZONE_SYNC_REPLY);

  /* notify requestors that we have passed the barrier */
  InitPvmMessage(pm);
  PackCore(pm, rep_typ);
  if (pvm_pkint(&(pm->ipm->mynode), 1, 1) < 0)
    { PvmError(); }
  if (pvm_pkuint(&sync_id, 1, 1) < 0)
    { PvmError(); }
  if (!SendSetupMessagesByTid(pm, tids, tidcnt, inzone))
    printf("Sending sync reply failed\n");
  /*   ELog("Leaving SendSyncReply"); */
  return 1;
}

static int WaitSyncReply(pm, sync_time, note_time, sync_src, sync_dst,
			 sync_id, req_typ)
     Postmaster * pm;
     double sync_time, note_time;
     int sync_src;		/* where this request comes from */
     int sync_dst;		/* where it is going */
     unsigned int sync_id;
     int req_typ;
{
  unsigned int sync_fl = 1 << sync_id;
  int inzone = req_typ != PAR_ZONE_SYNC_REQ; /* others are in the zone */
  Implementation * ipm = pm->ipm;
  unsigned int * fptr;
  int src, errtyp;
  double end_time;
  char buf[128];

  /*   ELog("Entering WaitSyncReply"); */
  /* assert preconditions */
  assert(pm != NULL);
  assert(sync_src == ipm->myznode || sync_src == ipm->myzone);
  assert(0 < sync_id && 32 > sync_id);
  assert(req_typ == PAR_SYNC_REQ ||
	 req_typ == PAR_GLOBAL_SYNC_REQ ||
	 req_typ == PAR_ZONE_SYNC_REQ);

  switch (req_typ) {
  case PAR_SYNC_REQ:
    fptr = &(pm->ipm->wait_on_sync);
    errtyp = SYNC_HUNG;
    sprintf(buf, "waiting for node 0 in barrier %d", sync_id);
    break;
  case PAR_GLOBAL_SYNC_REQ:
    fptr = &(pm->ipm->wait_on_global_sync);
    errtyp = GLOBAL_SYNC_HUNG;
    sprintf(buf, "waiting for node 0 in barrierall %d", sync_id);
    break;
  case PAR_ZONE_SYNC_REQ:
    fptr = &(pm->ipm->wait_on_zone_sync);
    errtyp = ZONE_SYNC_HUNG;
    sprintf(buf, "waiting for global node 0 in barrierall %d", sync_id);
    break;
  }

  assert((*fptr & sync_fl) == 0);
  *fptr |= sync_fl;

  /* pack and send a sync request message */
  InitPvmMessage(pm);
  PackCore(pm, req_typ);
  if (pvm_pkint(&sync_src, 1, 1) < 0)
    { PvmError(); }
  if (pvm_pkuint(&sync_id, 1, 1) < 0)
    { PvmError(); }
  SendKnownSetupMessage(pm, sync_dst, inzone);

  /* wait for the reply */
  end_time = pgen_clock() + sync_time * 2.0;
  CLR_SETUP_COND_TIMEOUT(pm, pm->msg_hang_time, note_time,
			 (*fptr & sync_fl), buf);
/*
  while ((pgen_clock() < end_time) && (*fptr & sync_fl)) {
    ClearSetupMessages(pm, 1);
  }
*/

  if (*fptr & sync_fl) {	/* must have timed out */
    ParError(errtyp, -1);
    return 0;
  }
  *fptr &= ~sync_fl;			/* note we passed the barrier */
  /*   ELog("Leaving WaitSyncReply"); */
  return 1;
}

static int SyncZeroZone(pm, sync_id, sync_time, note_time, req_typ, rep_typ,
			err1, fptr, cptr, clim, tids, tidcnt)
     Postmaster * pm;
     unsigned int sync_id;
     double sync_time, note_time;
     int req_typ, rep_typ, err1;
     unsigned int * fptr;
     short * cptr;
     int clim;
     int * tids;
     int tidcnt;
{
  unsigned int sync_fl = 1 << sync_id;
  char buf[256];
  Implementation * ipm = pm->ipm;

  /* ELog("Entering SyncZeroZone"); */
  if (fptr[0] & sync_fl)
    /* TODO: we should do some cleanup before returning */
    { ParError(err1, -1); return 0; }
  note_sync_request(pm, rep_typ, sync_id, 0, cptr, clim, tids, tidcnt,
		    fptr, buf);
  /* gather the local sync requests */
  if (!(WaitSync(pm, sync_time, note_time, req_typ, sync_id)))
    {
      /* ELog("Leaving SyncZeroZone"); */
      return 0;
    }
  /*  ELog("Leaving SyncZeroZone"); */
  return(1);
}

/* synchronize only the zone's nodes */
int SyncZoneNodes(pm, sync_id, sync_time, note_time)
     Postmaster * pm;
     unsigned int sync_id;
     double sync_time, note_time;
{
  unsigned int sync_fl;
  int i;
  Implementation * ipm = pm->ipm;
  
  /* ELog("Entering SyncZoneNodes"); */
  /* if only one node we're done */
  if (ipm->nznodes == 1) {
    /*    ELog("Leaving SyncZoneNodes (nznodes == 1)"); */
    return;
  }
  
  assert(sync_id >= 0 && sync_id < 32);

  /* convert numeric sync id to bit flag */
  sync_fl = 1 << sync_id;

  if (ipm->myznode == 0) {	/* zero node gathers sync requests */
    SyncZeroZone(pm, sync_id, sync_time,note_time,PAR_SYNC_REQ, PAR_SYNC_REPLY,
		 DUPLICATE_SYNC_IDS, ipm->synced, &(ipm->sync_cnt[sync_id]),
		 ipm->nznodes, ipm->zone_other_tids, ipm->nznodes-1);
  } else {			/* All other nodes in zone, must be some */
    if (!WaitSyncReply(pm, sync_time, note_time, ipm->myznode,
		       ipm->zone_init[pm->myzone], sync_id, PAR_SYNC_REQ))
      {
	/* ELog("Leaving SyncZoneNodes (0)"); */
	return 0;
      }
  }
  /* ELog("Leaving SyncZoneNodes (1)");  */
  return 1;
}

int SyncAllNodes(pm, sync_id, sync_time, note_time)
     Postmaster * pm;
     unsigned int sync_id;
     double sync_time, note_time;
{
  unsigned int sync_fl;
  int i;
  Implementation * ipm = pm->ipm;
  
  /*  ELog("Entering SyncAllNodes"); */
  /* if only one node we're done */
  if (ipm->nnodes == 1) {
    /*    ELog("Leaving SyncAllNodes (nnodes == 1)"); */
    return;
  }
  
  assert(sync_id >= 0 && sync_id < 32);

  /* convert numeric sync id to bit flag */
  sync_fl = 1 << sync_id;

  if (ipm->myznode == 0) {	/* zone's zero node gathers sync requests */
    SyncZeroZone(pm, sync_id, sync_time, note_time, PAR_GLOBAL_SYNC_REQ,
		 PAR_GLOBAL_SYNC_REPLY, DUPLICATE_GLOBAL_SYNC_IDS,
		 ipm->global_synced, &(ipm->global_sync_cnt[sync_id]),
		 ipm->nznodes, ipm->zone_other_tids, ipm->nznodes-1);
    /* sync other zones if any */
    if (ipm->nzones > 1)
      if (ipm->mynode == 0) {	/* global zero node syncs the zones */
	SyncZeroZone(pm, sync_id, sync_time, note_time, PAR_ZONE_SYNC_REQ,
		     PAR_ZONE_SYNC_REPLY, DUPLICATE_ZONE_SYNC_IDS,
		     ipm->zone_synced, &(ipm->zone_sync_cnt[sync_id]),
		     ipm->nzones, ipm->other_zone_tids, ipm->nzones-1);
      } else {
	/* send a global sync request to the zero zone's zero node and wait for reply */
	if (!(WaitSyncReply(pm, sync_time, note_time, pm->myzone,
			    ipm->zone_init[0], sync_id, PAR_ZONE_SYNC_REQ)))
	  {
	    /*	    ELog("Leaving SyncAllNodes (0) (case 1)"); */
	    return 0;
	  }
      }
  } else {			/* All other nodes in zone, must be some */
    if (!(WaitSyncReply(pm, sync_time, note_time, pm->mynode,
			ipm->zone_init[pm->myzone], sync_id,
			PAR_GLOBAL_SYNC_REQ)))
      {
	/*	ELog("Leaving SyncAllNodes (0) (case 2)"); */
	return 0;
      }
  }
  /*  ELog("Leaving SyncAllNodes (1)"); */
  return 1;
}

/* deal with reply from global zero node indicating other zones
   have reached the barrierall */
HandleSyncReply(pm,mtype)
     Postmaster * pm;
     int mtype;
{
  Implementation * ipm = pm->ipm;
  unsigned int sync_fl;
  unsigned int sync_id;
  int src, i;

  /*  ELog("Entering HandleSyncReply"); */
  if (pvm_upkint(&src, 1, 1) < 0)
    { PvmError(); }
  if (pvm_upkuint(&sync_id, 1, 1) < 0)
    { PvmError(); }
  
  assert(sync_id >= 0 && sync_id < 32);
  sync_fl = 1 << sync_id;
  switch (mtype) {
  case PAR_SYNC_REPLY:		/* from zone zero node, barrier reached */
    /* no zero node should receive this */
    assert(pm->mynode > 0);
    /* source should be zero node in my zone */
    assert(src == ipm->zone_init[ipm->myzone]);
    assert((ipm->wait_on_sync & sync_fl)); /* should be waiting for this sync */
    ipm->wait_on_sync &= ~sync_fl; /* reset bit */
    break;
  case PAR_GLOBAL_SYNC_REPLY:	/* from zone zero node, barrierall reached */
    /* no zero node should receive this */
    assert(pm->mynode > 0);
    /* source should be zero node in my zone */
    assert(src == ipm->zone_init[ipm->myzone]);
    assert((ipm->wait_on_global_sync & sync_fl)); /* should be waiting this id */
    ipm->wait_on_global_sync &= ~sync_fl; /* reset bit */
    break;
  case PAR_ZONE_SYNC_REPLY:	/* from global zero node, barrierall reached by other zones */
    /* only zone zero nodes should receive this */
    assert(pm->mynode == 0);
    /* source should be global zero node */
    assert(src == 0);
    assert((ipm->wait_on_zone_sync & sync_fl)); /* should be waiting this id */
    ipm->wait_on_zone_sync &= ~sync_fl; /* reset bit */
    /* now reset global_synced flags and notify others in zone */
    for (i = 0; i < pm->nnodes; i++)
      ipm->global_synced[i] &= ~sync_fl;
    ipm->global_sync_cnt[sync_id] = 0;
    if (ipm->nznodes > 1) {
      /* tell all the other nodes in the zone they can pass barrier */
      SendSyncReply(pm, PAR_GLOBAL_SYNC_REPLY, sync_id, ipm->zone_other_tids,
		    ipm->nznodes-1);
    }

    break;
  }
  /*  ELog("Leaving HandleSyncReply"); */
}

static void note_sync_request(pm, rep_typ, sync_id, src, pcnt, clim, tids, tidcnt,
		       fptr, err1)
     Postmaster * pm;
     int rep_typ;		/* the reply to send if any */
     unsigned int sync_id;	/* the id of the sync */
     int src;			/* index of requestor */
     short * pcnt;		/* pointer to counter */
     int clim;			/* counter limit */
     int * tids;		/* who to respond to */
     int tidcnt;		/* how many */
     unsigned int * fptr;	/* the flags saying who is in what barrier */
     char * err1;		/* error if duplicate */
{
  int i;
  unsigned int sync_fl = 1 << sync_id;
  char buf[256];

  /*  ELog("Entering note_sync_request"); */
  if ((fptr[src] & sync_fl) != 0) {	/* error if duplicate sync request */
    printf(err1);
  } else {
    fptr[src] |= sync_fl;	/* note it */
    (*pcnt)++;			/* increment counter */
    if (*pcnt == clim) {
      if (rep_typ != PAR_GLOBAL_SYNC_REPLY || pm->ipm->nzones == 1) {
	/* done, send continuation */
	/* global sync must wait until zone sync is done if multiple zones */
	SendSyncReply(pm, rep_typ, sync_id, tids, tidcnt);
	if (rep_typ == PAR_ZONE_SYNC_REPLY && pm->ipm->nznodes > 1)
	  /* send the global sync reply to this zone's nodes now */
	  SendSyncReply(pm, PAR_GLOBAL_SYNC_REPLY, sync_id,
			pm->ipm->zone_other_tids, pm->ipm->nznodes-1);
      }
      /* zero the synced lists before any requests can be handled */
      for (i = 0; i < clim; i++)
	fptr[i] &= ~sync_fl;	/* xor out the flag for each node */
      *pcnt = 0;		/* zero the counter */
    }
  }
  /*  ELog("Leaving note_sync_request"); */
}

/* deal with a sync request from a node */
/* return mtype if all have been received, 0 otherwise */
HandleSyncRequest(pm, mtype)
     Postmaster * pm;
     int mtype;
{
  int src;
  unsigned int sync_id, sync_fl, done = 0;
  Implementation * ipm = pm->ipm;
  char buf[256];

  /*  ELog("Entering HandleSyncRequest"); */

  /* only zone zero nodes get sync requests */
  assert(pm->mynode == 0);
  
  if (pvm_upkint(&src, 1, 1) < 0)
    { PvmError(); }
  if (pvm_upkuint(&sync_id, 1, 1) < 0)
    { PvmError(); }
  
  assert(sync_id >= 0 && sync_id < 32);
  sync_fl = 1 << sync_id;
  switch(mtype) {
  case PAR_SYNC_REQ:		/* from zone zero node, barrier reached */
    /* source should not be zero node, but be in my zone */
    assert(src > 0);
    assert(src < ipm->nznodes);

    sprintf(buf, "Error : 2 identical sync reqs (%d) from %d\n", sync_id, src);
    note_sync_request(pm, PAR_SYNC_REPLY, sync_id, src,
		      &(ipm->sync_cnt[sync_id]), ipm->nznodes,
		      ipm->zone_other_tids, ipm->nznodes-1,
		      ipm->synced, buf);

#if 0
    if(ipm->synced[src] & sync_fl != 0) {
      printf("Error : 2 identical sync reqs (%d) from %d\n",
	     sync_id, src);
    } else {
      ipm->synced[src] |= sync_fl;
      ipm->sync_cnt[sync_id]++;
    }
    if (ipm->sync_cnt[sync_id] == ipm->nznodes) { /* done, send continuation */
      SendSyncReply(pm, PAR_SYNC_REPLY, sync_id, 
		    ipm->zone_other_tids, ipm->nznodes-1);
      /* zero the synced lists before any requests can be handled */
      for (i = 0; i < ipm->nznodes; i++)
	ipm->synced[i] &= ~sync_fl;
      ipm->sync_cnt[sync_id] = 0;
    }
#endif
    break;
    
  case PAR_GLOBAL_SYNC_REQ:	/* from zone zero node, barrierall reached */
    /* source should not be zero node, but be in my zone */
    assert(src > 0);
    assert(src < ipm->nznodes);

    sprintf(buf, "Error : 2 identical global sync reqs (%d) from %d\n",
	    sync_id, src);
    note_sync_request(pm, PAR_GLOBAL_SYNC_REPLY, sync_id, src,
		      &(ipm->global_sync_cnt[sync_id]), ipm->nznodes,
		      ipm->zone_other_tids, ipm->nznodes-1,
		      ipm->global_synced, buf);

#if 0
    if(ipm->global_synced[src] & sync_fl != 0) {
      printf("Error : 2 identical global sync reqs (%d) from %d\n",
	     sync_id, src);
    } else {
      ipm->global_synced[src] |= sync_fl;
      ipm->global_sync_cnt[sync_id]++;
    }
#endif
    break;
  case PAR_ZONE_SYNC_REQ: /* barrierall reached by other zones */
    /* only global zero node should receive this */
    assert(ipm->mynode == 0);
    /* source should be another zero node */
    assert(src > 0 && src < ipm->nzones);

    sprintf(buf, "Error : 2 identical zone sync reqs (%d) from %d\n",
	    sync_id, src);
    note_sync_request(pm, PAR_ZONE_SYNC_REPLY, sync_id, src,
		      &(ipm->zone_sync_cnt[sync_id]), ipm->nzones,
		      ipm->other_zone_tids, ipm->nzones-1,
		      ipm->zone_synced, buf);

#if 0
    if(ipm->zone_synced[src] & sync_fl != 0) {
      printf("Error : 2 identical zone sync reqs (%d) from zone %d\n",
	     sync_id, src);
    } else {
      ipm->zone_synced[src] |= sync_fl;
      ipm->zone_sync_cnt[sync_id]++;
    }
#endif
    break;
  }
  /* ELog("Leaving HandleSyncRequest"); */
}

do_node_sync(argc,argv)
     int argc;
     char ** argv;
{
  Postmaster * pm = GetPostmaster();
  unsigned int sync_id;
  int res;
  double Atod();
  double sync_time, note_time;
  int temp;
  
  /*  ELog("Entering do_node_sync"); */
  if (argc>1) {
    sync_id=atoi(argv[1]);
    if (sync_id < 1 || sync_id > 24) {
      printf("Warning: barrier id is %d, must be in [1,24]. Setting to default \n",
	     sync_id);
      sync_id = PAR_DEFAULT_SYNC;
    }
  } else {
    sync_id = PAR_DEFAULT_SYNC;
  }
  
  if (pm == NULL)
    return;
  if (argc >= 3)
    sync_time = Atod(argv[2]);
  else
    sync_time = (double)(pm->msg_hang_time);

  if (argc >= 4)
    note_time = Atod(argv[3]);
  else
    note_time = (double) (pm->pvm_hang_time);

/* this causes barrier@all to deadlock and who knows what else
   use "waiton all; barrier" instead
  RequestRcommandsCompleted(pm);
  WaitFuturesResolved(pm);
  WaitRcommandsCompleted(pm);
*/
  temp = SyncZoneNodes(pm, sync_id, sync_time, note_time);
  /*  ELog("Leaving do_node_sync"); */
  return(temp);
}

do_all_node_sync(argc,argv)
     int argc;
     char ** argv;
{
  Postmaster * pm = GetPostmaster();
  double Atod();
  double sync_time, note_time;
  unsigned int sync_id;
  int res;
  int temp;

  /*  ELog("Entering do_all_node_sync"); */
  if (argc>1) {
    sync_id=atoi(argv[1]);
    if (sync_id < 1 || sync_id > 24) {
      printf("Warning: barrierall id is %d, must be in [1,24].  Setting to default \n",
	     sync_id);
      sync_id = PAR_DEFAULT_ALL_SYNC;
    }
  } else {
    sync_id = PAR_DEFAULT_ALL_SYNC;
  }
  
  if (pm == NULL)
    return;
  if (argc >= 3)
    sync_time = Atod(argv[2]);
  else
    sync_time = (double)(pm->msg_hang_time);

  if (argc >= 4)
    note_time = Atod(argv[3]);
  else
    note_time = (double) (pm->pvm_hang_time);

/* this causes barrier@all to deadlock and who knows what else
   use "waiton all; barrier" instead
  RequestRcommandsCompleted(pm);
  WaitFuturesResolved(pm);
  WaitRcommandsCompleted(pm);
*/
  temp = SyncAllNodes(pm, sync_id, sync_time, note_time);
  /* ELog("Leaving do_all_node_sync"); */
  return(temp);
}

void
ELog(s)
     char *s;
{
  if (event_log == NULL)
    InitELog();
#ifdef CRAY
  fprintf(event_log, "Time: %18.6f", rtclock()/ ((double) CLK_TCK));
#else
  fprintf(event_log, "Time: %18.6f", (double) time(NULL));
#endif  
  fprintf(event_log, " %s\n", s);
  fflush(event_log);
}

InitELog()
{
  char filename[32];
  Implementation * ipm;
  Postmaster *pm;

  pm = GetPostmaster();
  ipm = pm->ipm;
  sprintf(filename, "ELog%d", ipm->mynode);
  event_log = fopen(filename, "w");
  ELog("Event log opened.");
  /*  ISLog(ELog); */  /* hook into serial genesis */
}
