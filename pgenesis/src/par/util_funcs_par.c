static char rcsid[] = "$Id: util_funcs_par.c,v 1.2 2005/09/29 23:23:27 ghood Exp $";

/*
 * $Log: util_funcs_par.c,v $
 * Revision 1.2  2005/09/29 23:23:27  ghood
 * replaced dclock with pgen_clock since dclock was clashing with a system routine on the XT3
 *
 * Revision 1.1.1.1  2005/06/14 04:38:39  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.1  1999/12/19 03:59:43  mhucka
 * Adding PGENESIS from GENESIS 2.1 CDROM release.
 *
 * Revision 1.4  1997/01/21 22:20:28  ngoddard
 * paragon port
 *
 * Revision 1.3  1995/04/26 19:57:23  ngoddard
 * commenced allowing out-of-zone setups out of step
 *
 * Revision 1.2  1995/04/18  21:28:18  ngoddard
 * *** empty log message ***
 *
 * Revision 1.1  1995/04/03  22:04:47  ngoddard
 * Initial revision
 *
 * Revision 1.1  1994/09/07  19:01:00  ngoddard
 * passive messaging works, src figures map
 *
 * Revision 1.5  1993/12/30  00:06:20  ngoddard
 * Release 12/14/93
 *
 * Revision 1.2  1993/10/15  22:51:51  ngoddard
 * added Id and Log keywords
 *
 *
*/

#include "par_ext.h"

/*
** Function to decide if an element should be loaded in to a particular
** postmaster's region. At this stage it uses purely geometrical 
** decomposition, but later something more sophisticated may
** be needed 
*/

int IsInPostRegion(x,y,z)
     float x,y,z;
{
  Postmaster * pm;

  if ((pm = GetPostmaster()) == NULL)
    return(1);

  if (x > pm->xmin && x <= pm->xmax &&
      y > pm->ymin && y <= pm->ymax &&
      z > pm->zmin && z <= pm->zmax)
    return(1);
  return(0);
}

/* This function mallocs space to copy its argument to, copies the
   argument and returns the pointer to the space.  It is intended to
   make results that can be returned to the parser which expects to
   be able to free them.
*/

char * MallocedString(arg)
     char * arg;
{
  char * res = (char *) malloc(strlen(arg) + 1);
  strcpy(res,arg);
  return(res);
}

int do_mynode(argc,argv)
     int argc;
     char **argv;
{
  Postmaster * pm;

  if ((pm = GetPostmaster()) == NULL)
    return(0);
  return(pm->mynode);
}

int do_nnodes(argc,argv)
     int argc;
     char **argv;
{
  Postmaster * pm;

  if ((pm = GetPostmaster()) == NULL)
    return(0);
  return(pm->nnodes);
}

int do_myzone(argc,argv)
     int argc;
     char **argv;
{
  Postmaster * pm;

  if ((pm = GetPostmaster()) == NULL)
    return(0);
  return(pm->myzone);
}

int do_nzones(argc,argv)
     int argc;
     char **argv;
{
  Postmaster * pm;

  if ((pm = GetPostmaster()) == NULL)
    return(0);
  return(pm->nzones);
}

int do_mytotalnode(argc,argv)
     int argc;
     char **argv;
{
  Postmaster * pm;

  if ((pm = GetPostmaster()) == NULL)
    return(0);
  return(pm->ipm->mynode);
}

int do_totalnodes(argc,argv)
     int argc;
     char **argv;
{
  Postmaster * pm;

  if ((pm = GetPostmaster()) == NULL)
    return(0);
  return(pm->totalnodes);
}

