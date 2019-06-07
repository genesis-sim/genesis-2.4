static char rcsid[] = "$Id: par_show.c,v 1.2 2005/09/29 23:17:47 ghood Exp $";

/*
 * $Log: par_show.c,v $
 * Revision 1.3  2018/08/08 15:45:00 jcrone
 * Added header for explict function declaration of FieldHashFind(). This 
 * eliminated a rare segmentation fault we were encountering
 *
 * Revision 1.2  2005/09/29 23:17:47  ghood
 * added several free() calls to fix memory leaks
 *
 * Revision 1.1.1.1  2005/06/14 04:38:39  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.1  1999/12/19 03:59:42  mhucka
 * Adding PGENESIS from GENESIS 2.1 CDROM release.
 *
 * Revision 1.12  1997/08/11 15:49:10  ngoddard
 * realloc will not accept null pointer; macro RECALLOC does callor or realloc
 *
 * Revision 1.11  1997/07/22 23:03:13  ghood
 * Replaced getopt/optopt with G_getopt/G_optopt
 *
 * Revision 1.10  1997/05/20 15:47:52  ghood
 * Removed unnecessary line feed from rshowmsg output.
 *
 * Revision 1.9  1997/05/19 17:55:41  ghood
 * Added support for displaying remote passive messages; also fixed bug
 * that caused rshowmsg to truncate the list of active remote messages.
 *
 * Revision 1.8  1997/03/27 06:26:47  ngoddard
 * mods to scale up to larger networks
 *
 * Revision 1.7  1997/02/28 05:24:48  ngoddard
 * support for lookahead optimisation, bug fixes
 *
 * Revision 1.6  1997/01/31 05:14:22  ngoddard
 * barriers don't complete asyncs now, minor bug fixes
 *
 * Revision 1.5  1997/01/21 22:20:21  ngoddard
 * paragon port
 *
 * Revision 1.4  1997/01/21 20:19:45  ngoddard
 * bug fixes for zones, implement string literal fields, first release of pgenesis script
 *
 * Revision 1.3  1996/08/19 21:34:02  ghood
 * Made pvm calls type-consistent with their parameters.
 *
 * Revision 1.2  1996/06/21 16:10:42  ghood
 * General debugging
 *
 * Revision 1.1  1996/05/03 19:11:29  ngoddard
 * Initial revision
 *
 */

#include <stdio.h>
#include "par_ext.h"
#include "par_struct.h"
#include "newconn_defs.h"
#include "newconn_struct.h"
#include "pvm-defs.h"
#include "shell_func_ext.h"

/* EXTERNAL DECLARATIONS */
/* It would be better to just include newconn_ext.h, but that includes sim_ext,
   which causes problems because par_ext.h also includes it */
extern float rangauss();
extern void randomize_value();
extern int GetSynapseType();
extern Synapse *GetSynapseAddress();
extern double DoubleMessageData();
extern double FloatMessageData();
extern double IntMessageData();
extern double ShortMessageData();
extern double StringMessageData();

extern char * MallocedString();
extern char *WaitFutureResolved();

/* LOCAL DECLARATIONS */

typedef struct AddrIndex {
  void *addr;
  int index;
} AddrIndex;

static int AddrCompare();
static int AddrIndexCompare();
static void RememberAddress();
static void RememberAddrIndex();
static void RememberIndex();

/*********************************************************************
 *		PHASE 1:  INITIAL CHECKING AT SOURCE
 *********************************************************************/

char *
do_remote_show_msg(argc,argv)
     int argc;
     char **argv;
{
  Element 	*bptr;
  char 		*elementname;
  char 		*ptr;
  char * res = MallocedString("");

  /* Since this function does not use any other options, we
   ** wont put the -find option in either, though that would
   ** not be too hard */
  initopt(argc, argv, "element");
  if (G_getopt(argc, argv) != 0)
    {
      TraceScript();
      printoptusage(argc, argv);
      return(res);
    }

  elementname = optargv[1];
  
  if((bptr = GetElement(elementname)) == NULL)
    {
      TraceScript();
      printf("%s:  cannot find element '%s'\n",
	     optargv[0],
	     elementname);
      return(res);
    }

  printf("\nINCOMING MESSAGES\n");
  ShowLocalMsgIn(bptr->msgin,bptr->object, bptr->nmsgin);
  ShowRemoteMsgIn(bptr->msgin,bptr->object, bptr->nmsgin);
  printf("\nOUTGOING MESSAGES\n");
  ShowLocalMsgOut(bptr->msgout, bptr->nmsgout);
  ShowRemoteActiveMsgOut(bptr->msgout, bptr->nmsgout);
  ShowRemotePassiveMsgOut(bptr);
  return(res);
}

