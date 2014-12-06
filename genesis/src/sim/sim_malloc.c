static char rcsid[] = "$Id: sim_malloc.c,v 1.3 2005/06/27 19:01:07 svitak Exp $";

/*
** $Log: sim_malloc.c,v $
** Revision 1.3  2005/06/27 19:01:07  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.2  2005/06/20 19:31:23  svitak
** Removed malloc.h include and added stdlib.h where necessary. On many
** systems (e.g. Darwin), stdlib must be preceeded by stdio so stdio is
** always included (sorry about the effect on compile times).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.10  2002/01/15 21:36:17  mvanier
** Fixed a nasty bug that was causing the piriform simulation to crash
** when "deleteall -force" was invoked.
**
** Revision 1.9  2001/12/28 20:15:06  mhucka
** Fixed serious bug in sfreeall introduced during a previous mod.
**
** Revision 1.8  2001/06/29 21:28:36  mhucka
** Fixed pointer arithmetic bug I introduced in an earlier mod.
**
** Revision 1.7  2001/04/25 17:16:05  mhucka
** 1) Attempted to improve 64-bit portability using SGI IRIX 64-bit as an
**    example.
** 2) Cleaned up some silliness in a few places in the code.
**
** Revision 1.5  2000/07/12 06:13:23  mhucka
** Added #include of string.h, strings.h and malloc.h.
**
** Revision 1.4  1997/07/24 00:26:43  dhb
** Change from PSC:
**   Initialize size variable in smalloc_used()
**
** Revision 1.3  1993/10/15 23:12:13  dhb
** Fixed free of unallocated memory and reference to just freed memory
** in sfreeall().
**
 * Revision 1.2  1993/07/21  21:31:57  dhb
 * fixed rcsid variable type
 *
 * Revision 1.1  1992/10/27  20:16:27  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
/* mds3 changes */
#include "shell_func_ext.h"
#include "system_deps.h"

/*
** this blocksize is important in that it avoids a nasty
** bug in the DEC malloc routine which wastes large amounts
** of memory when allocating blocks smaller than this
*/
#define BLOCKSIZE 256000
struct mem_chunk {
    struct mem_chunk *adr;
    struct mem_chunk *next;
};

/*
** start_chunk 
**	points to the beginning of the memory allocation table.
**	This table consists of a linked list of table entries. Each entry
**	points to a block of memory given by BLOCKSIZE.
** current_chunk 
**	points to the beginning of the current allocation table entry.
** current_ptr 
**	points to the beginning of the current memory block being allocated.
** current_offset
**	gives the offset of the next allocatable memory location in the current 
**	memory block.
*/
static struct mem_chunk  start_chunk   = { NULL, NULL };
static struct mem_chunk *current_chunk = &start_chunk;
static struct mem_chunk *current_ptr   = NULL;
static ptrdiff_t  current_offset = 0;
static int        PGSIZE;

/*
** allocate a new memory allocation table
*/
void init_smalloc()
{
#ifndef i860
#ifdef __hpux
    PGSIZE = NBPG;
#elif defined(CRAY)
    PGSIZE = NBPC;
#elif defined(Solaris)
    PGSIZE = sysconf(_SC_PAGESIZE);
#else
    PGSIZE = getpagesize();
#endif
#endif

    current_chunk       = &start_chunk;
    current_ptr         = current_chunk->adr 
	= (struct mem_chunk *)malloc(BLOCKSIZE);
    current_chunk->next = NULL;
    current_offset      = 0;
}

/*
** Allocate a block of memory. This memory can only be freed
** using the sfree_all function which frees all blocks allocated
** using the smalloc function.
** This malloc has minimal housekeeping overhead and is therefore
** quite efficient when dealing with once-only allocations
** particularly of many small blocks.
*/

void *smalloc(size)
  size_t size;
{
    void *ptr;

#if defined(mips) || defined(sun4) || defined(Solaris) || defined(hpux) || defined(decalpha) || defined(paragon) || defined(GenericMachine)
    /*
    ** align the current_offset to word boundaries
    */

    current_offset += WORDSIZE 
	- ((ptrdiff_t)((char *)current_ptr + current_offset) % WORDSIZE);

    /*
    ** align the current_offset to page boundaries
    */

    if ((ptrdiff_t)((char *)current_ptr + current_offset) % PGSIZE + size > PGSIZE) 
    {
	current_offset += PGSIZE 
	    - ((ptrdiff_t)((char *)current_ptr + current_offset) % PGSIZE);
    }
#endif

    /*
    ** check for block overflow
    */

    if (((char *)current_ptr + current_offset + size)
	>= ((char *)current_ptr + (size_t)BLOCKSIZE)) 
    {
	/*
	** this chunk is full so allocate another
	*/
	current_chunk->next 
	    = (struct mem_chunk *)malloc(sizeof(struct mem_chunk));
	current_chunk = current_chunk->next;
	current_chunk->next = NULL;
	current_ptr = current_chunk->adr 
	    = (struct mem_chunk *)malloc((size_t)BLOCKSIZE);
	current_offset = 0;
    }

    /*
    ** return the memory
    */

    ptr = (char *)current_ptr + current_offset;
    current_offset += size;
    return ptr;
}

/*
** Placeholder function for individual block freeing which does nothing since
** this would require additional housekeeping overhead which is best left to
** malloc. 
*/

void sfree(ptr)
  char *ptr;
{
}

/*
** allocate and zero a block
*/
void *scalloc(n, size)
  int n;
  int size;
{
    void *ptr;
    
    ptr = smalloc(((size_t)n) * size);
    BZERO(ptr, ((size_t)n) * size);
    return ptr;
}

/*
** free all memory in the allocation table
*/
void sfreeall()
{
    struct mem_chunk *ptr, *old_ptr;

    /*
    ** Free all the allocated blocks, with the exception of start_chunk
    ** because it's a static item.
    */

    ptr = &start_chunk;

    if (ptr->adr != NULL)
	free(ptr->adr);

    ptr = ptr->next;
    while (ptr != NULL)
    {
	free(ptr->adr);
	old_ptr = ptr;
	ptr = ptr->next;
	free(old_ptr);
    }

    /*
    ** reinitialize the allocator
    */

    init_smalloc();
}


#undef BLOCKSIZE
