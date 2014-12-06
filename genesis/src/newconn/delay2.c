static char rcsid[] = "$Id: delay2.c,v 1.3 2005/07/20 20:02:01 svitak Exp $";

/* revised by r. maex bbf-uia okt96, see weight2.c for motivation */

/*
** $Log: delay2.c,v $
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
** Revision 1.4  2000/04/19 06:31:24  mhucka
** Patches to initopts lists from Dave Beeman dated 1 Feb. 2000.
**
** Revision 1.3  1997/07/29 15:14:29  dhb
** Removed call to defunct GetSynapseType() and fixed calling of
** GetSynapseAddress().
**
** Revision 1.2  1997/07/29 14:38:01  dhb
** getopt, optopt --> G_getopt, G_optopt
**
** Revision 1.1  1997/07/29 00:53:03  dhb
** Initial revision
**
 * Revision 1.6  1995/09/28  23:40:21  dhb
 * Added additional getopt checking for correct number of args.
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
 * SetDelays2
 *
 * FUNCTION
 *     Function to set up delay values of a group of synapses
 *     receiving input from a given list of presynaptic elements;
 *     called from PlanarDelay and VolumeDelay
 *
 * ARGUMENTS
 *     path      -- pathname of source elements
 *     destpath  -- pathname of destination element (required)
 *     radial    -- flag: 0 means fixed delays, 1 means scaled for distance
 *     scale     -- delay (if fixed) or propagation velocity (if radial)
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
 * AUTHORS
 *     Matt Wilson, Dave Bilitch, Mike Vanier, Reinoud Maex
 */

static void SetDelays2(path, destpath, radial, scale, mode,
		      absrandom, randarg1, randarg2, volume, add)
     char  *path, *destpath;
     short  radial, mode, absrandom, volume, add;
     float  scale, randarg1, randarg2;
{
  int          i, j;
  float        x, y, z;
  float        delay;
  Element     *source, *destination;
  ElementList *list, *destlist;
  MsgIn	      *mi;
  
  list = WildcardGetElement(path,1);

  if (destpath != NULL)
    destlist = WildcardGetElement(destpath, 1);
  else
    destlist = NULL;


    for(i=0;i<destlist->nelements;i++)
       {
        struct Synchan_type*    synchan;

	destination = destlist->element[i];

        if (!CheckClass(destination, ClassID("synchannel")))
		continue;

        synchan = (struct Synchan_type*) destination;

	for (j = 0; j < synchan->nsynapses; j++)
            {
             mi = GetSynapseAddress(synchan, j)->mi;

             if(mi->type != SPIKE) 
	      continue;

             source = mi->src;

             if (list != NULL)
	      if (!IsElementInList((Element *)source, list))
		continue;

             x = source->x;
             y = source->y;
             z = source->z;


              if (radial)
		{
		  /*
		   * Calculate delay based on radial distance multiplied
		   * by inverse velocity.
		   */
		  if (volume) 
		    {
		      delay = sqrt(sqr(x - destination->x) +
				   sqr(y - destination->y) +
				   sqr(z - destination->z)) / scale;
		    }
		  else /* planar */
		    {   
		      delay = sqrt(sqr(x - destination->x) +
				   sqr(y - destination->y)) / scale;
		    }
		}
	      else  /* fixed */
		{ 
		  /* Give all synapses a fixed delay. */
		  delay = scale; 
		} 
	      
	      /* Add a random component to the delay. */
	      randomize_value(&delay, mode, absrandom, randarg1, randarg2);
	      
	      if(add)  /* add this delay to the previous value */
		GetSynapseAddress(synchan, j)->delay += delay;
	      else     /* make this delay the delay of the synapse */
		GetSynapseAddress(synchan, j)->delay = delay;
	      

            } /* for synapse */

            printf("."); fflush(stdout);

     } /* for destlist */

     printf("\n");
     FreeElementList(list);

} /* SetDelays2 */





