static char rcsid[] = "$Id: bufman.c,v 1.2 2005/09/29 23:10:31 ghood Exp $";

/*
 * $Log: bufman.c,v $
 * Revision 1.2  2005/09/29 23:10:31  ghood
 * made the code use limits.h instead of values.h
 *
 * Revision 1.1.1.1  2005/06/14 04:38:39  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.1  1999/12/19 03:59:41  mhucka
 * Adding PGENESIS from GENESIS 2.1 CDROM release.
 *
 * Revision 1.4  1997/07/14 03:33:29  ngoddard
 * removed redundant assertions generating compiler warnings
 *
 * Revision 1.3  1997/02/28 05:24:45  ngoddard
 * support for lookahead optimisation, bug fixes
 *
 * Revision 1.2  1995/04/13 18:12:15  ngoddard
 * Commented StructInVectorItem
 *
 * Revision 1.1  1995/04/03  22:03:39  ngoddard
 * Initial revision
 *
 * Revision 1.1  1994/09/07  19:01:00  ngoddard
 * passive messaging works, src figures map
 *
 */

/* 
  TODO handle character strings (or any vector?)
  TODO turn on/off PASSIVE and ACTIVE messages before/after addmsg
  TODO make sure num_in_nodes, num_out_nodes are adjusted on errors
  */
#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include "par_ext.h"

unsigned short structcnt; 
Msg ** itermsg;
unsigned int * iterint;

void InitReferBufferIndex(bfm, idx)
     BufferManager * bfm;
     IndexType idx;
{
  SET_BUF_REFS(bfm, idx, 0);
}

void ReferBufferIndex(bfm, idx)
     BufferManager * bfm;
     IndexType idx;
{
  INC_BUF_REFS(bfm, idx);
}

/* TODO should try to consolidate the item/key arrays sometime */
void ExpandBufferFree(bfm)
     BufferManager * bfm;
{
  bfm->fre = (IndexType *)
    realloc(bfm->fre, (bfm->fresiz+FREE_BLK)*sizeof(IndexType));
  bfm->fresiz += FREE_BLK;
}

void RemoveBufferIndex(bfm, inidx)
     BufferManager * bfm;
     IndexType inidx;
{
  int blkno, idx;
  assert(bfm != NULL);
  blkno = BLOCK_NO(inidx);
  idx = INDEX_NO(inidx);
  assert(bfm->key != NULL);
  assert(bfm->key[blkno][idx] != 0);
  bfm->key[blkno][idx] = 0;		/* mark as unused */
  if (bfm->fresiz == bfm->free_cnt)
    ExpandBufferFree(bfm);
  bfm->fre[bfm->free_cnt++] = inidx;
}

void UnreferBufferIndex(bfm, idx)
     BufferManager * bfm;
     IndexType idx;
{
  if (!(DEC_BUF_REFS(bfm, idx)))
    RemoveBufferIndex(bfm, idx);
}

/* exhaustive search, expensive */
int BufferEmpty(bfm)
     BufferManager * bfm;
{
  int blkno, idx;
  for (blkno = 0; blkno < bfm->blkcnt; blkno++)
    if (bfm->key[blkno] != NULL)
      for (idx = 0; idx < bfm->siz; idx++)
	if (bfm->key[blkno][idx] != 0)
	  return 0;
  return 1;
}

/* accepts key, returns index */
IndexType FindBufferIndex(bfm, key)
     BufferManager * bfm;
     KeyType key;
     /* linear search, can't do better without more datastructure */
{ 
  int blk, idx;
  assert(key != 0);
  if (bfm == NULL)
    return 0;
  for (blk = 0; blk < bfm->blkcnt; blk++)
    if (bfm->key[blk] != NULL)
      for (idx = 0; idx < bfm->siz; idx++)
	if (bfm->key[blk][idx] == key) {
	  assert(blk+idx != 0);
	  return (MAKE_INDEX(blk,idx));
	}
  return 0;
}

BufferManager * InitBufferManager(blks, size, eltsiz)
     int blks;
     int size;
     int eltsiz;
{ 
  BufferManager * bfm = (BufferManager *) calloc(1, sizeof(BufferManager));
  bfm->eltsiz = eltsiz;
  bfm->fresiz = bfm->siz = size;
  bfm->blkcnt = bfm->blkinc = blks;
  bfm->item = (void **) calloc(blks, sizeof(void *)); /* mark unallocated */
  bfm->key = (KeyType **) calloc(blks, sizeof(KeyType *)); /* mark unallocated */
  bfm->fre = NULL;
  bfm->fresiz = 0;
  bfm->free_cnt = 0;
  return bfm;
}

