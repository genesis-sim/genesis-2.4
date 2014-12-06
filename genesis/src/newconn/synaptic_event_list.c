static char rcsid[] = "$Id: synaptic_event_list.c,v 1.3 2005/07/06 02:13:51 svitak Exp $";

/*
** $Log: synaptic_event_list.c,v $
** Revision 1.3  2005/07/06 02:13:51  svitak
** Fixed comment to point to correct doc NewSynapticObjects.txt.
**
** Revision 1.2  2005/07/01 20:20:53  svitak
** Changed *.doc references to *.txt as a consequence of .doc files
** being renamed to .txt.
**
** Revision 1.1.1.1  2005/06/14 04:38:29  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.4  1996/07/25 20:01:20  dhb
** Added routines for saving pending event list:
**   SaveSizeSynapticEvents() returns number of doubles required for the
**     save
**
** SaveSynapticEvents() does the save
**
**   RestoreSynapticEvents() does the restore
**
 * Revision 1.3  1996/07/22  22:26:29  dhb
 * Merged in 1.1.1.1 changes
 *
 * Revision 1.2  1995/09/28  00:00:12  venkat
 * Added a warning for would-be synapse hackers.
 * Removed some dead code.
 * These fixes come from Mike Vanier.
 *
 * Revision 1.1.1.1  1996/07/22  22:23:08  dhb
 * Changes from Mike Vanier for generic handling of synchan objects.
 *
 * Revision 1.1  1995/01/11  23:09:02  dhb
 * Initial revision
 *
*/

/*
 * The following functions are utilities to manage the synaptic 
 * event lists in synchan and derived objects.
 *
 * For those wishing to write their own synaptic objects, read the
 * guidelines in NewSynapticObjects.txt.
 *
 */

#include "newconn_ext.h"

/*
 * RemovePendingSynapticEvents
 *
 * FUNCTION
 *     If there are events on the pending list, put all pending
 *     event nodes back onto the free list.
 *
 * ARGUMENTS
 *     channel -- address of synchan element
 *
 * RETURN VALUE
 *     void
 *
 * AUTHOR
 *     Mike Vanier
 */

void RemovePendingSynapticEvents(channel)
     register struct Synchan_type *channel;
{
  SynapticEvent *event, *top_of_freelist;

  if (channel->PendingSynapticEvents == NULL)
    return;
    
  top_of_freelist = channel->FreeSynapticEvents;
  event = channel->PendingSynapticEvents;

  while (event->next != NULL)
    event = event->next;
    
  event->next = top_of_freelist; 

  channel->FreeSynapticEvents    = channel->PendingSynapticEvents;
  channel->PendingSynapticEvents = NULL;
  channel->pending_events = 0;
}




/*
 * FreeSynapticEventLists
 *
 * FUNCTION
 *     Frees synaptic event lists (both free and pending) for a given
 *     synchan element or elements of objects derived from synchan.
 *
 * ARGUMENTS
 *     channel -- address of synchan element
 *
 * RETURN VALUE
 *     void
 *
 * AUTHOR
 *     Mike Vanier
 */

void FreeSynapticEventLists(channel)
     register struct Synchan_type *channel;
{
  SynapticEvent *event, **addresslist;
  int i;

  RemovePendingSynapticEvents(channel);

  if (channel->FreeSynapticEvents == NULL)

    return;

  /*
   * Go through all the nodes and collect the addresses of all nodes
   * that were the first node to be allocated in a malloc.  Then
   * free all such nodes.
   */

  addresslist = (SynapticEvent **) malloc(channel->allocednodes *
					  sizeof(SynapticEvent *));

  for (event = channel->FreeSynapticEvents, i = 0; 
       event != NULL; 
       event = event->next)
    {
      if (event->alloced == 1)
	addresslist[i++] = event;
    }

  for (i = 0; i < channel->allocednodes; i++)
    free(addresslist[i]);

  free(addresslist);

  channel->event_buffer_size     = 0;
  channel->list_alloced          = 0;
  channel->allocednodes          = 0;
  channel->FreeSynapticEvents    = NULL;
  channel->PendingSynapticEvents = NULL;

}



/*
 * AllocateSynapticEventNodes
 *
 * FUNCTION
 *     This function allocates event nodes and links them into a list.
 *     It puts the first and last nodes allocated into firstnode and
 *     lastnode.  The last node points to NULL by default, but you can use
 *     lastnode to change this after the function returns.
 *
 * ARGUMENTS
 *    channel   -- address of synchan element
 *    num_nodes -- number of nodes to allocate
 *    firstnode -- address of first node allocated 
 *    lastnode  -- address of last node allocated
 *
 *    Note: firstnode and lastnode are passed as **SynapticEvent
 *          so their value can be changed (call by reference)
 *
 * RETURN VALUE
 *     int -- 0 = failure; 1 = success
 *
 * AUTHOR
 *     Mike Vanier
 */

int AllocateSynapticEventNodes(channel, num_nodes, firstnode, lastnode)
     register struct Synchan_type *channel;
     int num_nodes;
     SynapticEvent **firstnode, **lastnode;
{
  SynapticEvent *event;
  int i;

  event = (SynapticEvent *) malloc(num_nodes * sizeof(SynapticEvent));
  
  if (event == NULL)
    {
      ErrorMessage("Synchan", "Could not allocate memory for spike events.", 
		   channel);      
      return(0);
    }

  *firstnode = event;
  channel->event_buffer_size += num_nodes;
  
  /* Mark the first node as the first node allocated by malloc, for later freeing. */
  
  event->alloced = 1;
  ++channel->allocednodes;
  
  /* Link the new nodes together to make a list. */

  if (num_nodes > 1)
    {
      for (i = 0; i < (num_nodes - 1); i++)
	{
	  event->next = event + 1;
	  event++;
	  event->alloced = 0;
	}
    }

  event->next = NULL;
  *lastnode = event;

  return(1);
}

