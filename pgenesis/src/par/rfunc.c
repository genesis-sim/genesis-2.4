static char rcsid[] = "$Id: rfunc.c,v 1.2 2005/11/17 23:14:44 ghood Exp $";

/*
 * $Log: rfunc.c,v $
 * Revision 1.2  2005/11/17 23:14:44  ghood
 * Changed dclock to pgen_clock
 *
 * Revision 1.1.1.1  2005/06/14 04:38:39  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.2  2002/01/17 18:44:57  dbeeman
 * Bug fixes from Greg Hood for pgenesis 2.2a, entered by dbeeman
 *
 * Revision 1.1  1999/12/19 03:59:43  mhucka
 * Adding PGENESIS from GENESIS 2.1 CDROM release.
 *
 * Revision 1.26  1997/07/14 03:35:49  ngoddard
 * removed redundant conditional generating compiler warning
 *
 * Revision 1.25  1997/07/03 01:54:51  ghood
 * Fixed waiton error message.
 *
 * Revision 1.24  1997/02/20 21:13:40  ngoddard
 * support for xpvm; using pvm_trecv for timeout ops; fixed bugs related to futures; provide user warnings when appears stalled; barrer and barrierall take a third argument: how long to wait before stall is assumed; beginnings of performance stats package
 *
 * Revision 1.23  1997/01/31 05:14:29  ngoddard
 * barriers don't complete asyncs now, minor bug fixes
 *
 * Revision 1.22  1997/01/21 22:20:27  ngoddard
 * paragon port
 *
 * Revision 1.21  1997/01/21 20:19:49  ngoddard
 * bug fixes for zones, implement string literal fields, first release of pgenesis script
 *
 * Revision 1.20  1996/08/30 03:58:40  ghood
 * removed usage of initopt/getopt from do_async and do_remote_command_req
 *
 * Revision 1.19  1996/08/19 21:34:02  ghood
 * Made pvm calls type-consistent with their parameters.
 *
 * Revision 1.18  1996/08/13 21:25:07  ghood
 * General debugging to get parallel example scripts working.
 *
 * Revision 1.17  1996/07/30 15:44:21  ghood
 * Converted to use ExecuteStrFunction instead of a ResultString external.
 *
 * Revision 1.16  1996/06/28 03:07:52  ngoddard
 * now use ResultString rather than ConvertBuf() to get the
 * result of a call to ExecuteFunction.  Temporary hack.
 *
 * Revision 1.15  1996/06/22 03:18:17  ngoddard
 * don't call ReusePvmMessage if it hasn't been sent
 *
 * Revision 1.14  1996/06/21  16:10:48  ghood
 * General debugging
 *
 * Revision 1.13  1996/05/03 19:48:15  ghood
 * moving to biomed area
 *
 * Revision 1.12  1996/02/02  20:25:07  ngoddard
 * made CheckValidFuture return a value
 *
 * Revision 1.11  1995/10/03  17:56:06  geigel
 * Updates for T3D port
 *
 * Revision 1.10  1995/06/09  18:54:35  ngoddard
 * various fixes for ANSI C, function prototypes, and Solaris
 *
 * Revision 1.9  1995/06/07  20:58:25  ngoddard
 * all asyncs use a future until we fix the nores_count stuff
 *
 * Revision 1.8  1995/06/06  20:59:32  mckeon
 * Temporary change removed.
 *
 * Revision 1.7  1995/05/25  17:07:43  ngoddard
 * added function definition for compilation without prototypes
 *
 * Revision 1.6  1995/05/24  15:34:52  ngoddard
 * improved threads but still basic design flaw, also fixed comment leader in barrier tests
 *
 * Revision 1.5  1995/05/10  18:56:52  ngoddard
 * before making changes to allow ids on barriers
 *
 * Revision 1.4  1995/05/03  18:09:23  ngoddard
 * fixed initopt call for rcommand to func@node with no args
 *
 * Revision 1.3  1995/05/02  22:07:07  ngoddard
 * changed to use -execdir and -altsimrc
 *
 * Revision 1.2  1995/04/26  19:57:17  ngoddard
 * commenced allowing out-of-zone setups out of step
 *
 * Revision 1.1  1995/04/03  22:04:31  ngoddard
 * Initial revision
 *
 * Revision 1.1  1994/09/07  19:01:00  ngoddard
 * passive messaging works, src figures map
 *
 * Revision 1.5  1993/12/30  00:06:12  ngoddard
 * Release 12/14/93
 *
 * Revision 1.2  1993/10/15  22:51:51  ngoddard
 * added Id and Log keywords
 *
 *
 */

