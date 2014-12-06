/* $Id */
/*
 * $Log: xo_hash.c,v $
 * Revision 1.1  2005/06/17 21:23:52  svitak
 * This file was relocated from a directory with the same name minus the
 * leading underscore. This was done to allow comiling on case-insensitive
 * file systems. Makefile was changed to reflect the relocations.
 *
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.6  2001/04/25 17:11:02  mhucka
 * 1) Attempted to improve 64-bit portability using SGI IRIX 64-bit as an
 *    example.
 * 2) Cleaned up some silliness in a few places in the code.
 *
 * Revision 1.5  2001/04/18 22:39:35  mhucka
 * Miscellaneous portability fixes, mainly for SGI IRIX.
 *
 * Revision 1.4  2000/06/12 04:28:22  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.3  1994/11/07 22:27:47  bhalla
 * Added in another error check for xo_hash_find
 *
 * Revision 1.2  1994/02/02  20:26:05  bhalla
 * *** empty log message ***
 *
 * Revision 1.2  1994/02/02  20:26:05  bhalla
 * *** empty log message ***
 * */

/* Elmhash.c : this is very similar to Matt's version of the hash
** stuff, only I have munged it to handle pointers (normally used for
** elements) rather than character strings.
** Upi Bhalla, Mount Sinai, June 1993
*/

#include <stdio.h>
#include <math.h>
#include "hash.h"
#include "system_deps.h"

/*
** put in wraparound during the hash entry and search
*/

ADDR xo_hash_function(key,table)
char *key;
HASH *table;
{
	return((((ADDR)key & 0xff) | ((ADDR)key >> 8)) % table->size);
}

/* this one does not need to change from Matt's version
HASH *hash_create(size)
int size;
{
HASH *hash_table;

	hash_table = (HASH *)calloc(size,sizeof(HASH));
	hash_table->size = size;
	hash_table->entry = (ENTRY *)calloc(size+1,sizeof(ENTRY));
	return(hash_table);
}
*/

ENTRY *xo_hash_find(key,table)
char *key;
HASH *table;
{
ENTRY *end_ptr;
ENTRY *hash_ptr;

	if(key == NULL || table == NULL || table->size <= 0) return(NULL);
	hash_ptr = table->entry + xo_hash_function(key,table);
	end_ptr = table->entry + table->size;
	while(hash_ptr->key){
		if(key == hash_ptr->key){
			return(hash_ptr);
		}
		hash_ptr++;
		if(hash_ptr >= end_ptr || hash_ptr == table->entry) {
			return(NULL);
		}
	}
	return(NULL);
}

ENTRY *xo_hash_enter(item,table)
ENTRY *item;
HASH *table;
{
ENTRY *end_ptr;
ENTRY *hash_ptr;
char *key;

	if(item == NULL || table == NULL || table->size <= 0) return(NULL);
	/*
	** look for the next available spot
	*/
	key = item->key;
	hash_ptr = table->entry + xo_hash_function(key,table);
	end_ptr = table->entry + table->size;
	while(hash_ptr->key){
		/*
		** copy over existing keys if they exist
		*/
		if(key==hash_ptr->key){
			break;
		}
		hash_ptr++;
		if(hash_ptr >= end_ptr || hash_ptr == table->entry) {
			return(NULL);
		}
	}
	/*
	** leave the last entry empty to identify the end of the table
	*/
	BCOPY(item,hash_ptr,sizeof(ENTRY));
	return(hash_ptr);
}

