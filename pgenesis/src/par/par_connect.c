static char rcsid[] = "$Id: par_connect.c,v 1.2 2005/09/29 23:12:01 ghood Exp $";

/*
 * $Log: par_connect.c,v $
 *
 * Revision 1.3  2018/08/08 13:13:00 jcrone
 * Moved generation of random number for determining whether a
 * connection should be made from the destination node to the source
 * node. This ensures that the connectivity is repeatable from one
 * simulation to another.
 * Fixed memory leak in CompleteRemoteMessages().
 *
 * Revision 1.2  2005/09/29 23:12:01  ghood
 * updated for PGENESIS 2.3
 *
 * Revision 1.1.1.1  2005/06/14 04:38:39  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.15  2000/06/19 05:41:57  mhucka
 * It doesn't seem necessary to declare the return type of strcpy(), and in
 * fact on some systems it may confuse the compiler.  So let's try removing
 * it.
 *
 * Revision 1.14  1999/12/19 03:59:41  mhucka
 * Adding PGENESIS from GENESIS 2.1 CDROM release.
 *
 * Revision 1.13  1997/07/22 23:03:13  ghood
 * Replaced getopt/optopt with G_getopt/G_optopt
 *
 * Revision 1.12  1997/05/19 17:52:32  ghood
 * bug fix to MakeBufferIndex call in HandleSingleRemoteMessageRequest
 *
 * Revision 1.11  1997/03/27 06:26:45  ngoddard
 * mods to scale up to larger networks
 *
 * Revision 1.10 1997/02/20 21:13:33 ngoddard 
 * support for xpvm; using pvm_trecv for timeout ops; fixed bugs
 * related to futures; provide user warnings when appears stalled;
 * barrer and barrierall take a third argument: how long to wait
 * before stall is assumed; beginnings of performance stats package
 *
 * Revision 1.9  1997/02/18 04:28:59  ngoddard
 * number literal slots passed once at setup time, no longer on each step
 *
 * Revision 1.8  1997/02/10 02:46:16  ngoddard
 * add_remote_msg wasn't completing future under failure conditions
 *
 * Revision 1.7  1997/01/31  05:14:19  ngoddard
 * barriers don't complete asyncs now, minor bug fixes
 *
 * Revision 1.6  1997/01/21 22:20:19  ngoddard
 * paragon port
 *
 * Revision 1.5 1997/01/21 20:19:42 ngoddard bug fixes for zones,
 * implement string literal fields, first release of pgenesis script
 *
 * Revision 1.4  1996/08/19 21:34:02  ghood
 * Made pvm calls type-consistent with their parameters.
 *
 * Revision 1.3  1996/08/13 21:25:00  ghood
 * General debugging to get parallel example scripts working.
 *
 * Revision 1.2  1996/06/21 16:10:39  ghood
 * General debugging
 *
 * Revision 1.1  1996/05/03 19:11:23  ngoddard
 * Initial revision
 *
 */

#include <stdio.h>
#include "par_ext.h"
#include "par_struct.h"
#include "newconn_struct.h"
#include "newconn_defs.h"
#include "pvm-defs.h"

extern double StringMessageData();
extern double DoubleMessageData();
extern char * WaitFutureResolved();
extern ElementList *CreateElementList();
extern char * MallocedString();

static int minus1 = -1;
static int zero = 0;
static int BadFieldCount = BAD_FIELD_COUNT;
static int BadType = BAD_TYPE;
static int MixedType = MIXED_TYPE;
static int ParOk = PAR_OK;
static int ParRequestError = PAR_REQUEST_ERROR;
static int ParRequestPassive = PAR_REQUEST_PASSIVE;
static int ParRequestActive = PAR_REQUEST_ACTIVE;
static int ParRequestLiteral = PAR_REQUEST_LITERAL;

/* FORWARD DECLARATIONS */
char *remote_add_msg_entry();
char *remote_volume_connect_entry();
char *add_remote_msg();
char *AssignOutDataField();
void CompleteRemotePassiveMessage();
void CompleteRemoteLiteralMessage();
void CompleteRemoteActiveMessage();
static double LiteralMessageData();
void NoteIfNewSrc();
void RecoverRemoteMessageError();
char *ReferString();
void UnreferString();
unsigned char TypeFromFunc();
int HoldPointer();
void *ReleasePointer();
void PackMaskData();
void UnPackMaskData();

/* The strategy is as follows.  We do what checking we can at the source
   end.  Then we fill in information needed at the destination end.  Then
   we create the field index list.  This is a list of local indices of
   fields, arranged in order by src element number and slot number.  Each
   index uses the top two bits as flags.  The msbit means this is a "
   pending" index into pending_field[dst].  The next msbit means this is
   a new use of this index into the pending_field array (thus it can only
   be set if the msbit is set).  If neither is set, the index is into the
   data_index[dst] vector, which is used to index field_info e.g.,
   field_info[data_index[dst][i]].  Initially all indices are into the
   pending_field and all are new uses.  Field info is tranferred
   into the field_info when an error-free response is recieved from the
   destination (see below).  The pending_field_info entry is marked as
   reuseable by setting the field address to NULL.  Whenever an entry in
   pending_field_info is used, if its addr is NULL (a new use) the
   new-use bit in the index sent to the dst is set.  The index list is
   sent with each request for a message to the dst node.  The dst node
   does its checking.  If a message request has any problems, none of it
   is established but an error code returned to the sender.  If the
   message request is ok, the dst assigns new indices into the
   out_data[src] vector for any incoming indices marked as "new-use", and
   saves each new out_data index in
   pending_indices[incoming-new-use-index] to be used the next time the
   same incoming index marked as "pending" is used.  If the message
   request contained errors but there were "new-use" indices, then the
   pending_indices[incoming-new-use-index] is set to -1 meaning no
   out_data index has been assigned yet.  If the src gets a no_error
   response, it assigns indices into in_data[dst] in the same order that
   the dst assigned indices into out_data[src] [NOTE: this relies on PVM
   guaranteeing that messages are delivered in the order in which they
   are sent.  If this were not the case, the dst would have to establish
   the indices and ship them back to the src].  If the src gets an error
   response, it does not establish indices and does not tranfer fields
   from pending to field info.  At communication time, the src gets the
   field value for buffer[dst_node][i] by accessing
   field_info[field_info[field_info[dst_node][i].key].data.

   On the dst, out_data[src_node] is actually a vector of vectors of
   doubles.  As more messages are established, we add more vectors of
   doubles.  This is because AddMsg sets a pointer to one of the doubles
   so we can't move it with realloc.  When it comes time to implement
   message deletion, we'll have to maintain a list of free doubles in the
   vectors because we can't consolidate them.  On the src, we'll do the
   same thing to avoid reallocing.  The index into the out_data[src] and
   in_data[dst] will be communicated as a single integer, but used as
   e.g., out_data[src][index/BLK_SIZE][index%BLK_SIZE].

   Individual message requests (which may have multiple src/dst elements)
   are buffered in a list of MsgSpecs for the dst.  When enough messages
   are buffered for a node, we send them off to the node in a bunch to be
   processed.  For each request the destination does its stuff and
   buffers up specs of AddMsgs, using the buffer indices into
   out_data[src_node] to provide pointers for the slot array.  If the
   request is ok, the buffered AddMsgs are issued.  For each dst element
   the dst node notes if the message is active or passive.  If it is
   active, a message index is assigned which is used to index
   out_active[src][index] which is a vector of pointers to MsgOut's.  The
   list of buffer indices for the msg are saved.  [When it comes time to
   implement deleting messages, this means that we'll just mark a MsgOut
   as inoperative but not actually delete it (because then our MsgOut
   indices would need changing).  At some point deletion/index-reset will
   have to be done].  For each request, the dst packs back either (1) a
   series of error code and string (usually the pathname of the dst
   element) followed by an END_ERROR code, or (2) a REQUEST_PASSIVE code
   and the list of buffer indices for each src element, or (3) a
   REQUEST_ACTIVE code and the out_active[src] index for each src
   element.  When all requests have been processed, a REQUESTS_DONE code
   is packed (all these codes/strings/lists are packed into one PVM
   message) and the PVM message sent back to the src. [Note: this assumes
   any one message spec only refers to active OR passive messages, not
   both].

   The src gets the response and prints out information if there were
   errors.  If there were no errors, for a passive message a NOTIFY_DELETE
   message from the src element is added if one does not already exist.
   The NOTIFY_DELETE message has one slot whose data field points to the src
   element. [This will be used to notify the PM if the src is deleted.
   But we'd need to save other info].  For active messages an EVENT
   message is added from the srcelt if none exists for the srcelt/fields
   we want.  This message has one extra slot whose func is Literal and
   whose data is an index into the PM's list of active messages.  Each
   entry in the list of active messages is a list of dst_node,
   active-message-index pairs.  The EVENT action gets the index from the
   msgin and the slot values if any.  For each dst_node it adds the
   dst-node-specific active-message-index to the in_active[dst_node]
   buffer, incrementing the counter (zeroed in the PROCESS action), and
   the slot values to the in_active_data[dst_node] buffer, incrementing
   its counter (also zeroed in PROCESS).  In PROCESS, the counters and
   buffers are packed and transmitted to the dst_nodes.  The dst_nodes
   use each active-message-index to access a vector of MsgOuts which
   gives dst_elt and MsgIn to use in CallEventAction.  The MsgIn tells us
   how many slots to unpack (each a double) and where to put them
   (pointed to by the data field of the slot.
   */

/*
   We have two types of indices.  One is global (indexes some array for
   all source/destinations.  The other is local, i.e., src/dst specific,
   i.e. indexes some array[dst] or some array[src].
*/

/**********************************************************************
 *		PHASE 1:  INITIAL CHECKING AT SOURCE
 **********************************************************************/
