static char rcsid[] = "$Id: pvm-shared.c,v 1.2 2005/09/29 23:22:02 ghood Exp $";

/*
 * $Log: pvm-shared.c,v $
 * Revision 1.2  2005/09/29 23:22:02  ghood
 * updated for PGENESIS 2.3
 *
 * Revision 1.1.1.1  2005/06/14 04:38:39  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.5  2003/03/28 19:02:14  gen-dbeeman
 *
 * Fix from Greg Hood for pgenesis 2.2.1 to prevent a hang waiting for
 * other nodes to sync when barriers are used.
 *
 * Revision 1.4  2002/01/17 18:44:57  dbeeman
 * Bug fixes from Greg Hood for pgenesis 2.2a, entered by dbeeman
 *
 * Revision 1.3  2000/06/19 04:23:55  mhucka
 * Fix from Greg Hood:
 * Now uses nm on Linux systems to obtain the address for XgEventLoop.
 *
 * Revision 1.2  1999/12/19 04:01:21  mhucka
 * Fix for compiling under egcs under Red Hat 6.x
 *
 * Revision 1.22  1997/08/12 08:17:07  ghood
 * Origin uses nlist64 in 64-bit mode
 *
 * Revision 1.21  1997/08/10 23:46:57  ghood
 * now we check for either XgEventLoop or _XgEventLoop
 *
 * Revision 1.20  1997/07/14 23:40:06  ngoddard
 * LINUX doesn't prepend _ to symbol names
 *
 * Revision 1.19  1997/07/14 05:08:14  ngoddard
 * Redhat Linux 4.2 doesn't have nlist
 *
 * Revision 1.18  1997/07/07 05:39:32  ngoddard
 * function prototpyes for sunos
 *
 * Revision 1.17  1997/06/24 16:53:52  ghood
 * xgEventLoop no longer static
 *
 * Revision 1.16  1997/03/27 06:26:50  ngoddard
 * mods to scale up to larger networks
 *
 * Revision 1.15  1997/02/28 05:24:52  ngoddard
 * support for lookahead optimisation, bug fixes
 *
 * Revision 1.14  1997/02/20 21:13:39  ngoddard
 * support for xpvm; using pvm_trecv for timeout ops; fixed bugs related to futures; provide user warnings when appears stalled; barrer and barrierall take a third argument: how long to wait before stall is assumed; beginnings of performance stats package
 *
 * Revision 1.13  1997/01/31 05:14:27  ngoddard
 * barriers don't complete asyncs now, minor bug fixes
 *
 * Revision 1.12  1997/01/21 22:20:26  ngoddard
 * paragon port
 *
 * Revision 1.11  1997/01/21 20:19:48  ngoddard
 * bug fixes for zones, implement string literal fields, first release of pgenesis script
 *
 * Revision 1.10  1996/08/13 21:25:06  ghood
 * General debugging to get parallel example scripts working.
 *
 * Revision 1.9  1996/06/21 16:10:46  ghood
 * General debugging
 *
 * Revision 1.8  1996/05/03 19:48:13  ghood
 * moving to biomed area
 *
 * Revision 1.7  1995/10/03  17:56:06  geigel
 * Updates for T3D port
 *
 * Revision 1.6  1995/05/24  15:34:49  ngoddard
 * improved threads but still basic design flaw, also fixed comment leader in barrier tests
 *
 * Revision 1.5  1995/05/10  18:56:50  ngoddard
 * before making changes to allow ids on barriers
 *
 * Revision 1.4  1995/04/27  05:13:44  ngoddard
 * fixed bugs relating to in/outzone setup messages, passes zero, remote, addmsg tests
 *
 * Revision 1.3  1995/04/26  19:57:15  ngoddard
 * commenced allowing out-of-zone setups out of step
 *
 * Revision 1.2  1995/04/18  21:28:13  ngoddard
 * *** empty log message ***
 *
 * Revision 1.1  1995/04/03  22:04:26  ngoddard
 * Initial revision
 *
 * Revision 1.1  1994/09/07  19:01:00  ngoddard
 * passive messaging works, src figures map
 *
 * Revision 1.5  1993/12/30  00:06:13  ngoddard
 * Release 12/14/93
 *
 * Revision 1.2  1993/10/15  22:51:51  ngoddard
 * added Id and Log keywords
 *
 *
*/


