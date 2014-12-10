static char rcsid[] = "$Id: zones.c,v 1.1.1.1 2005/06/14 04:38:39 svitak Exp $";
/* version.c contains the most recent rcs version number */
#include "version.c"

/*
 * $Log: zones.c,v $
 * Revision 1.1.1.1  2005/06/14 04:38:39  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.1  1999/12/19 03:59:43  mhucka
 * Adding PGENESIS from GENESIS 2.1 CDROM release.
 *
 * Revision 1.7  1997/01/31 05:14:32  ngoddard
 * barriers don't complete asyncs now, minor bug fixes
 *
 * Revision 1.6  1997/01/21 22:20:29  ngoddard
 * paragon port
 *
 * Revision 1.5  1997/01/21 20:19:51  ngoddard
 * bug fixes for zones, implement string literal fields, first release of pgenesis script
 *
 * Revision 1.4  1995/11/09 17:26:05  ngoddard
 * some fixes for Solaris
 *
 * Revision 1.3  1995/05/24  15:34:58  ngoddard
 * improved threads but still basic design flaw, also fixed comment leader in barrier tests
 *
 * Revision 1.2  1995/04/26  19:57:21  ngoddard
 * commenced allowing out-of-zone setups out of step
 *
 * Revision 1.1  1995/04/03  22:04:52  ngoddard
 * Initial revision
 *
 * Revision 1.1  1994/09/07  19:01:00  ngoddard
 * passive messaging works, src figures map
 *
 * Revision 1.5  1993/12/30  00:06:12  ngoddard
 * Release 12/14/93
 *
 * Revision 1.2  1993/10/15  22:51:51  ngoddard
 * added Id and Log keywords
 *
 *
*/

#include <stdio.h>
#include "par_ext.h"

/* Syntax :	mode descriptions options
		modes can be :
		farm: each node is its own zone - default
		parallel : all nodes in same zone, lockstep
		local@+nodeid mynode myzone
		mixed nnodes nzones [nnodes nzones] ... 
			where the remainder are placed in separate 1-node zones
			and the zone numbers are in order of specification. If
			nnodes or nzones is <1 it means divide the remaining
			number of nodes by the non-negative value to obtain
			the other value. Remainders go on as before.
*/

static void make_zone_arrays(ipm)
     Implementation * ipm;
{
  ipm->zone_tids = (int **) malloc(ipm->nzones*sizeof(int *));
  ipm->zone_tids[0] = (int *) malloc(ipm->nnodes*sizeof(int));
  ipm->zone_siz =  (int *) malloc(ipm->nzones*sizeof(int));
  ipm->zone_init =  (int *) malloc(ipm->nzones*sizeof(int));
  ipm->zone =  (int *) malloc(ipm->nnodes*sizeof(int));
}

