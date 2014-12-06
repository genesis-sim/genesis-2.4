static char rcsid[] = "$Id: shell_env.c,v 1.2 2005/06/26 08:25:37 svitak Exp $";

/*
** $Log: shell_env.c,v $
** Revision 1.2  2005/06/26 08:25:37  svitak
** Provided explicit types for untyped funtions. Fixed return statements to
** match return type.
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.4  2001/04/25 17:17:00  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.3  1997/07/18 02:55:46  dhb
** Fix to getopt problem.  getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.2  1993/02/26 22:08:59  dhb
** 1.4 to 2.0 command argument changes.
**
** Revision 1.1  1992/12/11  19:04:26  dhb
** Initial revision
**
*/

#include "shell_ext.h"

/*
show_envp()
{
int i = 0;
extern char **global_envp;

    while(global_envp[i] != NULL){
	printf("%s\n",global_envp[i++]);
    }
}
*/

void show_environ()
{
int i = 0;
extern char **environ;

    while(environ[i] != NULL){
	printf("%s\n",environ[i++]);
    }
}

void do_printenv(argc,argv)
int		argc;
char	**argv;
{
char *var;
char * getenv();

    initopt(argc, argv, "environment-variable");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return;
      }

    if ((var = getenv(optargv[1]))) {
	printf("%s=%s\n",optargv[1],var);
    } else {
	printf("could not find %s\n",optargv[1]);
    }
}

char *do_getenv(argc,argv)
int	argc;
char	**argv;
{
char * getenv();

    initopt(argc, argv, "environment-variable");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return NULL;
      }

    return(CopyString(getenv(optargv[1])));
}

void do_setenv(argc,argv)
int	argc;
char	**argv;
{
char tmp[1000];
char *newenv;
int nxtarg;

    initopt(argc, argv, "environment-variable [new-value...]");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return;
      }

    if(optargc > 2){
	tmp[0] = '\0';
	nxtarg = 1;
	while(++nxtarg < optargc){
	    strcat(tmp,optargv[nxtarg]);
	    if(nxtarg < optargc-1)
		strcat(tmp," ");
	}
	newenv = (char *)malloc(strlen(tmp) + strlen(optargv[1]) + 2);
	strcpy(newenv,optargv[1]);
	strcat(newenv,"=");
	strcat(newenv,tmp);
    } else {
	newenv = (char *)malloc(strlen(optargv[1])+1);
	strcpy(newenv,argv[1]);
    }
    if(putenv(newenv) == 0){
	if(IsSilent()<1)
	printf("%s=%s\n",optargv[1],tmp);
    } else {
	if(IsSilent()<1)
	printf("could not change %s\n",optargv[1]);
    }
}

