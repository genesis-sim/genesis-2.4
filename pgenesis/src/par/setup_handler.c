static char rcsid[] = "$Id: setup_handler.c,v 1.2 2005/09/29 23:23:27 ghood Exp $";

/*
 * $Log: setup_handler.c,v $
 * Revision 1.2  2005/09/29 23:23:27  ghood
 * replaced dclock with pgen_clock since dclock was clashing with a system routine on the XT3
 *
 * Revision 1.1.1.1  2005/06/14 04:38:39  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.1  1999/12/19 03:59:43  mhucka
 * Adding PGENESIS from GENESIS 2.1 CDROM release.
 *
 * Revision 1.8  1997/05/19 18:05:42  ghood
 * Added support for displaying remote passive messages.
 *
 * Revision 1.7  1996/06/21 16:10:51  ghood
 * General debugging
 *
 * Revision 1.6  1996/05/03 19:48:17  ghood
 * moving to biomed area
 *
 * Revision 1.5  1995/06/09  18:54:39  ngoddard
 * various fixes for ANSI C, function prototypes, and Solaris
 *
 * Revision 1.4  1995/05/25  17:11:33  ngoddard
 * added function definition for compilation without prototypes
 *
 * Revision 1.3  1995/05/24  15:34:59  ngoddard
 * improved threads but still basic design flaw, also fixed comment leader in barrier tests
 *
 * Revision 1.2  1995/05/10  18:56:56  ngoddard
 * before making changes to allow ids on barriers
 *
 * Revision 1.1  1995/04/03  22:04:44  ngoddard
 * Initial revision
 *
 * Revision 1.1  1994/09/07  19:01:00  ngoddard
 * passive messaging works, src figures map
 *
 * Revision 1.5  1993/12/30  00:06:16  ngoddard
 * Release 12/14/93
 *
 * Revision 1.2  1993/10/15  22:51:51  ngoddard
 * added Id and Log keywords
 *
 *
 */

/****************************************************************
 **                                                             **
 **                  setup_handler.c :                          **
 **   A set of routines for handling the setup calls at the     **
 **   machine-independent level                                 **
 **                                                             **
 ****************************************************************/

#include "par_ext.h"

#ifdef FUNC_PROT
/* function prototypes for private routines */
static void AttemptQueuedRequests(Postmaster * pm);
static void AttemptRcmdDoneRequests(Postmaster * pm);
static RcmdDone * AddRcmdDoneObj(Postmaster * pm);
static ListObj * RemRcmdDoneObj(Postmaster * pm, RcmdDone * obj);
#else
static void AttemptQueuedRequests();
static void AttemptRcmdDoneRequests();
static RcmdDone * AddRcmdDoneObj();
static ListObj * RemRcmdDoneObj();
#endif

/* This function is called to handle a single setup message */
/* It returns a negative number on error, otherwise 0 */

int HandleSetupMessage(pm, src_tid, msg_type)
     Postmaster * pm;
     int src_tid, msg_type;
{
  /*   char str[128];
  sprintf(str, "HandleSetupMessage: type %d from tid %d\n", msg_type, src_tid);
  ELog(str); */

  AttemptQueuedRequests(pm);
  switch(msg_type) {
  case PAR_SYNC_REQ :
    HandleSyncRequest(pm, PAR_SYNC_REQ); break;
  case PAR_GLOBAL_SYNC_REQ :
    HandleSyncRequest(pm, PAR_GLOBAL_SYNC_REQ); break;
  case PAR_ZONE_SYNC_REQ :
    HandleSyncRequest(pm, PAR_ZONE_SYNC_REQ); break;
  case PAR_SYNC_REPLY :
    HandleSyncReply(pm, PAR_SYNC_REPLY); break;
  case PAR_GLOBAL_SYNC_REPLY :
    HandleSyncReply(pm, PAR_GLOBAL_SYNC_REPLY); break;
  case PAR_ZONE_SYNC_REPLY :
    HandleSyncReply(pm, PAR_ZONE_SYNC_REPLY); break;
  case PAR_COMMAND_REQ :
    HandleRemoteCommandReq(pm, src_tid); break;
  case PAR_COMMAND_REPLY :
    HandleRemoteCommandReply(pm, src_tid); break;
#if 0
  case PAR_GLOB_REQ :
    HandleRemoteGlobReq(pm, src_tid);
    break;
  case PAR_GLOB_REPLY :
    /* need handler here */
    break;
  case PAR_TAB_FILL :
    if (!UnpackMess(post, &ni))
      return 0;
    i = HandleRemoteTabFill(post);
    ParSend(post,ni->node,ni->zone,PAR_TAB_FILL_REPLY);
    if (i == 1)
      return(COMPLETED_PAR_TAB_FILL);
    break;
  case PAR_TAB_FILL_REPLY :
    /* This should only happen within a RemoteTabFill */
    if (!UnpackMess(post, &ni))
      return 0;
    return(PAR_TAB_FILL_REPLY);
    break;
  case PAR_DD_MSG_REQ :
    if (!UnpackMess(post, &ni))
      return 0;
    if (ni->zone != post->myzone) {
      printf("Error : rsendmsg to zone %d from n,z %d,%d\n",
	     post->myzone,ni->node,ni->zone);
      break;
    }
    /* NHG 			HandleDDMsgReq(post,ni->node); */
    break;
  case PAR_DD_MSG_REPLY :
    if (!UnpackMess(post, &ni))
      return 0;
    return(PAR_DD_MSG_REPLY);
    break;
  case PAR_SETUP_ERROR :
    if (!UnpackMess(post, &ni))
      return 0;
    /* identify error */
    /* Decide if to print it */
    printf("Setup Error received on %d,%d from %d,%d : %s\n",
	   post->mynode,post->myzone,ni->node,ni->zone,
	   ((struct setup_error_type *)
	    (post->mess_in->m_data))->error_message);
    /* If have a list of messages sent out, resend */
    break;
  case PAR_SYNTAX_ERROR :
    if (!UnpackMess(post, &ni))
      return 0;
    /* identify error */
    printf("Syntax Error : %s\n bounced from %d.%d back to %d.%d \n",
	   ((struct setup_error_type *)
	    (post->mess_in->m_data))->error_message,ni->node,ni->zone,post->mynode,post->myzone);
    break;
#endif
  case PAR_ADD_MSG_REQ:
    HandleRemoteMessageRequest(pm, src_tid);
    break;
  case PAR_ADD_MSG_REPLY:
    CompleteRemoteMessages(pm);
    break;
  case PAR_VOLUME_DELAY_REQ:
    HandleRemoteVolumeDelayRequest(pm, src_tid);
    break;
  case PAR_VOLUME_DELAY_REPLY:
    HandleRemoteVolumeDelayReply(pm);
    break;
  case PAR_VOLUME_WEIGHT_REQ:
    HandleRemoteVolumeWeightRequest(pm, src_tid);
    break;
  case PAR_VOLUME_WEIGHT_REPLY:
    HandleRemoteVolumeWeightReply(pm);
    break;
  case PAR_SHOW_ACTIVE_MSG_OUT_REQ:
    HandleRemoteShowActiveMsgOutRequest(pm, src_tid);
    break;
  case PAR_SHOW_PASSIVE_MSG_OUT_REQ:
    HandleRemoteShowPassiveMsgOutRequest(pm, src_tid);
    break;
  case PAR_SHOW_MSG_OUT_REPLY:
    HandleRemoteShowMsgOutReply(pm);
    break;
  case PAR_SHOW_MSG_IN_REQ:
    HandleRemoteShowMsgInRequest(pm, src_tid);
    break;
  case PAR_SHOW_MSG_IN_REPLY:
    HandleRemoteShowMsgInReply(pm);
    break;
  case PAR_RCOMMANDS_DONE_REQ:
    HandleRcommandsDoneRequest(pm, src_tid);
    break;
  case PAR_RCOMMANDS_DONE_REPLY:
    HandleRcommandsDoneReply(pm);
    break;
  default:
    printf("Do not know how to handle message type %d\n",
	   msg_type);
    return -1;
    break;
  }
  AttemptQueuedRequests(pm);
  return(0);
}

