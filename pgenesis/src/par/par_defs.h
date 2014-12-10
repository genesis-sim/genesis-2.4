/* #define DBGMSG(arg) Dbgmsg(arg) */
#define DBGMSG(arg)
void ELog(char *s);
#ifndef LOOKAHEAD_DEBUG
#define LOOKAHEAD_DEBUG 0
#endif

/* In the following list the BLKS constants determine how many
   new block pointers we should add at a time; the BLK_SIZ constants
   determine the size of each block */

#define IN_PENDING_BLKS 32
#define IN_PENDING_BLK_SIZ 1024
#define OUT_PENDING_BLKS 32
#define OUT_PENDING_BLK_SIZ 1024
#define SRC_BLKS 32
#define SRC_BLK_SIZ 1024
#define IN_FLD_BLKS 32
#define IN_FLD_BLK_SIZ 1024
#define IN_DATA_BLKS 32
#define IN_DATA_BLK_SIZ 1024
#define IN_ACTIVE_BLKS 32
#define IN_ACTIVE_BLK_SIZ 1024
#define OUT_FLD_BLKS 32
#define OUT_FLD_BLK_SIZ 1024
#define OUT_ACTIVE_BLKS 32
#define OUT_ACTIVE_BLK_BITS 6
#define OUT_ACTIVE_BLK_SIZ (1 << (OUT_ACTIVE_BLK_BITS - 1))
#define IN_ACT_BLK_SIZ 1024
#define OUT_ACT_BLK_SIZ 1024
#define FUTURE_BLK_CNT 32
#define FUTURE_BLK_SIZ 1024
#define BACK_PTR_BLKS 32
#define BACK_PTR_SIZ 4096

/* slot types */
#define STRING_SLOT 1
#define LITERAL_SLOT 2
#define VARIABLE_SLOT 3

/* result types */
#define NO_RESULT 0
#define COMPLETION_RESULT 1
#define SCALAR_RESULT 2
#define REDUCE_ADD_RESULT 3

#define OUT_ACTIVE 0x1
#define OUT_PASSIVE 0x2
#define IN_ACTIVE 0x4
#define IN_PASSIVE 0x8

/* where to report addmsg errors */
#define REMOTE_ERROR_DST 0
#define REMOTE_ERROR_SRC 1
#define REMOTE_ERROR_NONE 2

/* dhb's suggested macro */
#define MSGACTIVE(M) ((M)->type < 0)

/* for coding if a field index has been seen before */
/* #define PENDING        0x80000000 */
/* #define NEW_USE        0x40000000 */
/* #define INDEX_MASK     0x3fffffff */

#define PENDING        (((unsigned int) 1) << ((sizeof(int) * 8) - 1))
#define NEW_USE        (((unsigned int) 1) << ((sizeof(int) * 8) - 2))
#define INDEX_MASK     (~(PENDING | NEW_USE))

#define CODE_AGREED_INDEX(idx) (idx)
#define CODE_NEW_PENDING_INDEX(idx) (PENDING | NEW_USE | (idx))
#define CODE_OLD_PENDING_INDEX(idx) (PENDING | (idx))
#define INDEX(idx) ((idx) & INDEX_MASK)
#define PENDING_INDEX(idx) (((idx) & PENDING) != 0)
#define NEW_USE_INDEX(idx) (((idx) & NEW_USE) != 0)

/* for multiplexing a node number (12 bits) and buffer index (20 bits) */
/* ASSUMPTION at least 32 bit ints */
#define IDX_MASK       0xfffff
#define NODE_SHIFT     20

/* Genesis message types accepted by Postmaster*/
#define PAR_NOTIFY_DELETE 0
#define PAR_ACTIVE_MSG -1

/* sizes of fixed arrays */
#define MAX_RESULT_LEN 1024
#define MAX_PATH_LEN 1024
#define MAX_BUFFERED_MESSAGES 0
#define NSLOTS		20
#define MAX_TYPE_LEN 128

/* size of blocks for things malloced in blocks */
/* for incoming fields */
#define FIELD_BLK_SIZE 100
/* for outgoing fields */
#define OUTFLD_BLK_SIZE 100
/* for the list of src elts */
#define SRC_BLK_SIZE 100

/* error codes */
#define MSG_OUT_OF_ZONE 1
#define MSG_INVALID_DST_NODE 2
#define OUT_OF_MEMORY 3
#define UNPACK_CORE_FAIL 4
#define GLOBAL_SEND_FAIL 5
#define SEND_FAIL 6
#define SPAWN_FAILURE 7
#define COMPLETE_PASSIVE_MSG_FAIL 8
#define BAD_FIELD_COUNT 9
#define BAD_TYPE 10
#define PAR_OK 11
#define POINTER_ALREADY_RELEASED 12
#define PAR_REQUEST_ERROR 13
#define PAR_REQUEST_PASSIVE 14
#define PAR_REQUEST_ACTIVE 15
#define MIXED_TYPE 16
#define MSG_INVALID_DST_ZONE 17
#define UNMATCHED_SYNC_IDS 18
#define SYNC_HUNG 19
#define MULTI_SEND_FAIL 20
#define COMMAND_CLEAR_HUNG 21
#define FAILED_FUTURE_ALLOCATION 22
#define NO_POSTMASTER 23
#define DUPLICATE_SYNC_IDS 24
#define GLOBAL_SYNC_HUNG 25
#define ZONE_SYNC_HUNG 26
#define DUPLICATE_GLOBAL_SYNC_IDS 27
#define DUPLICATE_ZONE_SYNC_IDS 28
#define UNIMPLEMENTED_SLOT_FUNCTION 29
#define UNKNOWN_SLOT_TYPE 30
#define PAR_REQUEST_LITERAL 31