ShowLocalMsgIn(msg, object, nmsgs)
     Msg *msg;
     GenesisObject *object;
     int nmsgs;
{
    int		count;
    MsgList	*ml;
    Postmaster	*pm;

    if(msg == NULL){
	return(0);
    }

    pm = GetPostmaster();
    for(count = 0; count < nmsgs; count++){
	if (VISIBLE(msg->src) && msg->src != (Element *) pm) {
	    ml=GetMsgListByType(object,msg->type);
	    if (ml)
		    printf("MSG %d from '%s' %s type [%d] '%s' ", 
			    count,Pathname(msg->src),MSGINFORW(msg) ? "(forwarded)" : "",msg->type, ml->name);
	    else
		    printf("MSG %d from '%s' %s type [%d] '%s' ", 
			    count,Pathname(msg->src),MSGINFORW(msg) ? "(forwarded)" : "",msg->type, "nil");
	    DisplayMsgIn(msg, ml->slotname);
	    printf("\n");
	}

	msg = MSGINNEXT(msg);
    }
}

ShowRemoteMsgIn(msg, object, nmsgs)
     MsgIn *msg;
     GenesisObject	*object;
     int nmsgs;
{
  int count;
  BackPtr *pbp;
  int **sources;
  int *sizes;
  int *counts;
  MsgOut *mo;
  Postmaster *pm;
  Implementation *ipm;
  int future;
  int idx;
  BufferManager *bfm;
  int src_node;
  int blk;
  int ind;
  char *sourcesp, *infop;
  char *sp, *ip;
  int i;
  char **info;
  char **tail;
  int *info_size;
  MsgList *ml;
  int offset;
  int max_length = 256;
  int num;

  if (msg == NULL)
    return(0);

  pm = GetPostmaster();
  ipm = pm->ipm;
  assert(ipm != NULL);
  bfm = ipm->back_ptr;
  future = AllocateFutureIndex(pm, COMPLETION_RESULT, 0);
  assert(future);

  sources = (int **) calloc(ipm->nnodes, sizeof(int *));
  sizes = (int *) calloc(ipm->nnodes, sizeof(int));
  counts = (int *) calloc(ipm->nnodes, sizeof(int));
  info = (char **) calloc(ipm->nnodes, sizeof(char *));
  tail = (char **) calloc(ipm->nnodes, sizeof(char *));
  info_size = (int *) calloc(ipm->nnodes, sizeof(int));

  ipm->replies = (char **) calloc(ipm->nnodes, sizeof(char *));

  for (count = 0; count < nmsgs; count++)
    {
      if (msg->src == (Element *) pm)
	{
	  mo = GetMsgOutByMsgIn(msg->src, msg);
	  if (mo != NULL)
	    {
	      num = GetMsgOutNumber(pm, mo) + 1;
	      idx = FindBufferIndex(bfm, (KeyType) num);
	      assert(idx != 0);
	      blk = BLOCK_NO(idx);
	      ind = INDEX_NO(idx);
	      pbp = &(((BackPtr *) (bfm->item[blk]))[ind]);
	      src_node = pbp->src_node;
	      if (counts[src_node] >= sizes[src_node])
		{
		  sizes[src_node] = sizes[src_node] ? 2*sizes[src_node] : 64;
		  RECALLOC(sources[src_node], sizes[src_node], sizeof(int), int*);
		}
	      sources[src_node][counts[src_node]++] = pbp->idx;
	      offset = tail[src_node] - info[src_node];
	      if (offset + max_length > info_size[src_node])
		{
		  info_size[src_node]  = info_size[src_node] ? 2*info_size[src_node] : 1024;
		  RECALLOC(info[src_node], info_size[src_node], sizeof(char), char*);
		  tail[src_node] = info[src_node] + offset;
		}
	      ml=GetMsgListByType(object, msg->type);
	      sprintf(tail[src_node], "%s type [%d] '%s' ",
		      MSGINFORW(msg) ? "(forwarded)" : "",
		      msg->type, ml ? ml->name : "nil");
	      tail[src_node] += strlen(tail[src_node]);	
	      RemoteDisplayMsgIn(tail[src_node], msg, ml->slotname);
	      tail[src_node] += strlen(tail[src_node]);
	      sprintf(tail[src_node], "\n");
	      tail[src_node] += strlen(tail[src_node]);
	    }
	}
      msg = MSGINNEXT(msg);
    }

  for (src_node = 0; src_node < ipm->nznodes; ++src_node)
    if (counts[src_node] > 0)
      {
	IncrementFutureExpected(pm, future);

	InitPvmMessage(pm);
	PackCore(pm, PAR_SHOW_MSG_IN_REQ);
	if (pvm_pkint(&(pm->mynode), 1, 1) < 0 ||
	    pvm_pkint(&src_node, 1, 1) < 0 ||
	    pvm_pkint(&future, 1, 1) < 0 ||
	    pvm_pkint(&counts[src_node], 1, 1) < 0 ||
	    pvm_pkint(sources[src_node], counts[src_node], 1) < 0 ||
	    !SendLocalSetupMessage(pm, src_node))
	  PvmError();
      }

  (void) WaitFutureResolved(pm, future);

  /* combine the remote and local strings */
  for (i = 0; i < ipm->nnodes; ++i)
    {
      if (ipm->replies[i] != NULL && info[i] != NULL)
	{
	  sourcesp = ipm->replies[i];
	  infop = info[i];
	  while (*sourcesp != '\0')
	    {
	      sp = sourcesp;
	      while (*sp != '\0' && *sp != '\n')
		++sp;
	      if (*sp == '\n')
		*sp++ = '\0';
	      ip = infop;
	      while (*ip != '\0' && *ip != '\n')
		++ip;
	      if (*ip == '\n')
		*ip++ = '\0';
	      printf("MSG from '%s@%d' %s\n", sourcesp, i, infop);
	      sourcesp = sp;
	      infop = ip;
	    }
	}
      if (ipm->replies[i] != NULL)
	{
	  free(ipm->replies[i]);
	  ipm->replies[i] = NULL;
	}
      if (info[i] != NULL)
	free(info[i]);
      if (sources[i] != NULL)
	free(sources[i]);
    }

  free(sources);
  free(sizes);
  free(counts);
  free(info);
  free(tail);
  free(info_size);
  free(ipm->replies);
  ipm->replies = NULL;
}

