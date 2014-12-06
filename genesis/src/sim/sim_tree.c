static char rcsid[] = "$Id: sim_tree.c,v 1.2 2005/06/27 19:01:12 svitak Exp $";

/*
** $Log: sim_tree.c,v $
** Revision 1.2  2005/06/27 19:01:12  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:29  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.8  2001/04/25 17:17:03  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.7  1997/07/18 02:58:25  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.6  1997/05/07 22:47:17  dhb
** Fix to GetTreeCount() to validate an element index is only
** digits.  Previously GetTreeCount would return the first index
** in an index range as found in wildcard paths.
**
** Revision 1.5  1996/09/13 01:03:00  dhb
** Added valid_index handling to GetTreeCount().
**
 * Revision 1.4  1993/12/23  02:29:47  dhb
 * Extended Objects (mostly complete)
 *
 * Revision 1.3  1993/06/29  18:53:23  dhb
 * Extended objects (incomplete)
 *
 * Revision 1.2  1993/02/15  23:04:37  dhb
 * 1.4 to 2.0 command argument changes.
 *
 * 	do_change_element (ce) and do_list_elements (le) changed to use
 * 	GENESIS getopt routines.  do_pwe (pwe) not changed.
 *
 * Revision 1.1  1992/10/27  20:35:57  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include <ctype.h>
#include "shell_func_ext.h"
#include "sim_ext.h"

int GetTreeCount(pathname, valid_index)
char *pathname;
int *valid_index;
{
int count;

    *valid_index = 1;

    /*
    ** search for the index delimiter or the end of the pathname
    */
    while(*pathname != '[' && *pathname != '/' && 
    *pathname != '\0'){
	pathname++;
    }
    /*
    ** if the delimiter is found then get the count
    */
    if(*pathname == '['){
	char* cp;

	cp = pathname+1;
	while (isdigit(*cp))
	    cp++;

	if (*cp != ']')
	  {
	    *valid_index = 0;
	    return(0);
	  }

	*valid_index = sscanf(pathname+1,"%d",&count) == 1;
	return(count);
    } else {
    /*
    ** if no count is found then return 0
    */
	return(0);
    }
}


char *GetTreeName(pathname)
char *pathname;
{
static char name[80];
char *name_ptr;

    name_ptr = name;
    while(*pathname != '[' && *pathname != '/' && *pathname != '\0'){
	*name_ptr++ = *pathname++;
    }
    *name_ptr = '\0';
    return(name);
}

char *AdvancePathname(pathname)
char *pathname;
{
    while(*pathname != '/' && *pathname != '\0'){
	pathname++;
    }
    if(*pathname == '/') pathname++;
	return(pathname);
}

void ChangeWorkingElement(pathname)
char *pathname;
{
Element *element;

    if((element = GetElement(pathname)) != NULL){
	SetWorkingElement(element);
    } else {
	Error();
	printf("cannot change to '%s' from '%s'\n",
	pathname,
	Pathname(WorkingElement()));
    }
}

