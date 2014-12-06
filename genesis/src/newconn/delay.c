static char rcsid[] = "$Id: delay.c,v 1.3 2005/07/20 20:02:01 svitak Exp $";

/*
** $Log: delay.c,v $
** Revision 1.3  2005/07/20 20:02:01  svitak
** Added standard header files needed by some architectures.
**
** Revision 1.2  2005/06/27 19:00:43  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:29  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.11  2000/04/24 07:53:12  mhucka
** Fix from Mike Vanier: the function randomize_value had a bug, in
** failing to treat small arguments in case 2.
**
** Revision 1.10  1997/07/18 03:12:39  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.9  1996/07/23 18:31:56  venkat
** Fixed-some-typos
**
 * Revision 1.8  1996/07/23  00:38:21  dhb
 * Moved IsElementInList() to sim library.
 *
 * Revision 1.7  1996/07/22  21:45:03  dhb
 * Merged in 1.5.1.1 changes and fixed missing destpath argument in
 * VolumeDelay's initopt() string.
 *
 * Revision 1.6  1995/09/28  23:40:21  dhb
 * Added additional getopt checking for correct number of args.
 *
 * Revision 1.5.1.1  1996/07/22  21:28:44  dhb
 * Changes from Mike Vanier:
 *   Generic handling of synchan type objects
 *   Use of double in place of float in some circumstances
 *   Lots of indentation/whitespace changes
 *
 * Revision 1.5  1995/09/27  23:53:45  venkat
 * Added destpath option to planar/volumedelays and reformatted some code.
 * These fixes come from Mike Vanier.
 *
 * Revision 1.4  1995/03/28  08:05:37  mvanier
 * Added utility functions GetSynapseType and GetSynapseAddress
 * which will make it easier to make synchan variants in the
 * future.
 *
 * Revision 1.3  1995/03/28  07:13:21  mvanier
 * Made changes to allow delay functions to work
 * with synchan2 object.  Also added some utilities
 * to make such extensions easier in the future.
 *
 * Revision 1.2  1995/03/20  20:08:40  mvanier
 * Removed references to projections.
 *
 * Revision 1.1  1995/01/11  23:09:02  dhb
 * Initial revision
 *
*/

#include <math.h>
#include "newconn_ext.h"

/*
 * randomize_value
 *
 * FUNCTION
 *     Function to add a random component to a weight or delay.
 *     Note: this is only useful with floats.
 *
 * ARGUMENTS
 *     number  -- pointer to a float which will have randomness added to it.
 *     mode    -- type of randomness (see below).
 *     abs     -- whether random component is relative (0) or absolute (1).
 *     arg1    -- first argument of randomizer (float).
 *     arg2    -- second argument of randomizer (float).
 *     
 * RETURN VALUE
 *     void              
 *
 * AUTHOR
 *     Mike Vanier
 */

void randomize_value(number, mode, abs, arg1, arg2)
     float  *number;
     short   mode;
     short   abs;
     float   arg1, arg2;
{
  /* 
   * Random options:
   *
   * mode 0 --> no randomness
   * mode 1 --> uniform
   * mode 2 --> gaussian
   * mode 3 --> exponential
   */

  float scale = 0.00001, var, x, y;

  switch (mode)
    {
    case 0:	/* no randomness */		
      return;

    case 1:	/* uniform distribution */	
      /* arg1 = limit of uniform ditribution  */
      /* arg2 = irrelevant                    */
      scale = frandom(-arg1,arg1);
      break;

    case 2:	/* gaussian distribution */
      if (fabs(arg2) < TINY)  /* Don't add any randomness. */
          return;

      /* arg1 = standard deviation  */
      /* arg2 = maximum value       */
      var = arg1 * arg1;

      do
          scale = rangauss(0.0, var);
      while (fabs(scale) > fabs(arg2));

      break;

    case 3:	/* exponential distribution */
    /* arg1 = 1/e point of distribution  
     * arg2 = maximum value              
     * 
     * Note that this choice always gives larger values than the
     * original value.
     */

    do 
      {
	do 
	  x = urandom();  /* a random number between 0 and 1 */
	while (x == 0.0); /* don't want to take the log of 0 */
	y = -log(x); 

	/* 
	 * y is exponentially distributed between 0 and infinity,
	 * with the 1/e point at 1.0.
	 */

	scale = arg1 * y;  /* now the 1/e point is at arg1 */
      }
    while (scale > arg2);
    break;
  }

  if (abs)
    *number = *number + scale;
  else
    *number = *number + *number * scale;

  if (*number <= 0) *number = 0; 
}