ShowLocalMsgOut (msgin, nmsgs)
     Msg *msgin;
     int nmsgs;
{
  int count;
  int i;
  MsgList	*ml;
  Postmaster	*pm;

  if (msgin == NULL)
    return;

  pm = GetPostmaster();
  for(count = 0;count < nmsgs; count++)
    {
      if (VISIBLE(msgin->dst) && msgin->dst != (Element *) pm)
	{
	  ml=GetMsgListByType(msgin->dst->object, msgin->type);
	  if (ml)
	    printf("MSG %d to '%s' type [%d] '%s' ", 
		   count, Pathname(msgin->dst),
		   msgin->type, ml->name);
	  else
	    printf("MSG %d to '%s' type [%d] '%s' ", 
		   count, Pathname(msgin->dst),
		   msgin->type, "nil");
	  DisplayMsgIn(msgin, ml->slotname);
	  printf("\n");
	}
      
      msgin = MSGOUTNEXT(msgin);
    }
}


ShowRemoteActiveMsgOut(msg_out, nmsgs)
     MsgOut *msg_out;
     int nmsgs;
{
  int **dests;
  int *sizes;
  int *counts;
  int count;
  int i;
  MsgIn	*mi;
  MsgList *ml;
  int future;
  int act_idx;
  unsigned int *pdstidx;
  int dst_node, dst_idx;
  Postmaster *pm;
  Implementation *ipm;
  BufferManager *bfm;
  int blk;
  int ind;
  int cnt;
  int dstidx;

  if ((pm = GetPostmaster()) == NULL)
    return(0);
  ipm = pm->ipm;
  assert(ipm != NULL);
  future = AllocateFutureIndex(pm, COMPLETION_RESULT, 0);
  assert(future);

  bfm = ipm->in_active_msg;
  if (bfm == NULL)
    return(0);

  dests = (int **) calloc(ipm->nnodes, sizeof(int *));
  sizes = (int *) calloc(ipm->nnodes, sizeof(int));
  counts = (int *) calloc(ipm->nnodes, sizeof(int));
  
  for (count = 0; count < nmsgs; count++)
    {
      if (msg_out->dst == (Element *) pm)
	{
	  mi = GetMsgInByMsgOut(msg_out->dst, msg_out);
	  if (mi->type == PAR_ACTIVE_MSG)
	    {
	      assert(mi->nslots == 1);
	      act_idx = MSGVALUE(mi, 0);
	      /* INT_STRUCT_LOOP_HEAD(bfm, act_idx, dstidx) */
	      InitIntStructLoop(bfm, act_idx);
	      while (IterIntStructLoop(&dstidx))
		{
		  dst_node = dstidx >> (8*sizeof(unsigned short));
		  assert(dst_node >= 0);
		  assert(dst_node < ipm->nnodes);
		  dst_idx = dstidx - (dst_node << (8*sizeof(unsigned short)));
		  if (counts[dst_node] >= sizes[dst_node])
		    {
		      sizes[dst_node] = sizes[dst_node] ? 2*sizes[dst_node] : 64;
		      RECALLOC(dests[dst_node], sizes[dst_node], sizeof(int), int*);
		    }
		  dests[dst_node][counts[dst_node]++] = dst_idx;
		}
	      /* INT_STRUCT_LOOP_TAIL; */
	    }
	}
      msg_out = MSGOUTNEXT(msg_out);
    }
  
  for (dst_node = 0; dst_node < ipm->nznodes; ++dst_node)
    if (counts[dst_node] > 0)
      {
	IncrementFutureExpected(pm, future);
	
	InitPvmMessage(pm);
	PackCore(pm, PAR_SHOW_ACTIVE_MSG_OUT_REQ);
	if (pvm_pkint(&(pm->mynode), 1, 1) < 0 ||
	    pvm_pkint(&dst_node, 1, 1) < 0 ||
	    pvm_pkint(&future, 1, 1) < 0 ||
	    pvm_pkint(&counts[dst_node], 1, 1) < 0 ||
	    pvm_pkint(dests[dst_node], counts[dst_node], 1) < 0 ||
	    !SendLocalSetupMessage(pm, dst_node))
	  PvmError();
      }

  free(dests);
  free(sizes);
  free(counts);

  (void) WaitFutureResolved(pm, future);
}