#include "par_ext.h"
#include "pvm-defs.h"

#ifdef FUNC_PROT
/* private function prototypes */
void RespondRcommandsDoneRequest(Postmaster * pm, int src_node, int src_tid, int cnt);
extern char * MallocedString(char *);
#else
void RespondRcommandsDoneRequest();
extern char * MallocedString();
#endif

extern Postmaster * GetPostmaster();
extern double pgen_clock();
extern char * add_remote_msg();
extern char * remote_add_msg_entry();
extern char * remote_volume_connect_entry();
extern char * remote_volume_weight_entry();
extern char * remote_volume_delay_entry();

char * remote_command_req();
char * ReferString();
char *SendGlobReq();
char *ExecuteStrFunction();

NoteRcmdsRequested(pm, nodes, cnt)
     Postmaster * pm;
     int * nodes;
     int cnt;
{
  for(; cnt > 0; cnt--, nodes++) {
    printf("sending rcmd to node %d\n", *nodes);
    if (pm->ipm->nores_rcmds_sent[*nodes] == 0) {
      pm->ipm->nores_rcmds_cnt++;
      printf("Incremented rcmd_cnt to %d\n", pm->ipm->nores_rcmds_cnt);
    }
    pm->ipm->nores_rcmds_sent[*nodes]++;
  }
}

WaitFuturesResolved(pm)
     Postmaster * pm;
{
  /* make this function non-recursive to avoid nasty deadlocks */
  static int recurse = 1;
  if (recurse) {
    recurse = 0;
    CLR_SETUP_COND_TIMEOUT(pm, pm->msg_hang_time, pm->pvm_hang_time,
			   (pm->ipm->future_cnt > 0),
			   "Waiting for outstanding futures");
/*
    while (pm->ipm->future_cnt > 0)
      ClearSetupMessages(pm, 1);
*/
    recurse = 1;
  }
}

/* This variable keeps track of all functions and commands
 ** issued from each node */
static	int	funcid = 0;

void RequestRcommandsCompleted(pm)
     Postmaster * pm;
{
  int i, nnodes, rnode;
  Implementation * ipm = pm->ipm;
  int mtype;
  double endtime;

  if (ipm->nores_rcmds_cnt == 0)
    return;			/* no outstanding rcommands */

  for (i = nnodes = 0; i < ipm->nnodes; i++) {
    if (ipm->nores_rcmds_sent[i] > 0) {
      nnodes++;
      InitPvmMessage(pm);
      PackCore(pm, PAR_RCOMMANDS_DONE_REQ);
      if (pvm_pkint(&(ipm->mynode), 1, 1) < 0)
	{ PvmError(); }
      if (pvm_pkuint(&(ipm->nores_rcmds_sent[i]), 1, 1) < 0)
	{ PvmError(); }
      /* 0 implies out of zone, so ignore step count */
      SendKnownSetupMessage(pm, i, 0);
      printf("Requested node %d to complete rcmds, expecting %d\n", i,
	     ipm->nores_rcmds_sent[i]);
    }
  }
}

WaitRcommandsCompleted(pm)
     Postmaster * pm;
{
  /* make this function non-recursive to avoid nasty deadlocks */
  static int recurse = 1;
  if (recurse) {
    recurse = 0;
    CLR_SETUP_COND_TIMEOUT(pm, pm->msg_hang_time, pm->pvm_hang_time,
			   (pm->ipm->nores_rcmds_cnt > 0),
			   "Waiting for outstanding futures");
/*
    while (pm->ipm->nores_rcmds_cnt > 0) {
      printf("Waiting for rcmds to complete, %d nodes left\n", 
	     pm->ipm->nores_rcmds_cnt);
      ClearSetupMessages(pm, 1);
    }
*/
    recurse = 1;
  }
}