char * do_remote_add_msg (argc, argv)
     int  argc;
     char **argv;
{
  unsigned int future;
  Postmaster *pm;
  char * res = "";

  if ((pm = GetPostmaster()) != NULL)
    {
      future = AllocateFutureIndex(pm, COMPLETION_RESULT, 1);
      res = remote_add_msg_entry(argc, argv, FALSE, future);
    }
  return(MallocedString(res));
}

char * remote_add_msg_entry (argc, argv, async, future)
     int argc;
     char **argv;
     int async;
     unsigned int future;
{
  char *src_path;
  char *full_dst_path;
  char *typename;

  /* check the syntax */
  if (!CheckAddMsgSyntax(argc, argv, &src_path, &full_dst_path, &typename))
    return("");
  
  return(add_remote_msg(argc, argv, async, future, FALSE, 1.0, NULL, 0, NULL, 0, src_path, full_dst_path, typename));
}

int CheckAddMsgSyntax(argc, argv, src_path, dst_path, typename)
     int  argc;
     char **argv;
     char **src_path;
     char **dst_path;
     char **typename;
{
  char 		*ptr;
  int i;

  initopt(argc, argv, "source-element dest-delement msg-type [msg-fields]");

  if(G_getopt(argc, argv) != 0){
    TraceScript();
    printoptusage(argc, argv);
    return 0;
  }
  *src_path = optargv[1];
  *dst_path = optargv[2];
  *typename = optargv[3];
  return 1;
}

int FillSlotArray(src_list, pslot, sarray, slotname)
     ElementList * src_list;
     int * pslot;
     Slot * sarray;
     char ** slotname;
{
  int slot = 0;
  int nxtarg = 3;
  /*
   ** get the message specification information from the argument list
   */
  while(++nxtarg < optargc){
    if(optargv[nxtarg][0] == '*'){
      if(slot >= NSLOTS){
	Error();
	printf("exceeded the maximum number of data slots\n");
      } else { 
	sarray[slot].data = optargv[nxtarg]+1;
	AssignSlotFunc(sarray+slot,STRING);
	slotname[slot] = NULL;
	slot++;
      }
    } else 
      if(optargv[nxtarg][0] == '\''){
	if(slot >= NSLOTS){
	  Error();
	  printf("exceeded the maximum number of data slots\n");
	} else { 
	  /*
	   ** find the trailing quote if any
	   */
	  if(strchr(optargv[nxtarg]+1,'\'') != NULL){
	    /*
	     ** and remove it
	     */
	    *strchr(optargv[nxtarg]+1,'\'') = '\0';
	  }
	  sarray[slot].data = optargv[nxtarg]+1;
	  AssignSlotFunc(sarray+slot,STRING);
	  slotname[slot] = NULL;
	  slot++;
	}
      } else 
	/* 
	 ** variable
	 */
	if(is_alpha(optargv[nxtarg][0])){
	  if(slot >= NSLOTS){
	    Error();
	    printf("exceeded the maximum number of data slots\n");
	  } else { 
	    slotname[slot] = ReferString(optargv[nxtarg]);
	    sarray[slot].func = NULL; /* mark as variable */
	    slot++;
	  }
	} else
	  /* 
	   ** number 
	   ** ship once now, never changes (except EFIELD object?)
	   ** space allocated here is freed in once copied
	   */
	  if(is_num(optargv[nxtarg][0]) ||
	     (optargv[nxtarg][0] == '.' && is_num(optargv[nxtarg][1]))){ 
	    if(slot >= NSLOTS){
	      Error();
	      printf("exceeded the maximum number of data slots\n");
	    } else { 
	      sarray[slot].data = (char *) malloc(sizeof(double));
	      *((double *)(sarray[slot].data)) = Atof(optargv[nxtarg]);
	      AssignSlotFunc(sarray+slot,DOUBLE);
	      slotname[slot] = NULL;
	      slot++;
	    }
	  } else {
	    Error();
	    printoptusage(optargc, optargv);
	    return 0;
	  }
  }
  *pslot = slot;
  return 1;
}

char * do_remote_volume_connect (argc, argv)
     int argc;
     char **argv;
{
  unsigned int future;
  Postmaster *pm;
  char * res = "";

  if ((pm = GetPostmaster()) != NULL)
    {
      future = AllocateFutureIndex(pm, COMPLETION_RESULT, 1);
      res = remote_volume_connect_entry(argc, argv, FALSE, future);
    }
  return(MallocedString(res));
}

char * remote_volume_connect_entry (argc, argv, async, future)
     int argc;
     char **argv;
     int async;
     unsigned int future;
{
  char 			     *source_name;
  char 			     *dest_name;
  int 			      src_count,dst_count;
  struct extended_mask_type  *src_mask, *dst_mask;
  float			      pconnect;
  short			      relative;
  char *v;

  if (!CheckRemoteVolumeConnectSyntax(argc, argv,
				&source_name, &dest_name, &src_count, &dst_count,
				&src_mask, &dst_mask,
				&pconnect, &relative))
    return("");

  v = add_remote_msg(argc, argv, async, future, relative, pconnect, src_mask, src_count, 
			  dst_mask, dst_count, source_name, dest_name, "SPIKE");
  free(src_mask);
  free(dst_mask);
  return(v);
}

int CheckRemoteVolumeConnectSyntax(argc,argv,source_name,dest_name,src_count,dst_count,src_mask,dst_mask,pconnect,relative)
     int argc;
     char **argv;
     char **source_name;
     char **dest_name;
     int *src_count;
     int *dst_count;
     struct extended_mask_type **src_mask; 
     struct extended_mask_type **dst_mask;
     float *pconnect;
     short *relative;
{ 
  int status;
  
  /* set defaults */
  *src_mask = NULL;
  *dst_mask = NULL;
  *src_count = 0;
  *dst_count = 0;
  *relative = FALSE;
  *pconnect = 1.0;

  /* prepare to scan arguments */
  initopt(argc, argv, "source-path dest-path -relative -sourcemask {box,ellipsoid} x1 y1 z1 x2 y2 z2 -sourcehole {box,ellipsoid} x1 y1 z1 x2 y2 z2 -destmask {box,ellipsoid} x1 y1 z1 x2 y2 z2 -desthole {box,ellipsoid} x1 y1 z1 x2 y2 z2 -probability p");
    
  while ((status = G_getopt(argc, argv)) == 1)
    {
      if (strcmp(G_optopt, "-relative") == 0)
	*relative = 1;
      else if (strcmp(G_optopt, "-sourcemask") == 0)
	(*src_count)++;
      else if (strcmp(G_optopt, "-sourcehole") == 0)
	(*src_count)++;
      else if (strcmp(G_optopt, "-destmask") == 0)
	(*dst_count)++;
      else if (strcmp(G_optopt, "-desthole") == 0)
	(*dst_count)++;
      else if (strcmp(G_optopt, "-probability") == 0)
	*pconnect = Atof(optargv[1]);
    }
    
  if (status < 0)
    {
      printoptusage(argc, argv);
      return(FALSE);
    }
    
  if (*src_count < 1 || *dst_count < 1)
    {
      Error();
      printf("Must have at least one source and one destination mask\n");
      return(FALSE);
    }
  /*
   * Allocate memory for the appropriate number of source and destination 
   * masks.
   */
    
  *src_mask = (struct extended_mask_type*) 
    malloc((*src_count)*sizeof(struct extended_mask_type));
    
  if (*src_mask == NULL)
    {
      Error();
      perror(argv[0]);
      return(FALSE);
    }
    
  *dst_mask = (struct extended_mask_type*)
    malloc((*dst_count)*sizeof(struct extended_mask_type));
    
  if (*dst_mask == NULL)
    {
      Error();
      perror(argv[0]);
      free(*src_mask);
      return(FALSE);
    }
    
  /*
   * Set up the source and destination masks.
   * Re-initialize the argument list to extract the necessary parameters.
   */
    
  initopt(argc, argv, "source-path dest-path -relative -sourcemask {box,ellipsoid} x1 y1 z1 x2 y2 z2 -sourcehole {box,ellipsoid} x1 y1 z1 x2 y2 z2 -destmask {box,ellipsoid} x1 y1 z1 x2 y2 z2 -desthole {box,ellipsoid} x1 y1 z1 x2 y2 z2 -probability p");
    
  /* *src_count and *dst_count count the number of regions of each type */
    
  *src_count = 0;
  *dst_count = 0;
    
  while (G_getopt(argc, argv) == 1)
    {
      /*
       * "mask" is a generic mask pointer which we set to be the
       * address of the current *src_mask or *dst_mask.
       */
	
      struct extended_mask_type*	mask;      
      mask = NULL;
	
      if (strcmp(G_optopt, "-sourcemask") == 0)
	{
	  mask = (*src_mask) + (*src_count)++;
	  mask->type = CONNECTION_MASK;
	}
      else if (strcmp(G_optopt, "-destmask") == 0)
	{
	  mask = (*dst_mask) + (*dst_count)++;
	  mask->type = CONNECTION_MASK;
	}
      else if (strcmp(G_optopt, "-sourcehole") == 0)
	{
	  mask = (*src_mask) + (*src_count)++;
	  mask->type = CONNECTION_MASK_HOLE;
	}
      else if (strcmp(G_optopt, "-desthole") == 0)
	{
	  mask = (*dst_mask) + (*dst_count)++;
	  mask->type = CONNECTION_MASK_HOLE;
	}
	
      if (mask != NULL)
	{
	  /* 
	   * Choose between box-type mask and ellipsoid-type mask. 
	   * You can use a shorter designation than "box" or 
	   * "ellipsoid" (for example "b" or "e") as long as
	   * all the letters of the shorter word match the
	   * corresponding letters of one of the longer words
	   * ("box" or "ellipsoid").
	   */
	    
	  if (strncmp(optargv[1], "box", strlen(optargv[1])) == 0)
	    mask->shape = CONNECTION_MASK_BOX_SHAPE;
	  else if (strncmp(optargv[1], "ellipsoid", strlen(optargv[1])) == 0)
	    mask->shape = CONNECTION_MASK_ELLIPSOID_SHAPE;
	  else
	    {
	      Error();
	      printf("Only \"box\" and \"ellipsoid\" mask shapes allowed.\n");
	      return(FALSE);
	    }
	    
	  /* Set the parameters of the mask. */
	    
	  mask->x1 = Atof(optargv[2]); 
	  mask->y1 = Atof(optargv[3]); 
	  mask->z1 = Atof(optargv[4]);
	  mask->x2 = Atof(optargv[5]); 
	  mask->y2 = Atof(optargv[6]); 
	  mask->z2 = Atof(optargv[7]);
	}
    }
    
  /*
   * When all the other options have been dispensed with, what remains 
   * are the source and dest pathnames.
   */
    
  *source_name = optargv[1];
  *dest_name   = optargv[2];
  return(TRUE);
}