/*
 * GetSynapseAddress
 *
 * FUNCTION
 *     To return the address of a synapse; this function makes it easy
 *     to extend the synaptic functions to new kinds of synchan/synapses.
 *
 * ARGUMENTS
 *     synchan -- address of synchan element
 *     index   -- of synapse to be returned
 *
 * RETURN VALUE
 *     Synapse *    
 *
 * AUTHORS
 *     Mike Vanier
 */

Synapse *GetSynapseAddress(synchan, index)
     struct Synchan_type *synchan;
     int                  index;
{
  unsigned short size = synchan->synapse_size;

  /* 
   * We cast the synapse pointer to a char since a char is
   * assumed to be one byte in length and synapse_size is 
   * the size of the synapse in bytes.
   */

  char *synapse = (char *)synchan->synapse;  

  return ((Synapse *) (synapse + size * index));
}




/*
 * SetDelays
 *
 * FUNCTION
 *     Function to set up delay values of a group of synapses
 *     receiving input from a given list of presynaptic elements;
 *     called from PlanarDelay and VolumeDelay
 *
 * ARGUMENTS
 *     path      -- pathname of source elements
 *     destpath  -- pathname of destination element (optional)
 *     radial    -- flag: 0 means fixed delays, 1 means scaled for distance
 *     scale     -- delay (if fixed) or propagation velocity (if radial)
 *     mode      -- type of randomness to be added (see above)
 *     absrandom -- flag: 0 means relative randomness, 1 means absolute
 *     randarg1  -- argument for randomize_value call
 *     randarg2  -- argument for randomize_value call
 *     volume    -- flag: 0 means planar delays, 1 means volume delays 
 *                  (i.e. include z axis in calculating distances)
 *     
 * RETURN VALUE
 *     void              
 *
 * AUTHORS
 *     Matt Wilson, Dave Bilitch, Mike Vanier
 */

static void SetDelays(path, destpath, radial, scale, mode,
		      absrandom, randarg1, randarg2, volume, add)
     char   *path, *destpath;
     short   radial, mode, absrandom, volume, add;
     double  scale, randarg1, randarg2;
{
  int          i, j;
  float        delay;
  double       x, y, z;
  Element     *source;
  ElementList *list, *destlist;
  MsgIn	      *mi;
  MsgOut      *mo;
  
  list = WildcardGetElement(path,1);

  if (destpath != NULL)
    destlist = WildcardGetElement(destpath, 1);
  else
    destlist = NULL;

  for (i = 0; i < list->nelements; i++)
    {
     source = list->element[i];
     x = source->x;
     y = source->y;
     z = source->z;

     MSGOUTLOOP(source, mo) 
       {
	 struct Synchan_type* synchan;
	 
	 if (!CheckClass(mo->dst, ClassID("synchannel")))
	   continue;
	 mi = (MsgIn*) GetMsgInByMsgOut(mo->dst, mo);
	 if (mi->type != SPIKE) 
	   continue;
	 
	 synchan = (struct Synchan_type*) mo->dst;
	 
	 /*
	  * Check to see if the synchan is in the destination region if any.
	  */

	 if (destlist != NULL)
	   if (!IsElementInList((Element *)synchan, destlist))
	     continue;
	 
	 /*
	  * Since the location of the synapse will differ for different
	  * synapse types (due to different-sized synapses), we use a function 
	  * call to get the address of the synapse.  This is easily extensible 
	  * for variant synchan/synapse types (like hebbsynchan) but ABSOLUTELY 
	  * REQUIRES that the mi, weight and delay fields are at the beginning 
	  * of the synapse structure.  Similarly, we can cast all synchans to
	  * a regular synchan object for the purpose of getting nsynapses
	  * provided that the nsynapses field is always in the same place in each
	  * synchan.  I realize this is pretty brittle.
	  */      
	 
	 /* Find the synapse corresponding to the SPIKE message. */
	 
	 for (j = 0; j < synchan->nsynapses; j++)
	   {
	     if (mi == GetSynapseAddress(synchan, j)->mi)
	       {
		 if (radial)
		   {
		     /*
		      * Calculate delay based on radial distance multiplied
		      * by inverse velocity.
		      */
		     if (volume) 
		       {
			 delay = (float) (sqrt(sqr(x - mo->dst->x) +
					       sqr(y - mo->dst->y) +
					       sqr(z - mo->dst->z)) / scale);
		       }
		     else /* planar */
		       {    
			 delay = (float) (sqrt(sqr(x - mo->dst->x) +
					       sqr(y - mo->dst->y)) / scale);
		       }
		   }
		 else  /* fixed */
		   { 
		     /* Give all synapses a fixed delay. */
		     delay = (float) scale; 
		   } 
		 
		 /* Add a random component to the delay. */
		 randomize_value(&delay, mode, absrandom, (float) randarg1, (float) randarg2);
		 
		 if (add)  /* add this delay to the previous value */
		   GetSynapseAddress(synchan, j)->delay += delay;
		 else     /* make this delay the delay of the synapse */
		   GetSynapseAddress(synchan, j)->delay = delay;
		 
		 break;
	       }
	   }
       }
     printf("."); fflush(stdout);
   }
  printf("\n");
  FreeElementList(list);
}