/*
 * PlanarDelay2
 *
 * FUNCTION
 *     Command to set up delay values of a group of synapses
 *     receiving input from a given list of presynaptic elements
 *     normally arranged in a planar sheet.
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

int PlanarDelay2(argc,argv)
     int    argc;
     char **argv;
{
  char   *path, *destpath;
  int     status;
  short   mode;      /* type of randomness: see below */
  short   radial;    /* flag for options: 0 = fixed, 1 = radial */
  short   absrandom; /* flag for options: 0 = relative, 1 = absolute randomness */
  short   delayoptions, randoptions;  /* counters for options */ 
  float   scale;     /* = delay for -fixed option or conduction velocity 
		        for -radial option */ 
  float   randarg1, randarg2; /* variables for randomness options */
  int     add;       /* flag for options: 0 = don't add delay; 1 = add delay to *
		      *                   previous value.                       */
  
  mode = radial = absrandom = delayoptions = randoptions = add = 0;
  scale = randarg1 = randarg2 = 0.0;
  
  /* 
   * Random options:
   *
   * mode 0 --> no randomness
   * mode 1 --> uniform
   * mode 2 --> gaussian
   * mode 3 --> exponential
   */
 
  initopt(argc, argv, "sourcepath destpath -fixed delay -radial conduction_velocity -add -uniform scale -gaussian stdev maxdev -exponential mid max -absoluterandom");

  while((status = G_getopt(argc, argv)) == 1){
    /* The following options determine the way delays are calculated: */
    if(strcmp(G_optopt, "-fixed") == 0){
      radial = 0;
      scale = Atof(optargv[1]);
      delayoptions++;
    }
    else if(strcmp(G_optopt, "-radial") == 0){
      radial = 1;
      scale  = Atof(optargv[1]);
      delayoptions++;
    }
    else if(strcmp(G_optopt, "-add") == 0){
      add = 1;
    }
    /* The remaining options affect the type of randomness added to the delays: */
    else if(strcmp(G_optopt, "-uniform") == 0){
      mode       = 1;
      randarg1   = Atof(optargv[1]); /* range of uniform distribution */
      randarg2   = 0.0;
      randoptions++;
    }
    else if(strcmp(G_optopt, "-gaussian") == 0){
      mode     = 2;
      randarg1 = Atof(optargv[1]); /* standard deviation */
      randarg2 = Atof(optargv[2]); /* maximum absolute value of random component */
      randoptions++;
    }
    else if(strcmp(G_optopt, "-exponential") == 0){
      mode     = 3;
      randarg1 = Atof(optargv[1]); /* 1/e point of exponential distribution */
      randarg2 = Atof(optargv[2]); /* maximum absolute value of random component */
      randoptions++;
    }
    else if(strcmp(G_optopt, "-absoluterandom") == 0){
      absrandom = 1;
    }
  }

  if(status < 0 || optargc > 3){
    printoptusage(argc, argv);
    return(0);
  }
  
  if(delayoptions != 1){
    Error();
    printf("Must have exactly one of -fixed, -radial options.\n");
    return(0);
  }

  if(randoptions > 1){
    Error();
    printf("Must have at most one of -uniform, -gaussian, -exponential options.\n");
    return(0);
  }

  path = optargv[1];  /* path of source elements */

  if (optargc == 3)   /* there is a destination argument */
    destpath = optargv[2];
  else
    destpath = NULL;

  SetDelays2(path, destpath, radial, scale, mode, absrandom, randarg1, randarg2, 0, add);

  OK();
  return(1);
}




/*
 * VolumeDelay2
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

int VolumeDelay2(argc,argv)
     int    argc;
     char **argv;
{
  char   *path, *destpath;
  int     status;
  short   mode;      /* type of randomness: see below */
  short   radial;    /* flag for options: 0 = fixed, 1 = radial */
  short   absrandom; /* flag for options: 0 = relative, 1 = absolute randomness */
  short   delayoptions, randoptions;  /* counters for options */ 
  float   scale;     /* = delay for -fixed option or conduction velocity 
		        for -radial option */ 
  float   randarg1, randarg2; /* variables for randomness options */
  int     add;       /* flag for options: 0 = don't add delay; 1 = add delay to *
		      *                   previous value.                       */
  
  mode = radial = absrandom = delayoptions = randoptions = add = 0;
  scale = randarg1 = randarg2 = 0.0;
  
  /* 
   * Random options:
   *
   * mode 0 --> no randomness
   * mode 1 --> uniform
   * mode 2 --> gaussian
   * mode 3 --> exponential
   */

  initopt(argc, argv, "sourcepath destpath -fixed delay -radial conduction_velocity -add -uniform scale -gaussian stdev maxdev -exponential mid max -absoluterandom");

 
  while((status = G_getopt(argc, argv)) == 1){
    /* The following options determine the way delays are calculated: */
    if(strcmp(G_optopt, "-fixed") == 0){
      radial = 0;
      scale = Atof(optargv[1]);
      delayoptions++;
    }
    else if(strcmp(G_optopt, "-radial") == 0){
      radial = 1;
      scale  = Atof(optargv[1]);
      delayoptions++;
    }
    else if(strcmp(G_optopt, "-add") == 0){
      add = 1;
    }
    /* The remaining options affect the type of randomness added to the delays: */
    else if(strcmp(G_optopt, "-uniform") == 0){
      mode       = 1;
      randarg1   = Atof(optargv[1]); /* range of uniform distribution */
      randarg2   = 0.0;
      randoptions++;
    }
    else if(strcmp(G_optopt, "-gaussian") == 0){
      mode     = 2;
      randarg1 = Atof(optargv[1]); /* standard deviation */
      randarg2 = Atof(optargv[2]); /* maximum absolute value of random component */
      randoptions++;
    }
    else if(strcmp(G_optopt, "-exponential") == 0){
      mode     = 3;
      randarg1 = Atof(optargv[1]); /* 1/e point of exponential distribution */
      randarg2 = Atof(optargv[2]); /* maximum absolute value of random component */
      randoptions++;
    }
    else if(strcmp(G_optopt, "-absoluterandom") == 0){
      absrandom = 1;
    }
  }

  if(status < 0 || optargc > 3){
    printoptusage(argc, argv);
    return(0);
  }
  
  if(delayoptions != 1){
    Error();
    printf("Must have exactly one of -fixed, -radial options.\n");
    return(0);
  }

  if(randoptions > 1){
    Error();
    printf("Must have at most one of -uniform, -gaussian, -exponential options.\n");
    return(0);
  }

  path = optargv[1];  /* path of source elements */

  if (optargc == 3)   /* there is a destination argument */
    destpath = optargv[2];
  else
    destpath = NULL;

  SetDelays2(path, destpath, radial, scale, mode, absrandom, randarg1, randarg2, 1, add);

  OK();
  return(1);
}