/* ExpandBufferManager increases the size of the array of pointers to
blocks to the next quanta up from <size> pointers.  It does not
allocate any blocks. */

void ExpandBufferManager(bfm, size)
     BufferManager * bfm;
     int size;
{				
  int i, nnew;
  size = ((((size-1)/bfm->blkinc)+1)*bfm->blkinc);
  nnew = size - bfm->blkcnt;
  assert(nnew == bfm->blkinc);
  bfm->item = (void **) realloc(bfm->item, size*sizeof(void *));
  bfm->key = (KeyType **) realloc(bfm->key, size*sizeof(KeyType *));
  for (i = bfm->blkcnt; i < size; i++) {
    bfm->item[i] = NULL;	/* mark new entries as unallocated */
    bfm->key[i] = NULL;	/* mark new entries as unallocated */
  }
  bfm->blkcnt = size;
}

/* AddBufferBlock allocates the block whose index is bkno.  It calls
ExpandBufferManager if the item array isn't big enough, causing the
item array to expand */

void AddBufferBlock(bfm, blkno)
     BufferManager * bfm;
     int blkno;
{
  int i, offset = 0, nnew = bfm->siz; /* same size blocks */
  /* if this block is beyond existing range, expand nubmer of blocks */
  if (blkno >= bfm->blkcnt)
    ExpandBufferManager(bfm, blkno+1);
  /* allocate the blocks of items and keys */
  if (bfm->eltsiz > 0)
    bfm->item[blkno] = (void *) calloc(nnew, bfm->eltsiz); /* mark unused */
  else
    bfm->item[blkno] = NULL;
  bfm->key[blkno] = (KeyType *) calloc(nnew, sizeof(KeyType));/* mark unused */
  if (blkno == 0) {		/* don't use item zero */
    nnew--;
    offset = 1;
  }
  if (nnew+bfm->free_cnt > bfm->fresiz) {
    /* not enought room in free list */
    IndexType * tfre = bfm->fre;
    bfm->fre = (IndexType *) malloc((nnew+bfm->free_cnt)*sizeof(IndexType));
    if (tfre != NULL) {
      bcopy(tfre, bfm->fre+nnew, bfm->free_cnt*sizeof(IndexType));
      free(tfre);
    }
    bfm->fresiz = nnew+bfm->free_cnt;
  } else {
    /* copy free indices up nnew places toward top of free list */
    int i, j;
    for (i = bfm->free_cnt-1, j = i+nnew; i >= 0; i--, j--)
      bfm->fre[j] = bfm->fre[i];
    assert(j == nnew-1);
  }
  bfm->free_cnt += nnew;

  /* put all the new items in the free list before whatever is already there */
  /* important to assign free list in descending order */
  for (i = 0; i < nnew; i++)
    bfm->fre[i] = MAKE_INDEX(blkno,(offset+nnew-i-1));
}

/* accepts key, returns index */
IndexType EnterBufferIndex(bfm, key)
     BufferManager * bfm;
     KeyType key;
{ 
  IndexType res;
  int blkno, idx;
  while (bfm->free_cnt == 0) {	/* two iters needed if blksiz is 1 */
    for (blkno = 0; blkno < bfm->blkcnt; blkno++)
      if (bfm->key[blkno] == NULL)
	break;
    AddBufferBlock(bfm, blkno);
  }
  assert(bfm->free_cnt > 0);
  res = bfm->fre[--(bfm->free_cnt)];
  blkno = BLOCK_NO(res);
  idx = INDEX_NO(res);
  assert(bfm->key[blkno][idx] == 0);
  bfm->key[blkno][idx] = key;
  return res;
}

void CreateBufferIndex(pbfm, inidx, blkcnt, blksiz, eltsiz, overwrite)
     BufferManager ** pbfm;
     IndexType inidx;
     int blkcnt;
     int blksiz;
     int eltsiz;
     int overwrite;
{ 
  int res, blkno = BLOCK_NO(inidx), idx = INDEX_NO(inidx);
  BufferManager * bfm = *pbfm;
  
  if (bfm == NULL)
    bfm = *pbfm = InitBufferManager(blkcnt, blksiz, eltsiz);
  /* has the pointer to the block been allocated? */
  if (bfm->blkcnt <= blkno)
    AddBufferBlock(bfm, blkno);
  /* has the block been allocated ? */
  if (bfm->key[blkno] == NULL)
    AddBufferBlock(bfm, blkno);
  if (!overwrite)
    { assert(bfm->key[blkno][idx] == 0); }
  bfm->key[blkno][idx] = (KeyType) idx;	/* what else to put here? */
}