int FillTypeArray(nslot, sarray, slottype, string_val,
		  pstring_cnt, pstring_len)
     int nslot;
     Slot * sarray;
     char * slottype;
     char * string_val;		/* the strings, concatenated */
     int * pstring_cnt;		/* pointer to number of strings */
     int * pstring_len;		/* pointer to length of concatenated strings */
{
  int string_cnt = 0;		/* number of strings */
  int string_len = 0;		/* length of strings including nulls */
  int number_cnt = 0;		/* number of strings */
  int i;
  int t, nums;			/* current index into string_val */

  /* copy string slots and not indices */
  for (i = t = 0; i < nslot; i++)
    if (sarray[i].func == StringMessageData) /* literal string */
      {
	/* t is index into string_val for this string */
	slottype[i] = STRING_SLOT;
	strcpy(string_val+t, sarray[i].data);
	string_cnt++;
	t = (string_len += strlen(string_val+t) + 1);
      }
    else if (sarray[i].func == DoubleMessageData) /* literal number */
      {
	slottype[i] = LITERAL_SLOT;	/* mark for literal double */
	number_cnt++;
      }
    else			/* variable */
      slottype[i] = VARIABLE_SLOT;

  assert (t == string_len);
  *pstring_cnt = string_cnt;
  *pstring_len = string_len;
  return 1;
}

/* Blocks by default until the message is set up at both ends */
/* The <async> flag makes the call non-blocking, i.e. it returns */
/* before the dst node has done its stuff and before the src node */
/* has completed its stuff.  */
char * add_remote_msg(argc,argv, async, future, relative, pconnect, src_region,
			   nsrc_regions,dst_region,ndst_regions, src_path, full_dst_path, typename) 
     int  argc;
     char **argv;
     int async;			/* is this asynchronous? */
     unsigned int future;	/* token holding completion status */
     short relative;
     float pconnect;
     struct extended_mask_type *src_region;
     int nsrc_regions;
     struct extended_mask_type *dst_region;
     int ndst_regions;
     char *src_path;
     char *full_dst_path;
     char *typename;
{
  int dst_node;
  char dst_path[MAX_PATH_LEN];
  Element *source;
  ElementList * src_list,*templist;
  Slot sarray[NSLOTS];
  Postmaster * pm;
  int nslot, node_cnt, nonlocal;
  char * slotname[NSLOTS];
  char slottype[NSLOTS];
  char string_val[2048];
  IndexType * field_map;
  double * field_val;
  int * dst_nodes;
  int string_cnt = 0;
  int string_len = 0;
  Implementation * ipm;
  int i;
  short volumeq;
  char res[MAX_RESULT_LEN];
  char *saved_destination;
  float isConnected;
  
  /* find the postmaster */
  if ((pm = GetPostmaster()) == NULL)
    return("");

  ipm = pm->ipm;
  assert(ipm != NULL);

  /* check the dst path syntactically ok */
  if (!GetRemoteNodes(pm, full_dst_path, dst_path, &node_cnt,
		      &dst_nodes, &nonlocal))
    {
      printf("Parse error: GetRemoteNodes failed\n");
      CompleteFuture(pm, future);
      return("");
    }

  if (node_cnt == 0)
    {
      printf("Parse error: no valid nodes specified\n");
      CompleteFuture(pm, future);
      return("");
    }

  if (nonlocal)
    {
      ParError(MSG_INVALID_DST_ZONE, -1);
      CompleteFuture(pm, future);
      return("");
    }

  if (node_cnt > 1 && future != 0)
    AdjustFutureExpected(pm, future, node_cnt);

/* Flag specifying whether or not
   message contains volumetric data */
  volumeq = (short) ((nsrc_regions > 0) && (ndst_regions > 0)); 

  for (i = 0; i < node_cnt; i++)
    {
      dst_node = GlobalNodeToZoneNode(pm, dst_nodes[i]);
      
      /* if both are on this node, do the local addmsg */
      if (dst_node == pm->ipm->myznode)
	{
	  /* set dst element to the local path (i.e., without zone) */
	  /* argv[2] must be restored
	     so that when serial genesis frees up the argv structure,
	     everything is OK */
	  saved_destination = argv[2];
	  argv[2] = dst_path;
	  if (async)
	    {
	      PackRemoteCommand(pm, argc, argv, future, COMPLETION_RESULT);
	      SendLocalSetupMessage(pm, dst_node);
	    }
	  else
	    {
	      /* add the msg and note it in the future */
	      if (volumeq)
		VolumeConnect(src_path, src_region, nsrc_regions, 
			      dst_path, dst_region, ndst_regions,
			      pconnect, relative);
	      else
		do_add_msg(argc, argv);    /** PP **/
	      if (future != 0)
		NoteFuture(pm, future);
	    }
	  argv[2] = saved_destination;
	  continue;
	}
    
      /* check the src wildcard spec */
      if (!GetAddMsgEltList(src_path, &src_list))
	{
	  CompleteFuture(pm, future);
	  return("");
	}

      if (volumeq)
	{
	  int i,j;

	  /* count how many source elements are valid */
	  j = src_list->nelements;
	  for ( i = 0; i<src_list->nelements; i++)
	    if (!CheckClass(src_list->element[i],ClassID("spiking")) ||
		!IsElementWithinVolumeRegions(src_list->element[i],
					      src_region,
					      nsrc_regions,
					      0, 0.0, 0.0, 0.0))
		{
		  /* skip this element */
		  src_list->element[i] = NULL;
		  --j;
		}
	
	  /* make a new list for just the valid ones */
	  templist = CreateElementList(j);
	  templist->nelements = j;

	  /* copy the valid elements over */
	  j = 0;
	  for (i = 0; i < src_list->nelements; ++i)
	    if (src_list->element[i] != NULL)
	      templist->element[j++] = src_list->element[i];

	  /* replace the list */
	  FreeElementList(src_list);
	  src_list = templist;
	
	  /* printf("Source list filtered; now includes %d elements.\n", src_list->nelements); */
	  if (src_list->nelements <= 0)
	    {
	      CompleteFuture(pm, future);
	      return("");
	    }
	}
	    
	  
      /* Fill in func pointers in slots and collect slot names*/
      if (!FillSlotArray(src_list, &nslot, sarray, slotname))
	{
	  FreeElementList(src_list);
	  CompleteFuture(pm, future);
	  return("");
	}
    
      /* Get the internal slot types and strings */
      if (!FillTypeArray(nslot, sarray, slottype,
			 string_val, &string_cnt, &string_len))
	{
	  FreeElementList(src_list);
	  CompleteFuture(pm, future);
	  return("");
	}

      if (nslot)
	{
	  /* Get the field map indices and values */
	  field_map = (IndexType *)
	    calloc (nslot * src_list->nelements, sizeof(IndexType));
	  field_val = (double *)
	    calloc (nslot * src_list->nelements,  sizeof(double));
	  if (!FillMapIndexArray(pm, field_map, field_val, src_list, nslot,
				 sarray, slotname, slottype, dst_node))
	    {
	      FreeElementList(src_list);
	      NoteFuture(pm, future);
	      free(field_map);
	      free(field_val);
	      return("");
	    }
	}


      /* Determine whether a connection is made on the src_node rather */
      /* than the dest_node */
      if (pconnect >= 1.0 || urandom() <= pconnect)
        isConnected = 2.0;
      else
        isConnected = 0.0;

      /* add the message spec to those being buffered */
      /* this call empties the buffer if it is too full */
      if (!BufferAddMsg(pm, src_list, dst_path, dst_node, typename, nslot,
			sarray, slotname, slottype,
			string_val, string_cnt, string_len,
			src_path, field_map, field_val, future,
			volumeq, relative, isConnected, src_region,
			nsrc_regions, dst_region, ndst_regions))
	{
	  FreeElementList(src_list);
	  CompleteFuture(pm, future);
	  free(field_map);
	  free(field_val);
	  return("");
	}                     
      
      /* if synchronous, make sure the request is sent */
      if (!async && ipm->buffered_msg_cnt[dst_node] > 0)
	SendBufferedMsgs(pm, ipm, dst_node);

      /* TODO: we redo all of the above for each dst_node!  But the */
      /* only thing which is diferent for dst_nodes is a small amount */
      /* of code in FillMapIndexArray.  So, move the foreach dst loop. */
    }

  if (async)
    {
      /* return the index for the FutureItem */
      assert(sprintf(res, "%d", future) >= 0);
      return(CopyString(res));
    }
  else
    {
      /* synchronous */
      /* wait for the responses and return the future's value */
      return(WaitFutureResolved(pm, future));
    }
}

