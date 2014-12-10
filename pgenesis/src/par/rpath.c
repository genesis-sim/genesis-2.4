static char rcsid[] = "$Id: rpath.c,v 1.1.1.1 2005/06/14 04:38:39 svitak Exp $";

/*
 * $Log: rpath.c,v $
 * Revision 1.1.1.1  2005/06/14 04:38:39  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.1  1999/12/19 03:59:43  mhucka
 * Adding PGENESIS from GENESIS 2.1 CDROM release.
 *
 * Revision 1.5  1997/02/10 02:46:20  ngoddard
 * local path (no @) failed on Alpha
 *
 * Revision 1.4  1995/06/06 19:06:53  mckeon
 * Fixed slight bug when processing node specs w/o zone spec
 *
 * Revision 1.3  1995/05/25  19:30:00  mckeon
 * multiple node spec syntax now supported
 *
 * Revision 1.1  1995/04/03  22:04:35  ngoddard
 * Initial revision
 *
 * Revision 1.1  1994/09/07  19:01:00  ngoddard
 * passive messaging works, src figures map
 *
 * Revision 1.5  1993/12/30  00:06:19  ngoddard
 * Release 12/14/93
 *
 * Revision 1.2  1993/10/15  22:51:51  ngoddard
 * added Id and Log keywords
 *
 *
*/

#include <stdio.h>
#include "par_ext.h"

/*******************************************************************
**                                                                ** 
** This file contains functions for finding the components of     **
** path for a remote element. Since the parallel effort is still  **
** being evolved, all changes to the syntax should be kept        **
** separate from the rest of the code                             **
**                                                                ** 
** The present syntax is dest_path@node.zone                      **
** where both node and zone can be as follows :                   **
** positive integer : specifies a particular destination.         ** 
** '+' followed by an integer: operation to be ignored except on  **
** the specified node/zone. Thus 'command@+2' will be executed    **
** only once on node 2                                            **
** -1 : All nodes/zones                                           **
** -2 : all nodes/zones including the current one                 **
** nothing : specifies the local node/zone                        **
** node and zone ids can be used in this form in any combination, **
** except that the node id must always be there if there is going **
** to be a zone id                                                **
**                                                                ** 
**  By Upinder S. Bhalla, Caltech, June 1992                      **
** <Note: This syntax has been modified. See comment before       **
**        GetRemoteNodes() below.>                                ** 
*******************************************************************/

static int ParseNodeSpec(pm, dest_node,dest_zone,fullpath)
     Postmaster * pm;
     int		*dest_node;
     int		*dest_zone;
     char	**fullpath;
{
  int post_index;
  char	postpath[100];
  char	*temp;
  static int plusoptionwarning = 1;
  
  temp = *fullpath;
  
  if (**fullpath && (strlen(*fullpath) > 0) ) 
    {
      if (**fullpath=='+' && (strlen(*fullpath)>1)) 
	{
	  if (plusoptionwarning) 
	    {
	      printf("Warning: obselete +node usage deprecated, use 'if (mynode == node)...\n");
	      plusoptionwarning = 0;
	    }
	  (*fullpath)++;
	  *dest_node=atoi(*fullpath);
	  if (*dest_node!=pm->mynode)
	    return(0);
	} 
      else 
	{	
	  if (**fullpath == '-' || (**fullpath >= '0' && **fullpath <= '9'))
	    *dest_node=atoi(*fullpath);
	  else 
	    if (strncmp(*fullpath, "all",3) == 0)
	      *dest_node = -2;
	    else 
	      if (strncmp(*fullpath, "others", 5) == 0)
		*dest_node = -1;
	      else
		return 0;
	}
    } 
  else 
    return 0;
    
  
  while(**fullpath) 
    {
      if ((**fullpath=='.') || (**fullpath==',')) break;
      (*fullpath)++;
    }
  if (**fullpath && **fullpath=='.' && strlen(*fullpath) > 1) 
    {
      (*fullpath)++;
      if (**fullpath=='+' && strlen(*fullpath)>1) 
	{
	  if (plusoptionwarning) 
	    {
	      printf("Warning: obselete +zone usage deprecated, use 'if (myzone == zone)...\n");
	      plusoptionwarning = 0;
	    }
	  (*fullpath)++;
	  *dest_zone=atoi(*fullpath);
	  if (*dest_zone!=pm->myzone)
	    return(0);
	} 
      else 
	{
	  if (**fullpath == '-' || (**fullpath >= '0' && **fullpath <= '9'))
	    *dest_zone=atoi(*fullpath);
	  else 
	    if (strncmp(*fullpath, "all",3) == 0)
	      *dest_zone = -2;
	    else 
	      if (strncmp(*fullpath, "others", 5) == 0)
		*dest_zone = -1;
	      else
		return 0;
	}
    } 
  else 
    *dest_zone=pm->myzone;
    

  while(**fullpath) 
    {
      if(**fullpath==',') 
	{ 
	  (*fullpath)++; 
	  break; 
	}
      (*fullpath)++;
    }

  if (*dest_zone >=-2 && *dest_zone<0 &&
      *dest_node >=-2 && *dest_node<pm->totalnodes)
    return(1);			/* We assume it knows where it is going */
  if (*dest_zone >=0 && *dest_zone<pm->nzones) 
    {
      if (*dest_node>=-2 && *dest_node<0)
	return(1);		/* We assume it knows where it is going */
      if (*dest_node>=0 &&
	  *dest_node <pm->ipm->zone_siz[*dest_zone])
	return(1);		/* Specific destination is valid */
    }
  if (IsSilent() < 2) 
    printf("Invalid remote path %s\n",temp);
  return(0);
}