void HandleRcommandsDoneReply(pm)
     Postmaster * pm;
{
  int i, nnodes, rnode, ncmds;
  Implementation * ipm = pm->ipm;
  int mtype;
  double endtime;

  pvm_upkint(&rnode, 1, 1);
  assert(rnode >= 0);
  pvm_upkint(&ncmds, 1, 1);
  assert(ncmds >= 0);
  assert(ipm->nores_rcmds_sent[rnode] >= ncmds);
  printf("Got rcmds done reply from node %d, %d done, %d sent\n", rnode,
	 ncmds, ipm->nores_rcmds_sent[rnode]);
  if ((ipm->nores_rcmds_sent[rnode] -= ncmds) == 0) {
    ipm->nores_rcmds_cnt--;
    printf("decrementing rcmds_cnt to %d\n", ipm->nores_rcmds_cnt);
  }
}

void HandleRcommandsDoneRequest(pm, src_tid)
     Postmaster * pm;
     int src_tid;
{
  int cnt, src_node;
  if (pvm_upkint(&src_node, 1, 1) < 0)
    { PvmError(); }
  if (pvm_upkint(&cnt, 1, 1) < 0)
    { PvmError(); }
  printf("Got rcmds done request from node %d, did %d, want %d\n",
	 src_node, pm->ipm->nores_rcmds_handled[src_node], cnt);
  if (pm->ipm->nores_rcmds_handled[src_node] >= cnt)
    RespondRcommandsDoneRequest(pm, src_node, src_tid, cnt);
  else
    AddRcommandDoneReq(pm, src_node, src_tid, cnt);
}

void RespondRcommandsDoneRequest(pm, src_node, src_tid, cnt)
     Postmaster * pm;
     int src_node;
     int src_tid;
     int cnt;
{
  pm->ipm->nores_rcmds_handled[src_node] -= cnt;
  InitPvmMessage(pm);
  PackCore(pm, PAR_RCOMMANDS_DONE_REPLY);
  if (pvm_pkint(&(pm->ipm->mynode), 1, 1) < 0)
    { PvmError(); }
  if (pvm_pkint(&cnt, 1, 1) < 0)
    { PvmError(); }
  /* send remote so that it always gets through independent of step count */
  if (!SendRemoteSetupMessageByTid(pm, src_tid))
    { PvmError(); }
}


/*
By default blocks until the command has executed and any result
has returned.  The -result flag indicates there will be a character
string result, which is returned by do_remote_command_req.

With the -async flag it returns as soon as the request for the command
has been sent.  In this case no verification that the command has
executed is returned.  If the -result flag is given (one node only),
this function returns an integer handle.  This handle can be used in
a call to do_remote_command_result, which blocks until the result has
returned, and returns it as a string.  If the "-reduce type" option
is given, then the result returned by do_remote_command_result is
the reduction specified by <type>.

-reduce is not yet implemented.  -result is not yet implemented - use
rfunc instead.
*/

char * do_async(argc,argv)
     int argc;
     char	**argv;
{
  /* make everyone use a future until we fix the nores_count stuff */
  int result = COMPLETION_RESULT; /* should be NO_RESULT */
  int status;

  if (argc > 1 && strcmp(argv[argc-1], "-scalar") == 0)
    {
      if (result != NO_RESULT)
	printf("Warning: result type already specified, -scalar option ignored\n");
      else result = SCALAR_RESULT;
      --argc;
    }
  else if (argc > 1 && strcmp(argv[argc-1], "-complete") == 0)
    {
      if (result != NO_RESULT)
	printf("Warning: result type already specified, -complete option ignored\n");
      else result = COMPLETION_RESULT;
      --argc;
    }
  else if (argc > 2 && strcmp(argv[argc-2], "-reduce") == 0)
    {
      printf("Warning: -reduce option not implemented\n");
      if (result != NO_RESULT)
	printf("Warning: result type already specified, -reduce option ignored\n");
      else result = COMPLETION_RESULT;
      /* TODO: get the reduction type */
      argc -= 2;
    }

  /*  result = NO_RESULT; */

  /* strip off the "async" from the argc/argv */
  return(MallocedString(remote_command_req(argc-1, argv+1, 1, result)));
}