/****************************************************************
**                                                             **
**                     pvm-shared.c :                          **
** A set of routines for handling all the internode            **
** communications. This code is shared by both the master      **
** and the slave processes.                                    **
**                                                             **
****************************************************************/

#include "par_ext.h"
#include "tvdefs.h"

#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#include "pvm-defs.h"
#include <sys/resource.h>
#include <fcntl.h>
#if defined(T3D) || defined(PGON) || defined(linux)
#else
#include <nlist.h>
extern char *progname;
#endif

extern double (DoubleMessageData)();
extern double (FloatMessageData)();
extern double (IntMessageData)();
extern double (ShortMessageData)();
extern double (StringMessageData)();
extern double (ExtFieldMessageData)();
extern double (IndFieldMessageData)();
     

/* these should be in the postmaster object
**
** tid_array contains the tids in order by nodenum, with the
** master in tid_array[0]
** others_tid_array is the same except mytid has been removed
** so there are one fewer elements in the array
*/

int parlib_step = 0;
static int parallel_enable_flag;
PFI xgEventLoop = NULL;
double  pgen_clock();
int	ClearSetupMessages();
int	ClearOneMessage();
int	IncomingSetupCheck();
int     BadDest();

void ParError(etype, silent)
     int etype, silent;
{
  fprintf(stderr, "Parlib error %d: ", etype);
  switch(etype) {
  case MSG_OUT_OF_ZONE:
    fprintf(stderr,"inter-zone messages not allowed");
    break;
  case MSG_INVALID_DST_NODE:
    fprintf(stderr,"invalid destination node");
    break;
  case OUT_OF_MEMORY:
    fprintf(stderr,"exhausted memory");
    break;
  case UNPACK_CORE_FAIL:
    fprintf(stderr,"failed to unpack core fields");
    break;
  case GLOBAL_SEND_FAIL:
    fprintf(stderr,"failed a global send");
    break;
  case SEND_FAIL:
    fprintf(stderr,"failed a send");
    break;
  case SPAWN_FAILURE:
    fprintf(stderr,"failed to spawn workers");
    break;
  case COMPLETE_PASSIVE_MSG_FAIL:
    fprintf(stderr,"failed while trying to complete a passive message");
    break;
  case BAD_FIELD_COUNT:
    fprintf(stderr,
	    "remote node expected different number of fields in message");
    break;
  case BAD_TYPE:
    fprintf(stderr,"remote node reports no such message type");
    break;
  case PAR_OK:
    fprintf(stderr,"shouldn't be here");
    break;
  case POINTER_ALREADY_RELEASED:
    fprintf(stderr,"held pointer already released");
    break;
  case PAR_REQUEST_ERROR:
    fprintf(stderr,"errors in a raddmsg");
    break;
  case PAR_REQUEST_PASSIVE:
    fprintf(stderr,"errors in a raddmsg (passive)");
    break;
  case PAR_REQUEST_ACTIVE:
    fprintf(stderr,"errors in a raddmsg (active)");
    break;
  case MIXED_TYPE:
    fprintf(stderr,"both active and passive message types in one raddmsg");
    break;
  case MSG_INVALID_DST_ZONE:
    fprintf(stderr,"invalid destination zone");
    break;
  case UNMATCHED_SYNC_IDS:
    fprintf(stderr,"unmatched barrier id numbers");
    break;
  case DUPLICATE_SYNC_IDS:
    fprintf(stderr,"duplicate barrier id numbers");
    break;
  case DUPLICATE_GLOBAL_SYNC_IDS:
    fprintf(stderr,"duplicate barrierall id numbers");
    break;
  case DUPLICATE_ZONE_SYNC_IDS:
    fprintf(stderr,"duplicate barrierall id numbers in zone sync");
    break;
  case SYNC_HUNG:
    fprintf(stderr,"barrier hung (timed out)");
    break;
  case GLOBAL_SYNC_HUNG:
    fprintf(stderr,"barrierall hung in local zone barrier (timed out)");
    break;
  case ZONE_SYNC_HUNG:
    fprintf(stderr,"barrierall hung in syncing zone leaders (timed out)");
    break;
  case MULTI_SEND_FAIL:
    fprintf(stderr,"multiple send failed");
    break;
  case COMMAND_CLEAR_HUNG:
    fprintf(stderr,"failed to clear pending rcommands");
    break;
  case FAILED_FUTURE_ALLOCATION:
    fprintf(stderr,"failed to allocate a future index");
    break;
  case NO_POSTMASTER:
    fprintf(stderr, "postmaster doesn't exist");
    break;
  case UNIMPLEMENTED_SLOT_FUNCTION:
    fprintf(stderr, "slot function unimplemented");
    break;
  case UNKNOWN_SLOT_TYPE:
    fprintf(stderr, "slot type not recognized");
    break;
  default:
    fprintf(stderr,"unknown type");
  }
  fprintf(stderr, "\n");
  fflush(stderr);
  abort();
}

