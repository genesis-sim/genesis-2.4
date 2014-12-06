static char rcsid[] = "$Id: sim_strings.c,v 1.2 2005/06/27 19:01:12 svitak Exp $";

/*
** $Log: sim_strings.c,v $
** Revision 1.2  2005/06/27 19:01:12  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.6  2001/04/25 17:17:03  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.5  2000/03/27 10:34:45  mhucka
** Commented out statusline functionality, because it caused GENESIS to
** print garbage control characters upon exit, screwing up the user's
** terminal.  Also added return type declaractions for various things.
**
** Revision 1.4  1998/06/30 20:11:24  dhb
** Changes to Pathname() to allow more overlapping use of the
** returned pathnames.  Can now have PATHNAMES_BUF_SIZE characters
** of Pathname() strings in concurrent use.
**
** Revision 1.3  1997/07/18 02:58:25  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.2  1993/02/23 19:10:32  dhb
** 1.4 to 2.0 command argument changes.
**
** 	do_pathname changed to use GENESIS getopt routines.
**
 * Revision 1.1  1992/10/27  20:33:07  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include "shell_func_ext.h"
#include "sim_ext.h"
#include "sim_func_ext.h"

/*
** PATHNAMES_BUF_SIZE - determines the amount of memory available for
** pathnames returned by Pathname().  A single buffer is shared by all
** Pathname() returned strings, so at most you can have up to 
** PATHNAMES_BUF_SIZE characters in pathnames in simultaneous use.
*/

#ifndef PATHNAMES_BUF_SIZE
#define	PATHNAMES_BUF_SIZE	1000
#endif

/*
** parse a connection specification of the form
**
** 	:[a][a-b][#][@wildcard]
**
** If a path has been specified then return the list of elements
** The return value is the connection specification mode
*/
int ParseConnection(pathname,start_connection,end_connection,list)
char	*pathname;
int	*start_connection;
int	*end_connection;
ElementList	**list;
{
char	*delimiter;
int show_connections = 0;

    if ((delimiter = strchr(pathname,':'))) {
	*delimiter = '\0';
	if(delimiter[1] == '@'){
	    /*
	    ** target path specification
	    */
	    show_connections = 4;
	    *list = WildcardGetElement(delimiter+2,0);
	} else
	if(delimiter[1] == '#'){
	    /* 
	    ** # for all connections 
	    */
	    show_connections = 3;
	} else 
	if(strchr(delimiter+1,'-')){
	    /*
	    ** range form a-b
	    */
	    show_connections = 2;
	    sscanf(delimiter+1,"%d-%d",start_connection,end_connection);
	} else {
	    /*
	    ** single connection form a
	    */
	    show_connections = 1;
	    *start_connection = atoi(delimiter+1);
	}
    }
    return(show_connections);
}

char *GetNameAndIndex(name,index)
char *name;
int *index;
{
static char elementname[100];
char *index_start;

    if(name == NULL) {
	*index = 0;
	return(NULL);
    }
    /*
    ** get the index
    */
    *index = GetIndexComponent(name);
    /*
    ** eliminate the index component from the elementname
    */
    strcpy(elementname,name);
    if((index_start = strchr(elementname,'[')) != NULL){
	*index_start = '\0';
    }
    return(elementname);
}

int HasElementField(string)
char *string;
{
    if(string == NULL || strlen(string) == 0) return(0);
    return((strchr(string,':') != NULL));
}

char *SpecificField(field)
char *field;
{
char *specific_field;

    specific_field = strchr(field,'.');
    if(specific_field){
	return(specific_field+1);
    } else {
	printf("invalid specific field\n");
	return(NULL);
    }
}


