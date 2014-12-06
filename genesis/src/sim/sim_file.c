static char rcsid[] = "$Id: sim_file.c,v 1.2 2005/06/27 19:01:07 svitak Exp $";

/*
** $Log: sim_file.c,v $
** Revision 1.2  2005/06/27 19:01:07  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.7  2001/04/25 17:17:01  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.6  1997/07/29 00:05:34  dhb
** Added missing malloc() of fpargv in do_readfile().
**
** Revision 1.5  1997/07/18 21:48:21  dhb
** Now return an empty string on EOF for readfile rather than printing
** an error and returning an empty arglist (which resulted in termination
** of the script).
**
** Changed errors to stderr to go to stdout in do_readfile().
**
** Additional check for malloc() failure in do_readfile().
**
** Revision 1.4  1997/07/18 02:58:25  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.3  1995/06/16 06:00:30  dhb
** FreeBSD compatibility.
**
 * Revision 1.2  1993/02/22  23:39:56  dhb
 * 1.4 to 2.0 command argument changes.
 *
 * 	do_openfile, do_closefile, do_readfile, do_writefile, do_showfiles,
 * 	do_checkeof, do_flushfile changed to use GENESIS getopt routines.
 *
 * Revision 1.1  1992/10/27  20:04:04  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include "shell_func_ext.h"
#include "sim_ext.h"
#define MAXFILES 20

static FILE	*filefp[MAXFILES];
static char	*filename[MAXFILES];
static char	*filemode[MAXFILES];
static int      CloseFile();

void do_showfiles(argc, argv)

int	argc;
char**	argv;

{
int i;

    initopt(argc, argv, "");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return;
      }

    printf("\nOPEN ASCII FILES\n");
    printf("-----------------\n");
    for(i=0;i<MAXFILES;i++){
	if(filefp[i] != NULL){
	    printf("%-20s %s\n",filename[i],filemode[i]);
	}
    }
    printf("\n");
}

static FILE *GetFile(name)
char *name;
{
int i;

    if(name == NULL){
	return(NULL);
    }
    for(i=0;i<MAXFILES;i++){
	if(filename[i] != NULL && (strcmp(filename[i],name) == 0)){
	    return(filefp[i]);
	}
    }
    return(NULL);
}

static FILE *OpenFile(name,mode)
char *name;
char *mode;
{
int i;
FILE *fp;

    /*
    ** see whether the file is already open
    */
    if((fp = GetFile(name)) != NULL){
	/*
	** if it is then close it
	*/
	CloseFile(name);
    }
    /*
    ** try and open it
    */
    if((fp = fopen(name,mode)) == NULL){
	return(NULL);
    }
    /*
    ** if found then add it to the table
    */
    for(i=0;i<MAXFILES;i++){
	/*
	** find the first empty slot
	*/
	if(filefp[i] == NULL){
	    /*
	    ** add the file
	    */
	    filename[i] = CopyString(name);
	    filemode[i] = CopyString(mode);
	    filefp[i] = fp;
	    return(fp);
	}
    }
    return(NULL);
}

static int CloseFile(name)
char *name;
{
int i;

    if(name == NULL){
	return(0);
    }
    for(i=0;i<MAXFILES;i++){
	if(filename[i] != NULL && (strcmp(filename[i],name) == 0)){
	    free(filename[i]);
	    filename[i] = NULL;
	    if(filemode[i]) free(filemode[i]);
	    filemode[i] = NULL;
	    fclose(filefp[i]);
	    filefp[i] = NULL;
	    return(1);
	}
    }
    return(0);
}

int do_openfile(argc,argv)
int argc;
char **argv;
{
char *file;
char *mode;
FILE *fp;

    initopt(argc, argv, "file-name open-mode");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return(0);
      }

    file = optargv[1];
    mode = optargv[2];
    if((fp = OpenFile(file,mode)) == NULL){
	Error();
	printf("unable to open file '%s' with mode '%s'\n",file,mode);
	return(0);
    }
    return(1);
}

int do_closefile(argc,argv)
int argc;
char **argv;
{
    initopt(argc, argv, "file-name");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return(0);
      }

    if(!CloseFile(optargv[1])){
	Error();
	printf("unable to close file '%s'\n",optargv[1]);
	return(0);
    }
    return(1);
}