int PvmId(pm, node)
     Postmaster * pm;
     int node;
{
  return (pm->ipm->tid_array[node]);
}

int PvmError()
{
  fprintf(stderr, "***ERROR: pvm call failed!\n");
  abort();
}

static void UsePvmBuf(pm)
     Postmaster * pm;
{
  assert(pm->ipm->pvm_buf_ready == 1);
  pm->ipm->pvm_buf_ready = 0;
}

void ReusePvmMessage(pm)
     Postmaster * pm;
				/* reuse existing buffer */
{
  assert(pm->ipm->pvm_buf_ready == 0);
  pm->ipm->pvm_buf_ready = 1;
}  

int SendStartupMessage(pm, tids, tidcnt, mtype)
     Postmaster * pm;
     int * tids;
     int tidcnt;
     int mtype;
{
  UsePvmBuf(pm);
  pvm_mcast(tids, tidcnt, mtype);
}

int SendPvmMessages(pm, nodes, nodecnt, mtype)
     Postmaster * pm;
     int * nodes;
     int nodecnt;
     int mtype;
{
  static int * tids;
  static int tid_cnt = 0;
  int * ptids = tids;

  assert(nodes != NULL);
  assert(nodecnt > 0);

  if (tid_cnt < nodecnt) {
    if (tid_cnt > 0)
      free(tids);
    ptids = tids = (int *) malloc((tid_cnt = nodecnt)*sizeof(int));
  }

  while (--nodecnt >= 0)
    *(ptids++) = PvmId(pm, *(nodes++));

  UsePvmBuf(pm);
  if (pvm_mcast(tids, tid_cnt, mtype) < 0) {
    ParError(MULTI_SEND_FAIL, -1);
    return 0;
  }
  return (tid_cnt);
}

/* return the number of tids sent to */
int SendPvmMessage(pm, node, mtype)
     Postmaster * pm;
     int node;
     int mtype;
{
  int dst_tid;
  assert(node >= 0);
  dst_tid = PvmId(pm, node);
  UsePvmBuf(pm);
#if LOOKAHEAD_DEBUG
  fprintf(stdout, "node %d (%x) sending msg type %d to node %d (%x)\n",
	  pm->ipm->mynode, pm->ipm->tid_array[pm->ipm->mynode], mtype,
	  node, dst_tid);
  fflush(stdout);
#endif
  if (pvm_send(dst_tid, mtype) < 0) {
    ParError(SEND_FAIL, -1);
    return 0;
  }
#if LOOKAHEAD_DEBUG
  fprintf(stdout, "node %d msg sent\n", pm->ipm->mynode);
  fflush(stdout);
#endif
  return 1;
}

int SendPvmReply(pm, dst_tid, mtype)
     Postmaster * pm;
     int dst_tid, mtype;
{
  UsePvmBuf(pm);
  return (pvm_send(dst_tid, mtype) >= 0);
}

int SendLocalSetupMessage(pm, node)
     Postmaster * pm;
     int node;
{
  return (SendPvmMessage(pm, ZoneNodeToGlobalNode(pm, node), LOCAL_SETUP));
}

int SendLocalDataMessage(pm, node)
     Postmaster * pm;
     int node;
{
  return (SendPvmMessage(pm, ZoneNodeToGlobalNode(pm, node), LOCAL_DATA));
}