char * TakeFutureValue(pm, future, ftr)
     Postmaster * pm;
     unsigned int future;
     FutureItem * ftr;
{
  int i;
  char * res = NULL;
  BufferManager * bfm = pm->ipm->future;
  assert(ftr->restyp != NO_RESULT);
  switch (ftr->restyp) {
  case COMPLETION_RESULT:
    res = "0"; 
    break;
  case SCALAR_RESULT:
  case REDUCE_ADD_RESULT:
    res = ftr->res;
  }
  assert(res != NULL);
  RemoveBufferIndex(bfm, future); /* free up the future */
  return res;
}

/* 
 ** char * do_remote_global_req(argc,argv)
 ** Blocking, gets a remote global script variable converted to a 
 ** char string.
 ** Avoids the time and memory overhead associated with script functions.
 ** Should only be sent to one node at a time. If sent to 
 ** multiple nodes, it complains.
 */

char * remote_global_req(argc,argv, async, future)
     int argc;
     char	**argv;
     int async;
     int future;
{
  int		dest_node,dest_zone;
  char	glob_name[100];
  Postmaster * pm;
  int * dst_nodes, node_cnt, nonlocal;
  
  if (argc < 2) {
    printf("usage : %s global[@node[.zone]]\n",argv[0]);
    return(NULL);
  }
  
  /* find the postmaster */
  if ((pm = GetPostmaster()) == NULL)
    return 0;
  
  /* get the global node numbers */
  if (!GetRemoteNodes(pm, argv[1], glob_name, &node_cnt,
		      &dst_nodes, &nonlocal))
    { return 0; }

  if (node_cnt == 0)
    return 0;

  if (node_cnt > 1) {
    printf("Error : %s can only be called for one node\n",argv[0]);
  }
  
  dest_node = dst_nodes[0];

  return(CopyString(SendGlobReq(pm,glob_name,dest_node)));
}

char * do_remote_global_req(argc,argv)
     int argc;
     char	**argv;
{
  remote_global_req(argc, argv, 0, 0);
  return(MallocedString(""));
}

char * do_remote_command_req(argc,argv)
     int argc;
     char	**argv;
{
  int result = SCALAR_RESULT;
  int status;
  char *temp;

  /*   ELog("Entering do_remote_command_req");
  ELog(argv[0]);
  if (argc > 1)
    ELog(argv[1]); */
  DBGMSG("entering rcommand...");

  if (argc > 2 && strcmp(argv[argc-2], "-reduce") == 0)
    {
      printf("Warning: -reduce option not implemented\n");
      /* TODO: get the reduction type */
      --argc;
    }

  temp = remote_command_req(argc ,argv , 0, result);
  /*   ELog("Leaving do_remote_command_req"); */
  return(MallocedString(temp));
}

/*

syntax is: command@node-list [args]

if <async> is true, the command request is shipped out via PVM and the
routine returns immediately, otherwise it returns once the command has
completed on all nodes.  Note async execution can be local (i.e, a new
thread).

if <result> is set, then for <async> execution a handle is returned
which can be used to wait on completion and retrieve the result.  For
non <async> execution, the result is the result of the command
execution on the last node to finish (i.e, not well-defined for
multiple nodes).

*/

PackRemoteCommand(pm, argc, argv, future, result)
     Postmaster * pm;
     int argc;
     char ** argv;
     int future;
     int result;
{
  int i;
  InitPvmMessage(pm);
  PackCore(pm, PAR_COMMAND_REQ);
  if (pvm_pkint(&(pm->ipm->mynode),1,1) < 0)
    { PvmError(); }  
  if (pvm_pkint(&result,1,1) < 0)
    { PvmError(); }  
  if (result != NO_RESULT)
    if (pvm_pkint(&future,1,1) < 0)
      { PvmError(); }
  if (pvm_pkint(&argc,1,1) < 0)
    { PvmError(); }
  for(i = 0; i < argc; i++)
    if (pvm_pkstr(argv[i]) < 0)
      { PvmError(); }
}