ShowRemotePassiveMsgOut (elt)
     Element *elt;
{
  void **fields;
  int field_count;
  char *adr;
  char *ptr;
  char *tmp;
  char field[100];
  int prot;
  char name[100];
  char string[1000];
  int total;
  int i, j;
  void *fli;
  Postmaster *pm;
  Implementation *ipm;
  AddrIndex *addr_indices;
  int addr_indices_count;
  int *matches;
  int match_count;
  Info info;
  int dst_node;
  int blk, idx;
  BufferManager *bfm;
  int index;
  int future;

  if ((pm = GetPostmaster()) == NULL)
    return(0);
  ipm = pm->ipm;
  assert(ipm != NULL);
  future = AllocateFutureIndex(pm, COMPLETION_RESULT, 0);
  assert(future);

  /* find all outgoing passive messages */

  /* we first make a table of all the possible field references;
     this is kind of ugly */
  fields = NULL;
  field_count = 0;

  /* find addresses of the fixed fields of this element */
  adr = (char *) elt;
  ptr = (char *) FieldHashFind(elt->object->type);
  if (ptr != NULL)
    {
      strcpy(string, ptr);
      ptr = string;
      while (*ptr != '\0')
	{
	  /*
	  ** find the next field in the list by searching for a CR
	  */
	  tmp = ptr;
	  while (*tmp != '\n') tmp++;
	  *tmp = '\0';
	  /*
	  ** get the field name
	  */
	  strcpy(field,ptr);
	  /*
	  ** advance the ptr beyond the field name
	  */
	  ptr = tmp+1;
	  prot = GetFieldListProt(elt->object, field);
	  adr = (char *) elt;
	  strcpy(name, elt->object->type);
	  strcat(name, ".");
	  strcat(name, field);
	  if (!GetInfo(name, &info))
	    {
	      fprintf(stderr, "Could not find field %s\n", name);
	      exit(1);
	    }
	  if (info.dimensions == 0)
	    RememberAddress(&fields, &field_count, adr + info.offset);
	  else
	    {
	      total = 1;
	      for (i = 0; i < info.dimensions; ++i)
		total *= info.dimension_size[i];	
	      if (info.field_indirection)
		for (i = 0; i < total; ++i)
		  RememberAddress(&fields, &field_count,
				  *((char *) (adr + info.offset + i*sizeof(char *))));
	      else
		for (i = 0; i < total; ++i)
		  RememberAddress(&fields, &field_count,
				  adr + info.offset + i*info.type_size);
	    }
	}
    }

  /* then find addresses of the extended fields of this element */
  fli = FLIOpen(elt->object);
  while (FLIValid(fli))
    {
      switch(FLIType(fli))
	{
	case FIELD_EXTENDED:
	  RememberAddress(&fields, &field_count,
			  elt->extfields[((ExtFieldInfo*)FLIInfo(fli))->ext_index]);
	  break;

	case FIELD_INDIRECT:
	  {
	    Element*	cwe;
	    Element*	actElm;
	    Element*	indElm;
	    IndFieldInfo*	indInfo = FLIInfo(fli);
	    
	    /* Get the indirect element relative to the element */
	    cwe = WorkingElement();
	    SetWorkingElement(elt);
	    actElm = ActiveElement;
	    ActiveElement = elt;
	    indElm = GetElement(indInfo->ind_elm);
	    SetWorkingElement(cwe);
	    ActiveElement = actElm;

	    if (indElm == NULL)
	      {
		fprintf(stderr, "ShowRemotePassiveMsgOut: dangling indirect field reference\n");
		fprintf(stderr, "from element %s field %s to element %s field %s\n",
			Pathname(elt), FLIName(fli),
			indInfo->ind_elm, indInfo->ind_field);
		exit(1);
	      }
	    else
	      RememberAddress(&fields, &field_count, ElmFieldValue(indElm, indInfo->ind_field));
	  }
	  break;
	}
      FLINext(fli);
    }
  FLIClose(fli);

  /* now we sort the addresses */
  qsort(fields, field_count, sizeof(void *), AddrCompare);

  /* go through the postmaster's tables, looking for references
     to any of the fields */
  for (dst_node = 0; dst_node < ipm->nznodes; ++dst_node)
    {
      if (dst_node == pm->mynode)
	continue;
      addr_indices = NULL;
      addr_indices_count = 0;

      /* gather up all address/index pairs */
      /* NOTE: if message deletion is implemented, this code
	 will have to be changed */
      bfm = ipm->field_info;
      if (bfm != NULL)
	for (blk = 0; blk < bfm->blkcnt; ++blk)
	  if (bfm->key[blk] == NULL)
	    break;
	  else
	    for (idx = (blk == 0 ? 1 : 0); idx < bfm->siz; ++idx)
	      if (bfm->key[blk][idx] == 0)
		break;
	      else
		{
		  index = MAKE_INDEX(blk, idx);
		  if (!PENDING_INDEX(index))
		    RememberAddrIndex(&addr_indices, &addr_indices_count,
				      (void *) bfm->key[blk][idx], index);
		}

      /* sort the address/index pairs */
      if (addr_indices_count > 0)
	qsort(addr_indices, addr_indices_count,
	      sizeof(AddrIndex), AddrIndexCompare);

      /* now go through and find the intersection */
      matches = NULL;
      match_count = 0;

      j = 0;
      for (i = 0; i < field_count; ++i)
	if (j >= addr_indices_count)
	  break;
	else if (fields[i] < addr_indices[j].addr)
	  continue;
	else if (fields[i] > addr_indices[j].addr)
	  {
	    --i;
	    ++j;
	    continue;
	  }
	else
	  {
	    RememberIndex(&matches, &match_count, addr_indices[j].index);
	    ++j;
	  }

      if (match_count > 0)
	{
	  /* send the matching indices to the destination node */
	  IncrementFutureExpected(pm, future);
     
	  InitPvmMessage(pm);
	  PackCore(pm, PAR_SHOW_PASSIVE_MSG_OUT_REQ);
	  if (pvm_pkint(&(pm->mynode), 1, 1) < 0 ||
	      pvm_pkint(&dst_node, 1, 1) < 0 ||
	      pvm_pkint(&future, 1, 1) < 0 ||
	      pvm_pkint(&match_count, 1, 1) < 0 ||
	      pvm_pkint(matches, match_count, 1) < 0 ||
	      !SendLocalSetupMessage(pm, dst_node))
	    PvmError();
	}

      /* clean up */
      if (matches != NULL)
	free(matches);
      if (addr_indices != NULL)
	free(addr_indices);
    }
  
  /* clean up */
  if (fields != NULL)
    free(fields);

  (void) WaitFutureResolved(pm, future);
  printf("\n");
}