/* returns 0 on failure */
int SendSomeSetupMessageByTid(pm, dst_tid)
     Postmaster * pm;
     int dst_tid;
{
  /* for now assume remote - these are all replies */
  return (SendRemoteSetupMessageByTid(pm, dst_tid));
}

int SendLocalSetupMessageByTid(pm, dst_tid)
     Postmaster * pm;
     int dst_tid;
{
  UsePvmBuf(pm);
  return (pvm_send(dst_tid, LOCAL_SETUP) >= 0);
}

int SendRemoteSetupMessageByTid(pm, dst_tid)
     Postmaster * pm;
     int dst_tid;
{
  UsePvmBuf(pm);
  return (pvm_send(dst_tid, REMOTE_SETUP) >= 0);
}

/* don't know if in zone or not */
int SendSomeSetupMessage(pm, node)
     Postmaster * pm;
     int node;
{
  return (SendPvmMessage(pm, node,
			 (ZoneNode(pm, node) ? LOCAL_SETUP : REMOTE_SETUP)));
}

/* do know if in zone or not */
int SendKnownSetupMessage(pm, node, inzone)
     Postmaster * pm;
     int node;
     int inzone;
{
  return (SendPvmMessage(pm, node,(inzone ? LOCAL_SETUP : REMOTE_SETUP)));
}

/* don't know if in zone or not */
int SendSomeSetupMessages(pm, dst_nodes, node_cnt)
  Postmaster * pm;
  int * dst_nodes;
  int node_cnt;

{
  /* for now do the simple-minded thing */
  int i, res;
  for (i = res = 0; i < node_cnt; i++) {
    if (i > 0)		/* reuse the PVM message after initial use */
      ReusePvmMessage(pm);
    res += SendSomeSetupMessage(pm, dst_nodes[i]);
  }
  return res;
}

int SendSetupMessagesByTid(pm, tids, tidcnt, inzone)
     Postmaster * pm;
     int * tids;
     int tidcnt;
     int inzone;		/* local? */
{
  UsePvmBuf(pm);
  if (pvm_mcast(tids, tidcnt,
		(inzone ? LOCAL_SETUP : REMOTE_SETUP)) < 0) {
    ParError(MULTI_SEND_FAIL, -1);
    return 0;
  }
  return tidcnt;
}
     
int SendSetupMessageByTid(pm, dst_tid, inzone)
     Postmaster * pm;
     int dst_tid;
     int inzone;		/* local? */
{
  UsePvmBuf(pm);
  if (pvm_send(dst_tid, (inzone ? LOCAL_SETUP : REMOTE_SETUP)) < 0) {
    ParError(SEND_FAIL, -1);
    return 0;
  }
  return 1;
}
     
Postmaster * GetPostmaster()
{
  return ((Postmaster *) GetElement("/post"));
}

void PackCore(pm, mtype)
     Post * pm;
     int mtype;
{
  int step = GetParlibStep();
  pvm_pkint(&mtype,1,1);
  pvm_pkint(&step,1,1);
  pvm_pkint(&(pm->myzone),1,1);
#if 0
  pvm_pkint(&(pm->mynode),1,1);
#endif
}

int UnpackCore(bufid, src_tid, tag, mtype, step, zone)
     int bufid, *src_tid, *tag, *mtype, *step, *zone;
{
  int len;
  if (pvm_bufinfo (bufid, &len, tag, src_tid) < 0) return 0;
  if (pvm_upkint(mtype,1,1) < 0) return 0;
  if (pvm_upkint(step,1,1) < 0) return 0;
  if (pvm_upkint(zone,1,1) < 0) return 0;
#if 0
  if (pvm_upkint(&node,1,1) < 0) return 0;
#endif

  return 1;
}

void InitPvmMessage(pm)
     Postmaster * pm;
				/* start a new buffer */
{
  pvm_initsend(pm->ipm->encoding);
  ReusePvmMessage(pm);
}

/* convert a pvm tid to a genesis node number
** tid_array contains the tids in order by nodenum, EXCEPT:
** tid_array[0] and mytid have been switched
*/