void ListElements(element,recursive,showtype)
struct element_type *element;
short recursive;
short showtype;
{
struct element_type *child;
char name[100];
struct childlist_type {
    char name[80];
    int start;
    int end;
    int flags;
    short children;
    Element	*element;
    struct childlist_type *next;
} *childlist,*ptr,*newptr,*prev,*next;
char string[80];
int col;

    /*
    ** we have to do this list thing to establish groupings in
    ** the elements so we dont have to list every single one
    */
    childlist = NULL;
    col = 0;
    for(child=element->child;child;child=child->next){
	if (!VISIBLE(child))
	    continue;
	/*
	** search the child list for the name
	*/
	prev = childlist;
	for(ptr=childlist;ptr;ptr=ptr->next){
	    prev = ptr;
	    /*
	    ** if found then increment the count
	    */
	    if(strcmp(child->name,ptr->name) == 0){
		/*
		** check the secondary indicators like
		** enabled mode and children
		*/
		if(ptr->flags != child->flags || 
		ptr->element->object != child->object ||
		ptr->end+1 != child->index ||
		((child->child != NULL) != ptr->children)){
		    /*
		    ** if found then print out the current
		    ** entry 
		    */
		    if(!IsEnabled(ptr->element))
			strcpy(name,"*");
		    else
			name[0] = '\0';
		    strcat(name,ptr->name);

		    if(ptr->start != ptr->end){
			sprintf(string,"[%d-%d]",ptr->start,ptr->end);
			strcat(name,string);
		    } else {
			if(ptr->start != 0){
			    sprintf(string,"[%d]",ptr->start);
			    strcat(name,string);
			}
		    }
		    if(ptr->children)
			strcat(name,"/");
		    if(showtype){
			strcat(name," {");
			strcat(name,ptr->element->object->name);
			strcat(name,"}");
		    }
		    printf("%-40s",name);
		    if((col+1)%2 == 0) printf("\n");
		    col++;
		    /*
		    ** and set up the new one in its place
		    */
#ifdef OLD
		    ptr->end++;
#else
		    ptr->end = child->index;
#endif
		    ptr->start = ptr->end;
		    ptr->children = (child->child != NULL);
		    ptr->flags = child->flags;
		    ptr->element = child;
		} else {
#ifdef OLD
		    ptr->end++;
#else
		    ptr->end = child->index;
#endif
		}
		break;
	    }
	}
	/*
	** if not there then add to the list
	*/
	if(ptr == NULL){
	    newptr = (struct childlist_type *)
	    calloc(1,sizeof(struct childlist_type));
	    strcpy(newptr->name,child->name);
	    newptr->start = newptr->end = child->index;
	    newptr->children = (child->child != NULL);
	    newptr->flags = child->flags;
	    newptr->element = child;
	    if(childlist == NULL){
		childlist = newptr;
	    } else
	    prev->next = newptr;
	}
    }
    /*
    ** print out the remainder
    */
    if (childlist != NULL)
      {
	if (recursive > 1)
	    printf("\n%s:\n",Pathname(element));
	    
	for(ptr=childlist;ptr;ptr=next){
	    
	    if(!IsEnabled(ptr->element)){
		strcpy(name,"*");
	    } else {
		name[0] = '\0';
	    }
	    strcat(name,ptr->name);
	    if(ptr->start != ptr->end){
		sprintf(string,"[%d-%d]",ptr->start,ptr->end);
		strcat(name,string);
	    } else {
		if(ptr->start != 0){
		    sprintf(string,"[%d]",ptr->start);
		    strcat(name,string);
		}
	    }
	    if(ptr->children)
		strcat(name,"/");
	    if(showtype){
		strcat(name," {");
		strcat(name,ptr->element->object->name);
		strcat(name,"}");
	    }
	    printf("%-40s",name);

	    if((col+1)%2 == 0) printf("\n");
	    col++;
	    next = ptr->next;
	    free(ptr);
	}
	if(col%2 != 0) printf("\n");
    }
    /*
    ** if this is recursive then list the children of the children
    */
    if(recursive)
    for(child=element->child;child;child=child->next){
	if (VISIBLE(child))
	  {
	    ListElements(child,recursive+1,showtype);
	  }
    }
}

int CountChildren(blockmode,tree,class)
int		blockmode;
Element 	*tree;
int		class;
{
int 		count = 0;
short 		stk;

    if(tree == NULL) {
	return(0);
    }
    stk = PutElementStack(tree);
    while(NextElement(blockmode,class,stk)) count++;
    FreeElementStack(stk);
    return(count);
}

/*
** interpreter functions
*/
void do_change_element(argc,argv)
int argc;
char **argv;
{
    initopt(argc, argv, "path");
    if (G_getopt(argc, argv) != 0)
	printoptusage(argc, argv);
    else
	ChangeWorkingElement(optargv[1]);
}

void do_pwe(argc,argv)
int argc;
char **argv;
{
    printf("%s\n", Pathname(WorkingElement()));
}

void do_list_elements(argc,argv)
int argc;
char **argv;
{
Element 	*element;
int		status;
short 		recursive = 0;
short 		showtype = 0;

    element = WorkingElement();

    initopt(argc, argv, "[path] -recursive -types");
    while ((status = G_getopt(argc, argv)) == 1)
      {
	if(strcmp(G_optopt, "-recursive") == 0)
	    recursive = 1;
	else if(strcmp(G_optopt, "-types") == 0)
	    showtype = 1;
      }

    if (status < 0 || optargc > 2)
      {
	printoptusage(argc, argv);
	return;
      }

    if(optargc == 2)
      {
	/*
	** check for a valid element
	*/
	if((element = GetElement(optargv[1])) == NULL){
	    printf("cant find element '%s'\n",optargv[1]);
	    return;
        }
      }

    ListElements(element,recursive,showtype);
}

#ifdef LATER
void do_shift_element(argc,argv)
int argc;
char **argv;
{
Element 	*parent;
Element 	*element;
int shift;
int count;
int valid_index;
char *path;
char *name;

    if(argc < 2){
	printf("usage: %s [-]count\n",argv[0]);
	return;
    }
    shift = atoi(argv[1]);
    parent = WorkingElement()->parent;
    path = Pathname(WorkingElement());
    /*
    ** get the current index and add the shift to it
    */
    count  = GetTreeCount(path, &valid_index) + shift;
    if (!valid_index){
	Error();
	printf("missing or bad element index in shifte path '%s'\n", path);
	return;
    }
    name = GetTreeName(path);
    /*
    ** get the element with the new index
    */
    if((element = GetChildElement(parent,name,count)) != NULL){
	SetWorkingElement(element);
    } else {
	Error();
	printf("could not find element '%s[%d]' \n",
	name,count);
    }
}
#endif
