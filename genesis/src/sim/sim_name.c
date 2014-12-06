static char rcsid[] = "$Id: sim_name.c,v 1.2 2005/06/27 19:01:08 svitak Exp $";

/*
** $Log: sim_name.c,v $
** Revision 1.2  2005/06/27 19:01:08  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.4  2001/04/25 17:17:02  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.3  1994/10/18 15:40:19  dhb
** Name() now frees previous element name.
**
 * Revision 1.2  1993/07/21  21:32:47  dhb
 * fixed rcsid variable type
 *
 * Revision 1.1  1992/10/27  20:20:13  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include "shell_func_ext.h"
#include "sim_ext.h"
#include "hash.h"

static HASH *name_hash_table;

int Name(element,name)
Element	*element;
char 		*name;
{
    if(element == NULL || name == NULL ||strlen(name) == 0) {
	return(0);
    }
    /*
    ** assign the name 
    */
    if (element->name != NULL)
	free(element->name);
    element->name = (char *)malloc(strlen(name)+1);
    strcpy(element->name,name);
    return(1);
}

/*
** Hash table utilities
*/
void NameHashInit()
{
HASH *hash_create();

    /*
    ** create the table
    */
    name_hash_table = hash_create(2000);
}

int AddName(name)
char *name;
{
ENTRY	item,*hash_enter();

    item.data = name;
    item.key = name;
    /*
    ** put the name into the table
    */
    if(hash_enter(&item,name_hash_table) == NULL){
	Error();
	printf("name hash table full\n");
	return(0);
    };
    return(1);
}

int FindName(key)
char	*key;
{
ENTRY	*found_item,*hash_find();

    /*
    ** get the name from the table
    */
    if((found_item = hash_find(key,name_hash_table)) != NULL){
	return(1);
    } else
	return(0);
}

