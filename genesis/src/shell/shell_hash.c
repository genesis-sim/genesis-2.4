static char rcsid[] = "$Id: shell_hash.c,v 1.5 2005/06/29 16:42:39 svitak Exp $";

/*
** $Log: shell_hash.c,v $
** Revision 1.5  2005/06/29 16:42:39  svitak
** Removed local prototype of FieldHashFind.
**
** Revision 1.4  2005/06/26 08:25:37  svitak
** Provided explicit types for untyped funtions. Fixed return statements to
** match return type.
**
** Revision 1.3  2005/06/20 19:21:19  svitak
** Corrected quote style for stdio and stdlib includes.
**
** Revision 1.2  2005/06/17 19:24:18  svitak
** Changed HASH_SIZE from 10000 to prime 10009. This should help fix issues with
** "hash table full" on several architectures, especially Cygwin and 64-bit.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.12  2001/06/29 21:25:18  mhucka
** 1) Added FieldHashFindAndCopy from Hugo C.
** 2) Fixed bug in pointer calculations on some systems; the bug
**    was introduced by one of my earlier mods.
**
** Revision 1.11  2001/04/25 17:09:59  mhucka
** 1) Attempted to improve 64-bit portability using SGI IRIX 64-bit as an
**    example.
** 2) Cleaned up some silliness in a few places in the code.
**
** Revision 1.9  2000/09/21 19:38:49  mhucka
** Added long specifiers where needed in printf format strings.
**
** Revision 1.8  2000/05/19 18:41:26  mhucka
** I don't know how things got this far, but under Solaris 2.7, there is no
** sysconf variable named "_SC_PGSIZE"; it's actually "_SC_PAGESIZE".  But why
** this seemed to work before is a total mystery to me -- I've been compiling on
** the same Solaris 2.7 system for months :-).
**
** Revision 1.7  2000/03/08 20:41:18  mhucka
** Following tip from Hugo Cornelis, renamed PAGESIZE to another name because
** PAGESIZE is defined by the system include files in some OSes like Solaris.
**
** Revision 1.6  1996/06/28 22:46:34  dhb
** Added paragon to systems requiring alignment of memory in hmalloc().
** Previous paragon port changes included this but only for machine
** type GenericMachine which was used during porting.  Using
** GenericMachine for unknown machines (i.e. MACHINE=other) is still
** a good idea as alignment of memory should be the default.
**
** Revision 1.5  1996/06/06  20:48:31  dhb
** merged in 1.3.1.1 changes.
**
** Revision 1.4  1996/05/23  23:15:45  dhb
** t3d/e port
**
** Revision 1.3.1.1  1996/06/06  20:05:49  dhb
** Paragon port.
**
** Revision 1.3  1994/04/14  16:40:57  dhb
** Merged in 1.1.1.1
**
** Revision 1.2  1993/09/17  17:02:38  dhb
** Solaris compatability.
**
** Revision 1.1.1.1  1994/04/14  16:38:20  dhb
** Alpha compatability changes from ngoddard@psc.edu.
**
** Revision 1.1  1992/12/11  19:04:30  dhb
** Initial revision
**
*/

#include <stdlib.h>
#include <stddef.h>
#include "shell_ext.h"
#if defined(__hpux) || defined(CRAY)
#include <sys/param.h>
#endif
#if defined(Solaris) || (defined irix) || defined(Linux)
#include <unistd.h>
#endif

static HASH *info_hash_table;
static HASH *func_name_hash_table;
static HASH *func_adr_hash_table;
static HASH *field_hash_table;

/*
** A prime HASH_SIZE should fix issues with keys getting hashed to a
** small subset of buckets.
*/
#define HASH_SIZE 10009

/*
** hash table allocation routines
*/