static int pvmid_to_nodenum(pm, tid)
     Postmaster * pm;
     int tid;
{
  int res, *tid_array = pm->ipm->tid_array;
  int nnodes = pm->ipm->nnodes;

  for (res = 0;
       res < nnodes && tid != *tid_array;
       res++, tid_array++);
  return (res);
}

/* This function tries to receive a PVM message of type mtag */
/* with timout tim.  It handles X events every so often while stalled */
int ReceiveSomeMessage(pm, mtag, tim)
     Postmaster * pm;
     int mtag;			/* message type wanted */
     struct timeval * tim;		/* timout */
{
  int bufid = -1;

  assert(tim->tv_sec >= 0 && tim->tv_usec >= 0);
  if (xgEventLoop != NULL)
    {
      struct timeval timout, timdone, timcur, timupd;
      timupd.tv_sec = pm->xupdate_period; /* check X events */
      timupd.tv_usec = ((pm->xupdate_period - timupd.tv_sec) * 1000000);
      gettimeofday(&timcur, (struct timezone*)0);
      TVXADDY(&timdone, &timcur, tim); /* termination time */
      do
	{
	  /* handle pending X events */
	  (*(xgEventLoop))();
	  TVXSUBY(&timout, &timdone, &timcur);
	  if (TVXLTY(&timupd, &timout))
	    {
	      timout.tv_sec = timupd.tv_sec;
	      timout.tv_usec = timupd.tv_usec;
	    }
	  if (!TVISSET(&timout))
	    {
#if PVM_TRECV_FIXED
	      /* this should be equivalent to pvm_nrecv but it isn't */
	      bufid = pvm_trecv(-1, mtag, &timout);
#else
	      bufid = pvm_nrecv(-1, mtag);
#endif
	    }
	  else
	    bufid = pvm_trecv(-1, mtag, &timout);
	  if (bufid > 0) break;
	  gettimeofday(&timcur, (struct timezone*)0);
	}
      while (TVXLTY(&timcur, &timdone));
    }
  else
    {
      /* just wait for the message, or not */
      if (!TVISSET(tim))
	{
#if PVM_TRECV_FIXED
	/* this should be equivalent to pvm_nrecv but it isn't */
	  bufid = pvm_trecv(-1, mtag, tim);
#else
	  bufid = pvm_nrecv(-1, mtag);
#endif
	}
      else
	bufid = pvm_trecv(-1, mtag, tim);
    }
  return bufid;
}

/* This function clears up to count messages, waiting no longer than
   timeout before giving up,  It returns the number
   cleared or -n if there was an error, where n > 0 is 1 plus the
   number cleared including the one causing the error
*/
int ClearSetupMessages(pm, count, tim)
     Postmaster * pm;
     int count;			/* number of messages to clear */
     float tim;			/* number of seconds to wait before timeout */
     /* n >= 0 means up to n */
     /* -1 means clear messages until none left */
{
  int ret = 0;
  int bufid, cnt = 0;
  struct timeval timout;

  /* if thread handing is off, do nothing */
  if (!parallel_enable_flag)
    return(0);

  assert (tim >= 0.);
  timout.tv_sec = tim;
  timout.tv_usec = (tim - timout.tv_sec)*1000000;
  while (count != 0)		/* handle up to count new message(s) */
    {
      bufid = ReceiveSomeMessage(pm, PAR_SETUP_MESSAGE, &timout);
      if (bufid == 0)		/* timed out */
	return cnt;
      ret = ProcessSetupMessage(pm, bufid);
      if (count > 0)
	count--;
      cnt++; /* increment number handled */
      if (ret < 0)
	return (-1 - cnt);
    }
  return cnt;
}

int ProcessSetupMessage(pm, bufid)
     Postmaster * pm;
     int bufid;
{
  int src_tid, ret = 0, mtype, tmtag, step, zone;

  if (!UnpackCore(bufid, &src_tid, &tmtag, &mtype, &step, &zone))
    { ParError(UNPACK_CORE_FAIL, -1);   return (-1); }
  /* process this setup message */
  ret = HandleSetupMessage (pm, src_tid, mtype);
  return ret;
}

PFI GetxgEventLoop()
{
  return xgEventLoop;
}

