static char rcsid[] = "$Id: hash.c,v 1.2 2005/06/20 21:20:15 svitak Exp $";

/*
** $Log: hash.c,v $
** Revision 1.2  2005/06/20 21:20:15  svitak
** Fixed compiler warnings re: unused variables and functions with no return type.
** Default of int causes compiler to complain about return with no type, so void
** used as return type when none present.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.4  2001/04/25 17:09:48  mhucka
** 1) Attempted to improve 64-bit portability using SGI IRIX 64-bit as an
**    example.
** 2) Cleaned up some silliness in a few places in the code.
**
** Revision 1.2  2000/09/21 19:46:11  mhucka
** #include some files to get some function declarations.
**
** Revision 1.1  1992/12/11 19:05:29  dhb
** Initial revision
**
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <strings.h>
#include <string.h>
#include "hash.h"
#include "system_deps.h"

/*
** put in wraparound during the hash entry and search
*/
WORD hash_function(key,table)
char *key;
HASH *table;
{
WORD val = 0;

	while(*key != '\0'){
		val += (*key | *(key+1) << 8);
		key++;
	}
	return(val % table->size);
}

HASH *hash_create(size)
WORD size;
{
HASH *hash_table;

	hash_table = (HASH *)calloc(size,sizeof(HASH));
	hash_table->size = size;
	hash_table->entry = (ENTRY *)calloc(size+1,sizeof(ENTRY));
	return(hash_table);
}

ENTRY *hash_find(key,table)
char *key;
HASH *table;
{
ENTRY *end_ptr;
ENTRY *hash_ptr;

	if(key == NULL || table == NULL || table->size <= 0) return(NULL);
	hash_ptr = table->entry + hash_function(key,table);
	end_ptr = table->entry + table->size;
	while(hash_ptr->key){
		if(strcmp(key,hash_ptr->key) == 0){
			return(hash_ptr);
		}
		hash_ptr++;
		if(hash_ptr >= end_ptr || hash_ptr == table->entry) {
			return(NULL);
		}
	}
	return(NULL);
}

ENTRY *hash_enter(item,table)
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
	hash_ptr = table->entry + hash_function(key,table);
	end_ptr = table->entry + table->size;
	while(hash_ptr->key){
		/*
		** copy over existing keys if they exist
		*/
		if(strcmp(key,hash_ptr->key) == 0){
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

