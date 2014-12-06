static char rcsid[] = "$Id: weight.c,v 1.3 2005/07/20 20:02:01 svitak Exp $";

/*
** $Log: weight.c,v $
** Revision 1.3  2005/07/20 20:02:01  svitak
** Added standard header files needed by some architectures.
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
** Revision 1.9  1997/07/18 03:12:39  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.8  1996/07/23 18:31:56  venkat
** Fixed-some-typos
**
 * Revision 1.7  1996/07/22  23:36:47  dhb
 * Merged in 1.1.1.1 changes
 *
 * Revision 1.6  1995/09/28  23:40:21  dhb
 * Added additional getopt checking for correct number of args.
 *
 * Revision 1.1.1.1  1996/07/22  22:52:26  dhb
 * Changes from Mike Vanier:
 *   Synchan2 --> Synchan
 *   Generic handling of synchan types
 *
 * Revision 1.5  1995/09/27  23:55:59  venkat
 * Added destpath option to planar/volumeweight and reformatted some code.
 * These fixes come from Mike Vanier.
 *
 * Revision 1.4  1995/03/28  22:32:52  mvanier
 * Fixed planar/volumeweight to work with synchan2 as well
 * as synchan object.
 *
 * Revision 1.3  1995/03/20  20:00:56  mvanier
 * Made PlanarWeight and VolumeWeight always return a value.
 *
 * Revision 1.2  1995/03/20  19:53:18  mvanier
 * Got rid of references to "projections".
 *
 * Revision 1.1  1995/01/11  23:09:02  dhb
 * Initial revision
 *
*/

#include <math.h>
#include "newconn_ext.h"

/*
 * SetWeights
 *
 * FUNCTION
 *     Function to set up weight values of a group of postsynaptic elements
 *     receiving input from a group of presynaptic elements; 
 *     called from PlanarWeight and VolumeWeight
 *
 * ARGUMENTS
 *     path      -- pathname of source elements
 *     destpath  -- pathname of destination elements (optional)
 *     decay     -- flag: 0 means fixed weights, 1 means weights decay with distance
 *     scale     -- weight (if fixed) or decay_rate (if decay)
 *     decayarg1 -- first argument to decay option
 *     decayarg2 -- second argument to decay option
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
 
static void SetWeights(path, destpath, decay, scale, decayarg1, decayarg2,
		        mode, absrandom, randarg1, randarg2, volume)
     char   *path, *destpath;
     short   decay, mode, absrandom, volume;
     double  scale, decayarg1, decayarg2;
     double  randarg1, randarg2;
{
  int           i,j;
  float         weight;  
  double        x,y,z;
  double        maxval, minval;
  Element      *source;
  ElementList  *list, *destlist;
  MsgIn        *mi;
  MsgOut       *mo;

  maxval = decayarg1;
  minval = decayarg2;

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
	      if (mi == GetSynapseAddress(synchan, j)->mi)
		{
		  /*
		   * Note: all calculations are done with doubles and then
		   * the resulting weight value is converted to a float.
		   */

		  if (decay)
		    {
		      /*
		       * set the weight according to the radial distance between
		       * the source and destination elements and the velocity
		       */
		      if (volume) 
			{
			  weight = (float) ((maxval - minval) *
					    exp(-scale * sqrt(sqr(x - mo->dst->x) +
							      sqr(y - mo->dst->y) +
							      sqr(z - mo->dst->z))) 
					    + minval);
			}
		      else /* planar */
			{   
			  weight = (float) ((maxval - minval) *
					    exp(-scale * sqrt(sqr(x - mo->dst->x) +
							      sqr(y - mo->dst->y))) 
					    + minval);
			}
		    }
		  else /* fixed */
		    {
		      /*
		       * Give all synapses a fixed weight.
		       */
		      weight = (float) scale;
		    }
		  
		  /*
		   * Add a random component to the weight.
		   */
		  randomize_value(&weight, mode, absrandom, (float) randarg1, (float) randarg2);
		  GetSynapseAddress(synchan, j)->weight = weight;		  
		  break;
		}
	    
	  }
	printf(".");
	fflush(stdout);
      }
  printf("\n");
  FreeElementList(list);
}