/*
 * SetSynDelays
 *
 * FUNCTION
 *     Function to set up delay values of a group of synaptic
 *     objects; called from SynDelay
 *
 * ARGUMENTS
 *     path      -- pathname of synaptic elements
 *     scale     -- delay 
 *     mode      -- type of randomness to be added (see above)
 *     absrandom -- flag: 0 means relative randomness, 1 means absolute
 *     randarg1  -- argument for randomize_value call
 *     randarg2  -- argument for randomize_value call
 *     volume    -- flag: 0 means planar delays, 1 means volume delays 
 *                  (i.e. include z axis in calculating distances)
 *     
 *     
 *
 * RETURN VALUE
 *     void              
 *
 * AUTHOR
 *     Mike Vanier
 */

static void SetSynDelays(path, scale, mode, absrandom, randarg1, randarg2, add)
     char   *path;
     short   mode, absrandom, add;
     double  scale, randarg1, randarg2;
{
  int   i,j;
  float delay;
  struct Synchan_type *synchan;
  ElementList	*list;
  
  list = WildcardGetElement(path,1);

  for (i = 0; i < list->nelements; i++)
    {
      synchan = (struct Synchan_type*)list->element[i];

      if (!CheckClass((Element *)synchan, ClassID("synchannel")))
	continue;
      
      for (j = 0; j < synchan->nsynapses; j++)
	{
	  delay = (float) scale;
	  
	  /*
	   * Add a random component to the delay.
	   */

	  randomize_value(&delay, mode, absrandom, (float) randarg1, (float) randarg2);
	  
	  if (add)  /* add this delay to the previous value */
	    GetSynapseAddress(synchan, j)->delay += delay;
	  else     /* make this delay the delay of the synapse */
	    GetSynapseAddress(synchan, j)->delay = delay;
	}
    }
  printf("\n");
  FreeElementList(list);
}




/*
 * SynDelay
 *
 * FUNCTION
 *     Command to set up delay values of a group of postsynaptic
 *     objects; usually to add a small value to the delay.
 *
 * ARGUMENTS
 *     int   argc       - number of command arguments
 *     char* argv[]     - command arguments proper
 *     
 *
 * RETURN VALUE
 *     int              
 *
 * AUTHOR
 *     Mike Vanier
 */

int SynDelay(argc,argv)
     int argc;
     char **argv;
{
  char *path;
  int     status;
  short   mode;      /* type of randomness: see below */
  short   absrandom; /* flag for options: 0 = relative, 1 = absolute randomness */
  short   randoptions;  /* counter for option */ 
  float   delay;     
  double  randarg1, randarg2; /* variables for randomness options */
  int     add;       /* flag for options: 0 = don't add delay; 1 = add delay to previous value. */
  
  mode = absrandom = randoptions = add = 0;
  randarg1 = randarg2 = 0.0;

  /* 
   * Random options:
   *
   * mode 0 --> no randomness
   * mode 1 --> uniform
   * mode 2 --> gaussian
   * mode 3 --> exponential
   */
 
  initopt(argc, argv, "path delay -add -uniform scale -gaussian stdev maxdev -exponential mid max -absoluterandom");

  while ((status = G_getopt(argc, argv)) == 1)
    {
      if (strcmp(G_optopt, "-add") == 0)
	{
	  add = 1;
	}
      /* These options affect the type of randomness added to the delays: */
      else if (strcmp(G_optopt, "-uniform") == 0)
	{
	  mode      = 1;
	  randarg1  = Atod(optargv[1]); /* range of uniform distribution */
	  randarg2  = 0.0;
	  randoptions++;
	}
      else if (strcmp(G_optopt, "-gaussian") == 0)
	{
	  mode     = 2;
	  randarg1 = Atod(optargv[1]); /* standard deviation */
	  randarg2 = Atod(optargv[2]); /* maximum absolute value of random component */
	  randoptions++;
	}
      else if (strcmp(G_optopt, "-exponential") == 0)
	{
	  mode     = 3;
	  randarg1 = Atod(optargv[1]); /* 1/e point of exponential distribution */
	  randarg2 = Atod(optargv[2]); /* maximum absolute value of random component */
	  randoptions++;
	}
      else if (strcmp(G_optopt, "-absoluterandom") == 0)
	{
	  absrandom = 1;
	}
    }

  if (status < 0)
    {
      printoptusage(argc, argv);
      return(0);
    }
 
  if (randoptions > 1)
    {
      Error();
      printf("Must have at most one of -uniform, -gaussian, -exponential options.\n");
      return(0);
    }

  path  = optargv[1];  /* path of elements */
  delay = Atod(optargv[2]);

  SetSynDelays(path, delay, mode, absrandom, randarg1, randarg2, add);

  OK();
  return(1);
}




