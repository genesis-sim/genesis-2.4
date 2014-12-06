static char rcsid[] = "$Id: shell_sh.c,v 1.2 2005/10/24 06:31:59 svitak Exp $";

/*
** $Log: shell_sh.c,v $
** Revision 1.2  2005/10/24 06:31:59  svitak
** defined state as short in Autoshell().
**
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.4  2000/09/07 05:16:28  mhucka
** Implemented simple new function SetAutoShell().
**
** Revision 1.3  1997/07/18 02:55:46  dhb
** Fix to getopt problem.  getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.2  1993/02/23 23:56:13  dhb
** 1.4 to 2.0 command argument changes.
**
** Revision 1.1  1992/12/11  19:04:44  dhb
** Initial revision
**
*/

#include "shell_ext.h"

static short	autoshell = 1;

void SetAutoshell(state)
short state;
{
    autoshell = state;
}

int Autoshell()
{
    return autoshell;
}

void do_autoshell(argc,argv)
int		argc;
char	**argv;
{

    initopt(argc, argv, "[new-autoshell-state]");
    if (G_getopt(argc, argv) != 0 || optargc > 2)
      {
	printoptusage(argc, argv);
	return;
      }

    if(optargc == 2){
	autoshell = atoi(optargv[1]);
    } 
    if(IsSilent()<1){
	if(autoshell)
	    printf("autoshell TRUE\n");
	else 
	    printf("autoshell FALSE\n");
    }
}

void do_shell(argc,argv)
int argc;
char **argv;
{
extern char *ArgListToString();

    initopt(argc, argv, "[external-command]");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return;
      }

    if(optargc > 1)
	system(ArgListToString(optargc-1,optargv+1));
}

