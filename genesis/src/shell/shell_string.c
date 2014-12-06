static char rcsid[] = "$Id: shell_string.c,v 1.1.1.1 2005/06/14 04:38:34 svitak Exp $";

/*
** $Log: shell_string.c,v $
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.4  1997/07/18 02:55:46  dhb
** Fix to getopt problem.  getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.3  1995/08/02 02:21:47  venkat
** Defintion of FreeString
**
** Revision 1.2  1993/02/23  23:56:13  dhb
** 1.4 to 2.0 command argument changes.
**
** Revision 1.1  1992/12/11  19:04:47  dhb
** Initial revision
**
*/

#include "shell_ext.h"

char *CopyString(s)
char *s;
{
char *tmp;
    if(s == NULL){
	return(NULL);
    }
    tmp = (char *)malloc(strlen(s) + 1);
    strcpy(tmp,s);
    return(tmp);
}

void FreeString(s)
char *s;
{
  if(s==NULL) return;
  free(s);
}

char *do_chr(argc,argv)
int argc;
char **argv;
{
char s[2];

    initopt(argc, argv, "ascii-code");
    if (G_getopt(argc, argv) != 0)
      {
	printoptusage(argc, argv);
	return NULL;
      }

    s[0] = atoi(optargv[1]);
    s[1] = '\0';
    return(CopyString(s));
}
