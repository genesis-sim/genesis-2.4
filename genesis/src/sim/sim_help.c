static char rcsid[] = "$Id: sim_help.c,v 1.4 2005/10/16 20:48:35 svitak Exp $";

/*
** $Log: sim_help.c,v $
** Revision 1.4  2005/10/16 20:48:35  svitak
** Changed hard-coded character arrays from 100 to 256 or malloc'd and freed
** memory instead. This was only done where directory paths might have easily
** exceeded 100 characters.
**
** Revision 1.3  2005/07/01 20:20:53  svitak
** Changed *.doc references to *.txt as a consequence of .doc files
** being renamed to .txt.
**
** Revision 1.2  2005/06/27 19:01:07  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.9  2001/04/25 17:17:01  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.8  2000/07/10 01:06:44  mhucka
** It's kind of silly for there to be both a VERSION and VERSIONSTR.
** Just use one.
**
** Revision 1.7  1998/07/21 23:10:29  dhb
** do_version now returns a char* formated version string in the
** format %6.3f.
**
** Revision 1.6  1997/07/18 02:58:25  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.5  1996/07/20 00:10:56  dhb
** Fixed bug in error message for NULL topic.
**
 * Revision 1.4  1996/07/19  23:14:45  dhb
 * do_help() now allows topic to be optional.  If omitted do_help()
 * displays the README file in the first help directory in which it
 * is found.
 *
 * Revision 1.3  1994/04/13  16:59:36  dhb
 * help() wasn't parsing the help directory path correctly.
 *
 * Revision 1.2  1993/02/16  22:55:40  dhb
 * 1.4 to 2.0 command argument changes.
 *
 * 	do_help (help) changed to use GENESIS getopt routines.  Help directory
 * 	command arguments moved to "-help directory ..." command option.
 *
 * Revision 1.1  1992/10/27  20:08:46  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include "shell_func_ext.h"
#include "sim_ext.h"

int help(name,directories)
char *name;
char *directories;
{
FILE	*fp;
char line[200];
char helpname[1000];
char hname[256];
char *ptr;
char *helplist[100];
int cnt = 0;
int i;

    if(directories == NULL) return 0;
    strcpy(helpname,directories);
    /*
    ** parse the directory list
    */
    helplist[cnt++] = ptr = helpname;
    while ((ptr = strchr(ptr,' '))) {
	/*
	** find the separator
	*/
	*ptr++ = '\0';
	helplist[cnt++] = ptr; 
    }
    for(i=0;i<cnt;i++){
	if (name == NULL)
	    sprintf(hname,"%s/README",helplist[i]);
	else
	    sprintf(hname,"%s/%s.txt",helplist[i],name);
	/*
	** open the helpfile
	*/
	if((fp = fopen(hname,"r")) == NULL){
	    continue;
	}
	printf("\n");
	while(!feof(fp)){
	    GetLineFp(line,fp,1000);
	    if(line[0] == '#' && line[1] == '#'){
		fclose(fp);
		/*
		** find the white space
		*/
		for(ptr=line+2;ptr;ptr++){
		    if(*ptr == ' ' ||
		    *ptr == '\0' ||
		    *ptr == '\n' ||
		    *ptr == '\t'){
			*ptr = '\0';
			break;
		    }
		}
		sprintf(hname,"%s/%s.txt",helplist[i],line+2);
		/*
		** open the helpfile
		*/
		if((fp = fopen(hname,"r")) == NULL){
		    printf("could not find redirected helpfile '%s'\n",hname);
		    return(0);
		}
		continue;
	    }
	    printf("%s",line);
	}
	fclose(fp);
	return(1);
    }
    if (name == NULL)
	printf("no help available in:\n"); 
    else
	printf("no help available for '%s' in:\n",name); 
    for(i=0;i<cnt;i++){
	printf("'%s'\n",helplist[i]);
    }
    return(0);
}

void do_help(argc,argv)
int argc;
char **argv;
{
static char directories[1000] = "";
char	*string;
int	firstdir;
int	status;
int	i;

    firstdir = 1;

    initopt(argc, argv, "[topic] -helpdir directory ...");
    while ((status = G_getopt(argc, argv)) == 1)
      {
	if (strcmp(G_optopt, "-helpdir") == 0)
	  {
	    if (firstdir)
	      {
		directories[0] = '\0';
		firstdir = 0;
	      }
	    else
		strcat(directories, " ");

	    for (i = 1; i < optargc; i++)
	      {
		if (i > 1)
		    strcat(directories, " ");

		strcat(directories, optargv[i]);
	      }
	  }
      }

    if (status < 0 || optargc > 2)
      {
	printoptusage(argc, argv);
	return;
      }

    if(strlen(directories) == 0){
	if((string = getenv("GENESIS_HELP")) == NULL){
	    printf("Must specify a help directory. No default available\n");
	    return;
	} else {
	    strcpy(directories,string);
	}
    }

    help(optargc == 2 ? optargv[1] : NULL,directories);
}

char* do_version(argc,argv)
	int argc;
	char **argv;
{
	return(CopyString(VERSIONSTR));
}
