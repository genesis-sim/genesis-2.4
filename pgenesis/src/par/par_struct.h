/* $Id: par_struct.h,v 1.1.1.1 2005/06/14 04:38:39 svitak Exp $
 
 * $Log: par_struct.h,v $
 * Revision 1.1.1.1  2005/06/14 04:38:39  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.1  1999/12/19 03:59:42  mhucka
 * Adding PGENESIS from GENESIS 2.1 CDROM release.
 *
 * Revision 1.19  1997/07/15 00:16:23  ngoddard
 * *** empty log message ***
 *
 * Revision 1.18  1997/07/07 05:39:30  ngoddard
 * function prototpyes for sunos
 *
 * Revision 1.17  1997/05/19 17:47:48  ghood
 * Increased MAX_RCOMMAND_LEN to allow very long parameter strings
 *
 * Revision 1.16  1997/03/27 06:26:48  ngoddard
 * mods to scale up to larger networks
 *
 * Revision 1.15  1997/03/01 04:10:25  ngoddard
 * misc bug fixes for Alpha
 *
 * Revision 1.14  1997/02/28 05:24:50  ngoddard
 * support for lookahead optimisation, bug fixes
 *
 * Revision 1.13  1997/02/20 21:13:36  ngoddard
 * support for xpvm; using pvm_trecv for timeout ops; fixed bugs related to futures; provide user warnings when appears stalled; barrer and barrierall take a third argument: how long to wait before stall is assumed; beginnings of performance stats package
 *
 * Revision 1.12  1997/01/31 05:14:23  ngoddard
 * barriers don't complete asyncs now, minor bug fixes
 *
 * Revision 1.11  1997/01/21 22:20:22  ngoddard
 * paragon port
 *
 * $Log: par_struct.h,v $
 * Revision 1.1.1.1  2005/06/14 04:38:39  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.1  1999/12/19 03:59:42  mhucka
 * Adding PGENESIS from GENESIS 2.1 CDROM release.
 *
 * Revision 1.19  1997/07/15 00:16:23  ngoddard
 * *** empty log message ***
 *
 * Revision 1.18  1997/07/07 05:39:30  ngoddard
 * function prototpyes for sunos
 *
 * Revision 1.17  1997/05/19 17:47:48  ghood
 * Increased MAX_RCOMMAND_LEN to allow very long parameter strings
 *
 * Revision 1.16  1997/03/27 06:26:48  ngoddard
 * mods to scale up to larger networks
 *
 * Revision 1.15  1997/03/01 04:10:25  ngoddard
 * misc bug fixes for Alpha
 *
 * Revision 1.14  1997/02/28 05:24:50  ngoddard
 * support for lookahead optimisation, bug fixes
 *
 * Revision 1.13  1997/02/20 21:13:36  ngoddard
 * support for xpvm; using pvm_trecv for timeout ops; fixed bugs related to futures; provide user warnings when appears stalled; barrer and barrierall take a third argument: how long to wait before stall is assumed; beginnings of performance stats package
 *
 * Revision 1.12  1997/01/31 05:14:23  ngoddard
 * barriers don't complete asyncs now, minor bug fixes
 *
 * Revision 1.10  1997/01/21 20:19:45  ngoddard
 * bug fixes for zones, implement string literal fields, first release of pgenesis script
 *
 * Revision 1.9  1996/08/19 21:32:11  ghood
 * Made pvm calls type-consistent with their parameters.
 *
 * Revision 1.8  1996/08/13 21:25:03  ghood
 * General debugging to get parallel example scripts working.
 *
 * Revision 1.7  1996/06/21 16:10:42  ghood
 * General debugging
 *
 * Revision 1.6  1996/05/03 19:48:08  ghood
 * moving to biomed area
 *
 * Revision 1.5  1995/07/10  00:45:00  mckeon
 * Fixed pconnect's type promotion bug in the func prototypes
 *
 * Revision 1.4  1995/07/05  21:21:18  mckeon
 * Added skeleton for rvolumeconnect
 *
 * Revision 1.3  1995/05/24  15:34:42  ngoddard
 * improved threads but still basic design flaw, also fixed comment leader in barrier tests
 *
 * Revision 1.2  1995/05/10  18:56:40  ngoddard
 * before making changes to allow ids on barriers
 *
 * Revision 1.1  1995/04/03  22:04:12  ngoddard
 * Initial revision
 *
 * Revision 1.1  1994/09/07  19:01:00  ngoddard
 * passive messaging works, src figures map
 *
 * Revision 1.5  1993/12/30  00:06:21  ngoddard
 * Release 12/14/93
 *
 * Revision 1.2  1993/10/15  22:51:51  ngoddard
 * added Id and Log keywords
 *
 *
*/

