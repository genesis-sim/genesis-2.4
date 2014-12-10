#include <stddef.h>
/* size of chunks in which free lists are realloced */
#define FREE_BLK 128

/*

Buffer manager manages a vector <item> of items that will be accessed
at runtime by indices into the buffer.  The same indices index <key>
which holds handles.  <fre> is a vector of free indices.  <siz> is the
size of these arrays.  <free_cnt> is the number of free indices.  Free
indices should be consumed last first, and free_cnt decremented.  The
handles are all positive.  Free indices should access 0 values in
<key>.  This should be a parameterized C++ object.

Note that both keys and indices into the buffer manager are never 0.
This means the first item is never used.  We need this because some
indices are keys for other buffer managers, and a key of zero means an
unused entry.

The item array is allocated in chunks, so <item> is an array of
pointers to chunks Each chunk contains the same number of actual items
This may be more memory efficient: reallocs operate on the chunks, and
the array of chunks, but not all the chunks at once.  We assume the
fresiz is normally the same as siz.

*/

typedef unsigned int IndexType;
typedef unsigned int RefType;
typedef ptrdiff_t KeyType;	/* ANSI standard in stddef.h */

typedef struct _blocked_buffer_manager {
  void ** item;			/* the array of arrays of items */
  KeyType ** key;		/* the array of arrays of keys (can be addr) */
  short siz;			/* the size of the arrays of items/keys */
  short blkcnt;			/* the size of the arrays <item> and <key> */
  short blkinc;			/* the size in which item is expanded */
  short eltsiz;			/* the byte size of an item */
  IndexType * fre;		/* the array of free indices */
  int fresiz;			/* the size of the array of free indices */
  int free_cnt;			/* the number of free indices */
} BufferManager;

/* NOTE: in these structs it is required that <refs> be the first field! */
/* so that the BUF_MAN_REFS macro works */

#define SHIFT_NO (4*sizeof(IndexType))
#define BLOCK_NO(idx) ((idx) >> SHIFT_NO)
#define INDEX_NO(idx) ((idx) - (BLOCK_NO(idx) << SHIFT_NO))
#define MAKE_INDEX(blk,idx) ((IndexType) (((blk) << SHIFT_NO) | (idx)))

#define REF_ADDR(bfm, idx) ((int *) (((char *) (bfm->item[BLOCK_NO(idx)])) + \
				     (INDEX_NO(idx)*bfm->eltsiz)))
#define SET_BUF_REFS(bfm, idx, val) (*(REF_ADDR(bfm, idx))) = val
#define INC_BUF_REFS(bfm, idx) ++(*(REF_ADDR(bfm, idx)))
#define DEC_BUF_REFS(bfm, idx) --(*(REF_ADDR(bfm, idx)))

typedef struct _field_info {
  int refs;			/* the counter for references */
  unsigned char type;		/* index into AccessFunctions */
  double data;			/* the value */
} FieldInfo;
#define TYPE_ADDR(bfm, idx) \
  ((unsigned char *) (&((((FieldInfo *) (bfm->item[BLOCK_NO(idx)]))[INDEX_NO(idx)]).type)))
#define DATA_ADDR(bfm, idx) \
  ((double *) (&((((FieldInfo *) (bfm->item[BLOCK_NO(idx)]))[INDEX_NO(idx)]).data)))

typedef struct _field_infoB {
  int refs;			/* the counter for references */
  unsigned char type;		/* index into AccessFunctions */
} FieldInfoB;
#define PTYPE_ADDR(bfm, idx) \
  ((unsigned char *) (&((((FieldInfoB *) (bfm->item[BLOCK_NO(idx)]))[INDEX_NO(idx)]).type)))

typedef struct _in_act_type {
  IndexType idx;
  unsigned short node;
} InActType;
#define IAT_IDX_ADDR(bfm, idx) \
  ((IndexType *) (&((((InActType *) (bfm->item[BLOCK_NO(idx)]))[INDEX_NO(idx)]).idx)))
#define IAT_NODE_ADDR(bfm, idx) \
  ((unsigned short *) (&((((InActType *) (bfm->item[BLOCK_NO(idx)]))[INDEX_NO(idx)]).node)))


/* Vector items consist of a byte string.  The first sizeof(unsigned short)
   bytes indicate the number (siz) of structs of size nbytes in the vector.
   The remaining siz * nbytes bytes are the structs.
*/

/* loop through item vector, bfm points to buffer manager, ind is the index
   and blk the block */
/* I is a pointer to the current struct of type ITYPE in the loop body */

#define STRUCT_LOOP_HEAD(bfm, blk, ind, I, ITYPE) \
 { \
   unsigned short structcnt; \
   unsigned short * pmsgcnt = ((unsigned short **) (bfm->item[blk]))[ind]; \
   ITYPE * iterptr = (ITYPE *) pmsgcnt; \
   structcnt = *((unsigned short *) (pmsgcnt++));/* number of structs in vector */ \
   assert(structcnt >= 0); \
   while (structcnt-- > 0) \
     { \
       I = *(iterptr++);

#define STRUCT_LOOP_TAIL } }

/* specialized for a vector of pointers to active Msgs */
#define MSG_STRUCT_LOOP_HEAD(bfm, idx, iterator) \
  { \
     int blk, ind; \
     assert((idx) != 0); \
     blk = (idx) >> OUT_ACTIVE_BLK_BITS; \
     ind = (idx) - (blk << OUT_ACTIVE_BLK_BITS); \
     STRUCT_LOOP_HEAD(bfm, blk, ind, iterator, Msg *);

#define MSG_STRUCT_LOOP_TAIL STRUCT_LOOP_TAIL }

/* specialized for a vector of integers */
#define INT_STRUCT_LOOP_HEAD(bfm, idx, iterator) \
  { \
     int blk, ind; \
     assert((idx) != 0); \
     blk = BLOCK_NO(idx); \
     ind = INDEX_NO(idx); \
     STRUCT_LOOP_HEAD(bfm, blk, ind, iterator, int);

#define INT_STRUCT_LOOP_TAIL STRUCT_LOOP_TAIL }