/* When a message is added, we save pointers to the fields and the
type of the fields (used to index an array of access functions).  We
add to the list of fields needed by the dst_node pointers to the field
pointers and increment the count of pointers to the field pointers.
*/

int FillMapIndexArray(pm, field_map, field_val, src_list, nslot, psarray, slotname, slottype,
		      dst_node)
     Postmaster * pm;
     IndexType * field_map;
     double * field_val;
     ElementList * src_list;
     int nslot;
     Slot * psarray;
     char ** slotname;
     char * slottype;
     int dst_node;
{
  int i, j, k, res, newidx;
  IndexType info_idx, buf_idx;
  Implementation * ipm = pm->ipm;
  Slot sarray[NSLOTS];

  for (i = 0; i < src_list->nelements; i++){
    Element * src_element = src_list->element[i];
    for (j = 0; j < nslot; ++j)
      sarray[j] = psarray[j];
    if (!FillFieldAddrs(nslot, sarray, src_element, slotname))
      return 0;
    for (j = 0; j < nslot; j++)
      switch(slottype[j]) {
      case LITERAL_SLOT:		/*  literal number */
	/* copy the actual value now */
	/* note later updates will not propogate until RESET */
	field_val[i*nslot+j] = *((double *)(sarray[j].data));
	if (i == src_list->nelements - 1)
	  free(sarray[j].data);	/* not needed any longer */
	field_map[i*nslot+j] = 0; /* no field pending flags agree */
	break;
      case STRING:		/* literal string */
	field_map[i*nslot+j] = 0; /* no field pendingflags agree */
	break;
      case VARIABLE_SLOT:		/* variable */
	/* copy the actual value now */
	/* note later updates will not propogate until RESET */
	field_val[i*nslot+j] = *(sarray[j].data);
	/* get the index for the field in field_info */
	/* check we can store the pointer to the field in the buffer key */
	assert (sizeof(char *) <= sizeof(KeyType));
	info_idx = FindBufferIndex(ipm->field_info,
				   (KeyType) (sarray[j].data));
	if (info_idx > 0) {	/* some dst agreed already */
	  /* is it agreed with our dst? */
	  buf_idx = FindBufferIndex(ipm->in_data[dst_node], (KeyType) info_idx);
	  if (buf_idx > 0) {	/* established with dst */
	    field_map[i*nslot+j] = CODE_AGREED_INDEX(buf_idx); /* just use it */
	    continue;
	  } /* else {
	       agreed but not with this dst.  For efficiency we should save
	       info_idx if it is > 0 and use it when the message completes.
	       But we don't. } */
	}
	/* not agreed with this dst */
	/* check we can store the pointer to the field in the buffer key */
	assert(sizeof(KeyType) >= sizeof(char *));
	buf_idx = GetRefdBufferIndex(&(ipm->pending_field[dst_node]),
			     (KeyType) (sarray[j].data), IN_PENDING_BLKS,
				     IN_PENDING_BLK_SIZ, sizeof(FieldInfoB),
				   &newidx);
	if (newidx) {		/* save the type */
	  *(PTYPE_ADDR(ipm->pending_field[dst_node],buf_idx)) =
	    TypeFromFunc(pm, sarray[j].func);
	  field_map[i*nslot+j] = CODE_NEW_PENDING_INDEX(buf_idx);
	} else {
	  field_map[i*nslot+j] = CODE_OLD_PENDING_INDEX(buf_idx);
	}
      }	/* switch */
  }
  return 1;
}

int FillFieldAddrs(reqslots, sarray, src_element, slotname)
     int reqslots;
     Slot * sarray;
     Element * src_element;
     char ** slotname;
{
  int i;
  short slot_type;
  for(i=0;i<reqslots;i++){
    if(sarray[i].func == NULL){
      /* variable, get the func and address */
      int	prot;
      
      prot = GetFieldListProt(src_element->object, slotname[i]);
      if (prot == FIELD_HIDDEN)
	{
	  Error();
	  printf("raddmsg: could not find field '%s' on '%s'\n",
		 slotname[i], Pathname(src_element));
	  return 0;
	}
      
      if ((sarray[i].data = GetFieldAdr(src_element,slotname[i],&slot_type))
	  == NULL)
	{
	  if ((sarray[i].data =
	       (char *) GetExtFieldAdr(src_element, slotname[i],
				       &sarray[i].func)) == NULL)
	    {
	      Error();
	      printf("raddmsg: could not find field '%s' on '%s'\n",
		     slotname[i],
		     Pathname(src_element));
	      return 0;
	    }
	}
      else
	AssignSlotFunc(sarray+i, (int) slot_type);
    }
  }
  return 1;
}

int
BufferAddMsg (pm, src_list, dst_path, dst_node,
	      typename, nslot, sarray, slotname, slottype,
	      string_val, string_cnt, string_len,  
	      src_path, field_map, field_val, future,
	      volumeq, relative, pconnect,
	      src_region, nsrc_regions, dst_region, ndst_regions)
     Postmaster * pm;
     ElementList * src_list;
     char * dst_path;
     int dst_node;
     char * typename;
     int nslot;
     Slot * sarray;
     char ** slotname;
     char * slottype;
     char * string_val;
     int string_cnt;
     int string_len;
     char * src_path;
     IndexType * field_map;
     double * field_val;
     unsigned int future;
     short volumeq;
     short relative;
     float pconnect;
     struct extended_mask_type *src_region;
     int nsrc_regions;
     struct extended_mask_type *dst_region;
     int ndst_regions;
{
  int i, max_buffered_messages = MAX_BUFFERED_MESSAGES;
  Implementation * ipm = pm->ipm;
  
  /* create a message spec record and copy the details */
  MsgSpec * msg = (MsgSpec *) calloc(1, sizeof(MsgSpec)); 
  if (msg == NULL) {ParError(OUT_OF_MEMORY, -1); return 0; }    
  msg->type = PAR_ADD_MSG_REQ;
  msg->src_path = ReferString(src_path);                        
  msg->src_list = src_list;                                     
  msg->dst_path = ReferString(dst_path);                        
  msg->dst_node = dst_node;                                     
  msg->typename = ReferString(typename);                        
  msg->nslot = nslot;                                           
  if (nslot) 
    {
      msg->sarray = (Slot *) malloc(nslot*sizeof(Slot));            
      bcopy(sarray, msg->sarray, nslot*sizeof(Slot));               
      msg->slottype = (char *) malloc(nslot*sizeof(char));
      bcopy(slottype, msg->slottype, nslot*sizeof(char));           
      msg->slotname = (char **) malloc(nslot*sizeof(char*));        
      for (i = 0; i < nslot; i++)                                   
	msg->slotname[i] = ReferString(slotname[i]);               
    }
  if (string_len)
    {
      msg->string_val = (char *) malloc(string_len*sizeof(char));
      bcopy(string_val, msg->string_val, string_len*sizeof(char));           
    }
  msg->string_cnt = string_cnt;
  msg->string_len = string_len;
  msg->field_map = field_map;	/* NULL if no slots */
  msg->field_val = field_val;	/* NULL if no slots */
  msg->future = future;                                       
  msg->volumeq = volumeq; /* Flag specifying whether or not
			     message contains volumetric data */
  if (volumeq)
    {
      msg->src_region = (struct extended_mask_type *)
	malloc(nsrc_regions*sizeof(struct extended_mask_type));
      memcpy(msg->src_region, src_region,
	     nsrc_regions*sizeof(struct extended_mask_type));
      msg->nsrc_regions = nsrc_regions; 

      msg->dst_region = (struct extended_mask_type *)
	malloc(ndst_regions*sizeof(struct extended_mask_type));
      memcpy(msg->dst_region, dst_region,
	     ndst_regions*sizeof(struct extended_mask_type));
      msg->ndst_regions = ndst_regions; 
      msg->relative = relative;
      msg->pconnect = pconnect;
    }

  /* store the src elements ids */
  msg->srcid = (IndexType *) malloc(msg->src_list->nelements*sizeof(IndexType));
  for (i = 0; i < msg->src_list->nelements; i++) {
    int newidx;
    /* check we can store the pointer to the element in the buffer key */
    assert(sizeof(KeyType) >= sizeof(Element *));
    msg->srcid[i] =
      GetRefdBufferIndex(&(ipm->src), (KeyType) (msg->src_list->element[i]),
			 SRC_BLKS, SRC_BLK_SIZ, sizeof(RefType), &newidx);
  }

  /* add it to the list of buffered messages for the destination node */
  msg->next = ipm->buffered_msgs[dst_node];
  ipm->buffered_msgs[dst_node] = msg;
  
  /* send buffer if it is full enough */
  if (ipm->buffered_msg_cnt[dst_node]++ >= max_buffered_messages)
    SendBufferedMsgs(pm, ipm, dst_node);
  
  return 1;
}