#ifndef PARALLEL
#define PARALLEL

#include "struct_defs.h"
#include "bufman.h"
#include "requests.h"

#define SEND_BUF_SIZE 5000
#define PVM_NET

#define DELTA_NET	1
#define DELTA_BOTTLE_NET	2
#define DGRAM_NET	3
#define PVM_NET_TYPE	4


#define	ZONING	205

#define MESSAGE_NO_ERROR 0x00
#define MESSAGE_DEST_ERROR 0x01
#define MESSAGE_SRC_ERROR 0x02
#define MESSAGE_SETUP_ERROR 0x04
#define MESSAGE_SUM_ERROR 0x08

#define DATA_NO_ERROR	0x00
#define DATA_DEST_ERROR	0x101
#define DATA_SRC_ERROR	0x102
#define DATA_STEP_ERROR	0x104
#define DATA_SUM_ERROR	0x108
#define DATA_LEN_ERROR	0x110

/* By default, the simulator does checksumming and reports any errors */
/* Print core fields of arriving messages */
#define	CHECK_CORE_IN_MASK	0x01
/* Print core fields of departing messages */
#define	CHECK_CORE_OUT_MASK	0x02
/* Print contents of arriving messages */
#define CHECK_DATA_IN_MASK	0x20
/* Print contents of departing messages */
#define CHECK_DATA_OUT_MASK	0x40

/* Switches off reporting of checksum errors */
#define CHECK_SILENT_MASK 0x80
/* Does automatic correction */
#define CHECK_FIX_MASK	0x100

/* Turns off checksumming */
#define CHECK_SUM_OFF_MASK	0x2000
/* Use at your peril ! Turns off checking altogether */
#define	CHECK_OFF_MASK	0x1000

/* A nice large number */
#define MAX_NODES 1024

/* The number of characters in a remote command message */
#define MAX_RCOMMAND_LEN 50000
#define MAX_RCOMMAND_ARGS 32
#define M_DATA_SIZE 112			/* optimized for the Delta, so the
								** entire SetupMessage is < 1K */
#define M_STR_SIZE	40
#define M_PATH_SIZE	100
#define M_NET_ID_SIZE	10
#define M_VERIFY_SIZE	15


/* seconds till timeout on internode data messages */
/* We need to be generous for large simulations, since the code load
** time is factored into this */
/* #define HANG_TIME 120.0 */
#define HANG_TIME 10000.0	/* temporary while debugging */

/* Seconds till resend on internode msg and setup messages */
/* We need a value long enough to avoid duplicate sends, but 
** short enough not to waste time waiting */ 
#define MSG_ACK_TIME 5.02
#define SETUP_ACK_TIME 5.1

#define PAR_LOCAL_MSGID -20
#define PAR_DATA_ARRIVED -10
#define PAR_DATA_SENT -100


#define PAR_CHECKSUM_OFFSET 12	/* 12 bytes should safely cover the
								** sum and the messno */
#define MESSAGE_CORE \
	int	checksum; \
	long messno; \
	int m_type; \
	int	len; \
	int	step;	\
	int	src_node;	\
	int	dest_node;

/* NHG Implementation types */

typedef struct _FutureItem {
  int restyp;			/* type of result requested */
  char * res;			/* results from nodes */
  unsigned short expected;	/* expected number of results */
  unsigned short received;	/* number received */
} FutureItem;

typedef struct _BackPtr {
  short src_node;		/* source node */
  int idx;			/* index into the ipm->src buffer
				   of the source node's postmaster */
} BackPtr;

