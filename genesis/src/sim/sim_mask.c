static char rcsid[] = "$Id: sim_mask.c,v 1.1.1.1 2005/06/14 04:38:28 svitak Exp $";

/*
** $Log: sim_mask.c,v $
** Revision 1.1.1.1  2005/06/14 04:38:28  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.3  2001/04/25 17:17:02  mhucka
** Misc. small changes to improve portability and address compiler warnings.
**
** Revision 1.2  1993/02/16 18:36:21  dhb
** Fixed typing problem in rcsid variable declaration.
**
 * Revision 1.1  1992/10/27  20:18:16  dhb
 * Initial revision
 *
*/

#include <stdio.h>
#include "shell_func_ext.h"
#include "sim_ext.h"

struct mask_type *GetMaskFromArgv(count,nxtarg,argc,argv)
int *count;
int *nxtarg;
int argc;
char **argv;
{
int i;
struct mask_type *mask;
int argcount;

    argcount = *nxtarg;
    *count = atoi(argv[argcount++]);
    if(argc < argcount + *count){
	Error();
	printf("bad region specification\n");
	printf("format: n +-1 x1 y1 x1' y1'... +-1 xn yn xn' yn'\n");
	return(NULL);
    }
    mask = (struct mask_type *)
    malloc((*count) * sizeof(struct mask_type));
    for(i=0;i<(*count);i++){
	mask[i].type = atoi(argv[argcount++]); 
	mask[i].xl = Atof(argv[argcount++]); 
	mask[i].yl = Atof(argv[argcount++]); 
	mask[i].xh = Atof(argv[argcount++]); 
	mask[i].yh = Atof(argv[argcount++]); 
    }
    *nxtarg = argcount;
    return(mask);
}