SendBufferedMsgs(pm, ipm, dst_node)
     Postmaster * pm;
     Implementation * ipm;
     int dst_node;
{
  /* Our strategy is as follows: send a message to the remote pm */
  /* specifying the destination path and the number of source elements */
  /* This is sufficient for the remote pm to set up the remote portion */
  /* of the message.  The remote pm replies with number of slots needed */
  /* for the message.  Then we call AddMsg to make the local portion */
  /* of the message.  Then we do whatever the pm needs to do */
  
  MsgSpec * msg;
  int held_idx, held_cnt;

  /* save the number of messages currently buffered */
  held_cnt = ipm->buffered_msg_cnt[dst_node];
  /* save the pointer to the start of the list */
  held_idx = HoldPointer(ipm, (void *) ipm->buffered_msgs[dst_node]);
  /* reset buffer pointer so new msgs can be buffered */
  ipm->buffered_msgs[dst_node] = NULL;
  ipm->buffered_msg_cnt[dst_node] = 0;
  
  /* set up a PVM message buffer */
  InitPvmMessage(pm);
  /* the message tag */
  PackCore(pm,PAR_ADD_MSG_REQ);
  /* the sending zone node number */
  if (pvm_pkint(&(pm->mynode), 1, 1) < 0)
    PvmError();
  /* the number of messages in the list */
  if (pvm_pkint(&held_cnt, 1, 1) < 0)
    PvmError();
  /* the index where the pointer to the list is held - this will be returned */
  if (pvm_pkint(&held_idx, 1, 1) < 0)
    PvmError();
  /* pack the messages into the buffer */
  for (msg = ipm->held_pointers[held_idx]; msg; msg = msg->next)
    {
      /* destination path, number source elements, message type, number slots*/
      /* and the internal slot types */
      /* the future to return */
      if (pvm_pkuint(&msg->future, 1, 1) < 0)
	PvmError();
      if (pvm_pkstr(msg->dst_path) < 0)
	PvmError();
      if (pvm_pkint(&(msg->src_list->nelements), 1, 1) < 0)
	PvmError();
      /* send the src element unique ids keyed on address */
      if (pvm_pkuint(msg->srcid, msg->src_list->nelements, 1) < 0)
	PvmError();

      if (pvm_pkstr(msg->typename) < 0)
	PvmError();
      if (pvm_pkint(&(msg->nslot), 1, 1) < 0)
	PvmError();
      if (msg->nslot)
	if (pvm_pkbyte(msg->slottype, msg->nslot, 1) < 0)
	  PvmError();
      if (pvm_pkint(&(msg->string_cnt), 1, 1) < 0) /* number of string slots */
	PvmError();
      if (msg->string_cnt > 0) {
	if (pvm_pkint(&(msg->string_len), 1, 1) < 0) /* length of strings */
	  PvmError();
	if (pvm_pkbyte(msg->string_val, msg->string_len, 1) < 0) /* strings */
	  PvmError();
      }
      if (msg->nslot)
	{
	  /* check that we are packing the right number of bytes */
	  assert(sizeof(int) == sizeof(IndexType));
	  if (pvm_pkuint(msg->field_map,
			 msg->nslot*msg->src_list->nelements, 1) < 0)
	    PvmError();
	  if (pvm_pkdouble(msg->field_val,
			   msg->nslot*msg->src_list->nelements, 1) < 0)
	    PvmError();
	}
      if (pvm_pkshort(&(msg->volumeq), 1, 1) < 0)
	PvmError();
      if (msg->volumeq)
	{
	  PackMaskData(msg->ndst_regions, msg->dst_region);
	  
	  if (pvm_pkshort(&(msg->relative), 1, 1) < 0)
	    PvmError();
	  if (pvm_pkfloat(&(msg->pconnect), 1, 1) < 0)
	    PvmError();

	  if (msg->relative)
	    {
	      int i;
	      if (pvm_pkint(&(msg->src_list->nelements), 1, 1) < 0)
		PvmError();
	      for (i=0; i<msg->src_list->nelements; i++)
		if (pvm_pkfloat(&(msg->src_list->element[i]->x),1,1) < 0 ||
		    pvm_pkfloat(&(msg->src_list->element[i]->y),1,1) < 0 ||
		    pvm_pkfloat(&(msg->src_list->element[i]->z),1,1) < 0)
		  PvmError();
	    }
	}
	/** <done> **/
    }

  /* send to the destination pm */
  if (!SendLocalSetupMessage(pm, dst_node))
    PvmError();
  /* remote node checks nslots correct, sets up remote portion of the
     message.  Returns ACK if ok */
}

void PackMaskData (ndst_regions, dst_region)
     int ndst_regions;
     struct extended_mask_type *dst_region;
{
  int i;

  if (pvm_pkint(&ndst_regions, 1, 1) < 0)
    PvmError();
	  
  for (i=0; i < ndst_regions; i++)
    {
      if (pvm_pkshort(&(dst_region[i].type),1,1) < 0)
	PvmError();
      if (pvm_pkshort(&(dst_region[i].shape),1,1) < 0)
	PvmError();
      if (pvm_pkdouble(&(dst_region[i].x1),1,1) < 0)
	PvmError();
      if (pvm_pkdouble(&(dst_region[i].y1),1,1) < 0)
	PvmError();
      if (pvm_pkdouble(&(dst_region[i].z1),1,1) < 0)
	PvmError();
      if (pvm_pkdouble(&(dst_region[i].x2),1,1) < 0)
	PvmError();
      if (pvm_pkdouble(&(dst_region[i].y2),1,1) < 0)
	PvmError();
      if (pvm_pkdouble(&(dst_region[i].z2),1,1) < 0)
	PvmError();
    }
}

/* make sure all messages have been completed */
int FlushBufferedMessageRequests(pm)
     Postmaster * pm;
{
  int i, notified = 0;
  Implementation * ipm = pm->ipm;
  
  /* send of buffered requests */
  for (i = 0; i < ipm->nnodes; i++) 
    if (ipm->buffered_msg_cnt[i] > 0)
      SendBufferedMsgs(pm, ipm, i);

  /* wait for completion, with timeout */
  CLR_SETUP_COND_TIMEOUT(pm, pm->msg_hang_time, pm->pvm_hang_time, 
			 (ipm->held_cnt > 0),
			 "Waiting for buffered addmsg commands to complete");

  /* check ok */
  for (i = 0; i < ipm->nnodes; i++)
    if (ipm->pending_field[i] != NULL)
      { assert(BufferEmpty(ipm->pending_field[i])); }
}

/**********************************************************************
 *		PHASE 2:  COMPLETE CHECKING AT DESTINATION
 **********************************************************************/

void
HandleRemoteMessageRequest(pm, src_tid)
     Postmaster * pm;
     int src_tid;
{
  int src_node, msg_cnt, ret_index;
  int j;

  /* Initialize the response buffer */
  InitPvmMessage(pm);
  /* the message tag */
  PackCore(pm,PAR_ADD_MSG_REPLY);
  /* Now start unpacking the request */
  /* the sending zone node number */
  if (pvm_upkint(&src_node, 1, 1) < 0)
    PvmError();
  assert(PvmId(pm, ZoneNodeToGlobalNode(pm, src_node)) == src_tid);
  /* the number of messages in the list */
  if (pvm_upkint(&msg_cnt, 1, 1) < 0)
    PvmError();
  /* the index where the pointer to the list is held - this will be returned */
  if (pvm_upkint(&ret_index, 1, 1) < 0)
    PvmError();
  /* send it back */
  if (pvm_pkint(&ret_index, 1, 1) < 0)
    PvmError();
  /* unpack the messages from the buffer and set them up*/
  for (j = 0; j < msg_cnt; j++)
    HandleSingleRemoteMessageRequest(pm, src_node);

  /* send the response */
  if (!SendLocalSetupMessageByTid(pm, src_tid))
    PvmError();
}