/* PVM message tag ids */
#define PAR_DATA_SEND 100
#define PAR_SETUP_MESSAGE 200
/*
  #define LOCAL_SETUP ((PAR_SETUP_MESSAGE << 16) | (GetParlibStep() & 0xffff))
*/

#define LOCAL_SETUP PAR_SETUP_MESSAGE
#define REMOTE_SETUP PAR_SETUP_MESSAGE
#define LOCAL_DATA PAR_DATA_SEND

/* default PVM packing method */
#define PVM_DEF_PACK PvmDataDefault

/* List of message types. Now that we are free of depending on the
** delta message typing schemes, we can be profligate with the
** number of message types.
*/
#define PAR_SETUP_ERROR 0
#define PAR_SYNTAX_ERROR 1
#define PAR_MSG_REQ 10
#define PAR_MSG_REPLY 11
#define PAR_CONN_REQ 12
#define PAR_CONN_REPLY 13
#define PAR_3D_MSG_REQ 20
#define PAR_3D_MSG_REPLY 21
#define PAR_DD_MSG_REQ 22
#define PAR_DD_MSG_REPLY 23
#define PAR_3D_CONN_REQ 24
#define PAR_3D_CONN_REPLY 25
#define PAR_COMMAND_REQ 32
#define PAR_COMMAND_REPLY 33
#define PAR_GLOB_REQ 34
#define PAR_GLOB_REPLY 35
#define PAR_SYNC_REQ 40
#define PAR_SYNC_REPLY 41
#define PAR_ALL_SYNC_REQ 42
#define PAR_ALL_SYNC_REPLY 43
#define PAR_VERIFY_REQ 50
#define PAR_VERIFY_REPLY 51
#define PAR_TAB_FILL 60
#define PAR_TAB_FILL_REPLY 61
#define PAR_WORKER_ALIVE 62
#define PAR_NODEID_MSG 63
#define PAR_ADD_MSG_REQ 64
#define PAR_ADD_MSG_REPLY 65
#define PAR_GLOBAL_SYNC_REQ 66
#define PAR_GLOBAL_SYNC_REPLY 67
#define PAR_ZONE_SYNC_REQ 68
#define PAR_ZONE_SYNC_REPLY 69
#define PAR_RCOMMANDS_DONE_REQ 70
#define PAR_RCOMMANDS_DONE_REPLY 71
#define PAR_VOLUME_DELAY_REQ 72
#define PAR_VOLUME_DELAY_REPLY 73
#define PAR_VOLUME_WEIGHT_REQ 74
#define PAR_VOLUME_WEIGHT_REPLY 75
#define PAR_SHOW_ACTIVE_MSG_OUT_REQ 76
#define PAR_SHOW_PASSIVE_MSG_OUT_REQ 77
#define PAR_SHOW_MSG_OUT_REPLY 78
#define PAR_SHOW_MSG_IN_REQ 79
#define PAR_SHOW_MSG_IN_REPLY 80

#define PAR_DATA_MSG 100
#define PAR_MSG_ACK 101
#define PAR_ISACTIVE_MSG 102
#define DATA_MSG_PENDING 1000

#define PAR_SETUP_ACK 200

/* #'s 1-24 reserved for user */
#define PAR_DEFAULT_SYNC 31
#define PAR_DEFAULT_ALL_SYNC 30
#define PAR_OFF_ALL_SYNC 29
#define PAR_STEP_SYNC 28
#define PAR_RESET_SYNC 27

/* timeout on condition */
#define CLR_SETUP_COND_TIMEOUT(pm, ttim, ntim, cond, msg) \
{  \
   int i, notified = 0; \
   Implementation * ipm = (pm)->ipm; \
   double pgen_clock(), start_time, end_time, cur_time; \
   \
   end_time = (start_time = pgen_clock()) + (ttim); \
   while (((cur_time = pgen_clock()) < end_time) && /* timeout */ \
	  (cond))	/* completion condition */ \
    { \
      if (IsSilent() < 1) 	/* only notify if we aren't silent */ \
        if (notified)		/* we've been waiting a long time */ \
	  { \
	    fprintf(stdout, "."); \
	    fflush(stdout); \
	  } \
        else			/* notify user if stalled */ \
	  if (cur_time > start_time + (ntim)) \
	    { \
	      fprintf(stdout, (msg)); \
	      fflush(stdout); \
	      notified = 1; \
	    } \
      ClearSetupMessages(pm, 1, (ntim)); \
    } \
   if (notified) \
      fprintf(stdout, "\n"); \
}

#define RECALLOC( ptr, count, elsiz, ptr_type ) \
  if (ptr == NULL) { ptr = (ptr_type) calloc ( count, elsiz ); } \
  else { ptr = (ptr_type) realloc (ptr, count*elsiz); }