/*

The syntax is @node.zone,node.zone,node.zone... where a value of -1
means "all others" and a value of -2 means "all".  -1.-2 means every
node but me.  -1.-1 means every node except the one with my local
number in every other zone.  -2.-2 means every node.  -2.-1 means
every node in every other zone.  If -1 is given for node then there
must be a node whose local number corresponds to my local number in
every zone that is specified.  There must exist at least one node in
each node.zone spec.  Keywords "other" and "all" can be used instead
of -1 and -2.

GetRemoteNodes returns 0 if it fails, one otherwise.  It has out args
dst_path, pnode_cnt, pdst_nodes and nonlocal.  It mallocs space for the
list of node numbers and frees/mallocs next time if more is needed.  Thus
the caller need not worry about freeing pdst_nodes.

If my node is in the node list, it is in the first position.

*/

/* NOTE: this is NOT reentrant!! */
int GetRemoteNodes(pm, full_path, dst_path, pnode_cnt, pdst_nodes, nonlocal)
     Postmaster * pm;
     char * full_path;
     char * dst_path;		/* local path */
     int * pnode_cnt;		/* ptr to size of node/tid vectors */
     int ** pdst_nodes;		/* ptr to vector of nodes */
     int * nonlocal;		/* out param: 1 => out of zone spec */
{
  static int dst_node;
  static int dst_zone;
  static int dst_nodes_siz=0;
  static int *dst_nodes;
  static char *checklist=NULL;  /* Keeps track of whether or not a node is to be returned
				   in the node list. If(checklist[nd]) then nd will 
				   be returned in *pdst_nodes[] */
  int totalnodes=0;
  int node_cnt, i, j, k;
  char *temp, c;

  Implementation * ipm = pm->ipm;


  /* copy the first part (incl. @ or \0) of the fullpath into the dest_path */
  while((c = *dst_path++ = *full_path++) != '\0' && c != '@');
  if (c == '@')			/* back up and terminate the pre-@ string */
    *--dst_path = '\0';
  else
    {  /* If there is no @ symbol, it is on this node and zone */
      if (dst_nodes_siz<1)
	dst_nodes = (int *)(malloc((dst_nodes_siz=1)*sizeof(int)));
      *pnode_cnt = 1;
      dst_nodes[0] = ipm->zone_init[pm->myzone] + pm->mynode;
      *pdst_nodes = dst_nodes;
      *nonlocal = 0;
      return(1);
    }

  assert(*(full_path-1) == '@');
  if (!full_path)    /* Parse error if there's nothing after the '@' */
    return(0);


  /* Allocate and set the boolean array */
  if (checklist == NULL)
    checklist =(char *)(calloc(ipm->nnodes,sizeof(char)));
  memset(checklist, 0, (ipm->nnodes)*sizeof(char));
   
  /* Parse each node spec on the input string */ 
  while(ParseNodeSpec(pm,&dst_node,&dst_zone,&full_path))
    {  
      *nonlocal = 0;			/* assume local */
      if (dst_zone >= 0 ||
	  (dst_zone == -1 && ipm->nzones == 2) ||
	  (dst_zone == -2 && ipm->nzones == 1)) 
	{
	  /* one zone */
	  if (dst_zone == -2) 
	    {
	      dst_zone = ipm->myzone;
	    } 
	  else 
	    if (dst_zone == -1) 
	      {
		dst_zone = (ipm->myzone == 0); /* 0 if myzone 1, 1 if myzone zero */
	      }
	  if (dst_zone != ipm->myzone) 
	    {
	      *nonlocal = 1;
	    }
	  if (dst_node >= 0 ||
	      (dst_node == -1 && ipm->zone_siz[dst_zone] == 2) ||
	      (dst_node == -2 && ipm->zone_siz[dst_zone] == 1)) 
	    {
	      /* single node */
	      if (dst_node >= 0) 
		{
		  dst_node = ipm->zone_init[dst_zone]+dst_node; /* node number */
		} 
	      else 
		if (dst_node == -2) 
		  {
		    dst_node = ipm->zone_init[dst_zone];
		  } 
		else 
		  {			/* dst_node == -1 */
		    if (pm->mynode == 0)
		      dst_node = ipm->zone_init[dst_zone]+1;
		    else 
		      if (pm->mynode == 1)
			dst_node = ipm->zone_init[dst_zone];
		      else
			/* no corresponding node number in zone */
			return 0;
		  }
	      if (!checklist[dst_node])  /* If we haven't seen this node yet, */
		{                       /*  check it off and continue.       */
		  checklist[dst_node] = 1;
		  totalnodes++;
		}
	    } 
	  else 
	    {
	      /* multiple nodes, one zone */
	      if (dst_node == -1 && pm->mynode >= ipm->zone_siz[dst_zone])
		return 0;		/* no such node in zone */
	      else 
		{
		  node_cnt = ipm->zone_siz[dst_zone]-(dst_node == -1);
		  
		  for (i = 0; i < node_cnt+(dst_node == -1); i++) 
		    {
		      if (dst_node == -1 && i == pm->mynode)
			/* Skip my node if appropriate */
			continue; 

		      if (!checklist[ipm->zone_init[dst_zone]+i]) 
			{
			  checklist[ipm->zone_init[dst_zone]+i] = 1;
			  totalnodes++;
			}
		    }
		}
	    }
	}
      else 
	{
	  /* multiple zones */
	  if (dst_node >= 0) 
	    {	
	      /* single node per zone */
	      node_cnt = ipm->nzones-(dst_zone == -1);

	      for (i = j = 0; i < node_cnt+(dst_zone == -1); i++, j++) 
		{
		  if (dst_zone == -1 && i == pm->myzone) 
		    /* Skip my zone if appropriate */
		    continue; 
		  
		  if (dst_node >= ipm->zone_siz[i])
		    {
		      /* No such node in zone */
		      free(dst_nodes);
		      return 0;
		    }
		  
		  if (!checklist[ipm->zone_init[i]+dst_node])
		    {
		      checklist[ipm->zone_init[i]+dst_node] = 1;
		      totalnodes++;
		    }
		}
	    }
	 
	  else 
	    {			/* dst_node < 0 */
	      /* all or all except my zone and all or all except my node*/
	      if (dst_zone == -2) 
		{	/* every zone */
		  if (dst_node == -2) 
		    {	    /* everynode */
		      node_cnt = ipm->nnodes;
		    } 
		  else 
		    {		/* every node but me */
		      node_cnt = ipm->nnodes - ipm->nzones;
		    }
		} 
	      else 
		{		/* every zone but mine */
		  if (dst_node == -2) 
		    {	    /* everynode */
		      node_cnt = ipm->nnodes - ipm->zone_siz[ipm->myzone];
		    } 
		  else 
		    {		/* every node but me */
		      node_cnt = ipm->nnodes - ipm->zone_siz[ipm->myzone] -
			ipm->nzones + 1;
		    }
		}

	      for (i = 0; i < ipm->nzones; i++) 
		{
		  if (dst_zone == -1 && i == ipm->myzone) 
		  /* skip my zone if appropriate*/
		    continue; 
		    
		  if (dst_node == -1 && pm->mynode >= ipm->zone_siz[i])
		    {
		      /* no such node in zone */
		      free(dst_nodes);
		      return 0;
		    }
		  
		  for (j = 0; j < ipm->zone_siz[i]; j++) 
		    {
		      if (dst_node == -1 && j == pm->mynode) 
			/* Skip my node if appropriate */
			continue; 
			
		      if (!checklist[ipm->zone_init[i]+j])
			{
			  checklist[ipm->zone_init[i]+j] = 1;
			  totalnodes++;
			}
		    }
		}
	    }
	}
    }


  if (dst_nodes_siz < totalnodes)
    dst_nodes = (int *)(calloc(dst_nodes_siz = totalnodes, sizeof(int)));

  if (checklist[ipm->mynode])
    {
      /* If my node's on the checklist, put me first */
      dst_nodes[0] = ipm->mynode;
      i = 1;
    }
  else
    i = 0;

  /* Go down the checklist */
  for (j=0; (j<ipm->nnodes) && (i<dst_nodes_siz); j++)
    {
      if (j==ipm->mynode)
	/* Skip me */
	continue;
      if(checklist[j])
	dst_nodes[i++] = j;
    }

  *pnode_cnt = totalnodes;
  *pdst_nodes = dst_nodes;

  for (i = 0; i < dst_nodes_siz; i++)
    assert(dst_nodes[i] >= 0 && dst_nodes[i] < ipm->nnodes);
    
  return 1;
}