/**********************************************************************
 *		PHASE 2:  LOOKUP DESTINATIONS ON REMOTE NODE
 **********************************************************************/

HandleRemoteShowActiveMsgOutRequest (pm, src_tid)
     Postmaster *pm;
     int src_tid;
{
  Implementation *ipm;
  BufferManager *bfm;
  int dst_node;
  int *dests;
  int future;
  MsgOut *mo;
  MsgIn *mi;
  int blk, ind;
  int src_node;
  unsigned int *pmsgno;
  unsigned int msgno;
  int msgcnt;
  char *reply;
  char *tail;
  int max_length = 256;		/* maximum length of a single line */
  MsgList *ml;
  int reply_length;
  int size;
  int i;
  int dst_idx;
  int count;
  int offset;

  /* unpack the message */
  if (pvm_upkint(&src_node, 1, 1) < 0 ||
      pvm_upkint(&dst_node, 1, 1) < 0 ||
      pvm_upkint(&future, 1, 1) < 0 ||
      pvm_upkint(&count, 1, 1) < 0)
    PvmError();
  dests = (int *) malloc(count * sizeof(int));
  if (pvm_upkint(dests, count, 1) < 0)
    PvmError();

  ipm = pm->ipm;
  assert(ipm != NULL);
  bfm = ipm->out_active[src_node];
  assert(bfm != NULL);

  size = 1024;
  reply  = (char *) malloc(size*sizeof(char));
  tail = reply;
  *tail = '\0';

  for (i = 0; i < count; ++i)
    {
/*      MSG_STRUCT_LOOP_HEAD(bfm, dests[i], mo) */
      InitMsgStructLoop(bfm, dests[i]);
      while (IterMsgStructLoop(&mo))
	{
	  if (VISIBLE(mo->dst))
	    {
	      mi = (MsgIn *) GetMsgInByMsgOut(mo->dst, mo);
	      ml = GetMsgListByType(mo->dst->object, mi->type);
	      offset = tail - reply;
	      if (offset + max_length > size)
		{
		  size *= 2;
		  reply = (char *) realloc(reply, size);
		  tail = reply + offset;
		}
	      sprintf(tail, "MSG to '%s@%d' type [%d] '%s' ", 
		      Pathname(mo->dst), dst_node,
		      mi->type,
		      ml ? ml->name : "nil");
	      tail += strlen(tail);
	      RemoteDisplayMsgIn(tail, mi, ml->slotname);
	      tail += strlen(tail);
	      sprintf(tail, "\n");
	      tail += strlen(tail);
	    }
	}
/*      MSG_STRUCT_LOOP_TAIL; */
    }

  InitPvmMessage(pm);
  PackCore(pm, PAR_SHOW_MSG_OUT_REPLY);
  reply_length = strlen(reply);
  if (pvm_pkint(&dst_node, 1, 1) < 0 ||
      pvm_pkint(&future, 1, 1) < 0 ||
      pvm_pkint(&reply_length, 1, 1) < 0 ||
      pvm_pkstr(reply) < 0 ||
      !SendLocalSetupMessageByTid(pm, src_tid))
    PvmError();
  free(dests);
  free(reply);
}

