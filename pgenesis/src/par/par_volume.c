static char rcsid[] = "$Id: par_volume.c,v 1.2 2005/09/29 23:18:54 ghood Exp $";

/*
 * $Log: par_volume.c,v $
 * Revision 1.2  2005/09/29 23:18:54  ghood
 * updated for PGENESIS 2.3
 *
 * Revision 1.1.1.1  2005/06/14 04:38:39  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.2  1999/12/19 04:14:49  mhucka
 * Added new version of par_volume.c from Greg Hood, dated August 1999.
 * Greg says: "I made some changes in PGENESIS to rvolumedelay and
 * rvolumeweight to take optional destination path parameters (which
 * keeps them in agreement with volumedelay and volumeweight in
 * serial GENESIS)."
 *
 * Revision 1.15  1997/08/11 17:27:43  ngoddard
 * removed duplicat Log messages
 *
 * Revision 1.14  1997/08/11 15:49:10  ngoddard
 * realloc will not accept null pointer; macro RECALLOC does callor or realloc
 *
 * Revision 1.13  1997/07/22 23:03:13  ghood
 * Replaced getopt/optopt with G_getopt/G_optopt
 *
 * Revision 1.12  1997/03/27 06:26:49  ngoddard
 * mods to scale up to larger networks
 *
 * Revision 1.11  1997/02/28 05:24:50  ngoddard
 * support for lookahead optimisation, bug fixes
 *
 * Revision 1.10  1997/02/20 21:13:36  ngoddard
 * support for xpvm; using pvm_trecv for timeout ops; fixed bugs related to futures; provide user warnings when appears stalled; barrer and barrierall take a third argument: how long to wait before stall is assumed; beginnings of performance stats package
 *
 * Revision 1.9  1997/01/31 05:14:24  ngoddard
 * barriers don't complete asyncs now, minor bug fixes
 *
 * Revision 1.7  1997/01/21 20:19:46  ngoddard
 * bug fixes for zones, implement string literal fields, first release of pgenesis script
 *
 * Revision 1.6  1996/08/26 19:54:42  ghood
 * fixed bug that caused weight and delay messages to not be sent to
 * all destination nodes
 *
 * Revision 1.5  1996/08/19 21:34:02  ghood
 * Made pvm calls type-consistent with their parameters.
 *
 * Revision 1.4  1996/08/16 17:22:08  ghood
 * *** empty log message ***
 *
 * Revision 1.3  1996/08/13 21:25:03  ghood
 * General debugging to get parallel example scripts working.
 *
 * Revision 1.2  1996/06/21 16:10:44  ghood
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

/* EXTERNAL DECLARATIONS */
/* It would be better to just include newconn_ext.h, but that includes sim_ext,
   which causes problems because par_ext.h also includes it */
extern float rangauss();
extern void randomize_value();
extern Synapse *GetSynapseAddress();

extern char * MallocedString();
extern char *WaitFutureResolved();


/* FORWARD DECLARATIONS */
char *remote_volume_delay_entry();
char *remote_volume_weight_entry();
char *remote_volume_delay();
char *remote_volume_weight();

/*********************************************************************
 *		PHASE 1:  INITIAL CHECKING AT SOURCE
 *********************************************************************/
char *do_remote_volume_delay (argc, argv)
     int argc;
     char **argv;
{
  Postmaster *pm;
  int future = 0;
  char * res = "";

  DBGMSG("Entering do_remote_volume_delay\n");
  if ((pm = GetPostmaster()) != NULL)
    future = AllocateFutureIndex(pm, COMPLETION_RESULT, 1);
  res = remote_volume_delay_entry(argc, argv, FALSE, future);
  DBGMSG("Exiting do_remote_volume_delay\n");
  return(MallocedString(res));
}

char *do_remote_volume_weight (argc, argv)
     int argc;
     char **argv;
{
  Postmaster *pm;
  int future = 0;
  char * res = "";

  DBGMSG("Entering do_remote_volume_weight\n");
  if ((pm = GetPostmaster()) != NULL)
    future = AllocateFutureIndex(pm, COMPLETION_RESULT, 1);
  res = remote_volume_weight_entry(argc, argv, FALSE, future);
  DBGMSG("Exiting do_remote_volume_weight\n");
  return(MallocedString(res));
}

