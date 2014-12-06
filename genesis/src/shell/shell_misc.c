static char rcsid[] = "$Id: shell_misc.c,v 1.2 2005/06/26 08:25:37 svitak Exp $";

/*
** $Log: shell_misc.c,v $
** Revision 1.2  2005/06/26 08:25:37  svitak
** Provided explicit types for untyped funtions. Fixed return statements to
** match return type.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.10  2001/06/29 21:25:52  mhucka
** New mkdir function from Hugo C.
**
** Revision 1.9  2001/04/25 17:17:00  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.8  1997/07/24 00:30:57  dhb
** Merged in 1.5.1.1 changes
**
** Revision 1.7  1997/07/24 00:05:24  venkat
** Defined functions and structures to implement the command callbacks.
** Also modified do_exit() to process the command callbacks before exiting
**
** Revision 1.6  1997/07/18  02:55:46  dhb
** Fix to getopt problem.  getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.5.1.1  1997/07/18 16:03:25  dhb
** T3E mods from PSC
**
** Revision 1.5  1995/07/18  18:26:31  dhb
** Added extern for strrchr() to avoid type mismatch error.
**
** Revision 1.4  1995/07/14  20:09:55  dhb
** Previous do_echo() -format change precluded a conversion without
** a width specifier.  Will now accept this.
**
** Revision 1.3  1995/07/14  19:50:15  dhb
** do_echo() modified to verify -format option argument.  Only allows
** a single string conversion in the format.
**
** Revision 1.2  1993/02/26  22:09:38  dhb
** 1.4 to 2.0 command argument changes.
**
** Revision 1.1  1992/12/11  19:04:38  dhb
** Initial revision
**
*/

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "shell_ext.h"


typedef void (*PFV) ();

typedef struct _cmdCallback {
	PFV 		 	func;	
	struct _cmdCallback  	*next;
} cmdCallback;

typedef struct _cmdCallbackList {
	struct _cmdCallback 	*head;
} cmdCallbackList;

static cmdCallbackList cmdcallbacklist;

/** 
	This is called from SimStartup() -
	Initializes the 'cmdcallbacklist' that manages all command
	callbacks 
**/
void InitCommandCallbacks() 
{
	cmdcallbacklist.head = NULL;
}

/** 
	Also called from SimStartup() -
	Populates the 'cmdcallbacklist' with callback functions
**/

void AddCommandCallback(callbackfunc)
 PFV callbackfunc;
{
	cmdCallback *callback; 

	if(cmdcallbacklist.head == NULL) {
		cmdcallbacklist.head = (cmdCallback *) calloc(1, sizeof(struct _cmdCallback)); 
		callback = cmdcallbacklist.head;
	} else {
		cmdCallback *prevcallback;
		for (callback = cmdcallbacklist.head; callback != NULL; prevcallback=callback, callback=callback->next);		
		prevcallback->next = (cmdCallback *) calloc (1, sizeof(struct _cmdCallback));
		callback = prevcallback->next;
	}	
	
	callback->func = callbackfunc;
}

/** 
	Can be called from wherever callbacks need to be executed 
	Presently called from do_exit() to execute the quit callbacks
**/
void processCommandCallbacks() 
{
	cmdCallback *callback; 

	for (callback = cmdcallbacklist.head; callback != NULL; callback=callback->next)			callback->func();
}

void freeCommandCallbacks()
{
	cmdCallback *callback, *nextcallback; 

	for (callback = cmdcallbacklist.head; callback != NULL; callback=nextcallback) {
		nextcallback = callback->next;
		free(callback);
		callback = NULL;

		/* 
		   Updating the list here is superfluous - but just 
		   do it for completeness
		*/ 
		cmdcallbacklist.head = nextcallback;
	}

}


/*
** this is needed for qsort which destroys the values of the pointers
** passed to it
*/
int Strcmp(s1,s2)
char	**s1;
char	**s2;
{
    return(strcmp(*s1,*s2));
}

