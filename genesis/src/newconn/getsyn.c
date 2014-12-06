static char rcsid[] = "$Id: getsyn.c,v 1.3 2005/10/04 22:05:13 svitak Exp $";

/*
** $Log: getsyn.c,v $
** Revision 1.3  2005/10/04 22:05:13  svitak
** Merged branch tagged rel-2-3beta-fixes back to main trunk. All comments from
** the branch modifications should be included.
**
** Revision 1.2.2.1  2005/09/16 08:34:25  svitak
** Fixed check of dest_index in do_getsyndest. Fixed error printfs.
**
** Revision 1.2  2005/06/27 19:00:44  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:29  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.8  2001/06/29 21:17:06  mhucka
** Bug fix from Hugo Cornelis, to test for errors.
**
** Revision 1.7  1997/07/18 03:12:39  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.6  1996/07/22 21:52:28  dhb
** Changes from Mike Vanier:
**   Generic handling of synchan types
**   Indentation/whitespace changes
**
 * Revision 1.5  1995/04/11  16:54:53  dhb
 * do_getsynsrc() and do_getsyndest() have been fixed to return
 * allocated memory.
 *
 * Revision 1.4  1995/03/28  08:04:30  mvanier
 * Made utilities work with GetSynapseType and GetSynapseAddress functions
 * in delay.c
 *
 * Revision 1.3  1995/03/27  20:27:28  mvanier
 * Made synaptic utilities work with either synchan or synchan2;
 * reformatted some code.
 *
 * Revision 1.2  1995/03/20  19:27:30  mvanier
 * Synaptic utility functions now look for spiking elements
 * instead of projection elements
 *
 * Revision 1.1  1995/01/11  23:09:02  dhb
 * Initial revision
 *
*/

#include "newconn_ext.h"

/*
 * do_getsyncount
 *
 * FUNCTION
 *     Command to get the number of synapses on a synchan or
 *     the number of SPIKE messages projecting from an element
 *     or the number of synapses projecting from a given 
 *     presynaptic element to a given synchan.
 *
 * ARGUMENTS
 *     int argc	        - number of command arguments
 *     char* argv[]	- command arguments proper
 *
 * RETURN VALUE
 *     int		- number of synapses
 *
 * AUTHOR
 *     Mike Vanier
 */

int do_getsyncount(argc, argv)
     int	argc;
     char*	argv[];
{	
  Element	       *element, *src_element, *dest_element;
  struct Synchan_type  *synchan;
  int			count;
  int                   src, dest;
  MsgIn                *msgin;
  MsgOut               *msgout;
  int                   nmsgout;
  int			i;
  
  initopt(argc, argv, "[presynaptic-element] [postsynaptic-element]");
  
  count = 0;
  src_element  = NULL;
  dest_element = NULL;
  src = dest = 0;

 
  if (argc < 2 || argc > 3)
    {
      printoptusage(argc, argv);
      return -1;
    }  

  for (i = 1; i < argc; i++)
    {
      element = GetElement(argv[i]);
      if (element == NULL)
	{
	  InvalidPath(argv[0], argv[i]);
	  return -1;
	}
      else
	{
	  if (CheckClass(element, ClassID("synchannel")))
	    {
	      ++dest;
	      dest_element = element;
	    }
	  else if (CheckClass(element, ClassID("spiking"))) 
	    {
	      ++src;
	      src_element = element;
	    }
	  else /* not a presynaptic or postsynaptic element */
	    {
	      Error();
	      printf("do_getsyncount: '%s' is not a pre- or postsynaptic element.\n", Pathname(element));
	      printoptusage(argc, argv);
	      return -1;
	    }
	}
    }

 
  /* Check for whether we have more than one pre- or postsynaptic element */

  if (src > 1 || dest > 1) 
    {
      Error();
      printf("do_getsyncount: Can't have two pre- or postsynaptic elements in the function call.\n");
      printoptusage(argc, argv);
      return -1;
    }

  count = 0;

  /*
   * The following code works for either synchan or synchan2.
   */

  if (dest && !src)
    {
      synchan = (struct Synchan_type *)dest_element;
      return synchan->nsynapses;
    }
  else if (src && !dest)
    {
      nmsgout = src_element->nmsgout;
      for (i = 0; i < nmsgout; i++)
	{
	  if ((msgout = GetMsgOutByNumber(src_element, i)) == NULL)
	    {
	      Error();
	      printf("do_getsyncount: could not get OUT msg #%d from %s\n",
		      i, Pathname(src_element));
	      return -1;
	    } 
	  msgin = GetMsgInByMsgOut(msgout->dst, msgout);
	  if (msgin->type == SPIKE)
	    ++count;
	}
      return count;
    }
  else /* both src and dest elements specified */
    {
      nmsgout = src_element->nmsgout;
      for (i = 0; i < nmsgout; i++)
	{
	  if ((msgout = GetMsgOutByNumber(src_element, i)) == NULL)
	    {
	      Error();
	      printf("do_getsyncount: could not get OUT msg #%d from %s\n",
		      i, Pathname(src_element));
	      return -1;
	    } 
	  msgin = GetMsgInByMsgOut(msgout->dst, msgout);
	  if (msgin->type == SPIKE && msgout->dst == dest_element)
	    ++count;
	}
      return count;
    }
}	/* do_getsyncount */