/*
** the 256000 blocksize is important in that it avoids a nasty
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
**	points to the beginning of the memory allocation table
**	this table consists of a linked list of table entries. Each entry
**	points to a block of memory given by BLOCKSIZE
** current_chunk 
**	points to the beginning of the current allocation table entry.
** current_ptr 
**	points to the beginning of the current memory block being allocated.
** current_offset
**	gives the offset of the next allocatable memory location in the current 
**	memory block.
*/
static struct mem_chunk start_chunk = { NULL,NULL};
static struct mem_chunk *current_chunk = &start_chunk;
static struct mem_chunk *current_ptr = NULL;
static ptrdiff_t  current_offset = 0;
static int        PGSIZE;

/*
** allocate a new memory allocation table
*/
void init_hmalloc()
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
    current_chunk = &start_chunk;
    current_ptr = current_chunk->adr = (struct mem_chunk*)malloc((size_t)BLOCKSIZE);
    current_chunk->next = NULL;
    current_offset = 0;
}

/*
** allocate a block of memory. This memory can only be freed
** using the hfree_all function which frees all blocks allocated
** using the hmalloc function.
** This malloc has minimal housekeeping overhead and is therefore
** quite efficient when dealing with once-only allocations
** particularly of many small blocks
*/
void *hmalloc(size)
size_t size;
{
    void *ptr;

#if defined(mips) || defined(sun4) || defined(Solaris) || defined(hpux) || defined(decalpha) || defined(paragon) || defined(GenericMachine)
    /*
    ** align the current_offset to word boundaries
    */
    current_offset += WORDSIZE - ((ptrdiff_t)((char *)current_ptr + current_offset)%WORDSIZE);
    /*
    ** align the current_offset to page boundaries
    */
    if ((ptrdiff_t)((char *)current_ptr + current_offset)%PGSIZE + size > PGSIZE) {
	current_offset += PGSIZE - ((ptrdiff_t)((char *)current_ptr + current_offset)%PGSIZE);
    }
#endif
    /*
    ** check for block overflow
    */
    if (((char *)current_ptr + current_offset + size)
	>= ((char *)current_ptr + (size_t)BLOCKSIZE)) {
	/*
	** this chunk is full so allocate another
	*/
	current_chunk->next=(struct mem_chunk*)malloc(sizeof(struct mem_chunk));
	current_chunk = current_chunk->next;
	current_chunk->next = NULL;
	current_ptr = current_chunk->adr = (struct mem_chunk *)malloc((size_t)BLOCKSIZE);
	current_offset = 0;
    }
    /*
    ** return the memory
    */
    ptr = (char *)current_ptr + current_offset;
    current_offset += size;
    return(ptr);
}

/*
** free all memory in the allocation table
*/
void hfreeall()
{
struct mem_chunk *ptr;

    /*
    ** free all the allocated blocks
    */
    for(ptr = &start_chunk;ptr;ptr=ptr->next){
	free(ptr->adr);
	free(ptr);
    }
    /*
    ** reinitialize the allocator
    */
    init_hmalloc();
}

/*
** Hash table utilities
*/
void InfoHashInit()
{
    /*
    ** prepare the hash table memory allocator
    */
    init_hmalloc();
    /*
    ** create the hash table
    */
    info_hash_table = hash_create(HASH_SIZE);
}

void InfoHashPut(info)
Info *info;
{
ENTRY	item;

	item.data = hmalloc(sizeof(Info));
	BCOPY(info,item.data,sizeof(Info));
	item.key = ((Info *)(item.data))->name;
	/*
	* put the var into the table
	*/
	if(hash_enter(&item,info_hash_table) == NULL){
		Error();
		printf("hash table full\n");
	};
}

Info *InfoHashFind(key)
char	*key;
{
ENTRY	*found_item;

	/*
	* get the info from the table
	*/
	if((found_item = hash_find(key,info_hash_table)) != NULL){
		return((Info *)found_item->data);
	} else
		return(NULL);
}

void FieldHashInit()
{
	/*
	* create the table
	*/
	field_hash_table = hash_create(HASH_SIZE);
}