HandleRemoteShowPassiveMsgOutRequest (pm, src_tid)
     Postmaster *pm;
     int src_tid;
{
  Implementation *ipm;
  BufferManager *bfm;
  int dst_node;
  int *indices;
  int future;
  MsgOut *mo;
  MsgIn *mi;
  int src_node;
  char *reply;
  char *tail;
  int max_length = 256;		/* maximum length of a single line */
  MsgList *ml;
  int reply_length;
  int size;
  int i;
  int dst_idx;
  int count;
  int first, last, mid;
  void **addrs;
  int mapidx;
  Msg *msg;
  int m;
  int found;
  void *addr;
  int offset;

  /* unpack the message */
  if (pvm_upkint(&src_node, 1, 1) < 0 ||
      pvm_upkint(&dst_node, 1, 1) < 0 ||
      pvm_upkint(&future, 1, 1) < 0 ||
      pvm_upkint(&count, 1, 1) < 0)
    PvmError();
  indices = (int *) malloc(count * sizeof(int));
  if (pvm_upkint(indices, count, 1) < 0)
    PvmError();

  ipm = pm->ipm;
  assert(ipm != NULL);

  /* convert the indices to addresses */
  addrs = (void **) malloc(count * sizeof(void *));
  for (i = 0; i < count; ++i)
    {
      mapidx = INDEX(indices[i]);
      addrs[i] = GetBufferIndexItem(ipm->out_data[src_node], mapidx);
    }

  /* sort the fields */
  qsort(addrs, count, sizeof(void *), AddrCompare);

  size = 1024;
  reply  = (char *) malloc(size*sizeof(char));
  tail = reply;
  *tail = '\0';

  /* go through the postmaster's outgoing message list */
  msg = pm->msgout;
  for (m = 0; m < pm->nmsgout; ++m)
    {
      /* only deal with passive messages with visible destinations */
      if (msg->type >= 0 && VISIBLE(msg->dst))
	{
	  found = FALSE;
	  for (i = 0; i < msg->nslots; ++i)
	    {
	      addr = (void *) MSGPTR(msg,i);
	      /* use binary search to find if in table */
	      first = 0;
	      last = count - 1;
	      while (first <= last)
		{
		  mid = (first + last) / 2;
		  if (addr == addrs[mid])
		    {
		      found = TRUE;
		      break;
		    }
		  else if (addr < addrs[mid])
		    last = mid - 1;
		  else
		    first = mid + 1;
		}
	      if (found)
		break;
	    }

	  /* if it is there, generate the reply */
	  if (found)
	    {
	      ml = GetMsgListByType(msg->dst->object, msg->type);
	      offset = tail - reply;
	      if (offset + max_length > size)
		{
		  size *= 2;
		  reply = (char *) realloc(reply, size);
		  tail = reply + offset;
		}
	      sprintf(tail, "MSG to '%s@%d' type [%d] '%s' ", 
		      Pathname(msg->dst), dst_node,
		      msg->type,
		      ml ? ml->name : "nil");
	      tail += strlen(tail);
	      RemoteDisplayMsgIn(tail, msg, ml->slotname);
	      tail += strlen(tail);
	      sprintf(tail, "\n");
	      tail += strlen(tail);
	    }
	}

      msg = MSGOUTNEXT(msg);
    }

  /* send back the reply */
  InitPvmMessage(pm);
  PackCore(pm, PAR_SHOW_MSG_OUT_REPLY);
  reply_length = strlen(reply);
  if (pvm_pkint(&dst_node, 1, 1) < 0 ||
      pvm_pkint(&future, 1, 1) < 0 ||
      pvm_pkint(&reply_length, 1, 1) < 0 ||
      pvm_pkstr(reply) < 0 ||
      !SendLocalSetupMessageByTid(pm, src_tid))
    PvmError();
  free(reply);
  free(addrs);
  free(indices);
}