/*
 * PlanarDelay
 *
 * FUNCTION
 *     Command to set up delay values of a group of synapses
 *     receiving input from a given list of presynaptic elements
 *     normally arranged in a planar sheet.
 *
 * ARGUMENTS
 *     int   argc       - number of command arguments
 *     char* argv[]     - command arguments proper
 *     
 *
 * RETURN VALUE
 *     int -- 1 = success; 0 = failure          
 *
 * AUTHORS
 *     Matt Wilson, Dave Bilitch, Mike Vanier
 */

int PlanarDelay(argc,argv)
     int    argc;
     char **argv;
{
  char   *path, *destpath;
  int     status;
  short   mode;      /* type of randomness: see below */
  short   radial;    /* flag for options: 0 = fixed, 1 = radial */
  short   absrandom; /* flag for options: 0 = relative, 1 = absolute randomness */
  short   delayoptions, randoptions;  /* counters for options */ 
  double  scale;     /* = delay for -fixed option or conduction velocity 
		        for -radial option */ 
  double  randarg1, randarg2; /* variables for randomness options */
  int     add;       /* flag for options: 0 = don't add delay; 1 = add delay to previous value. */
  
  mode  = radial = absrandom = delayoptions = randoptions = add = 0;
  scale = randarg1 = randarg2 = 0.0;
  
  /* 
   * Random options:
   *
   * mode 0 --> no randomness
   * mode 1 --> uniform
   * mode 2 --> gaussian
   * mode 3 --> exponential
   */
 
  initopt(argc, argv, "sourcepath [destpath] -fixed delay -radial conduction_velocity -add -uniform scale -gaussian stdev maxdev -exponential mid max -absoluterandom");

  while ((status = G_getopt(argc, argv)) == 1)
    {
      /* The following options determine the way delays are calculated: */
      if (strcmp(G_optopt, "-fixed") == 0)
	{
	  radial = 0;
	  scale = Atod(optargv[1]);
	  delayoptions++;
	}
      else if (strcmp(G_optopt, "-radial") == 0)
	{
	  radial = 1;
	  scale  = Atod(optargv[1]);
	  delayoptions++;
	}
      else if (strcmp(G_optopt, "-add") == 0)
	{
	  add = 1;
	}
      /* The remaining options affect the type of randomness added to the delays: */
      else if (strcmp(G_optopt, "-uniform") == 0)
	{
	  mode      = 1;
	  randarg1  = Atod(optargv[1]); /* range of uniform distribution */
	  randarg2  = 0.0;
	  randoptions++;
	}
      else if (strcmp(G_optopt, "-gaussian") == 0)
	{
	  mode     = 2;
	  randarg1 = Atod(optargv[1]); /* standard deviation */
	  randarg2 = Atod(optargv[2]); /* maximum absolute value of random component */
	  randoptions++;
	}
      else if (strcmp(G_optopt, "-exponential") == 0)
	{
	  mode     = 3;
	  randarg1 = Atod(optargv[1]); /* 1/e point of exponential distribution */
	  randarg2 = Atod(optargv[2]); /* maximum absolute value of random component */
	  randoptions++;
	}
      else if (strcmp(G_optopt, "-absoluterandom") == 0)
	{
	  absrandom = 1;
	}
    }
  
  if(status < 0 || optargc > 3)
    {
      printoptusage(argc, argv);
      return(0);
    }
  
  if (delayoptions != 1)
    {
      Error();
      printf("Must have exactly one of -fixed, -radial options.\n");
      return(0);
    }

  if (randoptions > 1)
    {
      Error();
      printf("Must have at most one of -uniform, -gaussian, -exponential options.\n");
      return(0);
    }
  
  path = optargv[1];  /* path of source elements */
  
  if (optargc == 3)   /* there is a destination argument */
    destpath = optargv[2];
  else
    destpath = NULL;
  
  SetDelays(path, destpath, radial, scale, mode, absrandom, randarg1, randarg2, 0, add);
  
  OK();
  return(1);
}