unsigned int AllocateFutureIndex(pm, result, nnodes)
     Postmaster * pm;
     int result;
     int nnodes;
{
  Implementation * ipm = pm->ipm;
  unsigned int res =
    MakeBufferIndex(&(ipm->future), FUTURE_BLK_CNT,
		    FUTURE_BLK_SIZ, sizeof(FutureItem));
  BufferManager * bfm = ipm->future;
  FutureItem * ftr = GetBufferIndexItem(bfm,res);
  pm->ipm->future_cnt++;
  assert(result != NO_RESULT);
  ftr->restyp = result;
  ftr->expected = nnodes;
  ftr->received = 0;
  ftr->res = NULL;
  return(res);
}

IncrementFutureExpected(pm, future)
     Postmaster *pm;
     int future;
{
  BufferManager *bfm = pm->ipm->future;
  FutureItem *ftr = GetBufferIndexItem(bfm, future);
  ++ftr->expected;
}

AdjustFutureExpected(pm, future, nnodes)
     Postmaster * pm;
     int future;
     int nnodes;
{
  BufferManager * bfm = pm->ipm->future;
  FutureItem * ftr = GetBufferIndexItem(bfm,future);
  ftr->expected = nnodes;
}

char * WaitFutureResolved(pm, future)
     Postmaster * pm;
     unsigned int future;
{
  BufferManager * bfm = pm->ipm->future;
  FutureItem * ftr;
  /* nonlocal synchronous commands always have a future */
  assert(future != 0);
  ftr = (FutureItem *) GetBufferIndexItem(bfm, future);
  /* loop handling setup messages until the future is resolved */
  CLR_SETUP_COND_TIMEOUT(pm, pm->msg_hang_time, pm->pvm_hang_time,
			 (ftr->expected > ftr->received),
			 "Waiting for outstanding future");
/*
  while (ftr->expected > ftr->received)
    ClearSetupMessages(pm, 1);
*/
  return(TakeFutureValue(pm, future, ftr));
}

/* a remote reply has a future index and perhaps a result string */
FutureItem * NoteFuture(pm, future)
     Postmaster * pm;
     unsigned int future;
{
  BufferManager * bfm = pm->ipm->future;
  FutureItem * ftr;
  /* shouldn't be replies for remote commands which have no future */
  assert(future != 0);
  ftr = (FutureItem *) GetBufferIndexItem(bfm, future);
  assert(ftr->received < ftr->expected);
  /* is the future satisfied? */
  if (++ftr->received == ftr->expected) {
    assert(pm->ipm->future_cnt >0);
    pm->ipm->future_cnt--;
  }
  return ftr;
}

/* abort a future.  for error conditions */
void CompleteFuture(pm, future)
     Postmaster * pm;
     unsigned int future;
{
  BufferManager * bfm = pm->ipm->future;
  FutureItem * ftr;
  /* shouldn't be replies for remote commands which have no future */
  assert(future != 0);
  ftr = (FutureItem *) GetBufferIndexItem(bfm, future);
  /* is the future satisfied? */
  assert(pm->ipm->future_cnt >0);
  pm->ipm->future_cnt--;
}

HandleRemoteCommandReply(pm, src_tid)
     Postmaster * pm;
     int src_tid;
{
  int future;
  char res[MAX_RESULT_LEN];
  BufferManager * bfm = pm->ipm->future;
  FutureItem * ftr;
  if (pvm_upkint(&future, 1, 1) < 0)
    { PvmError(); return 0; }
  ftr = NoteFuture(pm, future);
  if (ftr->restyp != COMPLETION_RESULT) {
    if (pvm_upkstr(res) < 0)
      { PvmError(); return 0; }
    /* TODO: implement reduction combinations of ftr->res and res */
    if (ftr->res != NULL)
      UnreferString(ftr->res);
    ftr->res = ReferString(res);
  }
}