char *int_Pathname(elm, count)
Element*	elm;
int		count;
{
	static char	pathbuf[PATHNAMES_BUF_SIZE];
	static char*	pathp = pathbuf;

	char		indexbuf[30];
	int		indexcnt;
	char*		retp;
	char*		cp;

	if (elm->parent == NULL)
	  {
	    if (count+1 >= sizeof(pathbuf))
		return NULL;  /* path is too long to print */

	    if (count+1 >= pathbuf+sizeof(pathbuf) - pathp)
		pathp = pathbuf;

	    return pathp;
	  }

	if (elm->index != 0)
	  {
	    sprintf(indexbuf, "%d%n", elm->index, &indexcnt);
	    retp = int_Pathname(elm->parent, count+indexcnt+2+1+(int)strlen(elm->name));
	  }
	else
	    retp = int_Pathname(elm->parent, count+1+(int)strlen(elm->name));

	if (retp == NULL)
	    return NULL;	/* path too long to print */

	*pathp++ = '/';
	cp = elm->name;
	while (*cp != '\0')
	    *pathp++ = *cp++;

	if (elm->index != 0)
	  {
	    *pathp++ = '[';
	    cp = indexbuf;
	    while (*cp != '\0')
		*pathp++ = *cp++;
	    *pathp++ = ']';
	  }

        if (count == 0)
	    *pathp++ = '\0';

	return retp;
}


char *Pathname(element)
Element 	*element;
{
char 		*name;

    if(element == NULL){
	return("NULL");
    }
    if(element->parent == NULL){
	return("/");
    }

    name = int_Pathname(element, 0);
    if (name == NULL)
	return "(pathname too long)";

    return name;
}


char *GetParentComponent(string)
char *string;
{
static char 	pathname[100];
char 		*path_ptr;

    pathname[0] = '\0';
    if(string == NULL){
	return(pathname);
    }
    /*
    ** search back for the parent element
    */
    strcpy(pathname,string);
    /*
    ** search back in the string until reaching then next level
    ** in the tree
    */
    for(path_ptr = pathname+strlen(pathname);
	(path_ptr != pathname && *path_ptr != '/'); 
	path_ptr--);
    if(*path_ptr == '/'){
	path_ptr++;
    }
    *path_ptr = '\0';
    return(pathname);
}

char *GetBaseComponent(string)
char *string;
{
static char base[100];
char tmp[100];
char *tmp_ptr;
char *end_ptr;

    base[0] = '\0';
    if(string == NULL)
	return(base);
    strcpy(tmp,string);
    /*
    ** search back for the parent element
    */
    for(tmp_ptr=tmp+strlen(tmp);((tmp_ptr!=tmp) && (*tmp_ptr!='/'));tmp_ptr--);
    if(*tmp_ptr == '/'){ 
	tmp_ptr++;
    }
    /*
    ** search forward for the end of the base
    */
    for(end_ptr = tmp_ptr;((*end_ptr != '\0') && (*end_ptr != ':')); end_ptr++);
    *end_ptr = '\0';
    strcpy(base,tmp_ptr);
    return(base);
}

/*
** extract the string up to a colon
*/
char *GetPathComponent(string)
char *string;
{
static char pathname[100];
char *path_ptr;

    pathname[0] = '\0';
    if(string == NULL)
	return(pathname);
    path_ptr = pathname;
    while(*string != '\0' && *string != ':'){
	*path_ptr++ = *string++;
    }
    *path_ptr = '\0';
    return(pathname);
}

/*
** extract the string which follows a colon in the pathname
*/
char *GetFieldComponent(string)
char *string;
{
static char field[100];
char *field_ptr;

	field[0] = '\0';
	if(string == NULL)
		return(field);
	field_ptr = field;
	while(*string != '\0' && *string != ':') string++;
	if(*string == ':') string++;
	while(*string != '\0'){
		*field_ptr++ = *string++;
	}
	*field_ptr = '\0';
	return(field);
}

int GetIndexComponent(string)
char *string;
{
int index;

	if(string == NULL)
		return(0);
	while(*string != '\0' && *string != '[') string++;
	if(*string == '['){
		sscanf(string+1,"%d",&index);
		return(index);
	} else {
		return(0);
	}
}

#define	TAIL	1
#define	HEAD	2

char *do_pathname(argc,argv)
int argc;
char **argv;
{
char 	*path;
int	option;
int	status;

    option = 0;
    initopt(argc, argv, "path -tail -head");
    while ((status = G_getopt(argc, argv)) == 1)
      {
	if (strcmp(G_optopt, "-tail") == 0)
	    option = TAIL;
	else if (strcmp(G_optopt, "-head") == 0)
	    option = HEAD;
      }

    if(status < 0){
	printoptusage(argc, argv);
	return(NULL);
    }

    path = optargv[1];

    if (option == HEAD)
	return(CopyString(GetParentComponent(path)));
    else if (option == TAIL)
	return(CopyString(GetBaseComponent(path)));

    return(NULL);
}