int do_checkeof(argc,argv)
int argc;
char **argv;
{
FILE	*fp;

    initopt(argc, argv, "file-name");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return(0);
      }

    if((fp = GetFile(optargv[1])) == NULL){
	Error();
	printf("file '%s' is not open\n",optargv[1]);
	return(0);
    }
    return(feof(fp));
}

char **do_readfile(argc,argv)
int argc;
char **argv;
{
char	line[1000];
int	fpargc;
char	**fpargv;
FILE	*fp;
int	linemode = 0;
char	*filename;
char	*ptr;
int	status;

    initopt(argc, argv, "file-name -linemode");
    while ((status = G_getopt(argc, argv)) == 1)
      {
	if (strcmp(G_optopt, "-linemode") == 0)
	    linemode = 1;
      }

    if(status < 0){
	printoptusage(argc, argv);
	return(NULLArgv());
    }

    filename = optargv[1];
    fp = GetFile(filename);
    if(fp == NULL)
      {
	Error();
	printf("file '%s' is not open \n",filename);
	return(NULLArgv());
      }

    if(fgets(line,1000,fp) == NULL){
	if (feof(fp)){
	    fpargv = (char **)malloc(2*sizeof(char *));
	    if (fpargv == NULL){
		perror(argv[0]);
		return(NULL);
	    }

	    fpargv[0] = CopyString("");
	    fpargv[1] = NULL;

	    return(fpargv);
	}

	Error();
	printf("cannot read from file '%s'\n",filename);
	return(NULLArgv());
    }
    if(!linemode){
	StringToArgList(line,&fpargc,&fpargv);
    } else {
	fpargc = 1;
	fpargv = (char **)malloc(2*sizeof(char *));
	if (fpargv == NULL){
	    perror(argv[0]);
	    return(NULL);
	}
	/*
	** zap any CR
	*/
	if((ptr = strchr(line,'\n')) != 0){
	    *ptr = '\0';
	}
	fpargv[0] = CopyString(line);
	fpargv[1] = NULL;
    }
    return(fpargv);
}

int do_writefile(argc,argv)
int argc;
char **argv;
{
int i;
FILE	*fp;
int	cr;
char	*format;
int	userformat;
int	status;

    cr = 1;
    format = "%s";
    userformat = 0;

    initopt(argc, argv, "file-name [arguments...] -nonewline -format str");
    while ((status = G_getopt(argc, argv)) == 1)
      {
	if (strcmp(G_optopt, "-nonewline") == 0)
	    cr = 0;
	else if (strcmp(G_optopt, "-format") == 0)
	  {
	    userformat = 1;
	    format = optargv[1];
	  }
      }

    if(status < 0)
      {
	printoptusage(argc, argv);
	return(0);
      }

    if((fp = GetFile(optargv[1])) == NULL){
	Error();
	printf("file '%s' is not open \n",optargv[1]);
	return(0);
    }

    for(i=2;i<optargc;i++){
#ifdef mips
	if(fprintf(fp,format,optargv[i]) == EOF){
#else
	if(fprintf(fp,format,optargv[i]) <= 0){
#endif
	    Error();
	    printf("unable to write to file '%s'\n",optargv[1]);
	    return(0);
	}
	if(i < optargc-1 && !userformat){
	    /*
	    ** place a space after all but the last argument
	    */
	    fprintf(fp," ");
	}
    }
    if(cr){
	fprintf(fp,"\n");
    }
    OK();
    return(1);
}

int do_flushfile(argc,argv)

int argc;
char **argv;

{

FILE	*fp;

    initopt(argc, argv, "file-name");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return(0);
      }

    if((fp = GetFile(optargv[1])) == NULL){
	Error();
	printf("file '%s' is not open \n",optargv[1]);
	return(0);
    }

    fflush(fp);
    OK();

    return(1);

}

void CloseAllAsciiFiles()
{
int i;

    for(i=0;i<MAXFILES;i++){
	if(filefp[i] != NULL){
	    CloseFile(filename[i]);
	}
    }
}