char * remote_command_req(argc,argv,async,result)
     int argc;
     char **argv;
     int async;		/* should this command be executed asynchronously? */
     int result;		/* NONE, COMPLETION, SCALAR, REDUCE_? */
{
  int dest_node,dest_zone;
  int * dst_nodes, node_cnt, nonlocal;
  int i;
  int ret_funcid, req_funcid;
  char func_name[100];
  char * targs[MAX_RCOMMAND_ARGS];
  char ** Argv = targs;
  int nsend, sbufid, rbufid;
  Postmaster *pm;
  static int rcommand_used = 0;
  char res[MAX_RESULT_LEN];
  unsigned int future = 0;
  char *result_string;

  DBGMSG("entering remote_command_req...\n");
  if (argc > MAX_RCOMMAND_ARGS) {
    printf("Number of args %d overflow in %s \n", argc, argv[0]);
    return("0");
  }

  /* copy the argument pointers so we don't run over the ones we */
  /* were passed - these are freed by do_cmd !! */
  for (i = 0; i < argc; i++)
    Argv[i] = argv[i];

  /* If the rcommand call is used, just shift the argvs over one */
  if (strcmp(Argv[0],"rcommand")==0) {
    Argv++; 
    argc--;
    if (!rcommand_used) {
      printf("Warning: obsolete 'rcommand' is deprecated, use 'command@node-list'\n");
      rcommand_used = 1;
    }
  }
  
  /* find the postmaster */
  if ((pm = GetPostmaster()) == NULL)
    { ParError(NO_POSTMASTER, -1); return("0"); }
  
  /* get the global node numbers */
  if (!GetRemoteNodes(pm, Argv[0], func_name, &node_cnt,
		      &dst_nodes, &nonlocal))
    { return("0"); }

  if (node_cnt == 0)
    return("0");

  /* OK to here */

  /* reset Argv[0] stripping off node-list leaving just function */
  Argv[0] = func_name;

  /* get the result handle we'll use */
  if ((result != NO_RESULT) &&
      (async || node_cnt > 1 || dst_nodes[0] != pm->ipm->mynode))
    if ((future = AllocateFutureIndex(pm, result, node_cnt)) == 0)
      {ParError(FAILED_FUTURE_ALLOCATION, -1); return("0"); }
  
  /* deal with the special cases first, i.e. asynchronous raddmsg and */
  /* rglob where the only node is mynode */
  if (async && dst_nodes[0] == pm->ipm->mynode && node_cnt == 1) {
    if (result != NO_RESULT)
      { assert(future != 0); }
    if (strcmp(func_name, "raddmsg") == 0)
      return(remote_add_msg_entry(argc, Argv, async, future));
    else if (strcmp(func_name, "rvolumeconnect") == 0)
      return(remote_volume_connect_entry(argc, Argv, async, future));
    else if (strcmp(func_name, "rvolumeweight") == 0)
      return(remote_volume_weight_entry(argc, Argv, async, future));
    else if (strcmp(func_name, "rvolumedelay") == 0)
      return(remote_volume_delay_entry(argc, Argv, async, future));
  }

  /* handle messages from other nodes if we are trying to send a
     remote message ourself */
  if ((dst_nodes[0] != pm->ipm->mynode || node_cnt > 1) && !async)
    /* BARRIER WATCH */
    ClearSetupMessages(pm, 1, 0.);/* be fair - do we really want to do this? */

  /* Pack the PVM buffer if we'll use PVM */
  if (async || node_cnt > 1 || dst_nodes[0] != pm->ipm->mynode) {
    PackRemoteCommand(pm, argc, Argv, future, result);
  }

  if (dst_nodes[0] != pm->ipm->mynode || node_cnt > 1) {
    /* involves other nodes */

    /* send the message, could be to more than one node */
    if (dst_nodes[0] != pm->ipm->mynode) { /* all remote */
      nsend = SendSomeSetupMessages(pm, dst_nodes, node_cnt);
      if (result == NO_RESULT)
	NoteRcmdsRequested(pm, dst_nodes, node_cnt);
    } else {			/* all but my node */
      nsend = SendSomeSetupMessages(pm, dst_nodes+1, node_cnt-1);
      if (result == NO_RESULT)
	NoteRcmdsRequested(pm, dst_nodes+1, node_cnt-1);
    }
  }

  if (dst_nodes[0] == pm->ipm->mynode) {
    /* The local node is specified, do it now */
    if (async) {		/* for asynchronous operation, do it via PVM */
      if (node_cnt > 1)		/* if buffer already sent to other nodes */
	ReusePvmMessage(pm);	/* then reuse it */
      SendLocalSetupMessage(pm, pm->ipm->myznode);
      if (result == NO_RESULT)
	NoteRcmdsRequested(pm, dst_nodes, 1);
    } else {	/* just do the local thing */
      /* BARRIER WATCH: this causes sync barrier@all to deadlock */
      /* since the barrier waits for the future to be satisifed, */
      /* but that doesn't happen until we pass the barrier! */
      result_string = ExecuteStrFunction(argc,Argv);
      /* note the future is satisfied for this node */
      if (future != 0)
	NoteFuture(pm, future);
      /* is this the only node? if so, we're done */
      if (node_cnt == 1)
	/* return the result from where ExecuteFunction left it */
	return(ReferString(result_string));
    }
  }

  /* OK, so we've sent off all the requests to do the function */
  /* now, for synchronous commands we wait on the future */
  /* for asynchronous commands we return the future */

  if (async) {
    /* return the index for the FutureItem */
    assert(sprintf(res, "%d", future) >= 0);
    return(CopyString(res));
  } else {
    /* synchronous */
    /* wait for the responses and return the future's value */
    return(WaitFutureResolved(pm, future));
  }
}