/* Return a free index into the BufferManager, don't care about key */
int MakeBufferIndex(pbfm, blkcnt, blksiz, eltsiz)
     BufferManager ** pbfm;
     int blkcnt;
     int blksiz;
     int eltsiz;
{
  int res, i;
  BufferManager * bfm = *pbfm;
  /* if this is the first time, initialize the buffer manager */
  if (bfm == NULL)
    bfm = *pbfm = InitBufferManager(blkcnt, blksiz, eltsiz);
  else
    /* if there is no more space, make the buffer bigger */
    if (bfm->free_cnt == 0)
      ExpandBufferManager(bfm, bfm->blkcnt+blkcnt);
  /* enter key in buffer and return index */
  return(EnterBufferIndex(bfm, 1)); /* give it a non-zero key */
}  

/* Return the index into the BufferManager for the given key */
IndexType GetBufferIndex(pbfm, key, blkcnt, blksiz, eltsiz, newidx)
     BufferManager ** pbfm;
     KeyType key;
     int blkcnt;
     int blksiz;
     int eltsiz;
     int * newidx;
{
  int i;
  IndexType res;
  BufferManager * bfm = *pbfm;
  *newidx = 1;			/* assume new */
  /* if this is the first time, initialize the buffer manager */
  if (bfm == NULL)
    bfm = *pbfm = InitBufferManager(blkcnt, blksiz, eltsiz);
  else
    /* is key already in the buffer? */
    if ((res = FindBufferIndex(bfm, key)) > 0) {
      *newidx = 0;		/* not new */
      return res;
    } else {
      /* if there is no more space, make the buffer bigger */
      if (bfm->free_cnt == 0)
	ExpandBufferManager(bfm, bfm->blkcnt+blkcnt);
    }
  /* enter key in buffer and return index */
  return(EnterBufferIndex(bfm, key));
}  

/* accepts key, returns index */
IndexType GetRefdBufferIndex(pbfm, key, blkcnt, blksiz, eltsiz, newidx)
     BufferManager ** pbfm;
     KeyType key;
     int blkcnt;
     int blksiz;
     int eltsiz;
     int * newidx;
{
  IndexType idx;
  assert(eltsiz >= sizeof(RefType));
  idx =  GetBufferIndex(pbfm, key, blkcnt, blksiz, eltsiz, newidx);
  if (*newidx)
    InitReferBufferIndex(*pbfm, idx);
  ReferBufferIndex(*pbfm, idx);
  assert(idx != 0);
  return idx;
}

/* Vector items consist of a byte string.  The first sizeof(unsigned short)
   bytes indicate the number (siz) of structs of size nbytes in the vector.
   The remaining siz * nbytes bytes are the structs.
*/

void AddStructToVectorItem(bfm, inidx, ptr, nbytes)
     BufferManager * bfm;
     IndexType inidx;
     void * ptr;		/* pointer to struct */
     int nbytes;		/* size of struct */
{
  void * mbuf, ** pmbuf;
  int bk = BLOCK_NO(inidx), idx = INDEX_NO(inidx), i;
  unsigned short * psiz;	/* pointer to num structs in vector */
  assert (bfm != NULL);
  assert(bfm->item != NULL);
  assert(bfm->item[bk] != NULL);
  /* if the list of message numbers is empty, allocate it */
  pmbuf = &(((void **) (bfm->item[bk]))[idx]);
  if (*pmbuf == NULL) {
    *pmbuf = (void *) calloc(1, nbytes+sizeof(unsigned short));
  } else {			/* reallocate it */
    psiz = (unsigned short *) (*pmbuf);
    *pmbuf = (void *) realloc(*pmbuf,
			      ((*psiz+1)*nbytes) + sizeof(unsigned short));
  }
  /* store the data at the end of the list and increment the count */
  psiz = (unsigned short *) (*pmbuf);
  assert( *psiz < SHRT_MAX ); /* don't overflow counter */
  bcopy(ptr, (((char *) (*pmbuf)) + (*psiz)*nbytes) + sizeof(unsigned short),
	nbytes);
  (*psiz)++;
}