typedef struct _MsgSpec {
  int type;
  char * src_path;
  ElementList * src_list;
  char * dst_path;
  int dst_node;
  char * typename;
  int nslot;
  Slot * sarray;
  char * slottype;		/* type of each slot */
  char * string_val;		/* strings for slots */
  int string_cnt;		/* number of strings */
  int string_len;		/* combined length of strings including nulls*/
  char ** slotname;		/* vector of slot names */
  IndexType * field_map;	/* indices of fields */
  double * field_val;		/* values  of fields */
  IndexType * srcid;		/* indices of src elements */
  unsigned int future;		/* future to return */
  short volumeq;
  struct extended_mask_type *src_region;
  int nsrc_regions;
  struct extended_mask_type *dst_region;
  int ndst_regions;
  short relative;
  float pconnect;
  float decay;
  float scale;
  float decayarg1;
  float decayarg2;
  short mode;
  short absrandom;
  float randarg1;
  float randarg2;
  short radial;
  short add;
  int dst_idx;
  struct _MsgSpec * next;
} MsgSpec;

typedef struct _implementation_type {
  unsigned int user_flags;	/* aspects settable by the user */
  int mynode;			/* my global node number */
  int nnodes;			/* total number of Genesis nodes */
  int * tid_array;		/* PVM tids indexed by mynode */
  int * others_tid_array;	/* PVM tids of the other nodes (no index) */
  int encoding;			/* PvmDataDefault, DataRaw, DataInPlace */

  /* TODO: these should be in their own data structure */
  int myznode;			/* Genesis node number in zone */
  int myzone;			/* Genesis zone number */
  int nznodes;			/* number of Genesis nodes in my zone */
  int nzones;			/* number of Genesis zones */
  int ** zone_tids;		/* tid_array per zone */
  int * zone_other_tids;	/* other_tid_array for my zone */
  int * other_zone_tids;	/* zero nodes in other zones */
  int * zone_siz;		/* zone size, indexed by zone number */
  int * zone_init;		/* nodenum of first node in zone */
  int * zone;			/* indexed by Genesis node id, gives zone */
  unsigned int * zone_synced;	/* which zones have synced; vec bitflgs*/
  short * zone_sync_cnt;	/* num of zones that have synced; idx syncid*/
  unsigned int * global_synced;	/* which of my nodes have reached barrierall */
  short * global_sync_cnt;	/* num of my zones nodes have synced */
  unsigned int * synced;	/* which of my zones nodes have synced */
  short * sync_cnt;		/* number of my zones nodes have synced */
  unsigned int wait_on_sync;	/* bit set if waiting for given sync id */
  unsigned int wait_on_global_sync; /* bit set if waiting for given sync id */
  unsigned int wait_on_zone_sync; /* bit set if waiting for given sync id */

  /* These are used to keep track of who we should get/send from to */
  int num_in_nodes;		/* number of nodes we send messages to */
  int num_out_nodes;		/* number of nodes we get messages from */
  unsigned char * exist_msg;	/* type of messages the node sends us */
  unsigned char * check_msg;	/* used to check off msgs sent/recd */

  /* indexed by sending node number */
  double * lookahead;		/* how far we can get ahead of sending nodes */
  double * remain_ahead;	/* how much further we can get ahead */
  double * most_recent;		/* most recent time received */

  /* these implement the list of elements which are srcs for the pm */
  /* we could get this info from Genesis */
  BufferManager * src;	/* src elements sending via PM */
  /* key is address of src, item is a reference counter */

  /* this is the table of backpointers so that given a MsgOut
     on the destination node's postmaster, we can find the corresponding
     MsgIn on the source node's postmaster */
  BufferManager * back_ptr;
  /* key is MsgOut number on postmaster, item is a BackPtr */
     
  /* the following all have nodeno as the first index */
  BufferManager ** out_data; /* deal with field data */
  /* for out_data we only use the key pointer to know if a block exists */
  /* do not use the keys so could be a dummy pointer */
  /* item is the data (double) */

  BufferManager ** out_active;
  /* key is idx given by src_node to src_elt */
  /* item[idx] is vector of MsgNos (int), elt0 tells length  */

  /* OBSELETE? used to process outgoing active messages */
  short out_active_siz;		/* size of the arrays below */
  short * out_active_idx;	/* the indices into out_active[src] */
  double * out_active_time;	/* the times of the events */

  BufferManager ** in_data;	/* data to transmit to each node */
  /* key is index into field_info, item is the data (double) */

  MsgSpec ** buffered_msgs;	/* messages waiting to be set up */
  int * buffered_msg_cnt;	/* number of buffered messages */

  BufferManager ** pending_field; /* temp field info for src */
  /* key is field's addr, item is type (char) followed by ref count */
  /* ASSUMPTION: sizeof(key) == sizeof(addr) */

  BufferManager ** pending_index; /* temp field info for dst */
  /* key is field's pending index given by src, item is */
  /* the index into out_data assigned by the dst */

  /* these are initilized and filled in the PROCESS action */
  unsigned short ** in_active;	/* dst-specific active-msg numbers */
  double ** in_active_time;		/* time of occurrence */
  unsigned short * in_active_siz; /* size of in_active */
  unsigned short * in_active_cnt; /* the number of them this time */

  /* These are used to accumulate data for transmission to dst PM */
  BufferManager * field_info; /* fields we need to read */
  /* key is field's addr, item is field's type (char) followed by data (dbl) */
  /* followed by a reference count */
  /* ASSUMPTION: sizeof(key) == sizeof(addr) */

  BufferManager * in_active_msg; /* lists of dstnode/idx active messages */
  /* key is id given to src-elt/slots, currently slots not allowed */
  /* data[idx] is a vector of ints, [0] give length, each int holds a */
  /* dst_node/dst_idx spec */

  PFD access_func[9];		/* the access functions */

  int add_msg;			/* zero => ADDMSGOUT disallowed */
  int current_active_node;	/* node for which ADDMSGOUT applies */
  IndexType current_active_index; /* buffer idx for which ADDMSGOUT applies */

  /* These are used to hold MsgSpec lists while dst PM is responding */
  /* should use a buffer manager but why break code that works? */
  void ** held_pointers;	/* point to message lists in progress */
  char * held_used;		/* indicates which held pointers in use */
  int held_size;		/* size of held vectors */
  int held_cnt;			/* number actually held */

  unsigned int * nores_rcmds_sent; /* resultless rcmds sent since last sync */
  int nores_rcmds_cnt;	/* count of nodes with outstanding rcommands */
  unsigned int * nores_rcmds_handled; /* resultless rcmds handled per node */
  BufferManager * future;	/* futures waiting to be resolved */
  int future_cnt;		/* number outstanding */
  /* key is not used.  data[idx] is a pointer to a FutureItem */

  /* here are the lists of requests outstanding */
  struct list_obj * rcmd_done_head; /* requests for rcommands to be done */
  struct list_obj * free_rcmd_done; /* free list */

  char **replies;		/* used in do_remote_show_msg to gather
				   replies from the other processors */
  int pvm_buf_ready;		/* 0 => ready to send, 1 => sent */
} Implementation;


