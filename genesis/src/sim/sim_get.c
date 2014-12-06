static char rcsid[] = "$Id: sim_get.c,v 1.2 2005/06/27 19:01:07 svitak Exp $";

/*
** $Log: sim_get.c,v $
** Revision 1.2  2005/06/27 19:01:07  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.11  2001/04/25 17:17:01  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.10  1997/07/18 02:58:25  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.9  1996/09/13 01:03:00  dhb
** Added valid_index handling to GetTreeCount().
**
 * Revision 1.8  1996/05/16  22:08:30  dhb
 * Support for element path hash table.
 *
 * Revision 1.7  1994/09/16  20:44:34  dhb
 * Removed connection related code.  This functionality will be provided
 * in connection specific commands in the connection compatability library.
 *
 * Revision 1.6  1994/08/31  02:39:23  dhb
 * Fixed problem in ElmFieldValue() which didn't check protection of
 * fields with complex addressing properly (e.g. getfield X->table[0])
 * would allow user to get the value even if X is hidden.
 *
 * Revision 1.5  1994/04/16  20:40:54  dhb
 * Made path argument to getfield command optional.  If omitted
 * getfield uses the current working element.  This is more
 * consistent with how setfield works.
 *
 * Revision 1.4  1993/03/18  17:15:55  dhb
 * Element field protection.
 *
 * Revision 1.3  1993/03/10  23:11:35  dhb
 * Extended element fields
 *
 * Revision 1.2  1993/02/23  19:10:32  dhb
 * 1.4 to 2.0 command argument changes.
 *
 * 	do_getfield changed to use GENESIS getopt routines.
 *
 * Revision 1.1  1992/10/27  20:06:52  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include "shell_func_ext.h"
#include "sim_ext.h"

Element *GetElement(pathname)
char *pathname;
{
Element 	*element;
char 		*tree_ptr;
char 		*name;
int		count;
int		valid_index;
char 		*GetTreeName();
char 		*AdvancePathname();
extern Element*	ElementHashFind();

    /* Try to find element path in element hash table */
    if (*pathname == '/') {
	element = ElementHashFind(pathname);
    } else {
	char	fullpath[300];

	sprintf(fullpath, "%s/%s", Pathname(WorkingElement()), pathname);
	element = ElementHashFind(fullpath);
    }

    if (element != NULL)
	return element;

    /*
    ** start at the root?
    */
    if(pathname[0] == '/') {
	element = RootElement();
	tree_ptr = pathname+1;
    } else {
	element = WorkingElement();
	tree_ptr = pathname;
    }

    while(*tree_ptr != '\0' && element != NULL){
	name = GetTreeName(tree_ptr);
	count = GetTreeCount(tree_ptr, &valid_index);
	if (!valid_index){
	    return(NULL);
	}
	if(strcmp(name,"..") == 0){
	    /*
	    ** parent element
	    */
	    element = element->parent;
	} else 
	if(strcmp(name,".") == 0){
	    /*
	    ** current element
	    */
	    element = element;
	} else 
	if(strcmp(name,"^") == 0){
	    /*
	    ** recent element
	    */
	    element = RecentElement();
	} else {
	    element = (Element *)GetChildElement(element,name,count);
	}
	tree_ptr = AdvancePathname(tree_ptr);
    }
    return(element);
}

char *do_getfield(argc,argv)
     int argc;
     char **argv;
{
  char 		*pathname;
  char 		*value;
  char 		*field;
  Element 	*element;

  initopt(argc, argv, "[path] field");
  if (G_getopt(argc, argv) != 0 || optargc > 3)
    {
      printoptusage(argc, argv);
      return(NULL);
    }

    if (optargc == 3)
      {
	pathname = optargv[1];
	field = optargv[2];
      }
    else /* optargc == 2 */
      {
	pathname = ".";
	field = optargv[1];
      }

    if((element = GetElement(pathname)) ==NULL){
	Error();
	printf("could not find '%s'\n",pathname);
	return(NULL);
    }

	value = ElmFieldValue(element,field);
	if(value == NULL) {
	    Error();
	    printf("could not get the value for field '%s'\n",field);
	    return(NULL);
	}

    return(CopyString(value));
}

char* ElmFieldValue(element, fieldname)

Element*	element;
char*		fieldname;

{	/* ElmFieldValue --- Return value of an element field */

	char*	value;

	if (GetFieldListProt(element->object, FieldNameProper(fieldname)) == FIELD_HIDDEN)
	    return NULL;

	value = FieldValue(element,element->object,fieldname);
	if (value == NULL)
	    return GetExtField(element, fieldname);

	return value;

}	/* ElmFieldValue */