static void fill_zone_arrays(pm)
     Postmaster * pm;
{
  Implementation * ipm = pm->ipm;
  int i, j;

  /* make the mapping from global node to zone */
  for (j = ipm->nzones - 1, i = ipm->nnodes - 1; i >= 0; i--) {
    ipm->zone[i] = ((i == ipm->zone_init[j] - 1) ? --j : j);
  }
  /* fill in the zone_tids */
  for (i = 1; i < ipm->nzones; i++)
    ipm->zone_tids[i] = ipm->zone_tids[0]+ipm->zone_init[i];
  bcopy(ipm->tid_array, ipm->zone_tids[0], ipm->nnodes*sizeof(int));

  /* for node zero, fill in the other zone's zero node tids */
  if (ipm->mynode == 0) {
    ipm->other_zone_tids = (int *) malloc((ipm->nzones-1) * sizeof(int));
    for (i = 1; i < ipm->nzones; i++)
      ipm->other_zone_tids[i-1] = ipm->zone_tids[i][0];
    ipm->zone_synced = (unsigned int *)
      calloc(ipm->nzones,sizeof(unsigned int));
    ipm->zone_sync_cnt = (short *) calloc(32, sizeof(short));
  } else {
    ipm->other_zone_tids = NULL;
    ipm->zone_synced = NULL;
    ipm->zone_sync_cnt = NULL;
  }

  /* each zone's zero node, fill in the tids of the other nodes in the zone */
  if (ipm->myznode == 0) {
    int myzone = ipm->myzone;
    ipm->zone_other_tids = ipm->zone_tids[myzone]+1;
    ipm->synced = (unsigned int *)
      calloc(ipm->zone_siz[myzone],sizeof(unsigned int));
    ipm->sync_cnt = (short *) calloc(32,sizeof(short));
    ipm->global_synced = (unsigned int *)
      calloc(ipm->zone_siz[myzone],sizeof(unsigned int));
    ipm->global_sync_cnt = (short *) calloc(32,sizeof(short));
  } else {
    ipm->zone_other_tids = NULL;
    ipm->synced = NULL;
    ipm->sync_cnt = NULL;
    ipm->global_synced = NULL;
    ipm->global_sync_cnt = NULL;
  }

  /* fill in the stuff for the user to see */
  pm->myzone = ipm->myzone;
  pm->nzones = ipm->nzones;
  pm->mynode = ipm->myznode;
  pm->nnodes = ipm->nznodes;
}

void CreateFarmZones(pm)
     Postmaster * pm;
{
  Implementation * ipm = pm->ipm;
  int i;

  ipm->nzones = ipm->nnodes;
  make_zone_arrays(ipm);
  for (i = 0; i < ipm->nzones; i++) {
    ipm->zone_siz[i] = 1;
    ipm->zone_init[i] = i;
  }
  ipm->myznode = 0;
  ipm->nznodes = 1;
  ipm->myzone = ipm->mynode;
  fill_zone_arrays(pm);
} 

void CreateSingleZone(pm)
     Postmaster * pm;
{
  Implementation * ipm = pm->ipm;
  ipm->nzones = 1; 
  make_zone_arrays(ipm);
  ipm->zone_siz[0] = ipm->nnodes;
  ipm->zone_init[0] = 0;
  ipm->myznode = ipm->mynode;
  ipm->myzone = 0;
  ipm->nznodes = ipm->nnodes;
  fill_zone_arrays(pm);
}

