static char rcsid[] = "$Id: sim_hash.c,v 1.2 2005/06/27 19:01:07 svitak Exp $";

/*
** $Log: sim_hash.c,v $
** Revision 1.2  2005/06/27 19:01:07  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.5  2001/04/25 17:17:01  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.4  1996/05/16 22:11:26  dhb
** Added element path specific hash function (from Greg Hood <ghood@psc.edu>).
** Added a command function to show hash table status for debugging.
**
 * Revision 1.3  1996/05/09  18:02:26  dhb
 * Element hash table.  Initial implementaiton.
 *
 * Revision 1.2  1993/07/21  21:31:57  dhb
 * fixed rcsid variable type
 *
 * Revision 1.1  1992/10/27  20:08:06  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include "shell_func_ext.h"
#include "sim_ext.h"
#include "hash.h"

/*
** Element hash table is going to be a bit different from 
** other GENESIS hash tables.  The entries will be linked
** for bucket overflows.  The key value is the element path
** which can be quickly computed from the element pointer.
** This allows us to omit storing the key explicitly.  This
** also turns out nicely in that we can use the key pointer
** for our link to the next entry in a bucket and the bucket
** entries are the same size as normal GENESIS hash entries.
*/

/* !!! NOTE: sizeof(ENTRY) must equal sizeof(ELMENTRY) !!! */
typedef struct _elmentry
  {
    struct _elmentry*	next;
    Element*		data;
  } ELMENTRY;

HASH *element_hash_table;


/*
** need to replace GENESIS hash_function() with something more
** suited to maps of elements.  hash_function() hashes large
** maps into very few hash entries.
*/

int Ehash_function(key,table)
char *key;
HASH *table;
{
unsigned val = 0;

	while(*key != '\0'){
	    val = 137*val + ((int) *key++);
#if 0
int factor = 1;
		if (isdigit(*key)){
		    int dig = *key - '0';
		    val += (dig | dig << 8)*factor;
		    if (factor >= 10000)
			factor = 1;
		    else
			factor *= 10;
		}

		val += (*key | *(key+1) << 8);
		key++;
#endif
	}
	return(val % table->size);
}



/*
** Hash table utilities: element_hash_table
**
** The table grows (roughly 10x, always to a prime size) whenever the
** average chain length would exceed ELEMENT_HASH_LOAD_FACTOR.  Without
** this, a fixed-size table degrades toward O(n) per lookup/insert once
** the element count much exceeds the initial bucket count, which turns
** every O(1)-amortized Attach()/create into an O(n) operation again --
** i.e. building a large flat population of elements (e.g. many neurons
** under one parent) would still cost O(n^2) overall, just with a smaller
** constant than the linear sibling-list scan this table was added to
** replace.
*/

#define ELEMENT_HASH_LOAD_FACTOR 4

static int element_hash_count = 0;
static int element_hash_rehashing = 0;

static int NextHashSize(old)
int old;
{
static int primes[] = {10007, 100003, 1000003, 10000019, 100000007};
int i;

    for (i = 0; i < sizeof(primes)/sizeof(primes[0]); i++) {
	if (primes[i] > old)
	    return primes[i];
    }
    /* fallback for models larger than anything realistically hashed here */
    return old * 2 + 1;
}

static void ElementHashRehash()
{
HASH		*old_table;
int		old_size;
int		i;
ELMENTRY	*item, *next;
int		is_head;

    old_table = element_hash_table;
    old_size = old_table->size;

    element_hash_table = hash_create(NextHashSize(old_size));
    element_hash_rehashing = 1;

    for (i = 0; i < old_size; i++) {
	item = (ELMENTRY*) old_table->entry + i;
	is_head = 1;
	while (item != NULL) {
	    if (item->data != NULL)
		ElementHashPut(item->data);
	    next = item->next;
	    if (!is_head)
		free(item);
	    is_head = 0;
	    item = next;
	}
    }

    element_hash_rehashing = 0;
    free(old_table->entry);
    free(old_table);
}

void ElementHashInit()
{
HASH *hash_create();

	/*
	* create the table
	*/
	element_hash_table = hash_create(10007);
	element_hash_count = 0;
}

int ElementHashPut(elm)
Element *elm;
{
ELMENTRY	*item;
int		hashIndex;

    if (!element_hash_rehashing &&
    element_hash_count >= element_hash_table->size * ELEMENT_HASH_LOAD_FACTOR){
	ElementHashRehash();
    }

    hashIndex = Ehash_function(Pathname(elm), element_hash_table);
    item = (ELMENTRY*) element_hash_table->entry + hashIndex;
    if (item->data == NULL)
	item->data = elm;
    else
      {
	ELMENTRY*	new;

	new = (ELMENTRY*) malloc(sizeof(ELMENTRY));
	if (new == NULL) {
	    Error();
	    printf("hash table full\n");
	    return(0);
	}

	new->data = elm;
	new->next = item->next;
	item->next = new;
      }

    if (!element_hash_rehashing)
	element_hash_count++;

    return(1);
}


