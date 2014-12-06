static char rcsid[] = "$Id: sim_static.c,v 1.1.1.1 2005/06/14 04:38:28 svitak Exp $";

/*
** $Log: sim_static.c,v $
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.3  2001/04/25 17:17:03  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.2  1993/07/21 21:32:47  dhb
** fixed rcsid variable type
**
 * Revision 1.1  1992/10/27  20:30:56  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include "shell_func_ext.h"
#include "sim_ext.h"

do_get_file_static(argc,argv)
int argc;
char **argv;
{
int val;

	if(argc < 3){
		printf("usage: %s file var\n",argv[0]);
		return;
	}
	val = FindStaticInFile(argv[2],argv[1]); 
	if(val){
		printf("&%d = %d\n",val,*(((int *)val)));
	} else {
		printf("could not find %s in %s\n",argv[2],argv[1]);
	}
}

do_set_file_static(argc,argv)
int argc;
char **argv;
{
int val;

	if(argc < 4){
		printf("usage: %s file var value\n",argv[0]);
		return;
	}
	val = FindStaticInFile(argv[2],argv[1]); 
	if(val){
		printf("&%d = %d\n",val,*(((int *)val)));
		*(((int *)val)) = atoi(argv[3]);
		printf("new = %d\n",*(((int *)val)));
	} else {
		printf("could not find %s in %s\n",argv[2],argv[1]);
	}
}
do_get_func_static(argc,argv)
int argc;
char **argv;
{
int val;

	if(argc < 3){
		printf("usage: %s func var\n",argv[0]);
		return;
	}
	val = FindStaticInFunc(argv[2],argv[1]); 
	if(val){
		printf("&%d = %d\n",val,*(((int *)val)));
	} else {
		printf("could not find %s in %s\n",argv[2],argv[1]);
	}
}

do_set_func_static(argc,argv)
int argc;
char **argv;
{
int val;

	if(argc < 4){
		printf("usage: %s func var value\n",argv[0]);
		return;
	}
	val = FindStaticInFunc(argv[2],argv[1]); 
	if(val){
		printf("&%d = %d\n",val,*(((int *)val)));
		*(((int *)val)) = atoi(argv[3]);
		printf("new = %d\n",val,*(((int *)val)));
	} else {
		printf("could not find %s in %s\n",argv[2],argv[1]);
	}
}

do_debug_func(argc,argv)
int argc;
char **argv;
{
int val;

	if(argc < 2){
		printf("usage: %s func [value]\n",argv[0]);
		return;
	}
	val = FindStaticInFunc("func_trace",argv[1]); 
	if(val){
		if(argc > 2)
			*(((int *)val)) = atoi(argv[2]);
		printf("debug level %d in func '%s'\n",*(((int *)val)),argv[1]);
	} else {
		printf("could not find func_trace in '%s'\n",argv[1]);
	}
}

do_debug_file(argc,argv)
int argc;
char **argv;
{
int val;

	if(argc < 2){
		printf("usage: %s file [value]\n",argv[0]);
		return;
	}
	val = FindStaticInFile("file_trace",argv[1]); 
	if(val){
		if(argc > 2)
			*(((int *)val)) = atoi(argv[2]);
		printf("debug level %d in file '%s'\n",*(((int *)val)),argv[1]);
	} else {
		printf("could not find file_trace in '%s'\n",argv[1]);
	}
}
