static char rcsid[] = "$Id: sim_notes.c,v 1.3 2005/10/16 20:48:35 svitak Exp $";

/*
** $Log: sim_notes.c,v $
** Revision 1.3  2005/10/16 20:48:35  svitak
** Changed hard-coded character arrays from 100 to 256 or malloc'd and freed
** memory instead. This was only done where directory paths might have easily
** exceeded 100 characters.
**
** Revision 1.2  2005/06/27 19:01:08  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.5  2001/04/25 17:17:02  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.4  2000/06/12 04:54:19  mhucka
** Fixed trivial errors in some printf format strings.
**
** Revision 1.3  1997/07/29 00:10:01  dhb
** utsn was declared an used in approprately.
**
** Revision 1.2  1993/02/15 21:33:23  dhb
** Fixed error in rcsid static variable declatation.
**
 * Revision 1.1  1992/10/27  20:20:26  dhb
 * Initial revision
 *
*/
/* mds3 changes - moved outside function by Hugo Cornelis 2014-07-09 */
#ifdef SYSV
#include <sys/utsname.h>
struct utsname utsn;
#else
#include <unistd.h>
#endif

#include <stdio.h>
#include "shell_func_ext.h"
#include "sim_ext.h"

#ifndef __hpux
char *GetHeader()
{
char *user;
char *name;
char *directory;
char hostname[100];
static char line[1000];
char *getenv();

    if((user=getenv("USER")) == NULL){
	    user = "???";
    }
    if((name=getenv("NAME")) == NULL){
	    name = "???";
    }
    if((directory=getenv("PWD")) == NULL){
	    directory = "???";
    }
/* mds3 changes */
#ifdef SYSV
#ifdef i860
    strcpy(hostname, "Ajax");
#else
    uname(&utsn);
    strcpy(hostname, utsn.sysname);
#endif
#endif
#ifndef SYSV
    gethostname(hostname,100);
#endif
    sprintf(line,"%s : %s@%s : %s\n%s",
    name,
    user,
    hostname,
    directory,
    date()
    );
    return(line);
}
#endif

void Notes(fp)
FILE *fp;
{
char line[1000];
char *ptr;

    if(fp == NULL) return;
    printf("End with '.' alone on the last line\n");
    AlternatePrompt("->");
    EnableHistory(0);
    fprintf(fp,
"+--------------------------------------------------------------------------+\n"
    );
    strcpy(line,date());
    do{
	fprintf(fp,"%s\n",line);
	fflush(stdout);
	GetLineFp(line,stdin,1000);
	/*
	** zap the CR
	*/
	if ((ptr = strchr(line,'\n')))
	    *ptr = '\0';
    } while(!((line[0] == '.') && (line[1] == '\0')));
    fclose(fp);
    AlternatePrompt(NULL);
    EnableHistory(1);
}

void do_notes(argc,argv)
int argc;
char **argv;
{
FILE *fp;
static char current_file[256] = {""};

    if(argc > 1){
	strcpy(current_file,argv[1]);
    }
    /*
    ** if the current note file has not been specified then
    ** try the SIMNOTES env variable
    */
    if(strlen(current_file) == 0){
	if(getenv("SIMNOTES")){
	    strcpy(current_file,getenv("SIMNOTES"));
	} else {
	    /*
	    ** use the default
	    */
	    strcpy(current_file,"notes");
	}
    }
    /*
    ** open the default note file
    */
    fp = fopen(current_file,"a");
    if(fp == NULL){
	printf("unable to open file %s\n", current_file);
	return;
    }
    printf("using notes file '%s'\n",current_file);
    Notes(fp);
}