/*
 * SaveSynapticEvents
 *
 * FUNCTION
 *     Saves pending synaptic events in the SAVE2 file.  Integer values
 *     are converted to doubles as this is the only data format supported
 *     in SAVE2 files.  These values have 0.5 added to ensure conversion
 *     back to int gives the right number.
 *
 * ARGUMENTS
 *    channel   -- address of synchan element
 *    fp        -- FILE* to which to write
 *
 * RETURN VALUE
 *     None.
 *
 * AUTHOR
 *     Dave Bilitch
 */

void SaveSynapticEvents(channel, fp)
    struct Synchan_type *channel;
    FILE *fp;
{
      SynapticEvent *event;
      double	savedata[2];

      /* number of pending events */
      savedata[0] = (double)channel->pending_events + 0.5;
      fwrite(savedata,sizeof(double),1,fp);

      /* pending event arrival times and syn_nums */
      event = channel->PendingSynapticEvents;
      while (event != NULL)
	{
	  savedata[0] = (double)event->time + 0.5;
	  savedata[1] = (double)event->syn_num + 0.5;
	  fwrite(savedata,sizeof(double),2,fp);

	  event = event->next;
	}
}

/*
 * RestoreSynapticEvents
 *
 * FUNCTION
 *	Restore synaptic events for a synchan from a save file.
 *
 * ARGUMENTS
 *    channel   -- address of synchan element
 *    np	-- pointer to number of doubles left in the save file
 *    fp	-- FILE* from which to read
 *
 * RETURN VALUE
 *    int -- 0 = failure; 1 = success
 *
 * SIDE EFFECTS
 *
 *    *np     -- updated to reflect the number of remaining unread doubles
 *    channel -- event nodes will be allocated if they were not
 *		 previously; previously existing pending events are
 *               removed and any events in the save file will appear
 *		 in the pending events list
 *    fp      -- the file pointer moves to the end of the pending events
 *               in the save file (unless there is an error)
 *
 * AUTHOR
 *     Dave Bilitch
 */

int RestoreSynapticEvents(channel, np, fp)
    struct Synchan_type *channel;
    int *np;
    FILE *fp;
{
      SynapticEvent *last_event;
      SynapticEvent *event;
      SynapticEvent *lastnode;
      double savedata[2];
      int nr;
      int i;

      if (fread(savedata,sizeof(double),1,fp) != 1)
	{
	  ErrorMessage("Synchan","Invalid savedata",channel);
	  return ( 0 );
	}
      (*np)--;

      channel->pending_events = (int) savedata[0];
      if (channel->pending_events*2 != *np)
	{
	  channel->pending_events = 0;

	  ErrorMessage("Synchan","Invalid savedata",channel);
	  return ( 0 );
	}

      if (channel->list_alloced)
	  RemovePendingSynapticEvents( channel );
      else
	{
	  int allocnodes;

	  allocnodes = (int) ( channel->nsynapses * channel->nodes_per_synapse );

	  if ( allocnodes != 0 )  
	    {
	      if ( AllocateSynapticEventNodes( channel, allocnodes, 
					      &(channel->FreeSynapticEvents), &lastnode ) == 0 )
		return ( 0 );
	      
	      channel->PendingSynapticEvents = NULL;
	      channel->list_alloced = 1;
	    }
	}

      last_event = NULL;
      for (i = 0; i < channel->pending_events; i++)
	{
	  nr = fread(savedata,sizeof(double),2,fp);
	  if (nr != 2)
	    {
	      channel->pending_events = i;

	      ErrorMessage("Synchan","Invalid savedata",channel);
	      *np -= nr;
	      return ( 0 );
	    }
	  *np -= 2;

	  /* make sure syn_num refers to a valid synapse number */

	  if ((int)savedata[1] >= channel->nsynapses)
	    {
	      channel->pending_events = i;

	      ErrorMessage("Synchan","Invalid savedata",channel);
	      return ( 0 );
	    }

	  if ( channel->FreeSynapticEvents == NULL )
	    {
	      /* 
	       * Allocate more nodes in chunks of 
	       * (synapses + (nodes_per_synapse/2) + 1)
	       */

	      int allocnodes;
	      
	      allocnodes = (int) ( channel->nsynapses * 
				   channel->nodes_per_synapse / 2.0 ) + 1;

	      if ( AllocateSynapticEventNodes( channel, allocnodes, 
					       &(channel->FreeSynapticEvents ), &lastnode ) == 0 )
		return ( 0 );
	    }

	  /*
	   * Grab a new node and set its syn_num and time and link it
	   * into the pending event list.
	   */
	  
	  event = channel->FreeSynapticEvents;
	  channel->FreeSynapticEvents = event->next; 

	  event->time = (int) savedata[0];
	  event->syn_num = (int) savedata[1];

	  if (last_event == NULL)
	      channel->PendingSynapticEvents = event;
	  else
	      last_event->next = event;
	  event->next = NULL;

	  last_event = event;
	}

      return ( 1 );
}


/*
 * SaveSizeSynapticEvents
 *
 * FUNCTION
 *     Returns the number of doubles necessary to save the synaptic
 *     event data in the save file.
 *
 * ARGUMENTS
 *    channel   -- address of synchan element
 *
 * RETURN VALUE
 *     int -- number of doubles we're gonna write.
 *
 * AUTHOR
 *     Dave Bilitch
 */

int SaveSizeSynapticEvents(channel)
    struct Synchan_type	*channel;
{
	return channel->pending_events*2 + 1;
}