int ElementHashPutTree(elmtree)

Element*	elmtree;

{
	Element*	elm;

	if (elmtree == NULL)
	    return 0;

	if (!ElementHashPut(elmtree))
	    return 0;

	for (elm = elmtree->child; elm != NULL; elm = elm->next)
	    if (!ElementHashPutTree(elm))
		return 0;

	return 1;
}


void ElementHashRemove(elm)

Element*	elm;

{
    ELMENTRY*	item;
    int		hashIndex;
    char	key[300];

    strcpy(key, Pathname(elm));
    hashIndex = Ehash_function(key, element_hash_table);
    item = (ELMENTRY*) element_hash_table->entry + hashIndex;
    if (item->data != NULL)
      {
	ELMENTRY* prev = NULL;
	while (item != NULL)
	  {
	    if (strcmp(Pathname(item->data), key) == 0)
	      {
		if (prev == NULL)
		    if (item->next == NULL)
			item->data = NULL;
		    else {
			ELMENTRY* old = item->next;

			*item = *old;
			free(old);
		    }
		else {
		    prev->next = item->next;
		    free(item);
		}

		element_hash_count--;
		break;
	      }

	    prev = item;
	    item = item->next;
	  }
      }
}


void ElementHashRemoveTree(elmtree)

Element*	elmtree;

{
	Element*	elm;

	if (elmtree == NULL)
	    return;

	ElementHashRemove(elmtree);
	for (elm = elmtree->child; elm != NULL; elm = elm->next)
	    ElementHashRemoveTree(elm);
}

Element *ElementHashFind(key)
char	*key;
{
ELMENTRY	*item;
int		hashIndex;

    /*
    ** get the element from the table
    */
    hashIndex = Ehash_function(key, element_hash_table);
    item = (ELMENTRY*) element_hash_table->entry + hashIndex;
    if (item->data != NULL)
	while (item != NULL)
	  {
	    if (strcmp(Pathname(item->data), key) == 0)
		return item->data;

	    item = item->next;
	  }

    return(NULL);
}


void hashelminfo(path)

char*	path;

{
	int		hashno;
	ELMENTRY*	entry;

	hashno = Ehash_function(path, element_hash_table);
	entry = (ELMENTRY*) element_hash_table->entry + hashno;

	printf("%s hashes to %d ", path, hashno);
	if (entry->data != NULL)
	  {
	    ELMENTRY*	chain;
	    int		nElmHashed;

	    nElmHashed = 1;
	    for (chain = entry->next; chain != NULL; chain = chain->next)
		nElmHashed++;

	    printf("which has %d elements hashed\n", nElmHashed);
	  }
	else
	    printf("which is empty\n");
}
void do_hashinfo(argc, argv)

int	argc;
char*	argv[];

{
	ELMENTRY*	entry;
	int		maxChained;
	int		totChained;
	int		nElmHashed;
	int		nBucketsFilled;
	int		nChained;
	int	i;

	if (argc > 1)
	  {
	    for (i = 1; i < argc; i++)
		hashelminfo(argv[i]);

	    return;
	  }

	maxChained = 0;
	totChained = 0;
	nElmHashed = 0;
	nBucketsFilled = 0;
	entry = (ELMENTRY*) element_hash_table->entry;
	for (i = 0; i < element_hash_table->size; i++)
	  {
	    if (entry->data != NULL)
	      {
		ELMENTRY*	chain;

		nElmHashed++;
		nBucketsFilled++;
		nChained = 0;
		for (chain = entry->next; chain != NULL; chain = chain->next)
		  {
		    nElmHashed++;
		    nChained++;
		  }

		totChained += nChained;
		if (nChained > maxChained)
		    maxChained = nChained;
	      }
	    entry++;
	  }

	if (nBucketsFilled == 0)
	    printf("Element hash table is empty\n");
	else
	  {
#if LONGWORDS
	    printf("%d out of %ld hash buckets filled (%ld%%); %d elements hashed\n",
#else
	    printf("%d out of %d hash buckets filled (%d%%); %d elements hashed\n",
#endif
		    nBucketsFilled, element_hash_table->size,
		    100*nBucketsFilled / element_hash_table->size,
		    nElmHashed);
	    printf("Chaining: longest %d  average %g\n",
		    maxChained, (double)totChained/(double)nBucketsFilled);
	  }
}