void CreateMixedZones(pm, argc, argv)
     Postmaster * pm;
     int argc;
     char ** argv;
{
  Implementation * ipm = pm->ipm;
  int startnode, startzone, i, j, k, nnodes, nzones;
  /* initialize these in case we are in the zeroth zone */
  /* go through once just to get the number of zones */
  for (startnode = startzone = 0, k = 1; k < argc; k++) {
    nnodes = atoi(argv[k++]);
    nzones = atoi(argv[k]);
    if (nnodes < 1 && nzones >= 1)
      nnodes = (ipm->nnodes - startnode)/nzones;
    else if (nzones < 1 && nnodes >= 1)
      nzones = (ipm->nnodes - startnode)/nnodes;
    else if (nzones < 1 && nnodes < 1) {
      nzones = 1;
      nnodes = ipm->nnodes - startnode;
    }
    startzone += nzones;
    startnode += nnodes*nzones;
  }
  /* add in the single-node zones */
  ipm->nzones = startzone+ipm->nnodes-startnode;
  make_zone_arrays(ipm);

  /* now go through the args again, setting the size and init for each zone */
  for (startnode = startzone = 0, k = 1; k < argc; k++) {
    nnodes = atoi(argv[k++]);
    nzones = atoi(argv[k]);
    if (nnodes < 1 && nzones >= 1)
      nnodes = (ipm->nnodes - startnode)/nzones;
    else if (nzones < 1 && nnodes >= 1)
      nzones = (ipm->nnodes - startnode)/nnodes;
    else if (nzones < 1 && nnodes < 1) {
      nzones = 1;
      nnodes = ipm->nnodes - startnode;
    }
    /* fill in siz and init for the specified zones */
    for (i = startzone; i < startzone+nzones; i++) {
      ipm->zone_siz[i] = nnodes;
      if (i == 0)
	ipm->zone_init[0] = 0;
      else
	ipm->zone_init[i] =
	  ipm->zone_init[i-1] + ipm->zone_siz[i-1];
      if (ipm->mynode >= ipm->zone_init[startzone])
	ipm->nznodes = ipm->zone_siz[startzone];
    }
    startzone += nzones;
    startnode += nnodes*nzones;
  }
  if (ipm->mynode >= startnode)
    ipm->nznodes = 1;
  for (k = startnode; k < ipm->nnodes; k++) {
    ipm->zone_siz[startzone] = 1;
    ipm->zone_init[startzone++] = startnode++;
  }
  /* set myznode and myzone */
  for (ipm->myznode = -1, i = 0; i < ipm->nzones; i++)
    if (ipm->mynode >= ipm->zone_init[i] &&
	ipm->mynode < ipm->zone_init[i] + ipm->zone_siz[i]) {
      ipm->myzone = i;
      ipm->myznode = ipm->mynode - ipm->zone_init[ipm->myzone];
      break;
    }
  assert(ipm->myznode >= 0);
  fill_zone_arrays(pm);
  for (k = 0; k < ipm->nzones; k++) {
    assert(ipm->zone_siz[k] > 0);
    assert(ipm->zone_init[k] >= k);
  }
  assert(ipm->nznodes > 0);
  assert(ipm->myznode == ipm->mynode - ipm->zone_init[ipm->myzone]);
}

int CreateZones(pm, argc, argv) 
     Postmaster * pm;
     int argc;
     char ** argv;
{
  int	i, j, k;
  int	done_flag = 0;
  int	nnodes, nzones;
  int startnode, startzone;
  Implementation * ipm = pm->ipm;

  /* PRECONDITIONS */
  assert(pm != NULL);
  assert(argc >= 0);
  assert(argv != NULL);
  for (i = 0; i < argc; i++)
    { assert(argv[i] != NULL); }

  if (argc == 0) { /* farming */
    CreateFarmZones(pm);
  } else {
    switch(argv[0][1]) {
    case 'f' : /* farming, default */
      CreateFarmZones(pm);
      break;
    case 'l' : /* local : each node defines its own */
      printf("local zoning not implemented, defaulting to single zone\n");
    case 'p' : /* true parallel,  lockstep */
      CreateSingleZone(pm);
      break;
    case 'm' : /* mixed, see syntax spec */
      if (argc<3) 
	{
	  printf("usage : mixed nnodes nzones [nnodes nzones] ...\n");
	  return;
	}
      CreateMixedZones(pm,argc,argv);
      break;
    default :
      CreateFarmZones(pm);
    }
  }    
  
  /* POSTCONDITIONS */
  return 1;
}

/* return 1 if node is in pm's zone, 0 otherwise */
int ZoneNode(pm, node)
     Postmaster * pm;
     int node;
{
  if (node < pm->ipm->zone_init[pm->myzone] ||
      node >= (pm->ipm->zone_init[pm->myzone] + pm->ipm->zone_siz[pm->myzone]))
    return 0;
  return 1;
}

/* translate a zone node into a global node */
int ZoneNodeToGlobalNode(pm, znode)
     Postmaster * pm;
     int znode;
{
  if (pm->nzones == 1)
    return(znode);
  else
    return(pm->ipm->zone_init[pm->myzone] + znode);
} 

/* translate a global node into a zone node */
int GlobalNodeToZoneNode(pm, gnode)
     Postmaster * pm;
     int gnode;
{
  if (pm->nzones == 1)
    return (gnode);
  else
    return(gnode - pm->ipm->zone_init[pm->myzone]);
}