char * remote_volume_delay_entry (argc, argv, async, future)
     int argc;
     char **argv;
     int async;
     int future;
{
  char *src_name, *dest_name;
  short radial, mode, absrandom, add;
  float scale, randarg1, randarg2;
  Postmaster *pm;

  if (!CheckRemoteVolumeDelaySyntax(argc, argv,
				    &src_name,
				    &dest_name,
				    &radial, &scale,
				    &mode, &absrandom, &randarg1, &randarg2,
				    &add))
    {
      if ((pm = GetPostmaster()) != NULL)
	NoteFuture(pm, future);
      return("");
    }

  return(remote_volume_delay(argc, argv, async, future, src_name, dest_name,
			     radial, scale, mode, absrandom,
			     randarg1, randarg2, add));
}

char * remote_volume_weight_entry (argc, argv, async, future)
     int argc;
     char **argv;
     int async;
     int future;
{
  char *src_name, *dest_name;
  short decay, mode, absrandom;
  float scale, decayarg1, decayarg2, randarg1, randarg2;
  Postmaster *pm;

  if (!CheckRemoteVolumeWeightSyntax(argc, argv,
				    &src_name, &dest_name,
				    &decay, &scale, &decayarg1, &decayarg2,
				    &mode, &absrandom, &randarg1, &randarg2))
    {
      if ((pm = GetPostmaster()) != NULL)
	NoteFuture(pm, future);
      return("");
    }

  return(remote_volume_weight(argc, argv, async, future, src_name, dest_name,
			     decay, scale, decayarg1, decayarg2,
			     mode, absrandom, randarg1, randarg2));
}

int CheckRemoteVolumeDelaySyntax(argc,argv,src_name,dest_name,radial,scale,mode,absrandom,randarg1,randarg2,add)
     int argc;
     char **argv;
     char **src_name;
     char **dest_name;
     short *radial, *mode, *absrandom, *add;
     float *scale, *randarg1, *randarg2;
{
  int     status;
  short   delayoptions, randoptions;  /* counters for options */ 
  
  *mode = *radial = *absrandom = *add = delayoptions = randoptions = 0;
  *scale = *randarg1 = *randarg2 = 0.0;
  
  initopt(argc, argv, "sourcepath [destpath] -fixed delay -radial conduction_velocity -add -uniform scale -gaussian stdev maxdev -exponential mid max -absoluterandom");

  while((status = G_getopt(argc, argv)) == 1){
    /* The following options determine the way delays are calculated: */
    if(strcmp(G_optopt, "-fixed") == 0){
      *radial = 0;
      *scale = Atof(optargv[1]);
      delayoptions++;
    }
    else if(strcmp(G_optopt, "-radial") == 0){
      *radial = 1;
      *scale  = Atof(optargv[1]);
      delayoptions++;
    }
    else if(strcmp(G_optopt, "-add") == 0){
      *add = 1;
    }
    /* The remaining options affect the type of randomness added to the delays: */
    else if(strcmp(G_optopt, "-uniform") == 0){
      *mode       = 1;
      *randarg1   = Atof(optargv[1]); /* range of uniform distribution */
      *randarg2   = 0.0;
      randoptions++;
    }
    else if(strcmp(G_optopt, "-gaussian") == 0){
      *mode     = 2;
      *randarg1 = Atof(optargv[1]); /* standard deviation */
      *randarg2 = Atof(optargv[2]); /* maximum absolute value of random component */
      randoptions++;
    }
    else if(strcmp(G_optopt, "-exponential") == 0){
      *mode     = 3;
      *randarg1 = Atof(optargv[1]); /* 1/e point of exponential distribution */
      *randarg2 = Atof(optargv[2]); /* maximum absolute value of random component */
      randoptions++;
    }
    else if(strcmp(G_optopt, "-absoluterandom") == 0){
      *absrandom = 1;
    }
  }

  if(status < 0){
    printoptusage(argc, argv);
    return(FALSE);
  }
  
  if(delayoptions != 1){
    Error();
    printf("Must have exactly one of -fixed, -radial options.\n");
    return(FALSE);
  }

  if(randoptions > 1){
    Error();
    printf("Must have at most one of -uniform, -gaussian, -exponential options.\n");
    return(FALSE);
  }

  *src_name = optargv[1];  /* path of source elements */

  if (optargc == 3) /* there is a destination argument */
    *dest_name = optargv[2];
  else
    *dest_name = NULL;
  return(TRUE);
}