HandleRemoteShowMsgInRequest (pm, src_tid)
     Postmaster *pm;
     int src_tid;
{
  int *sources;
  char *reply;
  char *tail;
  int max_length = 256;		/* maximum length of a single line */
  int reply_length;
  int dst_node;
  int src_node;
  int future;
  int count;
  int size;
  int i;
  MsgIn *mi;
  Implementation *ipm;
  BufferManager *bfm;
  int offset;
  int src_idx;
  int blk, ind;
  Element *src;

  ipm = pm->ipm;
  bfm = ipm->src;

  if (pvm_upkint(&dst_node, 1, 1) < 0 ||
      pvm_upkint(&src_node, 1, 1) < 0 ||
      pvm_upkint(&future, 1, 1) < 0 ||
      pvm_upkint(&count, 1, 1) < 0)
    PvmError();
  sources = (int *) malloc(count * sizeof(int));
  if (pvm_upkint(sources, count, 1) < 0)
    PvmError();

  size = 1024;
  reply = (char *) malloc(size*sizeof(char));
  tail = reply;
  *tail = '\0';

  for (i = 0; i < count; ++i)
    {
      src_idx = sources[i];
      blk = BLOCK_NO(src_idx);
      ind = INDEX_NO(src_idx);
      src = (Element *) bfm->key[blk][ind];
      offset = tail - reply;
      if (offset + max_length > size)
	{
	  size *= 2;
	  reply = (char *) realloc(reply, size);
	  tail = reply + offset;
	}
      sprintf(tail, "%s\n", VISIBLE(src) ? Pathname(src) : "");
      tail += strlen(tail);
    }

  InitPvmMessage(pm);
  PackCore(pm, PAR_SHOW_MSG_IN_REPLY);
  reply_length = strlen(reply);
  if (pvm_pkint(&src_node, 1, 1) < 0 ||
      pvm_pkint(&future, 1, 1) < 0 ||
      pvm_pkint(&reply_length, 1, 1) < 0 ||
      pvm_pkstr(reply) < 0 ||
      !SendLocalSetupMessageByTid(pm, src_tid))
    PvmError();
  free(sources);
  free(reply);
}


/**********************************************************************
 *		PHASE 3:  FINAL HANDLING AT SOURCE
 **********************************************************************/

int
HandleRemoteShowMsgOutReply (pm)
     Postmaster *pm;
{
  int future;
  int reply_length;
  char *reply;
  int dst_node;
  
  if (pvm_upkint(&dst_node, 1, 1) < 0 ||
      pvm_upkint(&future, 1, 1) < 0 ||
      pvm_upkint(&reply_length, 1, 1) < 0)
    PvmError();
  reply = (char *) malloc(reply_length+1);
  if (pvm_upkstr(reply) < 0)
    PvmError();

  printf("%s", reply);
  free(reply);

  if (future != 0)
    NoteFuture(pm, future);
  return 1;
}