static void AttemptQueuedRequests(pm)
     Postmaster * pm;
{				
  AttemptRcmdDoneRequests(pm);	/* try to complet rcmd done requests */
}

static void AttemptRcmdDoneRequests(pm)
     Postmaster * pm;
{
  Implementation * ipm;
  ListObj * t, * pt;
  RcmdDone * rc;
  assert(pm != NULL);
  ipm = pm->ipm;
  for (t = ipm->rcmd_done_head, pt = NULL;
       t != NULL;) {
    rc = (RcmdDone *) (t->object);
    if (ipm->nores_rcmds_handled[rc->src_node] >= rc->cnt) {
      RespondRcommandsDoneRequest(pm, rc->src_node, rc->src_tid, rc->cnt);
      t = RemRcmdDoneObj(pm, rc);
    } else {
      pt = t;
      t = t->next;
    }
  }
}

/* public routine */
void AddRcommandDoneReq(pm, src_node, src_tid, cnt)
     Postmaster * pm;
     int src_node;
     int src_tid;
     int cnt;
{
  RcmdDone * optr = AddRcmdDoneObj(pm);
  optr->src_node = src_node;
  optr->src_tid = src_tid;
  optr->cnt = cnt;
}

static RcmdDone * AddRcmdDoneObj(pm)
     Postmaster * pm;
{
  Implementation * ipm;
  RcmdDone * res;
  ListObj * t;
  assert(pm != NULL);
  ipm = pm->ipm;
  /* if none on free list, make one and put it on the free list */
  if (ipm->free_rcmd_done == NULL) {
    ipm->free_rcmd_done = (ListObj *) malloc(sizeof(ListObj));
    ipm->free_rcmd_done->type = PAR_RCOMMANDS_DONE_REQ;
    ipm->free_rcmd_done->object = (RcmdDone *) malloc(sizeof(RcmdDone));
    ipm->free_rcmd_done->next = NULL;
  }
  /* get the first entry on the free list and put it on the chain */
  t = ipm->free_rcmd_done->next;
  ipm->free_rcmd_done->next = ipm->rcmd_done_head;
  ipm->rcmd_done_head = ipm->free_rcmd_done;
  ipm->free_rcmd_done = t;
  return ((RcmdDone *) (ipm->rcmd_done_head->object));
}

/* removes an object from the list and returns the pointer to the next
   object on the list */
static ListObj * RemRcmdDoneObj(pm, obj)
     Postmaster * pm;
     RcmdDone * obj;
{
  Implementation * ipm;
  ListObj * res;
  ListObj * t, * pt;
  assert(pm != NULL);
  ipm = pm->ipm;
  for (t = ipm->rcmd_done_head, pt = NULL;
       t != NULL && t->object != obj;
       pt = t, t = t->next);
  if (pt)
    pt->next = t->next;
 else
   ipm->rcmd_done_head = t->next;
  res = t->next;
  t->next = ipm->free_rcmd_done;
  ipm->free_rcmd_done = t;
  return res;
}

    