int do_cd(argc,argv)
int	argc;
char	**argv;
{
char string[200];
char *home;
char *getenv();

    initopt(argc, argv, "path");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return(0);
      }

    if(optargv[1][0] == '~'){
	home = getenv("HOME");
	if(!home){
	    printf("cant find home directory\n");
	    return(0);
	} else {
	    sprintf(string,"%s%s",home,optargv[1]+1);
	}
    } else {
	strcpy(string,optargv[1]);
    }
    if(chdir(string) == -1){
	printf("could not change to '%s'\n",optargv[1]);
	return(0);
    }
    return(1);
}

int do_mkdir(argc,argv)
int	argc;
char	**argv;
{
char string[200];
char *home;
char *getenv();

    initopt(argc, argv, "path");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return(0);
      }

    if(optargv[1][0] == '~'){
	home = getenv("HOME");
	if(!home){
	    printf("cant find home directory\n");
	    return(0);
	} else {
	    sprintf(string,"%s%s",home,optargv[1]+1);
	}
    } else {
	strcpy(string,optargv[1]);
    }
    if(mkdir(string, 0x1ff) == -1){
	printf("could not create '%s'\n",optargv[1]);
	return(0);
    }
    return(1);
}

void do_printargs(argc,argv)
int argc;
char **argv;
{
int		i;

    for(i=1;i<argc;i++){
	printf("argc %d : %s\n",i,argv[i]);
    }
}

void do_echo(argc,argv)
int argc;
char **argv;
{
extern char* strrchr();

int	i;
int	nxtarg;
int	cr;
char	*format;
int	userformat;
int	status;
/* Upi Bhalla hacks to try to get echo to work on the Delta */
#ifdef i860
char	line[200];
char	temp[100];

line[0]='\0';
#endif

    nxtarg = 0;
    cr = 1;
    format = "%s";
    userformat = 0;
    initopt(argc, argv, "[arguments] -nonewline -format format-string");
    while ((status = G_getopt(argc, argv)) == 1)
      {
	if(strcmp(G_optopt,"-nonewline") == 0){
	    /*
	    ** no cr option
	    */
	    cr = 0;
	} else if(strcmp(G_optopt,"-format") == 0){
	    /*
	    ** user formatting
	    */
	    char	*conv;
	    char	convchar;

	    format = optargv[1];
	    userformat = 1;
	    conv = strchr(format, '%');
	    if (conv == NULL || strrchr(format, '%') != conv ||
		    (conv[1] != 's' &&
		     (sscanf(conv+1, "%*f%c", &convchar) != 1 ||
		      convchar != 's'
		     )
		    )
	       ){
		Error();
		printf("bad format value '%s' in echo statement\n", format);
		return;
	    }
	}
      }

    if (status < 0)
      {
	printoptusage(argc, argv);
	return;
      }

#ifdef i860
    for(i=1;i<optargc;i++){
	/*
	** print out each argument
	*/
	sprintf(temp,format,optargv[i]);
	strcat(line,temp);
	if(i < optargc-1 && !userformat){
	    /*
	    ** place a space after all but the last argument
	    */
		strcat(line," ");
	}
    }
    if(cr){
		printf("%s\n",line);
    } else {
		printf("%s",line);
		fflush(stdout);
    }
#else
    for(i=1;i<optargc;i++){
	/*
	** print out each argument
	*/
	lprintf(format,optargv[i]);
	if(i < optargc-1 && !userformat){
	    /*
	    ** place a space after all but the last argument
	    */
	    lprintf(" ");
	}
    }
    if(cr){
	lprintf("\n");
    } else {
	fflush(stdout);
    }
#endif
}

void do_exit(argc, argv)

int	argc;
char**	argv;

{
    initopt(argc, argv, "");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return;
      }

    /** Call any QUIT actions first **/
    processCommandCallbacks();
    freeCommandCallbacks();

    if(Dynamic()){
	system("rm /tmp/tmp.dfl");
    }
    if(!IsBatchMode()){
	restore_tty();
    }
#ifdef T3E
    globalexit(0);
#else
    exit(0);
#endif
}