/*
 * VolumeDelay
 *
 * FUNCTION
 *     Command to set up delay values of a group of synapses
 *     receiving input from a given list of presynaptic elements
 *     in a given volume.
 *
 * ARGUMENTS
 *     int argc         - number of command arguments
 *     char* argv[]     - command arguments proper
 *     
 *
 * RETURN VALUE
 *     int -- 1 = success; 0 = failure                
 *
 * AUTHORS
 *     Matt Wilson, Dave Bilitch, Mike Vanier
 */

int VolumeDelay(argc,argv)
     int    argc;
     char **argv;
{
  char   *path, *destpath;
  int     status;
  short   mode;      /* type of randomness: see below */
  short   radial;    /* flag for options: 0 = fixed, 1 = radial */
  short   absrandom; /* flag for options: 0 = relative, 1 = absolute randomness */
  short   delayoptions, randoptions;  /* counters for options */ 
  double  scale;     /* = delay for -fixed option or conduction velocity 
		        for -radial option */ 
  double  randarg1, randarg2; /* variables for randomness options */
  int     add;       /* flag for options: 0 = don't add delay; 1 = add delay to previous value. */
  
  mode  = radial = absrandom = delayoptions = randoptions = add = 0;
  scale = randarg1 = randarg2 = 0.0;
  
  /* 
   * Random options:
   *
   * mode 0 --> no randomness
   * mode 1 --> uniform
   * mode 2 --> gaussian
   * mode 3 --> exponential
   */

  initopt(argc, argv, "sourcepath [destpath] -fixed delay -radial conduction_velocity -add -uniform scale -gaussian stdev maxdev -exponential mid max -absoluterandom");

 
  while ((status = G_getopt(argc, argv)) == 1)
    {
      /* The following options determine the way delays are calculated: */
      if (strcmp(G_optopt, "-fixed") == 0)
	{
	  radial = 0;
	  scale = Atod(optargv[1]);
	  delayoptions++;
	}
      else if (strcmp(G_optopt, "-radial") == 0)
	{
	  radial = 1;
	  scale  = Atod(optargv[1]);
	  delayoptions++;
	}
      else if (strcmp(G_optopt, "-add") == 0)
	{
	  add = 1;
	}
      /* The remaining options affect the type of randomness added to the delays: */
      else if (strcmp(G_optopt, "-uniform") == 0)
	{
	  mode       = 1;
	  randarg1   = Atod(optargv[1]); /* range of uniform distribution */
	  randarg2   = 0.0;
	  randoptions++;
	}
      else if (strcmp(G_optopt, "-gaussian") == 0)
	{
	  mode     = 2;
	  randarg1 = Atod(optargv[1]); /* standard deviation */
	  randarg2 = Atod(optargv[2]); /* maximum absolute value of random component */
	  randoptions++;
	}
      else if (strcmp(G_optopt, "-exponential") == 0)
	{
	  mode     = 3;
	  randarg1 = Atod(optargv[1]); /* 1/e point of exponential distribution */
	  randarg2 = Atod(optargv[2]); /* maximum absolute value of random component */
	  randoptions++;
	}
      else if (strcmp(G_optopt, "-absoluterandom") == 0)
	{
	  absrandom = 1;
	}
    }

  if (status < 0 || optargc > 3)
    {
      printoptusage(argc, argv);
      return(0);
    }
  
  if (delayoptions != 1)
    {
      Error();
      printf("Must have exactly one of -fixed, -radial options.\n");
      return(0);
    }
  
  if (randoptions > 1)
    {
      Error();
      printf("Must have at most one of -uniform, -gaussian, -exponential options.\n");
      return(0);
    }
  
  path = optargv[1];  /* path of source elements */
  
  if (optargc == 3)   /* there is a destination argument */
    destpath = optargv[2];
  else
    destpath = NULL;
  
  SetDelays(path, destpath, radial, scale, mode, absrandom, randarg1, randarg2, 1, add);

  OK();
  return(1);
}