int ClearSpecificSetupMessages (pm, srcnode)
     Postmaster *pm;
     int srcnode;
{
  /* interim hack till I get the parallel stuff cleaned up */
  /* should clear messages from srcnode only */
  return(ClearSetupMessages(pm, 1, 0.));
}

static preinit_implementation(ipm)
     Implementation * ipm;
{
  ipm->tid_array = NULL;
  ipm->encoding = PVM_DEF_PACK;
  ipm->pvm_buf_ready = 0;
}

/* TODO: a lot of the vectors below are indexed by zone node, not
   global node, so we need only allocat nznodes of them instead of nnodes
   except we don't know nznodes at this point.
*/
static postinit_implementation(pm, ipm)
     Postmaster * pm;
     Implementation * ipm;
{
  int nnodes = ipm->nnodes = pm->nnodes, i;
  ipm->mynode = pm->mynode;
  ipm->add_msg = 0;
  /* allocate pointers to the BufferManagers for each node */
  ipm->out_data = (BufferManager**)calloc(nnodes,sizeof(BufferManager*));
  ipm->out_active = (BufferManager**)calloc(nnodes,sizeof(BufferManager*));
  ipm->in_data = (BufferManager**)calloc(nnodes,sizeof(BufferManager*));
  ipm->pending_field = (BufferManager**)calloc(nnodes,sizeof(BufferManager*));
  ipm->pending_index = (BufferManager**)calloc(nnodes,sizeof(BufferManager*));
  ipm->in_active = (unsigned short **) calloc(nnodes,sizeof(unsigned short *));
  ipm->in_active_time = (double **) calloc(nnodes,sizeof(double *));
  ipm->in_active_cnt = (unsigned short *)calloc(nnodes,sizeof(unsigned short));
  ipm->in_active_siz = (unsigned short *)calloc(nnodes,sizeof(unsigned short));
  ipm->out_active_time = NULL;
  ipm->out_active_idx = NULL;
  ipm->out_active_siz = 0;
  ipm->exist_msg = (unsigned char *) calloc(nnodes, sizeof(unsigned char));
  ipm->check_msg = (unsigned char *) calloc(nnodes, sizeof(unsigned char));
  ipm->lookahead = (double *) calloc(nnodes, sizeof(double));
  ipm->remain_ahead = (double *) calloc(nnodes, sizeof(double));
  ipm->most_recent = (double *) calloc(nnodes, sizeof(double));

  ipm->wait_on_sync = 0;
  ipm->wait_on_global_sync = 0;

  ipm->buffered_msgs = (MsgSpec **) calloc(nnodes,sizeof(MsgSpec *));
  ipm->buffered_msg_cnt = (int *) calloc(nnodes,sizeof(int));
  ipm->held_pointers = NULL;
  ipm->num_in_nodes = 0;
  ipm->num_out_nodes = 0;
  ipm->access_func[0] = DoubleMessageData;
  ipm->access_func[1] = FloatMessageData;
  ipm->access_func[2] = IntMessageData;
  ipm->access_func[3] = ShortMessageData;
  ipm->access_func[4] = StringMessageData;
  ipm->access_func[5] = ExtFieldMessageData;
  ipm->access_func[6] = IndFieldMessageData;
  ipm->access_func[7] = NULL;

  ipm->nores_rcmds_sent = (unsigned int *)
    calloc(nnodes, sizeof(unsigned int));
  ipm->nores_rcmds_handled = (unsigned int *)
    calloc(nnodes, sizeof(unsigned int));
  ipm->nores_rcmds_cnt = 0;
  ipm->future = NULL;
  ipm->future_cnt = 0;

  ipm->rcmd_done_head = NULL;
  ipm->free_rcmd_done = NULL;

  pm->remote_info = 0;
  pm->delete_msgs = 0;
  pm->delete_elts = 0;
  pm->copy_elts = 0;
  pm->sync_before_step = 1;
  pm->min_delay = 1;
  pm->remote_error_report = REMOTE_ERROR_DST;
  pm->msg_hang_time = (float)HANG_TIME;
  pm->pvm_hang_time = 3.0;   /* seconds to stall in pvm */
  pm->xupdate_period = .01;  /* process X events every 10 msec when stalled */
}