HandleSingleRemoteMessageRequest (pm, src_node)
     Postmaster *pm;
     int src_node;
{
  Implementation * ipm = pm->ipm;
  int nelements, nslot, dslots, type, i, j, k;
  char dst_path[MAX_PATH_LEN];
  char typename[MAX_TYPE_LEN];
  char slottype[NSLOTS];
  char * string_val;		/* this is malloced, never freed */
  int string_cnt;
  int string_len;
  ElementList * dst_list;
  Element * dst_element;
  Slot sarray[NSLOTS];
  IndexType * field_map = NULL;
  double * field_val = NULL;
  int fail, l, ptype, varslots;
  unsigned int * srcid;
  unsigned int future;
  short volumeq,relative;
  int ndst_regions;
  struct extended_mask_type *dst_region;
  float pconnect;
  ElementList *templist;
  unsigned char *conn_matrix;
  int any_src_used;
  unsigned char *src_used;
  int newidx;
  BackPtr bp;
  int blk, ind;
  int bp_idx;
  struct Loc {
    float x, y, z;
  } *src_locs;
  IndexType idx;

  /* for each buffered message spec, try to AddMsg for each src element */
  /* unpack the future */
  if (pvm_upkuint(&future, 1, 1) < 0)
    PvmError();
  /* send it right back */
  if (pvm_pkuint(&future, 1, 1) < 0)
    PvmError();
  /* destination path */
  if (pvm_upkstr(dst_path) < 0)
    PvmError();
  /* number source elements */
  if (pvm_upkint(&nelements, 1, 1) < 0)
    PvmError();
  srcid = (unsigned int *) malloc(nelements*sizeof(unsigned int));
  /* TODO make sure we free this */
  /* source elements ids */
  if (pvm_upkuint(srcid, nelements, 1) < 0)
    PvmError();
  /* message type name*/
  if (pvm_upkstr(typename) < 0)
	PvmError();
  /* number slots */
  if (pvm_upkint(&nslot, 1, 1) < 0)
    PvmError();
  /* internal slot types */
  if (nslot)
    if (pvm_upkbyte(slottype, nslot, 1) < 0)
      PvmError();
  /* slots which are strings */
  if (pvm_upkint(&string_cnt, 1, 1) < 0) /* number of string slots */
    PvmError();
  if (string_cnt > 0) {
    if (pvm_upkint(&string_len, 1, 1) < 0) /* length of strings */
      PvmError();
    string_val = (char *) calloc(string_len, sizeof(char));
    if (pvm_upkbyte(string_val, string_len, 1) < 0) /* strings */
      PvmError();
  }
  if (nslot)
    {	
      /* field map */
      if (field_map == NULL)
	field_map = (IndexType *) calloc(nelements*nslot, sizeof(IndexType));
      else
	field_map = (IndexType *) realloc(field_map, nelements*nslot*sizeof(IndexType));
      /* field val */
      if (field_val == NULL)
	field_val = (double *) malloc(nelements*nslot*sizeof(double));
      else
	field_val = (double *) realloc(field_map, nelements*nslot*sizeof(double));
      /* FIX to deallocate the field_map and field val */
      assert(sizeof(int) == sizeof(IndexType));
      if (pvm_upkuint(field_map, nslot*nelements, 1) < 0)
	PvmError();
      if (pvm_upkdouble(field_val, nslot*nelements, 1) < 0)
	PvmError();
    }

  /* check the dst wildcard spec */
  if (!GetAddMsgEltList(dst_path, &dst_list))
    /* return fact that no valid destinations, skip the rest */
    {
      if (pvm_pkint(&minus1, 1, 1) < 0)
	PvmError();
      free(srcid);
      return;
    }

  if (pvm_upkshort(&(volumeq), 1, 1) < 0)
    PvmError();
  if (volumeq)
    {
      UnPackMaskData(&ndst_regions, &dst_region);
	
      if (pvm_upkshort(&relative, 1, 1) < 0 ||
	  pvm_upkfloat(&pconnect, 1, 1) < 0)
	PvmError();
	
      /* unpack the source locations if relative */
      src_locs = (struct Loc *) calloc(nelements, sizeof(struct Loc));
      if (relative)
	{
	  int i;

	  if (pvm_upkint(&i, 1, 1) < 0)
	    PvmError();
	  assert(i == nelements);
	  for (i = 0; i < nelements; i++)
	    if (pvm_upkfloat(&(src_locs[i].x),1,1) < 0 ||
		pvm_upkfloat(&(src_locs[i].y),1,1) < 0 ||
		pvm_upkfloat(&(src_locs[i].z),1,1) < 0)
	      PvmError();
	}

      /* make sure all destinations are of type synchannel */
      for (i = 0; i < dst_list->nelements; i++)
	if (!CheckClass(dst_list->element[i], ClassID("synchannel")))
	  {
	    Error();
	    printf("Destination element is not of class synchannel.\n");
	    if (pvm_pkint(&minus1, 1, 1) < 0)
	      PvmError();
	    FreeElementList(dst_list);
	    free(dst_region);
	    free(srcid);
	    free(src_locs);
	    return;
	  }

      conn_matrix = (unsigned char *) malloc(nelements * dst_list->nelements);
      memset(conn_matrix, 0, nelements * dst_list->nelements);
      any_src_used = FALSE;
      src_used = (unsigned char *) malloc(nelements);
      memset(src_used, FALSE, nelements);
      for (i = 0; i < nelements; ++i)
	for (j = 0; j < dst_list->nelements; ++j)
	  if (IsElementWithinVolumeRegions(dst_list->element[j],
					   dst_region,
					   ndst_regions,
					   relative, 
					   src_locs[i].x,
					   src_locs[i].y,
					   src_locs[i].z) &&
	      (pconnect >= 1.0))
	    {
	      conn_matrix[i*dst_list->nelements + j] = 1;
	      any_src_used = TRUE;
	      src_used[i] = TRUE;
	    }
      free(dst_region);
      free(src_locs);

      if (!any_src_used)
	{
	  if (pvm_pkint(&zero, 1, 1) < 0)
	    PvmError();
	  FreeElementList(dst_list);
	  free(srcid);
	  return;
	}
    }

  /* the number of destinations */
  if (pvm_pkint(&(dst_list->nelements), 1, 1) < 0)
    PvmError();

  fail = FALSE;
  type = 0;
  /* for each destination element */
  for (k=0; k<dst_list->nelements; k++, ptype = type)
    {
      dst_element = dst_list->element[k];
      
      /* check the msg type is ok */
      if (!CheckMsgType(dst_element, typename, &type, &dslots))
	/* return error code, skip */
	{
	  fail = TRUE;
	  if (pvm_pkint(&BadType, 1, 1) < 0)
	    PvmError();
	  if (pvm_pkstr(dst_element->name) < 0)
	    PvmError();
	  continue;
	}
		
      if (k > 0 && ((type >= 0 && ptype < 0) ||
		    (type < 0 && ptype >= 0)))
	{
	  fail = TRUE;		/* some active, some passive */
	  if (pvm_pkint(&MixedType, 1, 1) < 0)
	    PvmError();
	  if (pvm_pkstr(dst_element->name) < 0)
	    PvmError();
	  continue;
	}

      if (dslots != nslot)
	{
	  /* return error code, skip */
	  fail = TRUE;
	  if (pvm_pkint(&BadFieldCount, 1, 1) < 0)
	    PvmError();
	  if (pvm_pkstr(dst_element->name) < 0)
	    PvmError();
	  continue;
	}
    }				/* for each dst */

  if (fail)
    {
      /* pack back the error code */
      pvm_pkint(&ParRequestError, 1, 1);
      FreeElementList(dst_list);
      free(srcid);
      return;
    }

  /* check that not all slots are literals */
  for (i = varslots = 0; i < nslot; i++)
    if (slottype[i] == VARIABLE_SLOT)
      {
	varslots = 1;
	break;
      }

  /* do the AddMsgs, etc */
  if (!(ipm->exist_msg[src_node] & (IN_PASSIVE | IN_ACTIVE)))
    if (type < 0 || varslots)	/* if active or there are data slots */
      ipm->num_out_nodes++;
  if (type >= 0)		/* FIX: >=0 means passive */
    {
      if (varslots)
	{
	  /* note we get passives from this node */
	  ipm->exist_msg[src_node] |= IN_PASSIVE;
	  pvm_pkint(&ParRequestPassive, 1, 1);
	}
      else
	/* only literal-valued slots */
	  pvm_pkint(&ParRequestLiteral, 1, 1);
    }
  else
    {
      /* note we get actives from this node */
      ipm->exist_msg[src_node] |= IN_ACTIVE;
      pvm_pkint(&ParRequestActive, 1, 1);
      /* can't handle active messages with slots yet */
      assert(nslot == 0);
    }
  
  /* for each source element */
  for (l = 0; l < nelements; l++)
    {
      int str_ind = 0;
      if (volumeq && !src_used[l])
	{
	  unsigned short act_idx = 0xffff;
	  pvm_pkushort(&act_idx, 1, 1);
	  continue;
	}

      for (i = 0; i < nslot; i++)
	switch(slottype[i]) {
	case VARIABLE_SLOT:
	  idx = AssignOutDataIndex(pm, src_node,
					     field_map[l*nslot+i]);
#ifndef NDEBUG
	  field_map[l*nslot+i] = idx;
#endif
	  sarray[i].data = AssignOutDataField(pm, src_node, idx);
	  /* copy the actual value now */
	  *(sarray[i].data) = field_val[l*nslot+i];
	  AssignSlotFunc(sarray+i, DOUBLE);
	  break;
	case LITERAL_SLOT:		/* literal number */
	  sarray[i].data = (char *) malloc(sizeof(double));
	  *((double *)(sarray[i].data)) = field_val[l*nslot+i];
	  AssignSlotFunc(sarray+i,DOUBLE);
	  break;
	case STRING_SLOT:
	  sarray[i].data = string_val+str_ind;
	  str_ind += strlen(sarray[i].data) + 1;
	  AssignSlotFunc(sarray+i, STRING);
#ifndef NDEBUG
	  field_map[l*nslot+i] = 0;
#endif
	  break;
	default:
	  ParError(UNKNOWN_SLOT_TYPE, -1);
	  sarray[i].data = NULL; /* cause a segmentation violation */
	  AssignSlotFunc(sarray+i, (int) slottype[i]);
	  break;
	}
      if (type < 0)
	{
	  /* if active, pack back out_active index */
	  /* find the index into out_active[src_node] to use */
	  BufferManager ** pbfm = &(pm->ipm->out_active[src_node]);
	  int newidx;		/* don't care really */
	  IndexType idx;
	  unsigned short act_idx;
	  idx = GetBufferIndex(pbfm, (KeyType) srcid[l],OUT_ACTIVE_BLKS,
			       OUT_ACTIVE_BLK_SIZ, sizeof(int*), &newidx);
	  /* make the index a short to halve communication */
	  {
	    unsigned short blk = BLOCK_NO(idx), ind = INDEX_NO(idx);
	    assert((blk<<OUT_ACTIVE_BLK_BITS) < (1 << (8*sizeof(short))));
	    act_idx = ((blk<<OUT_ACTIVE_BLK_BITS) | ind);
	  }
	  /* save node and index for when ADDMSGOUT action is handled */
	  ipm->current_active_node = src_node;
	  ipm->current_active_index = idx;
	  /* idx identifies what to do in PROCESS */
	  pvm_pkushort(&act_idx, 1, 1);
	}

      /* for each destination element */
      for (k=0; k<dst_list->nelements; k++)
	if (!volumeq || conn_matrix[l*dst_list->nelements + k])
	  {
	    dst_element = dst_list->element[k];
		
	    /* BIG HOLE HERE */
	    /* have to save info to enable deleting messages!! */

	    ipm->add_msg = 1;	/* allow an AddMsgOut */
	    AddMsg(pm, dst_element, type, nslot, sarray);

	    /* save a BackPtr so that we trace backwards */
	    bp.src_node = src_node;
	    bp.idx = srcid[l];
#if 0
	    /* requires linear search for key */
	    bp_idx = GetBufferIndex(&ipm->back_ptr,
				 (KeyType) (pm->nmsgout),
				 BACK_PTR_BLKS, BACK_PTR_SIZ,
				 sizeof(BackPtr),
				 &newidx);
	    blk = BLOCK_NO(bp_idx);
	    ind = INDEX_NO(bp_idx);
#else
	    /* assumes no message deletion so key is unique */
	    bp_idx = MakeBufferIndex(&ipm->back_ptr,
				 BACK_PTR_BLKS, BACK_PTR_SIZ,
				 sizeof(BackPtr));
	    blk = BLOCK_NO(bp_idx);
	    ind = INDEX_NO(bp_idx);
	    (ipm->back_ptr->key[blk])[ind] = (KeyType) (pm->nmsgout);
#endif
	    ((BackPtr *) ipm->back_ptr->item[blk])[ind] = bp;
	  }			/* for each dst */
    }				/* for each src */

  if (type >= 0 && nslot > 0)
    {
      /* passive, if debug pack back out_data indices */
#ifndef NDEBUG
      assert(sizeof(int) == sizeof(IndexType));
      if (pvm_pkuint(field_map, nslot*nelements, 1) < 0)
	PvmError();
#endif
    }

  if (volumeq)
    {
      free(conn_matrix);
      free(src_used);
    }
}

