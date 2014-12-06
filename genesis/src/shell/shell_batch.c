static char rcsid[] = "$Id: shell_batch.c,v 1.1.1.1 2005/06/14 04:38:34 svitak Exp $";

/*
** $Log: shell_batch.c,v $
** Revision 1.1.1.1  2005/06/14 04:38:34  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.2  2000/09/21 19:37:16  mhucka
** Added return types for SetBatchMode() and IsBatchMode().
**
** Revision 1.1  1992/12/11 19:04:21  dhb
** Initial revision
**
*/


static int batch=0;

void SetBatchMode(x)
int x;
{
    batch = x;
}

int IsBatchMode()
{
    return(batch);
}