int CheckRemoteVolumeWeightSyntax(argc,argv,src_name,dest_name,decay,scale,decayarg1,decayarg2,mode,absrandom,randarg1,randarg2)
     int argc;
     char **argv;
     char **src_name, **dest_name;
     short *decay, *mode, *absrandom;
     float *scale, *decayarg1, *decayarg2, *randarg1, *randarg2;
{
  int     status;
  short   weightoptions, randoptions;  /* counters for options */ 

  *mode = *decay = *absrandom = weightoptions = randoptions = 0;
  *scale = *decayarg1 = *decayarg2 = *randarg1 = *randarg2 = 0.0;

  initopt(argc, argv, "sourcepath [destpath] -fixed weight -decay decay_rate max_weight min_weight -uniform scale -gaussian stdev maxdev -exponential mid max -absoluterandom");

  while((status = G_getopt(argc, argv)) == 1){
    /* The following options determine the way weights are calculated: */
    if(strcmp(G_optopt, "-fixed") == 0){
      *decay = 0;
      *scale = Atof(optargv[1]);
      weightoptions++;
    }
    else if(strcmp(G_optopt, "-decay") == 0){
      *decay  = 1;
      *scale  = Atof(optargv[1]);
      *decayarg1 = Atof(optargv[2]);
      *decayarg2 = Atof(optargv[3]);
      weightoptions++;
    }
    /* The remaining options affect the type of randomness added to the delays: */
    else if(strcmp(G_optopt, "-uniform") == 0){
      *mode       = 1;
      *randarg1   = Atof(optargv[1]); /* range of uniform distribution */
      *randarg2   = 0.0;
      randoptions++;
    }
    else if(strcmp(G_optopt, "-gaussian") == 0){
      *mode     = 2;
      *randarg1 = Atof(optargv[1]); /* standard deviation */
      *randarg2 = Atof(optargv[2]); /* maximum absolute value of random component */
      randoptions++;
    }
    else if(strcmp(G_optopt, "-exponential") == 0){
      *mode     = 3;
      *randarg1 = Atof(optargv[1]); /* 1/e point of exponential distribution */
      *randarg2 = Atof(optargv[2]); /* maximum absolute value of random component */
      randoptions++;
    }
    else if(strcmp(G_optopt, "-absoluterandom") == 0){
      *absrandom = 1;
    }
  }
 
  if (status < 0)
    {
      printoptusage(argc, argv);
      return(FALSE);
    }
  
  if (weightoptions != 1)
    {
      Error();
      printf("Must have exactly one of -fixed, -decay options.\n");
      return(FALSE);
    }
 
  if (randoptions > 1)
    {
      Error();
      printf("Must have at most one of -uniform, -gaussian, -exponential options.\n");
      return(FALSE);
    }
 
  *src_name = optargv[1];  /* path of source elements */

  if (optargc == 3) /* there is a destination argument */
    *dest_name = optargv[2];
  else
    *dest_name = NULL;
  return(TRUE);
}     