/*
 * do_getsynindex
 *
 * FUNCTION
 *     Command to get a synapse index in a synchan element given the
 *     source element.
 *
 * ARGUMENTS
 *     int argc	        - number of command arguments
 *     char* argv[]	- command arguments proper
 *
 * RETURN VALUE
 *     int		- Index of the specified synapse or -1 if not found.
 *
 * AUTHORS
 *     David Bilitch, Mike Vanier
 */

int do_getsynindex(argc, argv)
     int	argc;
     char*	argv[];
{	
  Element 	       *element, *src_element, *dest_element;
  struct Synchan_type  *synchan;
  int			count;
  int                   src, dest;
  int			status;
  int                   num;
  int			i;
  
  initopt(argc, argv, "presynaptic-element postsynaptic-element -num n");
  
  if (argc < 3 || argc > 5)
    {
      printoptusage(argc, argv);
      return -1;
    }
  
  num = 0;
  src_element  = NULL;
  dest_element = NULL;
  src = dest = 0;
  
  while ((status = G_getopt(argc, argv)) == 1)
    {
      if (strcmp("-num", G_optopt) == 0)
	{
	  num = atoi(optargv[1]);
	  if (num < 0)
	    {
	      Error();
	      printf("do_getsynindex: Argument to -num must be >= 0\n");
	      return -1;
	    }
	}
    }
  
  if (status < 0)
    {
      printoptusage(argc, argv);
      return -1;
    }
  
  for (i = 1; i < 3; i++)
    {
      element = GetElement(argv[i]);
      if (element == NULL)
	{
	  InvalidPath(argv[0], argv[i]);
	  return -1;
	}
      else
	{
	  if (CheckClass(element, ClassID("synchannel")))
	    {
	      ++dest;
	      dest_element = element;
	    }
	  else if (CheckClass(element, ClassID("spiking"))) 
	    {
	      ++src;
	      src_element = element;
	    }
	  else /* not a presynaptic or postsynaptic element */
	    {
	      Error();
	      printf("do_getsynindex: '%s' is not a pre- or postsynaptic element.\n", 
		     Pathname(element));
	      printoptusage(argc, argv);
	      return -1;
	    }
	}
    }
  
  /* Check for whether we have more than one pre- or postsynaptic element */
  
  if (src > 1 || dest > 1) 
    {
      Error();
      printf("Can't have two pre- or postsynaptic elements in the function call.\n");
      printoptusage(argc, argv);
      return -1;
    }
  
  count = 0;

  synchan = (struct Synchan_type *)dest_element;
  for (i = 0; i < synchan->nsynapses; i++)
    {
      if (GetSynapseAddress(synchan, i)->mi->src == src_element)
	{
	  if (count == num)
	    return i;
	  else 
	    count++;
	}
    }
  
  Error();
  printf("do_getsynindex: Synapse does not exist\n");
  return -1;
}	/* do_getsynindex */




/*
 * do_getsynsrc
 *
 * FUNCTION
 *     Command to get the source path of a synapse given the
 *     index and the destination
 *
 * ARGUMENTS
 *     int argc	        - number of command arguments
 *     char* argv[]	- command arguments proper
 *
 * RETURN VALUE
 *     char *		- pathname
 *
 * AUTHOR
 *     Mike Vanier
 */