/*
 * PlanarWeight
 *
 * FUNCTION
 *     Command to set up weight values.  The presynaptic elements
 *     projecting to these synapses are generally in a planar sheet.
 *     In practice this means that the z-coordinate is ignored.
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

int PlanarWeight(argc,argv)
     int argc;
     char **argv;
{
  double  minval, maxval;
  char 	 *path, *destpath;
  double  scale = 0.0;
  int     status;
  short   mode;      /* type of randomness: see below */
  short   decay;     /* flag for options: 0 = fixed, 1 = weights decay with distance */
  short   absrandom; /* flag for options: 0 = relative, 1 = absolute randomness */
  short   weightoptions, randoptions;  /* counters for options */ 
  double  randarg1, randarg2; /* variables for randomness options */

  mode = decay = absrandom = weightoptions = randoptions = 0;
  randarg1 = randarg2 = maxval = minval = 0.0;

  /* 
   * Random options:
   *
   * mode 0 --> no randomness
   * mode 1 --> uniform
   * mode 2 --> gaussian
   * mode 3 --> exponential
   */
  
  initopt(argc, argv, "sourcepath [destpath] -fixed weight -decay decay_rate max_weight min_weight -uniform scale -gaussian stdev maxdev -exponential mid max -absoluterandom");

  while ((status = G_getopt(argc, argv)) == 1)
    {
      /* The following options determine the way weights are calculated: */
      if (strcmp(G_optopt, "-fixed") == 0)
	{
	  decay = 0;
	  scale = Atod(optargv[1]);
	  weightoptions++;
	}
      else if (strcmp(G_optopt, "-decay") == 0)
	{
	  decay  = 1;
	  scale  = Atod(optargv[1]);
	  maxval = Atod(optargv[2]);
	  minval = Atod(optargv[3]);
	  weightoptions++;
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
  
  if (weightoptions != 1)
    {
      Error();
      printf("Must have exactly one of -fixed, -decay options.\n");
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

  SetWeights(path, destpath, decay, scale, maxval, minval,
	     mode, absrandom, randarg1, randarg2, 0);

  OK();
  return(1);
}




/*
 * VolumeWeight
 *
 * FUNCTION
 *     Command to set up weight values.  The presynaptic elements
 *     projecting to these synapses are in a given volume.
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

int VolumeWeight(argc,argv)
     int argc;
     char **argv;
{
  double  minval, maxval;
  char 	 *path, *destpath;
  double  scale = 0.0;

  int     status;
  short   mode;      /* type of randomness: see below */
  short   decay;     /* flag for options: 0 = fixed, 1 = weights decay with distance */
  short   absrandom; /* flag for options: 0 = relative, 1 = absolute randomness */
  short   weightoptions, randoptions;  /* counters for options */ 
  double  randarg1, randarg2; /* variables for randomness options */

  mode = decay = absrandom = weightoptions = randoptions = 0;
  randarg1 = randarg2 = maxval = minval = 0.0;

  /* 
   * Random options:
   *
   * mode 0 --> no randomness
   * mode 1 --> uniform
   * mode 2 --> gaussian
   * mode 3 --> exponential
   */
  
  initopt(argc, argv, "sourcepath [destpath] -fixed weight -decay decay_rate max_weight min_weight -uniform scale -gaussian stdev maxdev -exponential mid max -absoluterandom");

  while ((status = G_getopt(argc, argv)) == 1)
    {
      /* The following options determine the way weights are calculated: */
      if (strcmp(G_optopt, "-fixed") == 0)
	{
	  decay = 0;
	  scale = Atod(optargv[1]);
	  weightoptions++;
	}
      else if (strcmp(G_optopt, "-decay") == 0)
	{
	  decay  = 1;
	  scale  = Atod(optargv[1]);
	  maxval = Atod(optargv[2]);
	  minval = Atod(optargv[3]);
	  weightoptions++;
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
  
  if (weightoptions != 1)
    {
      Error();
      printf("Must have exactly one of -fixed, -decay options.\n");
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

  SetWeights(path, destpath, decay, scale, maxval, minval,
	     mode, absrandom, randarg1, randarg2, 1);

  OK();
  return(1);
}