char *remote_volume_delay(argc,argv,async,future,src_name,dest_name,radial,scale,mode,absrandom,randarg1,randarg2,add)
     int argc;
     char **argv;
     int async;
     unsigned int future;
     char *src_name;
     char *dest_name;
     short radial, mode, absrandom, add;
     float scale, randarg1, randarg2;
{
  Postmaster *pm;
  Implementation *ipm;
  BufferManager *bfm;
  float x, y, z;
  int i, j;
  Element *source;
  ElementList *src_list, *dest_list;
  MsgOut *mo;
  MsgIn *mi;
  int act_idx, blk, ind, cnt;
  unsigned int *pdstidx;
  int dst_node, dst_idx;
  unsigned int dstidx;
  char res[MAX_RESULT_LEN];
  struct Dest {
    int idx;
    float x, y, z;
  } **dests, *p;
  int *sizes;
  int *counts;
  float temp_scale, temp_randarg1, temp_randarg2;

  /* make local copies of the floating point arguments so that
     ALPHA compiler does the right thing */
  temp_scale = scale;
  temp_randarg1 = randarg1;
  temp_randarg2 = randarg2;

  src_list = WildcardGetElement(src_name,1);
  if (src_list->nelements == 0)
    {
      if ((pm = GetPostmaster()) != NULL)
	NoteFuture(pm, future);
      return("");
    }

  if (dest_name != NULL)
    dest_list = WildcardGetElement(dest_name, 1);
  else
    dest_list = NULL;

  /* modify all local synapses */
  for(i=0; i<src_list->nelements; i++)
    {
      source = src_list->element[i];
      x = source->x;
      y = source->y;
      z = source->z;
      MSGOUTLOOP(source, mo) {
	struct Synchan_type* synchan;

	if (!CheckClass(mo->dst, ClassID("synchannel")))
	  continue;
	mi = (MsgIn*) GetMsgInByMsgOut(mo->dst, mo);
	if(mi->type != SPIKE) 
	  continue;

	 /*
	  * Check to see if the synchan is in the destination list if any.
	  */
	if (dest_list != NULL &&
	    !IsElementInList(mo->dst, dest_list))
	  continue;

	SetSingleDelay(mi, mo, x, y, z, radial, scale, mode, absrandom, randarg1, randarg2, add);
      }
    }

  /* find the postmaster */
  if ((pm = GetPostmaster()) == NULL)
    return("");

  ipm = pm->ipm;
  assert(ipm != NULL);
  bfm = ipm->in_active_msg;

  dests = (struct Dest **) calloc(ipm->nnodes, sizeof(struct Dest *));
  sizes = (int *) calloc(ipm->nnodes, sizeof(int));
  counts = (int *) calloc(ipm->nnodes, sizeof(int));

  for (i = 0; i < src_list->nelements; ++i)
    {
      source = src_list->element[i];
      x = source->x;
      y = source->y;
      z = source->z;
      MSGOUTLOOP(source, mo) {
	if (mo->dst != (Element *) pm)
	  continue;
	mi = (MsgIn*) GetMsgInByMsgOut(mo->dst, mo);
	if (mi->type != PAR_ACTIVE_MSG)
	  continue;
	
	act_idx = MSGVALUE(mi, 0);
/*	INT_STRUCT_LOOP_HEAD(bfm, act_idx, dstidx) */
	InitIntStructLoop(bfm, act_idx);
	while (IterIntStructLoop(&dstidx))
	  {
	    dst_node = dstidx >> (8*sizeof(unsigned short));
	    assert(dst_node >= 0);
	    assert(dst_node < ipm->nnodes);
	    dst_idx = dstidx - (dst_node << (8*sizeof(unsigned short)));

	    /* record that we need to send a message to this node */
	    if (counts[dst_node] >= sizes[dst_node])
	      {
		sizes[dst_node] = sizes[dst_node] ? 2*sizes[dst_node] : 64;
                RECALLOC(dests[dst_node], sizes[dst_node], sizeof(struct Dest), struct Dest * );
	      }

	    p = &(dests[dst_node][counts[dst_node]++]);
	    p->idx = dst_idx;
	    p->x = x;
	    p->y = y;
	    p->z = z;
	  }
/*	INT_STRUCT_LOOP_TAIL; */
      }
    }
  FreeElementList(src_list);
  FreeElementList(dest_list);

  for (i = 0; i < ipm->nznodes; ++i)
    if (counts[i] > 0)
      {
	InitPvmMessage(pm);
	PackCore(pm, PAR_VOLUME_DELAY_REQ);
	if (pvm_pkint(&(pm->mynode), 1, 1) < 0 ||
	    pvm_pkint(&i, 1, 1) < 0 ||
	    pvm_pkint(&(counts[i]), 1, 1) < 0 ||
	    pvm_pkuint(&future, 1, 1) < 0 ||
	    pvm_pkshort(&radial, 1, 1) < 0 ||
	    pvm_pkfloat(&temp_scale, 1, 1) < 0 ||
	    pvm_pkshort(&mode, 1, 1) < 0 ||
	    pvm_pkshort(&absrandom, 1, 1) < 0 ||
	    pvm_pkfloat(&temp_randarg1, 1, 1) < 0 ||
	    pvm_pkfloat(&temp_randarg2, 1, 1) < 0 ||
	    pvm_pkshort(&add, 1, 1) < 0 ||
	    pvm_pkstr(dest_name != NULL ? dest_name : "") < 0)
	  PvmError();
	if (future != 0)
	  IncrementFutureExpected(pm, future);
	for (j = 0; j < counts[i]; ++j)
	  {
	    p = &(dests[i][j]);
	    if (pvm_pkint(&(p->idx), 1, 1) < 0 ||
		pvm_pkfloat(&(p->x), 1, 1) < 0 ||
		pvm_pkfloat(&(p->y), 1, 1) < 0 ||
		pvm_pkfloat(&(p->z), 1, 1) < 0)
	      PvmError();
	  }
	if (!SendLocalSetupMessage(pm, i))
	  PvmError();
      }

  for (i = 0; i < ipm->nnodes; ++i)
    if (dests[i] != NULL)
      free(dests[i]);
  free(dests);
  free(sizes);
  free(counts);

  NoteFuture(pm, future);	/* AllocateFuture said expect one */

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

char *remote_volume_weight(argc,argv,async,future,src_name,dest_name,decay,scale,decayarg1,decayarg2,mode,absrandom,randarg1,randarg2)
     int argc;
     char **argv;
     int async;
     unsigned int future;
     char *src_name;
     char *dest_name;
     short decay, mode, absrandom;
     float scale, decayarg1, decayarg2, randarg1, randarg2;
{
  Postmaster *pm;
  Implementation *ipm;
  BufferManager *bfm;
  int i, j;
  float x, y, z;
  Element *source;
  ElementList *src_list, *dest_list;
  MsgOut *mo;
  MsgIn *mi;
  int act_idx, blk, ind, cnt;
  unsigned int *pdstidx;
  int dst_node, dst_idx;
  unsigned int dstidx;
  char res[MAX_RESULT_LEN];
  struct Dest {
    int idx;
    float x, y, z;
  } **dests, *p;
  int *sizes;
  int *counts;
  float temp_scale, temp_decayarg1, temp_decayarg2,
    temp_randarg1, temp_randarg2;

  /* make local copies of the floating point arguments so that
     ALPHA compiler does the right thing */
  temp_scale = scale;
  temp_decayarg1 = decayarg1;
  temp_decayarg2 = decayarg2;
  temp_randarg1 = randarg1;
  temp_randarg2 = randarg2;

  src_list = WildcardGetElement(src_name,1);
  if (src_list->nelements == 0)
    {
      if ((pm = GetPostmaster()) != NULL)
	NoteFuture(pm, future);
      return("");
    }

  if (dest_name != NULL)
    dest_list = WildcardGetElement(dest_name, 1);
  else
    dest_list = NULL;

  /* modify all local synapses */
  for(i=0; i<src_list->nelements; i++)
    {
      source = src_list->element[i];
      x = source->x;
      y = source->y;
      z = source->z;
      MSGOUTLOOP(source, mo) {
	if (!CheckClass(mo->dst, ClassID("synchannel")))
	  continue;
	mi = (MsgIn*) GetMsgInByMsgOut(mo->dst, mo);
	if(mi->type != SPIKE) 
	  continue;

	/*
	 * Check to see if the synchan is in the destination list if any.
	 */
	if (dest_list != NULL &&
	    !IsElementInList(mo->dst, dest_list))
	  continue;

	SetSingleWeight(mi, mo, x, y, z, decay, scale, decayarg1, decayarg2, mode, absrandom, randarg1, randarg2);
      }
    }

  /* find the postmaster */
  if ((pm = GetPostmaster()) == NULL)
    return("");
  ipm = pm->ipm;
  assert(ipm != NULL);
  bfm = ipm->in_active_msg;

  dests = (struct Dest **) calloc(ipm->nnodes, sizeof(struct Dest *));
  sizes = (int *) calloc(ipm->nnodes, sizeof(int));
  counts = (int *) calloc(ipm->nnodes, sizeof(int));

  for (i = 0; i < src_list->nelements; ++i)
    {
      source = src_list->element[i];
      x = source->x;
      y = source->y;
      z = source->z;
      MSGOUTLOOP(source, mo) {
	if (mo->dst != (Element *) pm)
	  continue;
	mi = (MsgIn*) GetMsgInByMsgOut(mo->dst, mo);
	if (mi->type != PAR_ACTIVE_MSG)
	  continue;
	
	act_idx = MSGVALUE(mi, 0);

/*	INT_STRUCT_LOOP_HEAD(bfm, act_idx, dstidx) */
	InitIntStructLoop(bfm, act_idx);
	while (IterIntStructLoop(&dstidx))
	  {
	    dst_node = dstidx >> (8*sizeof(unsigned short));
	    assert(dst_node >= 0);
	    assert(dst_node < ipm->nnodes);
	    dst_idx = dstidx - (dst_node << (8*sizeof(unsigned short)));
	    
	    /* record that we need to send a message to this node */
	    if (counts[dst_node] >= sizes[dst_node])
	      {
		sizes[dst_node] = sizes[dst_node] ? 2*sizes[dst_node] : 64;
                RECALLOC(dests[dst_node], sizes[dst_node], sizeof(struct Dest), struct Dest * );
	      }

	    p = &(dests[dst_node][counts[dst_node]++]);
	    p->idx = dst_idx;
	    p->x = x;
	    p->y = y;
	    p->z = z;
	  }
/*	INT_STRUCT_LOOP_TAIL; */
      }
    }
  FreeElementList(src_list);
  FreeElementList(dest_list);

  for (i = 0; i < ipm->nznodes; ++i)
    if (counts[i] > 0)
      {
	InitPvmMessage(pm);
	PackCore(pm, PAR_VOLUME_WEIGHT_REQ);
	if (pvm_pkint(&(pm->mynode), 1, 1) < 0 ||
	    pvm_pkint(&i, 1, 1) < 0 ||
	    pvm_pkint(&(counts[i]), 1, 1) < 0 ||
	    pvm_pkuint(&future, 1, 1) < 0 ||
	    pvm_pkshort(&decay, 1, 1) < 0 ||
	    pvm_pkfloat(&temp_scale, 1, 1) < 0 ||
	    pvm_pkfloat(&temp_decayarg1, 1, 1) < 0 ||
	    pvm_pkfloat(&temp_decayarg2, 1, 1) < 0 ||
	    pvm_pkshort(&mode, 1, 1) < 0 ||
	    pvm_pkshort(&absrandom, 1, 1) < 0 ||
	    pvm_pkfloat(&temp_randarg1, 1, 1) < 0 ||
	    pvm_pkfloat(&temp_randarg2, 1, 1) < 0 ||
	    pvm_pkstr(dest_name != NULL ? dest_name : "") < 0)
	  PvmError();
	if (future != 0)
	  IncrementFutureExpected(pm, future);
	for (j = 0; j < counts[i]; ++j)
	  {
	    p = &(dests[i][j]);
	    if (pvm_pkint(&(p->idx), 1, 1) < 0 ||
		pvm_pkfloat(&(p->x), 1, 1) < 0 ||
		pvm_pkfloat(&(p->y), 1, 1) < 0 ||
		pvm_pkfloat(&(p->z), 1, 1) < 0)
	      PvmError();
	  }
	if (!SendLocalSetupMessage(pm, i))
	  PvmError();
      }

  for (i = 0; i < ipm->nnodes; ++i)
    if (dests[i] != NULL)
      free(dests[i]);
  free(dests);
  free(sizes);
  free(counts);

  NoteFuture(pm, future);	/* AllocateFuture said expect one */

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


/**********************************************************************
 *		PHASE 2:  FIND SYNAPSES ON DESTINATION NODE
 **********************************************************************/

HandleRemoteVolumeDelayRequest(pm, src_tid)
     Postmaster * pm;
     int src_tid;
{
  Implementation *ipm;
  BufferManager *bfm;
  int dst_node;
  int dst_idx;
  int future;
  float x, y, z;
  short radial, mode, absrandom, add;
  float scale, randarg1, randarg2;
  MsgOut *mo;
  MsgIn *mi;
  int blk, ind;
  int src_node;
  unsigned int *pmsgno;
  unsigned int msgno;
  int msgcnt;
  int reply;
  int i;
  int count;
  char dest_name[M_PATH_SIZE];
  ElementList *dest_list;

  /* unpack the message */
  if (pvm_upkint(&src_node, 1, 1) < 0 ||
      pvm_upkint(&dst_node, 1, 1) < 0 ||
      pvm_upkint(&count, 1, 1) < 0 ||
      pvm_upkint(&future, 1, 1) < 0 ||
      pvm_upkshort(&radial, 1, 1) < 0 ||
      pvm_upkfloat(&scale, 1, 1) < 0 ||
      pvm_upkshort(&mode, 1, 1) < 0 ||
      pvm_upkshort(&absrandom, 1, 1) < 0 ||
      pvm_upkfloat(&randarg1, 1, 1) < 0 ||
      pvm_upkfloat(&randarg2, 1, 1) < 0 ||
      pvm_upkshort(&add, 1, 1) < 0 ||
      pvm_upkstr(dest_name) < 0)
    PvmError();
     
  if (dest_name[0] != '\0')
    dest_list = WildcardGetElement(dest_name, 1);
  else
    dest_list = NULL;

  ipm = pm->ipm;
  assert(ipm != NULL);
  bfm = ipm->out_active[src_node];
  assert(bfm != NULL);

  for (i = 0; i < count; ++i)
    {
      if (pvm_upkint(&dst_idx, 1, 1) < 0 ||
	  pvm_upkfloat(&x, 1, 1) < 0 ||
	  pvm_upkfloat(&y, 1, 1) < 0 ||
	  pvm_upkfloat(&z, 1, 1) < 0)
	PvmError();

/*      MSG_STRUCT_LOOP_HEAD(bfm, dst_idx, mo) */
      InitMsgStructLoop(bfm, dst_idx);
      while (IterMsgStructLoop(&mo))
	{
	  if (!CheckClass(mo->dst, ClassID("synchannel")))
	    continue;
	  mi = (MsgIn *) GetMsgInByMsgOut(mo->dst, mo);
	  if (mi->type != SPIKE)
	    continue;
	  /*
	   * Check to see if the synchan is in the destination list if any.
	   */
	  if (dest_list != NULL && !IsElementInList(mo->dst, dest_list))
	    continue;
	  SetSingleDelay(mi, mo, x, y, z, radial, scale, mode, absrandom, randarg1, randarg2, add);
	}
/*      MSG_STRUCT_LOOP_TAIL; */
    }
  FreeElementList(dest_list);

  reply = TRUE;
  InitPvmMessage(pm);
  PackCore(pm, PAR_VOLUME_DELAY_REPLY);
  if (pvm_pkint(&reply, 1, 1) < 0 ||
      pvm_pkint(&future, 1, 1) < 0 ||
      !SendLocalSetupMessageByTid(pm, src_tid))
    PvmError();
}

HandleRemoteVolumeWeightRequest(pm, src_tid)
     Postmaster * pm;
     int src_tid;
{
  Implementation *ipm;
  BufferManager *bfm;
  int dst_node;
  int dst_idx;
  int future;
  float x, y, z;
  short decay, mode, absrandom;
  float scale, decayarg1, decayarg2, randarg1, randarg2;
  MsgOut *mo;
  MsgIn *mi;
  int blk, ind;
  int src_node;
  unsigned int *pmsgno;
  unsigned int msgno;
  int msgcnt;
  int reply;
  int i;
  int count;
  char dest_name[M_PATH_SIZE];
  ElementList *dest_list;

  if (pvm_upkint(&src_node, 1, 1) < 0 ||
      pvm_upkint(&dst_node, 1, 1) < 0 ||
      pvm_upkint(&count, 1, 1) < 0 ||
      pvm_upkint(&future, 1, 1) < 0 ||
      pvm_upkshort(&decay, 1, 1) < 0 ||
      pvm_upkfloat(&scale, 1, 1) < 0 ||
      pvm_upkfloat(&decayarg1, 1, 1) < 0 ||
      pvm_upkfloat(&decayarg2, 1, 1) < 0 ||
      pvm_upkshort(&mode, 1, 1) < 0 ||
      pvm_upkshort(&absrandom, 1, 1) < 0 ||
      pvm_upkfloat(&randarg1, 1, 1) < 0 ||
      pvm_upkfloat(&randarg2, 1, 1) < 0 ||
      pvm_upkstr(dest_name) < 0)
    PvmError();

  if (dest_name[0] != '\0')
    dest_list = WildcardGetElement(dest_name, 1);
  else
    dest_list = NULL;

  ipm = pm->ipm;
  assert(ipm != NULL);
  bfm = ipm->out_active[src_node];
  assert(bfm != NULL);

  for (i = 0; i < count; ++i)
    {
      if (pvm_upkint(&dst_idx, 1, 1) < 0 ||
	  pvm_upkfloat(&x, 1, 1) < 0 ||
	  pvm_upkfloat(&y, 1, 1) < 0 ||
	  pvm_upkfloat(&z, 1, 1) < 0)
	PvmError();

/*      MSG_STRUCT_LOOP_HEAD(bfm, dst_idx, mo) */
      InitMsgStructLoop(bfm, dst_idx);
      while (IterMsgStructLoop(&mo))
	{
	  if (!CheckClass(mo->dst, ClassID("synchannel")))
	    continue;
	  mi = (MsgIn *) GetMsgInByMsgOut(mo->dst, mo);
	  if (mi->type != SPIKE)
	    continue;
	  /*
	   * Check to see if the synchan is in the destination list if any.
	   */
	  if (dest_list != NULL && !IsElementInList(mo->dst, dest_list))
	    continue;
	  SetSingleWeight(mi, mo, x, y, z, decay, scale, decayarg1, decayarg2, mode, absrandom, randarg1, randarg2);
	}
/*      MSG_STRUCT_LOOP_TAIL; */
    }
  FreeElementList(dest_list);

  reply = TRUE;
  InitPvmMessage(pm);
  PackCore(pm, PAR_VOLUME_WEIGHT_REPLY);
  if (pvm_pkint(&reply, 1, 1) < 0 ||
      pvm_pkint(&future, 1, 1) < 0 ||
      !SendLocalSetupMessageByTid(pm, src_tid))
    PvmError();
}

SetSingleDelay(mi, mo, x, y, z, radial, scale, mode, absrandom, randarg1, randarg2, add)
     MsgIn *mi;
     MsgOut *mo;
     float x, y, z;
     short radial, mode, absrandom, add;
     float scale, randarg1, randarg2;
{
  int j;
  float delay;
  struct Synchan_type *synchan;

  /*
   * Check to see if the dest object is a synchan or a synchan2.
   * A syntype of 1 = synchan; 2 = synchan2.
   * This can be extended to deal with hebbsynchans etc.
   */

  synchan = (struct Synchan_type*) mo->dst;
      
  /*
   * Since the location of the synapse will differ for different
   * synapse types (due to different-sized synapses), we use a function 
   * call to get the address of the synapse.  This is easily extensible 
   * for variant synchan/synapse types (like hebbsynchan) but ABSOLUTELY 
   * REQUIRES that the mi, weight and delay fields are at the beginning 
   * of the synapse structure.  Similarly, we can cast all synchans to
   * a regular synchan object for the purpose of getting nsynapses
   * provided that the nsynapses field is always in the same place in each
   * synchan.  I realize this is pretty brittle.
   */      

  /* Find the synapse corresponding to the SPIKE message. */
  
  for (j = 0; j < synchan->nsynapses; j++)
    {
      if (mi == GetSynapseAddress(synchan, j)->mi)
	{
	  if (radial)
	    {
	      /*
	       * Calculate delay based on radial distance multiplied
	       * by inverse velocity.
	       */
	      delay = sqrt(sqr(x - mo->dst->x) +
			   sqr(y - mo->dst->y) +
			   sqr(z - mo->dst->z)) / scale;
	    }
	  else  /* fixed */
	    { 
	      /* Give all synapses a fixed delay. */
	      delay = scale; 
	    } 
	      
	  /* Add a random component to the delay. */
	  randomize_value(&delay, mode, absrandom, randarg1, randarg2);
	      
	  if(add)  /* add this delay to the previous value */
	    GetSynapseAddress(synchan, j)->delay += delay;
	  else     /* make this delay the delay of the synapse */
	    GetSynapseAddress(synchan, j)->delay = delay;

	  break;
	}
    }
}

SetSingleWeight (mi, mo, x, y, z, decay, scale, decayarg1, decayarg2, mode, absrandom, randarg1, randarg2)
     MsgIn *mi;
     MsgOut *mo;
     float x, y, z;
     short decay, mode, absrandom;
     float scale, decayarg1, decayarg2, randarg1, randarg2;
{
  int j;
  struct Synchan_type *synchan;
  float weight;

  /*
   * Check to see if the dest object is a synchan or a synchan2.
   * A syntype of 1 = synchan; 2 = synchan2.
   * This can be extended to deal with hebbsynchans etc.
   */
	    
  synchan = (struct Synchan_type*) mo->dst;
      
  /*
   * Since the location of the synapse will differ for different
   * synapse types (due to different-sized synapses), we use a function 
   * call to get the address of the synapse.  This is easily extensible 
   * for variant synchan/synapse types (like hebbsynchan) but ABSOLUTELY 
   * REQUIRES that the mi, weight and delay fields are at the beginning 
   * of the synapse structure.  Similarly, we can cast all synchans to
   * a regular synchan object for the purpose of getting nsynapses
   * provided that the nsynapses field is always in the same place in each
   * synchan.  I realize this is pretty brittle.
   */      
 
  /* Find the synapse corresponding to the SPIKE message. */

  for (j = 0; j < synchan->nsynapses; j++)
    if (mi == GetSynapseAddress(synchan, j)->mi)
      {
	if(decay)
	  {
	    /*
	     ** set the weight according to the radial distance between
	     ** the source and dst and the velocity
	     */
	    weight = (decayarg1 - decayarg2) *
	      exp(-scale * sqrt(sqr(x - mo->dst->x) +
				sqr(y - mo->dst->y) +
				sqr(z - mo->dst->z))) + decayarg2;
	  }
	else{ /* fixed */
	  /*
	   ** Give all synapses a fixed weight.
	   */
	  weight = scale;
	}

	/*
	 ** Add a random component to the weight.
	 */
	randomize_value(&weight, mode, absrandom, randarg1, randarg2);

	GetSynapseAddress(synchan, j)->weight = weight;
	
	break;
      }
  
}

/**********************************************************************
 *		PHASE 3:  FINAL CLEANUP AT SOURCE
 **********************************************************************/

int
HandleRemoteVolumeDelayReply (pm)
     Postmaster *pm;
{
  int result;
  int future;

  if (pvm_upkint(&result, 1, 1) < 0 ||
      pvm_upkint(&future, 1, 1) < 0)
    PvmError();
  if (result == 0)
    {
      Error();
      printf("remote volume delay failed\n");
    }
  if (future != 0)
    NoteFuture(pm, future);
  return 1;
}

int
HandleRemoteVolumeWeightReply (pm)
     Postmaster *pm;
{
  int result;
  int future;

  if (pvm_upkint(&result, 1, 1) < 0 ||
      pvm_upkint(&future, 1, 1) < 0)
    PvmError();
  if (result == 0)
    {
      Error();
      printf("remote volume weight failed\n");
    }
  if (future != 0)
    NoteFuture(pm, future);
  return 1;
}