/* 
 ** char * do_remote_global_poll(argc,argv)
 ** Blocking, polls a remote global script variable till it is true.
 ** Can specify the time between polls, defaults to 1 sec.
 ** Returns the value of the remote variable.
 ** Should only be sent to one node at a time. If sent to 
 ** multiple nodes, it complains. If sent to current node,
 ** it complains.
 */
char * do_remote_global_poll(argc,argv)
     int argc;
     char	**argv;
{
  int		dest_node,dest_zone;
  char	glob_name[100];
  char	*val;
  double	poll_time=1.0;
  double	hang_time;
  double	pollt;
  double	hangt;
  double	Atod(),pgen_clock();
  int	ret;
  Postmaster * pm;
  int * dst_nodes, node_cnt, nonlocal;
  
  if (argc < 2) {
    printf("usage : %s global[@node[.zone]] [time] [hang_time]\n",argv[0]);
    return(NULL);
  }
  
  /* find the postmaster */
  if ((pm = GetPostmaster()) == NULL)
    return 0;
  
  /* get the global node numbers */
  if (!GetRemoteNodes(pm, argv[1], glob_name, &node_cnt,
		      &dst_nodes, &nonlocal))
    { return 0; }

  if (node_cnt == 0)
    return 0;

  if (node_cnt > 1) {
    printf("Error : %s can only be called for one node\n",argv[0]);
  }
  
  dest_node = dst_nodes[0];

  if (argc==3)
    poll_time=Atod(argv[2]);
  
  if (argc==4)
    hang_time=Atod(argv[3]);
  else
    hang_time=pm->msg_hang_time;
  
  hangt=pgen_clock()+hang_time;
  while(pgen_clock()<hangt) {
    pollt=pgen_clock()+poll_time;
    val=SendGlobReq(pm,glob_name,dest_node);
    if ((ret=atoi(val))!=0) break;
    while(pgen_clock()<pollt)
      do_flick();
  }
  return(CopyString(val));
}

/* needs a handler for the reply */
char *SendGlobReq(pm,glob_name,dest_node)
     Postmaster	*pm;
     char *glob_name;
     int dest_node;
{
  char	*get_glob_val();
  static char res[512];
  int ret_funcid, req_funcid;

  if (dest_node==pm->ipm->mynode) {
    /* This is just a local request, do it here */
    return(get_glob_val(glob_name));
  }
  
/*  ClearSetupMessages(pm, 1);	/* be fair */
  
  req_funcid = funcid++;	/* make it reentrant */
  InitPvmMessage(pm);
  PackCore(pm, PAR_GLOB_REQ);
  if (pvm_pkint(&req_funcid, 1, 1) < 0)
    { PvmError(); }
  if (pvm_pkstr(glob_name) < 0)
    { PvmError(); }
  
  SendSomeSetupMessage(pm, dest_node);
  
  while (ClearSpecificSetupMessages(pm,dest_node) != PAR_GLOB_REPLY);
  
  if (pvm_upkint(&ret_funcid, 1, 1) < 0)
    { PvmError(); }
  if (ret_funcid != req_funcid) { /* Disaster ! */
    printf("Error : remote glob var request got wrong reply\n");
    return(NULL);
  }
  if (pvm_upkstr(res) < 0)
    { PvmError(); }
  return(res);
}

