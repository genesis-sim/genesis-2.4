static char rcsid[] = "$Id: shell_argv.c,v 1.5 2005/07/07 19:03:42 svitak Exp $";

/*
** $Log: shell_argv.c,v $
** Revision 1.5  2005/07/07 19:03:42  svitak
** Eliminated traces of old TraceScript function so as not to confuse it
** with the newer, less usable TraceScript found in ss/eval.c.
**
** Revision 1.4  2005/06/26 08:25:37  svitak
** Provided explicit types for untyped funtions. Fixed return statements to
** match return type.
**
** Revision 1.3  2005/06/25 21:25:45  svitak
** printoptusage() is used everywhere. Created a new file for it's declaration
** and included it where appropriate.
**
** Revision 1.2  2005/06/20 21:20:14  svitak
** Fixed compiler warnings re: unused variables and functions with no return type.
** Default of int causes compiler to complain about return with no type, so void
** used as return type when none present.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.7  2001/04/25 17:16:59  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.5  2000/07/12 06:13:07  mhucka
** Added #include of string.h.
**
** Revision 1.4  1997/07/18 02:55:46  dhb
** Fix to getopt problem.  getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.3  1994/10/18 17:00:25  dhb
** Added FreeArgv() function to free script arg lists.
**
** Revision 1.2  1993/02/23  23:56:13  dhb
** 1.4 to 2.0 command argument changes.
**
** Revision 1.1  1992/12/11  19:04:20  dhb
** Initial revision
**
*/

#include <stdio.h>
#include <math.h>
#include <string.h>
#include "header.h"
/* mds3 changes */
#include "system_deps.h"
#include "shell_func_ext.h"
#include "ss/parse.h"
#include "ss/ss_func_ext.h"

#define ARGV_RESIZE 10
#define ALPHA	1
#define SYMBOL	2


struct token_type {
    char 	*token;
    int		type;
};

char **NULLArgv()
{
char **argv;
    argv = (char **)malloc(sizeof(char *));
    argv[0] = NULL;
    return(argv);
}

char **CopyArgv(argc,argv)
int argc;
char **argv;
{
char **newargv;
int i;
size_t	length;

    if(argc == 0) return(NULL);
    newargv = (char **)calloc(argc +1,sizeof(char *));
    for(i=0;i<argc;i++){
	length = strlen(argv[i]);
	newargv[i] = (char *)malloc(length + 1);
	strcpy(newargv[i],argv[i]);
    }
    return(newargv);
}

void FreeArgv(argc, argv)

int	argc;
char*	argv[];

{	/* FreeArgv --- Free arg list memory and args */

	int	arg;

	if (argv == NULL)
	    return;

	for (arg = 0; arg < argc; arg++)
	    if (argv[arg] != NULL)
		free(argv[arg]);

	free(argv);

}	/* FreeArgv */


void ReallocateArgList(argv,size)
char ***argv;
int *size;
{
char **new_argv;
int		new_size;
int		i;

    /*
    ** make the new list
    */
    new_size = *size + ARGV_RESIZE;
    new_argv = (char **)malloc(new_size*sizeof(char *));
    /*
    ** copy the old arguments
    */
    for(i=0;i<*size;i++){
	new_argv[i] = (*argv)[i];	
    }
    /*
    ** free the old list
    */
    free(*argv);
    /*
    ** set the pointers to the new list
    */
    *size = new_size;
    *argv = new_argv;
}

#ifdef NEW
void StringToArgList(string,argc,argv,expression_mode)
char 	*string;
int	*argc;
char 	***argv;
int	expression_mode;
{
char	**new_argv;
char	*sptr;
int	cnt;
short	string_on;
int	argv_size;
int	length;
char	*begin;

    /*
    ** set up a pointer to the new string copy
    */
    sptr = string;
    /*
    ** keep track of the number of arguments in the list
    */
    cnt = 0;
    /*
    ** allocate the list
    */
    argv_size = ARGV_RESIZE;
    new_argv = (char **)malloc(argv_size*sizeof(char *));
    /*
    ** construct the argument list 
    */
    while(!IsStringEnd(*sptr)){
	/*
	* skip leading blanks
	*/
	string_on = 0;
	while(IsWhiteSpace(*sptr) || IsStringDelimiter(*sptr)){
	    if(IsStringDelimiter(*sptr)){
		string_on = 1;
		sptr++;
		break;
	    }
	    sptr++;
	}
	if(IsStringEnd(*sptr)){
	    break;
	}
	/*
	* point to the beginning of the arg
	*/
	new_argv[cnt] = sptr;
	/*
	* find the end of the arg
	*/
	length = 0;
	begin = sptr;
	while(!IsStringEnd(*sptr)){
	    if(!string_on){
		/*
		** break on white space
		*/
		if(IsWhiteSpace(*sptr)){
		    break;
		}
	    }
	    if(IsStringDelimiter(*sptr)){
		/*
		** end of the string delimiter
		*/
		string_on = 0;
		sptr++;
		break;
	    }
	    sptr++;
	    length++;
	}
	/*
	** copy the argument
	*/
	new_argv[cnt] = (char *)calloc(length+1,sizeof(char));
	BCOPY(begin,new_argv[cnt],length);
	/*
	** increment the argument count
	** and check against the current size of the argv list
	**
	** if the argv list is too small then reallocate
	*/
	if(++cnt >= argv_size){
	    ReallocateArgList(&new_argv,&argv_size);
	}
    }
    new_argv[cnt] = NULL;
    *argc = cnt;
    *argv = new_argv;
}
#endif

char *ArgListToString(argc,argv)
int argc;
char **argv;
{
int	size;
char *string;
int	i;

    /*
    ** determine the size of the final string
    */
    size = 0;
    for(i = 0;i<argc;i++){
	size += strlen(argv[i]) + 1;
    }
    string = (char *)calloc(1,size+1);
    for(i = 0;i<argc;i++){
	strcat(string,argv[i]);
	strcat(string," ");
    }
    return(string);
}


int do_argc(argc, argv)

int	argc;
char**	argv;

{
    initopt(argc, argv, "");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return 0;
      }

    return(PTArgc());
}

#ifdef NEW
char **do_argv(argc,argv)
int argc;
char **argv;
{
char 	**list;
char	**PTArgv();

    initopt(argc, argv, "[arg-number]");
    if (G_getopt(argc, argv) != 0 || optargc > 2)
      {
	printoptusage(argc, argv);
	return NULLArgv();
      }

    if(optargc == 2){
	list = PTArgv(atoi(optargv[1]));
    } else {
	list = PTArgv(-1);
    }
    return(list);
}
#endif