struct post_type {
  ELEMENT_TYPE
  /* user-visible readonly fields */
  int mynode;			/* Genesis node number in zone */
  int myzone;			/* Genesis zone number */
  int nnodes;			/* number of Genesis nodes in zone */
  int nzones;			/* number of Genesis zones */
  int totalnodes;		/* number of Genesis nodes in all zones */

  /* user-settable fields */
  int remote_info;		/* boolean: should remote info be provided */
  int delete_msgs;		/* boolean: is message deletion allowed */
  int delete_elts;		/* boolean: is element deletion allowed */
  int copy_elts;		/* boolean: is element copy allowed */
  int sync_before_step;		/* boolean: not needed for multiple steps */
  int min_delay;		/* minimum step delay in active messages */
  int remote_error_report;	/* setup errors reported on src, dst, none */
  int perfmon;			/* boolean: collect performance statistics */

  float	msg_hang_time;	/* PGENESIS timeout on all commands, seconds */
  float pvm_hang_time;		/* timout to give pvm commands when looping */
  float xupdate_period;		/* process X events this often when stalled*/ 
  float	xmin,xmax,ymin,ymax,zmin,zmax;	/* boundaries of this
					 ** postmaster's domain */
  /* hidden fields */
  Implementation * ipm;		/* the gory details */

};

typedef struct post_type Post;
typedef struct post_type Postmaster;
typedef struct src_info_type  SrcInfo;
typedef struct dest_info_type DestInfo;
typedef struct setup_message_type SetupMessage;