int EnrollInPvm()
{
  int tid;

  if (IsSilent() < 2)
    {
      printf("Enrolling in PVM...");
      fflush(stdout);
    }

  tid = pvm_mytid();		/* enroll in PVM */
  if (tid < 0)
    {
      /* failed to enroll */
      fprintf(stdout," failed! ... is the PVM demon running?\n");
      fflush(stdout);
      exit(1);
    }
  
  if (IsSilent() < 2)
    {
      fprintf(stdout, "ok, tid is %x\n", tid);
      fflush(stdout);
    }
#if (USE_PVM) && (XPVM)
  {
#include "pvmtev.h" 

    Pvmtmask trace_mask;
    int xpvm_tid;
    int i;

    if ( (xpvm_tid = pvm_gettid("xpvm",0)) > 0)
      {
	printf("Setting up XPVM ttid:%i\n",xpvm_tid);
	
	/* Set Self Trace & Output Destinations */
	pvm_setopt( PvmSelfTraceTid, xpvm_tid);
	pvm_setopt( PvmSelfTraceCode, 666);
	pvm_setopt( PvmSelfOutputTid, xpvm_tid);
	pvm_setopt( PvmSelfOutputCode, 667);
		
	pvm_setopt( PvmTraceTid, xpvm_tid);
	pvm_setopt( PvmTraceCode, 666);
	pvm_setopt( PvmOutputTid, xpvm_tid);
	pvm_setopt( PvmOutputCode, 667);

	/* debugging */
	printf("PvmSelfTraceTid %i\n",pvm_getopt( PvmSelfTraceTid));
	printf("PvmSelfTraceCode %i\n",pvm_getopt( PvmSelfTraceCode));
	printf("PvmSelfOutputTid %i\n",pvm_getopt( PvmSelfOutputTid));
	printf("PvmSelfOutputCode %i\n",pvm_getopt( PvmSelfOutputCode));
	  
	printf("PvmTraceTid %i\n",pvm_getopt( PvmTraceTid));
	printf("PvmTraceCode %i\n",pvm_getopt( PvmTraceCode));
	printf("PvmOutputTid %i\n",pvm_getopt( PvmOutputTid));
	printf("PvmOutputCode %i\n",pvm_getopt( PvmOutputCode));
	/* */
	
	TEV_INIT_MASK( trace_mask);
	/*
	   TEV_SET_MASK( trace_mask, TEV_MCAST0 );
	   TEV_SET_MASK( trace_mask, TEV_SEND0 );
	   TEV_SET_MASK( trace_mask, TEV_RECV0 );
	   TEV_SET_MASK( trace_mask, TEV_NRECV0 );
	   */
	for (i = TEV_FIRST+1; i < TEV_MAX; i++)
	  TEV_SET_MASK( trace_mask, i );

	pvm_settmask( PvmTaskSelf, trace_mask);
	
	pvm_settmask( PvmTaskChild, trace_mask);
      }
    else
      printf("XPVM not running, cannot trace\n");
  }
#endif

  return tid;
}

PFI LookupNamedFunction (name)
     char *name;
{
#if defined(BGL)
  return((PFI) NULL);
#elif defined(Linux)
  FILE *f;
  unsigned int value;
  char *cmd;
  cmd = (char *) malloc(strlen(progname) + 128);
  sprintf(cmd, "nm -g %s |grep 'T XgEventLoop'", progname);
  f = popen(cmd, "r");
  if (fscanf(f, "%x", &value) != 1)
    value = 0;
  pclose(f);
  free(cmd);
  return((PFI) value);
#elif defined(decalpha) || defined(irix) || defined(Solaris) || defined(MacOSXDarwin) || defined(mips) || defined(hpux) || defined(aix)
#if defined(_MIPS_SZPTR) && (_MIPS_SZPTR == 64)
  struct nlist64 nl[2];
#else
  struct nlist nl[2];
#endif

  nl[0].n_name = name;
  nl[1].n_name = NULL;
#if defined(_MIPS_SZPTR) && (_MIPS_SZPTR == 64)
  nlist64(progname, &nl[0]);
#else
  nlist(progname, &nl[0]);
#endif
  if (nl[0].n_value > 0)
    return((PFI) nl[0].n_value);
  else
    return((PFI) NULL);
#else
  return((PFI) NULL);
#endif
}