/*
** makes it own copies of the key and field and places it into the
** hash table
*/
void FieldHashPut(key,fields)
char *key;
char *fields;
{
ENTRY	item;

	item.key = (char *)hmalloc(strlen(key)+1);
	strcpy(item.key,key);
	item.data = (char *)hmalloc(strlen(fields)+1);
	strcpy(item.data,fields);

	/*
	* put the fields into the table
	*/
	if(hash_enter(&item,field_hash_table) == NULL){
		Error();
		printf("hash table full\n");
	};
}

char *FieldHashFind(key)
char	*key;
{
ENTRY	*found_item;

	/*
	* get the info from the table
	*/
	if((found_item = hash_find(key,field_hash_table)) != NULL){
		return(found_item->data);
	} else
		return(NULL);
}

void FuncAdrHashInit()
{
	/*
	* create the table
	*/
	func_adr_hash_table = hash_create(HASH_SIZE);
}

char *FieldHashFindAndCopy(type)
char *type;
{

#ifdef LKJSDF_SAFE_RETURN_CODES

    /* static */ char	*pcString = NULL;
    /* static */ int iString = 0;

    char *pcOrig = FieldHashFind(type);

    int iSize;

    if (!pcOrig)
    {
	return(NULL);
    }

    iSize = strlen(pcOrig);

    if (iString < 1 + iSize)
      {
	iString = 100 + iSize;
	pcString = (char *)malloc(/* pcString, */iString);

	if (!pcString)
	  {
	    iString = 0;

	    return((char *)-1);
	  }
      }

    strcpy(pcString,pcOrig);

    return(pcString);
#else

    return(CopyString(FieldHashFind(type)));

#endif
}

void FuncAdrHashPut(func)
FuncTable *func;
{
ENTRY	item;
char adr[80];

	item.data = hmalloc(sizeof(FuncTable));
	BCOPY(func,item.data,sizeof(FuncTable));
#ifdef LONGWORDS
	sprintf(adr, "%ld", (ADDR)func->adr);
#else
	sprintf(adr, "%d", (ADDR)func->adr);
#endif
	item.key = (char *)hmalloc(strlen(adr)+1);
	strcpy(item.key,adr);
	/*
	* put the fields into the table
	*/
	if(hash_enter(&item,func_adr_hash_table) == NULL){
		Error();
		printf("hash table full\n");
	};
}

FuncTable *FuncAdrHashFind(key)
char	*key;
{
ENTRY	*found_item;

	/*
	* get the info from the table
	*/
	if((found_item = hash_find(key,func_adr_hash_table)) != NULL){
		return((FuncTable *)(found_item->data));
	} else
		return(NULL);
}

void FuncNameHashInit()
{
	/*
	* create the table
	*/
	func_name_hash_table = hash_create(HASH_SIZE);
}

void FuncNameHashPut(func)
FuncTable *func;
{
ENTRY	item;

	item.data = hmalloc(sizeof(FuncTable));
	BCOPY(func,item.data,sizeof(FuncTable));
	item.key = (char *)hmalloc(strlen(func->name)+1);
	strcpy(item.key,func->name);
	/*
	* put the fields into the table
	*/
	if(hash_enter(&item,func_name_hash_table) == NULL){
		Error();
		printf("hash table full\n");
	};
}

FuncTable *FuncNameHashFind(key)
char	*key;
{
ENTRY	*found_item;

	/*
	* get the info from the table
	*/
	if((found_item = hash_find(key,func_name_hash_table)) != NULL){
		return((FuncTable *)(found_item->data));
	} else
		return(NULL);
}

void HashFunc(name,adr,type)
char *name;
PFI adr;
char *type;
{
FuncTable func;

    func.name = CopyString(name);
    func.type = CopyString(type);
    func.adr = (PFI)adr;
    FuncNameHashPut(&func);
    FuncAdrHashPut(&func);
}


/*
** Run all the hash init functions.
*/
void HashInit()
{
    InfoHashInit();
    FieldHashInit();
    FuncNameHashInit();
    FuncAdrHashInit();
}

#undef BLOCKSIZE