void UnPackMaskData(p_ndst_regions, p_dst_region) 
     int *p_ndst_regions;
     struct extended_mask_type **p_dst_region;
{ 
 int i;
 struct extended_mask_type *dst_region;
  
 if (pvm_upkint(p_ndst_regions, 1, 1) < 0)
   PvmError();
 
 dst_region = (struct extended_mask_type *)(calloc(*p_ndst_regions, sizeof(struct extended_mask_type)));
 *p_dst_region = dst_region;

 for (i=0; i< *p_ndst_regions; i++)
   if (pvm_upkshort(&(dst_region[i].type),1,1) < 0 ||
       pvm_upkshort(&(dst_region[i].shape),1,1) < 0 ||
       pvm_upkdouble(&(dst_region[i].x1),1,1) < 0 ||
       pvm_upkdouble(&(dst_region[i].y1),1,1) < 0 ||
       pvm_upkdouble(&(dst_region[i].z1),1,1) < 0 ||
       pvm_upkdouble(&(dst_region[i].x2),1,1) < 0 ||
       pvm_upkdouble(&(dst_region[i].y2),1,1) < 0 ||
       pvm_upkdouble(&(dst_region[i].z2),1,1) < 0)
     PvmError();
}

int CheckMsgType(elt, typename, type, reqslots)
     Element * elt;
     char * typename;
     int * type;
     int * reqslots;
{
  MsgList * msgspec;
  if((msgspec = GetMsgListByName(elt->object,typename)) == NULL){
    Error();
    printf("%s is not a valid msg type for '%s'\n",
	   typename,
	   Pathname(elt));
    return 0;
  }
  *type = msgspec->type;
  *reqslots = msgspec->slots;
  return 1;
}

/* The data fields used for sending messages out to Elements are */
/* malloced in fixed-size chunks.  out_data[src_node] is double **, a */
/* vector of pointers to these chunks. out_blk_cnt keeps a count of the */
/* number of chunks that have been allocated.  out_cnt keeps track of the */
/* largest index used. */
/* Map indexes come in three flavors: agreed, new-pending and old-pending */
/* Agreed means the src and dst have already established this index. */
/* new-pending means that this is the first time the src has sent this */
/* field, and it is an index into the src's pending_field array (which has */
/* entries reused.  old-pending means that the src has sent this field */
/* before but it has not yet been agreed */

int AssignOutDataIndex(pm, src_node, map_idx)
     Postmaster * pm;
     int src_node;
     IndexType map_idx;
{
  Implementation * ipm = pm->ipm;
  int blk_no, idx, newidx;
  /* establish the index we want to use */
  if (!(PENDING_INDEX(map_idx))) /* agreed */
    idx = INDEX(map_idx);
  else {
    IndexType mapid = INDEX(map_idx);
    int blk = BLOCK_NO(mapid), ind = INDEX_NO(mapid);
    if (NEW_USE_INDEX(map_idx))
      { /* new-pending */
	/* allocate the next new out_data index, no meaningful key */
	
	idx = MakeBufferIndex(&(ipm->out_data[src_node]),	
			      OUT_FLD_BLKS, OUT_FLD_BLK_SIZ, sizeof(double));
	
	/* save the src's pending index in the dsts pending_index buffer */
	/* on second new use of this pending index (i.e., previous was */
	/* agreed), this will simply overwrite the previous entry, ok. */
	
	CreateBufferIndex(&(ipm->pending_index[src_node]),mapid,
			  OUT_PENDING_BLKS,OUT_PENDING_BLK_SIZ,
			  sizeof(IndexType), 1);

	/* key is the pending_index, set the data to be the idx into */
	/* out_data so that if this pending_index comes in later as old,  */
	/* we can retrieve the idx into out_data as below */

	SetBufferIndexItem(ipm->pending_index[src_node], mapid, &idx);
      }
    else		/* expect in old-pending */
      {	
	/* retrieve the out_data index from the pending_index buffer */
	idx = *((IndexType *) GetBufferIndexItem(ipm->pending_index[src_node],
					       mapid));
	assert(idx != 0);
      }
  }
  return idx;
}

char * AssignOutDataField(pm, src_node, idx)
     Postmaster * pm;
     int src_node;
     IndexType idx;
{
  BufferManager * bfm = pm->ipm->out_data[src_node];
  int blk = BLOCK_NO(idx), ind = INDEX_NO(idx);
  return ((char *) &(((double *) (bfm->item[blk]))[ind]));
}

/**********************************************************************
 *		PHASE 3:  FINAL SETUP AT SOURCE
 **********************************************************************/
CompleteRemoteMessages(pm)
     Postmaster * pm;
{
  /* Response format is: */
  /*   Pack(return_index */
  /*     for each message: */
  /*           Pack(status) */
  
  int held_idx, ndsts, rstatus;
  MsgSpec * msg;
  MsgSpec * prev;
  Implementation * ipm = pm->ipm;
  unsigned int future;

  if (pvm_upkint(&held_idx, 1, 1) < 0)
    PvmError();
  for (msg = (MsgSpec *) ReleasePointer(ipm, held_idx), prev=NULL;
       msg != NULL;
       prev=msg, msg = msg->next){
    free(prev);
    if (pvm_upkuint(&future, 1, 1) < 0)
      PvmError();
    else {
      if (future != 0)
	NoteFuture(pm, future);
    }
    if (pvm_upkint(&ndsts, 1, 1) < 0)
      PvmError();
    if (ndsts <= 0) {
      if (ndsts < 0)
	fprintf(stderr, "raddmsg: %s to %s@%d failed, no destination found\n",
		msg->src_path, msg->dst_path, msg->dst_node);
      ReclaimMsgSpec(msg); 
      continue;
      /* TODO recover fields from pending field map */
    }
    if (pvm_upkint(&rstatus, 1, 1) < 0)
      PvmError();
    if (rstatus == PAR_REQUEST_PASSIVE)
      CompleteRemotePassiveMessage(pm,msg);
    else if (rstatus == PAR_REQUEST_ACTIVE)
      CompleteRemoteActiveMessage(pm, msg);
    else if (rstatus == PAR_REQUEST_LITERAL)
      CompleteRemoteLiteralMessage(pm, msg);
    else 			/* must be an error */
      RecoverRemoteMessageError(pm, msg, rstatus);
    /* reclaim space */
    ReclaimMsgSpec(msg);
  }				/* for each msg */
  free(prev);
  return 1;
}

/*    passive - passive code, then if no debug nothing, */
/*             if debug, agreed fieldmap  */
void CompleteRemotePassiveMessage(pm,msg)
     Postmaster * pm;
     MsgSpec * msg;
{
  Implementation * ipm = pm->ipm;
  int i, j, dst_node = msg->dst_node; 
#ifndef NDEBUG
  int * dstfldmap = NULL;
  if (msg->nslot)
    {
      dstfldmap = (int *)
	calloc(msg->src_list->nelements*msg->nslot, sizeof(int));
      if (pvm_upkint(dstfldmap, msg->src_list->nelements*msg->nslot, 1) < 0)
	PvmError();
    }
#endif
  if (!(ipm->exist_msg[msg->dst_node] & (OUT_PASSIVE | OUT_ACTIVE)))
    ipm->num_in_nodes++;
  /* note we send passives to this node */
  ipm->exist_msg[msg->dst_node] |= OUT_PASSIVE;
  /* for each field_map index that is pending, remove from */
  /* pending_field and add to field_info */
  for (i = 0; i < msg->src_list->nelements; i++) {
    for (j = 0; j < msg->nslot; j++) {
      if (PENDING_INDEX(msg->field_map[i*msg->nslot+j])) {
	int pidx = INDEX(msg->field_map[i*msg->nslot+j]);
	KeyType fadr = ipm->pending_field[dst_node]->
	  key[BLOCK_NO(pidx)][INDEX_NO(pidx)];
	int newidx;
	IndexType idx =
	  GetRefdBufferIndex(&(ipm->field_info), fadr, IN_FLD_BLKS,
			     IN_FLD_BLK_SIZ, sizeof(FieldInfo), &newidx);
	IndexType idi_idx =
	  GetBufferIndex(&(ipm->in_data[dst_node]), (KeyType) idx, 
			 IN_DATA_BLKS, IN_DATA_BLK_SIZ, sizeof(double),
			 &newidx);
	if (newidx) {
	  *(TYPE_ADDR(ipm->field_info, idx)) =
	    *(PTYPE_ADDR(ipm->pending_field[dst_node], pidx));
	}
#ifndef NDEBUG
	assert(idi_idx == dstfldmap[i*msg->nslot+j]);
#endif
	UnreferBufferIndex(ipm->pending_field[dst_node], pidx);
      }			/* if PENDING_INDEX */
    }			/* for nslot */
    NoteIfNewSrc(pm, msg->src_list->element[i]);
    ReferBufferIndex(ipm->src, msg->srcid[i]);
  }				/* for each src element */
#ifndef NDEBUG
  if (msg->nslot)
    free(dstfldmap);
#endif
}