#if FUNC_PROT
/* from bufman.c */
extern void InitReferBufferIndex(BufferManager * bfm, IndexType idx);
extern void ReferBufferIndex(BufferManager * bfm, IndexType idx);
extern void UnreferBufferIndex(BufferManager * bfm, IndexType idx);
extern void RemoveBufferIndex(BufferManager * bfm, IndexType idx);
extern void ExpandBufferFree(BufferManager * bfm);
extern IndexType FindBufferIndex(BufferManager * bfm, KeyType key);
extern IndexType EnterBufferIndex(BufferManager * bfm, KeyType key);
extern BufferManager * InitBufferManager(int blks, int size, int eltsiz);
extern void ExpandBufferManager(BufferManager * bfm, int size);
extern void AddBufferBlock(BufferManager * bfm, int blkno);
extern IndexType GetBufferIndex(BufferManager ** pbfm, KeyType key,
			  int blkcnt, int blksiz, int eltsiz, int *newidx);
extern IndexType GetRefdBufferIndex(BufferManager ** pbfm, KeyType key,
			  int blkcnt, int blksiz, int eltsiz, int *newidx);
extern void CreateBufferIndex(BufferManager ** pbfm, IndexType idx,
			  int blkcnt, int blksiz, int eltsiz, int overwrite);
extern int BufferEmpty(BufferManager * bfm);
extern void SetBufferIndexItem(BufferManager * bfm, IndexType mapid, void * data);
extern void * GetBufferIndexItem(BufferManager * bfm, IndexType mapid);
void AddStructToVectorItem(BufferManager *, IndexType, void *, int);
int StructInVectorItem(BufferManager *, IndexType, void *, int);

/* pvm-shared.c public routines */
extern int GetParlibStep();
extern void SetParlibStep(int n);
extern Postmaster * GetPostmaster();
extern int SendStartupMessage(Postmaster * pm, int * tids, int tidcnt, int mtype);
extern void ReusePvmMessage(Postmaster * pm);
extern int ProcessSetupMessage(Postmaster * pm, int bufid);

/* par_connect.c public routines */
extern char *do_remote_add_msg(int argc, char **argv);
extern char *remote_add_msg_entry(int argc, char **argv, int async, unsigned int future);
extern char *do_remote_volumeconnect(int argc, char **argv);
extern char *remote_volume_connect_entry(int argc, char **argv, int async, unsigned int future);
extern void HandleRemoteMessageRequest(Postmaster *pm, int src_tid);

/* par_show.c public routines */
extern char *do_remote_who_msg(int argc, char **argv);

/* par_volume.c public routines */
extern char *do_remote_volume_delay(int argc, char **argv);
extern char *do_remote_volume_weight(int argc, char **argv);

/* rsync.c public routines */
extern int SyncZoneNodes (Postmaster * pm, unsigned int sync_id, double sync_time, double note_time); 
extern int SyncAllNodes (Postmaster * pm, unsigned int sync_id, double sync_time, double note_time);
extern int HandleSyncReply (Postmaster * pm, int mtype); 
extern int HandleSyncRequest (Postmaster * pm, int mtype); 

/* setup_handler.c public routines */
extern void AddRcommandDoneReq(Postmaster * pm, int src_node, int src_tid, int cnt);

/* NHG end of implementation types */
#else /* not FUNC_PROT */
extern void InitReferBufferIndex();
extern void ReferBufferIndex();
extern void UnreferBufferIndex();
extern void RemoveBufferIndex();
extern void ExpandBufferFree();
extern IndexType FindBufferIndex();
extern IndexType EnterBufferIndex();
extern BufferManager * InitBufferManager();
extern void ExpandBufferManager();
extern void AddBufferBlock();
extern IndexType GetBufferIndex();
extern IndexType GetRefdBufferIndex();
extern void CreateBufferIndex();
extern void AddIntToBuffer();
extern int BufferEmpty();
extern void SetBufferIndexItem();
extern void * GetBufferIndexItem();
extern char *do_remote_add_msg();
extern char *remote_add_msg_entry();
extern char *do_remote_volumeconnect();
extern char *remote_volume_connect_entry();
extern void HandleRemoteMessageRequest();
extern char *do_remote_who_msg();
extern char *do_remote_volume_delay();
extern char *do_remote_volume_weight();

/* pvm-shared.c public routines */
extern int GetParlibStep();
extern void SetParlibStep();
extern Postmaster * GetPostmaster();
extern int ProcessSetupMessage();

/* rsync.c public routines */
extern int SyncZoneNodes();
extern int SyncAllNodes();
extern int HandleSyncReply();
extern int HandleSyncRequest();

/* setup_handler.c public routines */
extern void AddRcommandDoneReq();

#endif /* FUNC_PROT */

#endif /* PARALLEL */