int ParallelStartup(pm,action)
     Post	*pm;
     Action * action;
{
  int i, parent, id, tid;

  xgEventLoop = LookupNamedFunction("XgEventLoop");
  if (xgEventLoop == (PFI) NULL)
    xgEventLoop = LookupNamedFunction("_XgEventLoop");

  /*  xgEventLoop = GetFuncAddress("XgEventLoop"); */

  pm->ipm = (Implementation *) calloc(1, sizeof(Implementation));
  preinit_implementation(pm->ipm);

#if USE_MPI
  if (pvm_mytid() == 0)
    parent = -1;
  else
    parent = 0;
#endif

#if USE_PVM
#if (ALL_SPAWN)
  {				/* master is first to join group */
    char * groupname = "_PGENESIS_";
    int instance = pvm_joingroup(groupname);
    if (instance == 0)
      parent = -1;		/* i.e., I'm master */
    else
      parent = pvm_gettid(groupname, 0); /* get the master's tid */
  }
#else
  parent = pvm_parent();	/* negative implies no parent */
#endif
#endif

  if (parent < 0)	        /* This is the master */
    MasterStartup (pm, action);
  else			/* This is the worker */
    WorkerStartup (pm, action, parent);

  postinit_implementation(pm, pm->ipm);
  
  pm->totalnodes = pm->nnodes;
  pvm_config (&i, &id, NULL);
  if (IsSilent() < 2) {
    printf("mynode = %d, totalnodes = %d, nnodes = %d\n",
	   pm->mynode, pm->totalnodes, pm->nnodes);
    fflush (stdout);
  }
  return (1);
}

int do_realmynode (argc,argv)
     int     argc;
     char	**argv;
{
  int temp;

  temp = pvm_mytid ();
  return(temp);
}



int do_realnnodes(argc,argv)
     int     argc;
     char	**argv;
{
  int     temp;

  pvm_config (&temp, NULL, NULL);
  return (temp);
}


/*
 ** Sets the parallel_enable flag to 1 just long enough for 
 ** one pass through the ClearSetupMessages routine, then restores it
 */
int do_clear_parallel(argc,argv)
     int argc;
     char	**argv;
{
  int oldflag;
  Postmaster * pm;

  if ((pm = GetPostmaster()) == NULL)
    return(0);

  oldflag=parallel_enable_flag;
  parallel_enable_flag=1;
  ClearSetupMessages(pm, -1, 0.);	/* clear all buffered PVM message */
  parallel_enable_flag=oldflag;
  return(1);
}

/*
 ** Sets the parallel_enable flag to 1 just long enough for 
 ** one PVM message to be accepted.  Returns number processed.
 */
int do_one_parallel(argc,argv)
     int argc;
     char	**argv;
{
  int oldflag, res;
  Postmaster * pm;

  if ((pm = GetPostmaster()) == NULL)
    return(0);

  oldflag=parallel_enable_flag;
  parallel_enable_flag=1;
  res = ClearSetupMessages(pm, 1, 0.); /* clear up to 1 buffered PVM msg */
  parallel_enable_flag=oldflag;
  return(res);
}

int do_disable_parallel(argc,argv)
     int argc;
     char	**argv;
{
  parallel_enable_flag=0;
  return(1);
}

int do_enable_parallel(argc,argv)
     int argc;
     char	**argv;
{
  parallel_enable_flag=1;
  return(1);
}

do_paroff()
{
  Postmaster * pm = GetPostmaster();
  if (pm != NULL)
    {
      fflush(stdout);
      fflush(stderr);
      SyncAllNodes(pm, PAR_OFF_ALL_SYNC, pm->msg_hang_time, 10000.);
      pvm_exit();
    }
  return 1;
}

int IsParallelEnabled()
{
  return(parallel_enable_flag);
}

do_flick()
{
#ifdef DELTA
  flick();
#endif /* DELTA */
}

double pgen_clock() 
{
  time_t tloc;

  tloc=time(NULL);
  return((double)tloc);
}

int GetParlibStep ()
{
  return(parlib_step);
}

void SetParlibStep (n)
     int n;
{
  parlib_step = n;
}