HandleRemoteCommandReq(pm, src_tid)
     Postmaster *pm;
     int src_tid;
{
  int argc;
  char	*argv[MAX_RCOMMAND_ARGS];
  char argvc[MAX_RCOMMAND_LEN];
  int i, argvci = 0;
  int future, restyp, src_node;
  char *result_string;
  
  /*  ELog("Entering HandleRemoteCommandReq"); */
  if (pvm_upkint(&src_node,1,1) < 0)
    { PvmError(); }  
  if (pvm_upkint(&restyp,1,1) < 0)
    { PvmError(); }  
  if (restyp != NO_RESULT)
    if (pvm_upkint(&future,1,1) < 0)
      { PvmError(); }
  if (pvm_upkint(&argc,1,1) < 0)
    { PvmError(); }
  for (i = 0; i < argc; i++)
    { 
      argv[i] = argvc+argvci;	/* set a pointer into the character array */
      if (pvm_upkstr(argv[i]) < 0)
	{ PvmError(); }
      argvci += strlen(argv[i])+1;
    }

  /*  if (argc > 0)
    ELog(argv[0]);
  if (argc > 1)
    ELog(argv[1]); */
  /* count it as done in case we get pushed on the stack */
  /* so that barriers work ok? */
  if (restyp == NO_RESULT)
    pm->ipm->nores_rcmds_handled[src_node]++;

  result_string = ExecuteStrFunction(argc,argv);

  if (restyp != NO_RESULT) {
    /* respond with function result */
    InitPvmMessage(pm);
    PackCore(pm, PAR_COMMAND_REPLY);
    if (pvm_pkint(&future, 1, 1) < 0)
      PvmError();
    if (restyp != COMPLETION_RESULT)
      /* pack the result string */
      if (pvm_pkstr(result_string) < 0)
	PvmError();
    SendSomeSetupMessageByTid(pm, src_tid);
  }
  /*  ELog("Leaving HandleRemoteCommandReq"); */
}


HandleRemoteGlobReq(pm, src_tid)
     Postmaster	*pm;
     int src_tid;
{
  char * GetScriptStr();

  int funcid;
  char glob_name[512];

  if (pvm_upkint(&funcid, 1, 1) < 0)
    { PvmError(); }
  if (pvm_upkstr(glob_name) < 0)
    { PvmError(); }
  
  InitPvmMessage(pm);
  PackCore(pm, PAR_GLOB_REPLY);
  if (pvm_pkint(&funcid, 1, 1) < 0)
    { PvmError(); }
  if (pvm_pkstr(GetScriptStr(glob_name)) < 0)
    { PvmError(); }
  SendSomeSetupMessageByTid(pm, src_tid);
  
/*  ClearSetupMessages(pm, 1);	/* be fair */
}
 

int CheckValidFuture(pm, future)
     Postmaster * pm;
     unsigned int future;
{
  FutureItem * ftr;
  BufferManager * bfm = pm->ipm->future;
  assert(bfm != NULL);
  if (future <= 0)
    return(0);
  return(CheckValidIndex(bfm, future));
}

char * do_take(argc, argv)
     int argc;
     char ** argv;
{
  int future;
  Postmaster * pm = GetPostmaster();

  if (pm == NULL)
    {
      ParError(NO_POSTMASTER, -1);
      return(MallocedString("0"));
    }

  initopt(argc, argv, "future");
  
  if (argc != 2)
    {
      printf("Error: waiton only takes a single argument\n");
      return(MallocedString("0"));
    }

  if (strcmp(argv[1], "all") == 0)
    {
      WaitFuturesResolved(pm);
      return(MallocedString("0"));
    }

  if (sscanf(argv[1], "%d", &future) != 1)
    {
      printf("Error: waiton requires either 'all' or an integer future\n");
      printf("       as an argument.\n");
      return(MallocedString("0"));
    }

  /* TODO: should check that bfm is allocated, future is valid, etc */
  if (pm->ipm->future == NULL)
    {
      printf("No futures are outstanding!\n");
      return(MallocedString("0"));
    }

  if (!CheckValidFuture(pm, future))
    {
      printf("Invalid future handed to 'waiton'!\n");
      return(MallocedString("0"));
    }

  return(MallocedString(WaitFutureResolved(pm, future)));
}