char *do_getsynsrc(argc, argv)
     int	argc;
     char*	argv[];
{
  Element	       *element;
  MsgIn                *msgin;
  struct Synchan_type  *synchan;
  int                   index;
  int                   nsyn;
  
  initopt(argc, argv, "postsynaptic-element synapse-index");
  
  if (argc != 3)
    {
      printoptusage(argc, argv);
      return NULL;
    }
  
  element = GetElement(argv[1]);
  if (element == NULL)
    {
      InvalidPath(argv[0], argv[1]);
      return NULL;
    }
  else
    {
      if (!CheckClass(element, ClassID("synchannel")))
	{
	  Error();
	  printf("do_getsynsrc: '%s' is not a postsynaptic element.\n", Pathname(element));
	      printoptusage(argc, argv);
	      return NULL;
	}
    }

  index = atoi(argv[2]);

  synchan = (struct Synchan_type *)element; 

  nsyn    = synchan->nsynapses;

  if (index < 0)
    {
      Error();
      printf("synapse index must be >= 0\n");
      printoptusage(argc, argv);
      return NULL;
    }

  if (index >= nsyn)
    {
      Error();
      printf("synapse index out of range\n");
      printoptusage(argc, argv);
      return NULL;
    }

  synchan = (struct Synchan_type *)element;
  msgin = GetSynapseAddress(synchan, index)->mi;

  return CopyString(Pathname(msgin->src));
}




/*
 * do_getsyndest
 *
 * FUNCTION
 *     Command to get the destination path of a synapse or the
 *     synapse index given the source path
 *
 * ARGUMENTS
 *     int argc	        - number of command arguments
 *     char* argv[]	- command arguments proper
 *
 * RETURN VALUE
 *     char *		- pathname or synapse index (as a string)
 *
 * AUTHOR
 *     Mike Vanier
 */

char *do_getsyndest(argc, argv)
     int	argc;
     char*	argv[];
{
  Element	       *element, *dest_element;
  MsgIn                *msgin;
  MsgOut               *msgout;
  struct Synchan_type  *synchan;
  char                 *dest_name;
  char                  dest_num_str[20];
  int                   dest_index;
  int                   i, idx, count;
  int                   spikemsgnum;
  int                   nmsgout;
  int                   status;
  
  initopt(argc, argv, "presynaptic-element spikemsg-number -index");
  
  if (argc < 3 || argc > 4)
    {
      printoptusage(argc, argv);
      return NULL;
    }
  
  idx = 0; /* This is a flag which determines whether to return the index. */

  while ((status = G_getopt(argc, argv)) == 1)
    {
      if (strcmp("-index", G_optopt) == 0)
	idx = 1;
    }

  if (status < 0)
    {
      printoptusage(argc, argv);
      return NULL;
    }
  
  element = GetElement(argv[1]);
  if (element == NULL)
    {
      InvalidPath(argv[0], argv[1]);
      return NULL;
    }
  else
    {
      if (!CheckClass(element, ClassID("spiking")))
	{
	  Error();
	  printf("do_getsyndest: '%s' is not a presynaptic element.\n", Pathname(element));
	      printoptusage(argc, argv);
	      return NULL;
	}
    }

  spikemsgnum = atoi(argv[2]);

  if (spikemsgnum < 0)
    {
      Error();
      printf("spike message number must be >= 0\n");
      printoptusage(argc, argv);
      return NULL;
    }

  count = 0;
  dest_name  = NULL;

  /* Look for the correct SPIKE message in the message list. */

  nmsgout = element->nmsgout;
  for (i = 0; i < nmsgout; i++)
    {
      if ((msgout = GetMsgOutByNumber(element, i)) == NULL)
	{
	  Error();
	  printf("do_getsyndest: could not get OUT msg #%d from %s\n",
		 i, Pathname(element));
	  return NULL;
	} 

      msgin = GetMsgInByMsgOut(msgout->dst, msgout);

      if (msgin->type == SPIKE)
	{
	  if (count == spikemsgnum) 
	    {
	      dest_element = (Element *) msgout->dst;
	      dest_name = Pathname(dest_element);
	      break;
	    }
	  ++count;
	}
    }

  if (dest_name == NULL)
    {
      Error();
      printf("do_getsyndest: spike message number %d not found\n", spikemsgnum);
      return NULL;
    }

  if (!idx)
    return CopyString(dest_name);

  /* 
   * If -index was selected, find the index by finding which synapse's
   * msgin is equal to the one that corresponds to the msgout field
   * of the presynaptic element's appropriate spike message.
   */

  if (!CheckClass(dest_element, ClassID("synchannel")))
    {
      Error();
      printf("do_getsyndest: '%s' does not implement synapses.\n", Pathname(dest_element));
      return NULL;
    }

  dest_index = -1;
  synchan = (struct Synchan_type *)dest_element;
  for (i = 0; i < synchan->nsynapses; i++)
    {
      if (GetSynapseAddress(synchan, i)->mi == msgin)
	{
	  dest_index = i;
	  break;
	}
    }

  if (dest_index == -1)
    {
      Error();
      printf("do_getsyndest: '%s': cannot find synapse address.\n", Pathname(dest_element));
      return NULL;
    }
      
  sprintf(dest_num_str, "%d", dest_index);
  return CopyString(dest_num_str);
}