/* StructInVectorItem looks up a vector stored in the item specified
   by bfm and inidx.  It searches the vector for an entry that is
   byte-equivalent to the nbytes stored at *ptr
*/

int StructInVectorItem(bfm, inidx, ptr, nbytes)
     BufferManager * bfm;	/* the BufferManager */
     IndexType inidx;		/* the index of the vector in bfm */
     void * ptr;		/* pointer to the sought byte values */
     int nbytes;		/* number of bytes to match */
{
  void * mbuf, ** pmbuf;
  /* get the block and index into the block */
  int bk = BLOCK_NO(inidx), idx = INDEX_NO(inidx);
  assert (bfm != NULL);
  assert(bfm->item != NULL);
  assert(bfm->item[bk] != NULL);
  /* get the pointer to the vector */
  pmbuf = &(((void **) (bfm->item[bk]))[idx]);
  /* if the vector is empty, fail */
  if (*pmbuf != NULL) {
    /* get the number of entries in the vector */
    unsigned short i = *((unsigned short *) (*pmbuf));
    /* loop through the vector looking for a byte-match with ptr,nbytes */
    for (; i > 0; i--)
      if (!memcmp((char *) ptr, (((char *) (*pmbuf)) + ((i-1) *nbytes)) +
		  sizeof(unsigned short), nbytes))
	/* found it, succeed */
	return 1;
  }
  /* if vector empty or bytes not found in vector, fail */
  return 0;
}

void SetBufferIndexItem(bfm, inidx, pval)
     BufferManager * bfm;
     IndexType inidx;
     void * pval;
{
  int blk = BLOCK_NO(inidx);
  int idx = INDEX_NO(inidx);
  bcopy(pval, (((char *) (bfm->item[blk])) + (idx*bfm->eltsiz)), bfm->eltsiz);
}

void * GetBufferIndexItem(bfm, inidx)
     BufferManager * bfm;
     IndexType inidx;
{
  int blk = BLOCK_NO(inidx);
  int idx = INDEX_NO(inidx);
  return((void *) (((char *) (bfm->item[blk])) + (idx*bfm->eltsiz)));
}

int CheckValidIndex(bfm, inidx)
     BufferManager * bfm;
     IndexType inidx;
{
  int blk, idx;
  assert(bfm != NULL);
  assert(inidx != 0);
  blk = BLOCK_NO(inidx);
  idx = INDEX_NO(inidx);
  if (blk < 0 || blk > bfm->blkcnt || bfm->key[blk] == NULL ||
      bfm->key[blk][idx] == 0)
    return 0;
  return 1;
}


void InitMsgStructLoop(bfm, idx)
     BufferManager * bfm;
     unsigned int idx;
{
  int blk, ind;
  unsigned short * pmsgcnt;
  assert((idx) != 0); 
  blk = (idx) >> OUT_ACTIVE_BLK_BITS; 
  ind = (idx) - (blk << OUT_ACTIVE_BLK_BITS); 
  pmsgcnt = ((unsigned short **) (bfm->item[blk]))[ind]; 
  /* number of structs in vector */ 
  structcnt = *((unsigned short *) (pmsgcnt++));
  itermsg = (Msg**) pmsgcnt;
}

int IterMsgStructLoop(pmsg )
     Msg ** pmsg;
{
  while (structcnt-- > 0)
    {
      bcopy(itermsg,pmsg, sizeof(Msg *));
      itermsg++;
      return 1;
    }
  return 0;
}

void InitIntStructLoop(bfm, idx)
     BufferManager * bfm;
     unsigned int idx;
{
  int blk, ind;
  unsigned short * pmsgcnt;
  assert((idx) != 0); 
  blk = BLOCK_NO(idx);
  ind = INDEX_NO(idx);
  pmsgcnt = ((unsigned short **) (bfm->item[blk]))[ind]; 
  /* number of structs in vector */ 
  structcnt = *((unsigned short *) (pmsgcnt++));
  iterint = (unsigned int*) pmsgcnt;
}

int IterIntStructLoop(pint )
     unsigned int * pint;
{
  while (structcnt-- > 0)
    {
      bcopy(iterint,pint, sizeof(unsigned int));
      iterint++;
      return 1;
    }
  return 0;
}

