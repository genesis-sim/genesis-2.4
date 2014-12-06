static char rcsid[] = "$Id: shell_info.c,v 1.1.1.1 2005/06/14 04:38:34 svitak Exp $";

/*
** $Log: shell_info.c,v $
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.1  1992/12/11 19:04:32  dhb
** Initial revision
**
*/

#include "shell_ext.h"

void do_getinfo(argc,argv)
int		argc;
char	**argv;
{
Info info;

    if(argc > 1){
	GetInfo(argv[1],&info);
    } else {
	printf("usage: getinfo symbol\n");
    }
}

