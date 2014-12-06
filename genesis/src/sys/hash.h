/*
** $Id: hash.h,v 1.1.1.1 2005/06/14 04:38:34 svitak Exp $
** $Log: hash.h,v $
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.2  2001/04/25 17:17:04  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.1  1992/12/11 19:05:39  dhb
** Initial revision
**
*/

#include "system_deps.h"

typedef struct entry_type { 
	char *key; 
	void *data; 
} ENTRY;

typedef struct hash_type {
	WORD   size;
	ENTRY *entry;
} HASH;

WORD   hash_function();
HASH  *hash_create();
ENTRY *hash_find();
ENTRY *hash_enter();