/*    literal - passive code, then if no debug nothing, */
/*             if debug, agreed fieldmap  */
void CompleteRemoteLiteralMessage(pm,msg)
     Postmaster * pm;
     MsgSpec * msg;
{
  Implementation * ipm = pm->ipm;
  int i, j, dst_node = msg->dst_node; 
#ifndef NDEBUG
  int * dstfldmap = NULL;
  if (msg->nslot)
    {
      dstfldmap = (int *)
	malloc(msg->src_list->nelements*msg->nslot*sizeof(int));
      if (pvm_upkint(dstfldmap, msg->src_list->nelements*msg->nslot, 1) < 0)
	PvmError();
    }
#endif
  for (i = 0; i < msg->src_list->nelements; i++) {
    NoteIfNewSrc(pm, msg->src_list->element[i]);
    ReferBufferIndex(ipm->src, msg->srcid[i]);
  }				/* for each src element */
#ifndef NDEBUG
  if (msg->nslot)
    free(dstfldmap);
#endif
}

/* active - active code, then for each src element, one integer index */
void CompleteRemoteActiveMessage(pm, msg)
     Postmaster * pm;
     MsgSpec * msg;
{
  Implementation * ipm = pm->ipm;
  int i;
  Slot slot[1];
  /* TODO: act_idx should have a size to avoid mallocing *every* time */
  unsigned short  * act_idx = (unsigned short *)
    malloc(msg->src_list->nelements*sizeof(unsigned short));
  /* can't handle active messages with slots yet */
  assert(msg->nslot == 0);
  /* get the active-msg indices, one for each src elt */
  if (pvm_upkushort(act_idx, msg->src_list->nelements, 1) < 0)
    PvmError();
  slot[0].func = LiteralMessageData;
  for (i = 0; i < msg->src_list->nelements; i++)
    /* skip over sources which were not used */
    if (act_idx[i] != 0xffff)
      {
	BufferManager ** pbfm = &(ipm->in_active_msg);
	/* establish in index for the active msg */
	int newidx = 0;
	int idx = GetBufferIndex(pbfm, (KeyType) msg->srcid[i],
				 IN_ACTIVE_BLKS, IN_ACTIVE_BLK_SIZ,
				 sizeof(InActType *), &newidx);
	/* ASSUMPTION: number of nodes fits in a short */
	assert(msg->dst_node < (1 << (8*sizeof(unsigned short))));
	assert(sizeof(unsigned int) == 2*sizeof(unsigned short));
	if (!(ipm->exist_msg[msg->dst_node] & (OUT_PASSIVE | OUT_ACTIVE)))
	  ipm->num_in_nodes++;
	/* note that this dst should get active messages */
	ipm->exist_msg[msg->dst_node] |= OUT_ACTIVE;
	if (newidx) {
	  unsigned int dst_idx =
	    (msg->dst_node << (8*sizeof(unsigned short))) | act_idx[i];
	  AddStructToVectorItem(*pbfm, idx, &dst_idx, sizeof(unsigned int));
	  /* store the index in the in_active_msg buffer */
	  slot[0].data = (char *) (idx);
	  AddMsg(msg->src_list->element[i],pm,PAR_ACTIVE_MSG,1,slot);
	} else {
	  unsigned int dst_idx =
	    (msg->dst_node << (8*sizeof(unsigned short))) | act_idx[i];
	  assert(act_idx != 0);

	  /* if non-existant for this destination node, we'll have 
	     to add it here */
	  if (!StructInVectorItem(*pbfm, idx, &dst_idx, sizeof(unsigned int))) 
	    AddStructToVectorItem(*pbfm, idx, &dst_idx, sizeof(unsigned int));
	}
	NoteIfNewSrc(pm, msg->src_list->element[i]);
	ReferBufferIndex(ipm->src, msg->srcid[i]);
      }

  /* TODO: act_idx should have a size, not be free'd */
  free(act_idx);
}

/* if there isn't a NOTIFY_DELETE msg from the src, add one */
void NoteIfNewSrc(pm, src_element)
     Postmaster * pm;
     Element* src_element;
{
  Slot slot;
  if (GetMsgNumByDestSrc(src_element,pm,PAR_NOTIFY_DELETE) < 0) {
    slot.func = LiteralMessageData;
    slot.data = (char *) src_element;
    AddMsg(src_element,pm,PAR_NOTIFY_DELETE,1,&slot);
  }
}

static double LiteralMessageData(slot)
     Slot * slot;
{
  return ((double) ((int) (slot->data)));
}

/*   error - series of errorcode,string pairs, terminated with special code */
void RecoverRemoteMessageError(pm, msg, rstatus)
     Postmaster * pm;
     MsgSpec * msg;
     int rstatus;
{
  Implementation * ipm = pm->ipm;
  int i;
  char dst_name[MAX_PATH_LEN];
  /* get rid of reference to src elts */
  for (i = 0; i < msg->src_list->nelements; i++) {
    UnreferBufferIndex(ipm->src, msg->srcid[i]);
  }
  /* print the error messages */
  while (rstatus != PAR_REQUEST_ERROR) {
    pvm_upkstr(dst_name);
    fprintf(stderr, "raddmsg: %s to %s@%d type %s failed (code %d)\n",
	    msg->src_path, dst_name, msg->dst_node, msg->typename,
	    rstatus);
    if (pvm_upkint(&rstatus, 1, 1) < 0)
      PvmError();
  }
  /* TODO recover fields from field map */
}

/* Returns -1 if not found, otherwise the outgoing message number */
/* The argument 'msgtype' is the integer message type. */

int GetMsgNumByDestSrc(src,dest,mtype)
     Element *src;
     Element *dest;
     int mtype;
{
  MsgOut *msg = src->msgout;
  int foundit = 0;
  int msgcount;

  for(msgcount = 0; msgcount < src->nmsgout; msgcount++) {
    if (msg->dst == dest && mtype == msg->type) {
      foundit = 1;
      break;
    }
    msg = MSGOUTNEXT(msg);
  }
  if (foundit) return(msgcount);
  return(-1);
}

ReclaimMsgSpec(msg)
     MsgSpec * msg;
{
  int i;
  /* free all the data structures associated with the MsgSpec */

  /* Unrefer the strings */
  UnreferString(msg->typename);
  UnreferString(msg->dst_path);
  UnreferString(msg->src_path);
  for (i = 0; i < msg->nslot; i++)
    UnreferString(msg->slotname[i]);

  /* free the stuff we malloced */
  free(msg->srcid);
  if (msg->nslot)
    {
      free(msg->sarray);
      free(msg->slottype);
      free(msg->slotname);
      free(msg->field_map); 
      free(msg->field_val); 
    }
  if (msg->volumeq)
    {
      free(msg->src_region);
      free(msg->dst_region);
    }
  if (msg->string_cnt)
    free (msg->string_val);

  /* free the list of src elements */
  FreeElementList(msg->src_list);
}

/**********************************************************************
 *		UTILITIES
 **********************************************************************/

char * ReferString(str)
     char * str;
{
  return(CopyString(str));
}

void UnreferString(str)
     char * str;
{
  FreeString(str);
}

unsigned char TypeFromFunc(pm, func)
     Postmaster * pm;
     PFD func;
{
  unsigned char i;
  Implementation * ipm = pm->ipm;

  for (i = 0; ipm->access_func[i] != NULL; ++i)
    if (ipm->access_func[i] == func)
      return(i);
  ParError(UNIMPLEMENTED_SLOT_FUNCTION, -1);
  return(0);
}

int GetAddMsgEltList(path, list)
     char * path;
     ElementList ** list;
{
  ElementList * lst;
  lst = WildcardGetElement(path,0);
  if(lst->nelements == 0){
    InvalidPath(optargv[0],path);
    FreeElementList(lst);
    return 0;
  }
  *list = lst;
  return 1;
}

/* should use a buffer manager to hold pointers but this works */
int HoldPointer(ipm, ptr)
     Implementation * ipm;
     void * ptr;
{
  int res;
  if (ipm->held_pointers == NULL){
    ipm->held_size = 2*ipm->nnodes;
    ipm->held_pointers = (void **)
      malloc (ipm->held_size*sizeof(void *));
    ipm->held_used = (char *) calloc(ipm->held_size, sizeof(char));
    ipm->held_cnt = 0;
    res = 0;
  } else {
    for (res= 0; res < ipm->held_size; res++)
      if (ipm->held_used[res] == 0)
	break;
    if (res == ipm->held_size){
      ipm->held_pointers = (void **)
	realloc(ipm->held_pointers, 2*ipm->held_size*sizeof(void*));
      ipm->held_used = (char *)
	realloc(ipm->held_used, 2*ipm->held_size*sizeof(char *));
      memset(ipm->held_used+ipm->held_size, 0, ipm->held_size);
      ipm->held_size *= 2;
    }
  }
  ipm->held_cnt++;
  ipm->held_pointers[res] = ptr;
  ipm->held_used[res] = 1;
  return res;
}

void * ReleasePointer(ipm, idx)
     Implementation * ipm;
     int idx;
{
  void * res;
  if (ipm->held_used[idx] == 0) {
    ParError(POINTER_ALREADY_RELEASED, -1);
    return NULL;
  }
  ipm->held_used[idx] = 0;
  res = ipm->held_pointers[idx];
  ipm->held_cnt--;
#if 0
  /* if not in use, reclaim space */
  if (--ipm->held_cnt == 0){
    free(ipm->held_pointers);
    free(ipm->held_used);
    ipm->held_pointers = NULL;	/* mark as uninitialized */
  }
#endif
  return (res);
}