int
HandleRemoteShowMsgInReply (pm)
     Postmaster *pm;
{
  int future;
  int reply_length;
  char *reply;
  int src_node;
  Implementation *ipm;
  
  ipm = pm->ipm;
  if (pvm_upkint(&src_node, 1, 1) < 0 ||
      pvm_upkint(&future, 1, 1) < 0 ||
      pvm_upkint(&reply_length, 1, 1) < 0)
    PvmError();
  ipm->replies[src_node] = (char *) malloc(reply_length+1);
  if (pvm_upkstr(ipm->replies[src_node]) < 0)
    PvmError();

  if (future != 0)
    NoteFuture(pm, future);
  return 1;
}


/**********************************************************************
 *		UTILITY FUNCTIONS
 **********************************************************************/

RemoteDisplayMsgIn(str, msg_in, slotnames)
     char *str;
     MsgIn *msg_in;
     char **slotnames;
{
  Slot *slot;
  int i;
  PFD func;

  if (msg_in == NULL)
    return;

  slot = (Slot *) (msg_in + 1);
  for (i = 0; i < msg_in->nslots && slotnames[i] != NULL; i++)
    {
      sprintf(str, "< %s = ", slotnames[i]);
      str += strlen(str);
      func = slot[i].func;
      if (func == DoubleMessageData)
	sprintf(str, "%g",*((double *)(slot[i].data)));
      else if (func == FloatMessageData)
	sprintf(str, "%g",*((float *)(slot[i].data)));
      else if (func == IntMessageData)
	sprintf(str, "%d",*((int *)(slot[i].data)));
      else if (func == ShortMessageData)
	sprintf(str, "%d",*((short *)(slot[i].data)));
      else if (func == StringMessageData)
	sprintf(str, "%s",slot[i].data);
      else /* externally defined slot type, just call the conversion
	    * function and print as a double
	    */
	sprintf(str, "%g", func(slot+i));
      str += strlen(str);
      sprintf(str, " > ");
    }
}

static int AddrCompare (p1, p2)
     void **p1, **p2;
{
  if (*p1 < *p2)
    return(-1);
  else if (*p1 == *p2)
    return(0);
  else
    return(1);
}

static int AddrIndexCompare (p1, p2)
     AddrIndex *p1, *p2;
{
  if (p1->addr < p2->addr)
    return(-1);
  else if (p1->addr == p2->addr)
    return(0);
  else
    return(1);
}

static void RememberAddress (pfields, pfield_count, addr)
     void ***pfields;
     int *pfield_count;
     void *addr;
{
  if (*pfield_count == 0)
      *pfields = (void **) malloc(256*sizeof(void *));
  else if (*pfield_count == 256)
      *pfields = (void **) realloc(*pfields, 4096*sizeof(void *));
  else if (*pfield_count == 4096)
      *pfields = (void **) realloc(*pfields, 65536*sizeof(void *));
  else if (*pfield_count == 65536)
      *pfields = (void **) realloc(*pfields, 1048576*sizeof(void *));
  (*pfields)[(*pfield_count)++] = addr;
}

static void RememberAddrIndex (pfields, pfield_count, addr, index)
     AddrIndex **pfields;
     int *pfield_count;
     void *addr;
     int index;
{
  if (*pfield_count == 0)
      *pfields = (AddrIndex *) malloc(256*sizeof(AddrIndex));
  else if (*pfield_count == 256)
      *pfields = (AddrIndex *) realloc(*pfields, 4096*sizeof(AddrIndex));
  else if (*pfield_count == 4096)
      *pfields = (AddrIndex *) realloc(*pfields, 65536*sizeof(AddrIndex));
  else if (*pfield_count == 65536)
      *pfields = (AddrIndex *) realloc(*pfields, 1048576*sizeof(AddrIndex));
  (*pfields)[*pfield_count].addr = addr;
  (*pfields)[*pfield_count].index = index;
  ++(*pfield_count);
}

static void RememberIndex (pfields, pfield_count, index)
     int **pfields;
     int *pfield_count;
     int index;
{
  if (*pfield_count == 0)
      *pfields = (int *) malloc(256*sizeof(int));
  else if (*pfield_count == 256)
      *pfields = (int *) realloc(*pfields, 4096*sizeof(int));
  else if (*pfield_count == 4096)
      *pfields = (int *) realloc(*pfields, 65536*sizeof(int));
  else if (*pfield_count == 65536)
      *pfields = (int *) realloc(*pfields, 1048576*sizeof(int));
  (*pfields)[(*pfield_count)++] = index;
}
