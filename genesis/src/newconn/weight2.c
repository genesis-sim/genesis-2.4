static char rcsid[] = "$Id: weight2.c,v 1.3 2005/07/20 20:02:01 svitak Exp $";

/* EDS21d 97/01/28,  BBF-UIA 1/97 */

/*
** $Log: weight2.c,v $
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
** Revision 1.4  2001/06/29 21:17:38  mhucka
** Initialized some variables that gcc thought might be used before being
** initialized.
**
** Revision 1.3  2000/04/19 06:31:24  mhucka
** Patches to initopts lists from Dave Beeman dated 1 Feb. 2000.
**
** Revision 1.2  1997/07/18 03:12:39  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.1  1997/05/29 07:45:18  dhb
** Initial revision
**
**
** MAEX BBF-UIA 1996
** This is a modified version of weight.c.  The function SetWeights in
 * weight.c is rather slow. It loops through the input plane, and
 * through the msgout list of each input element (MSGOUTLOOP). For
 * each outgoing msg then, the msgin list of the destinator is looped
 * to find the corresponding incoming message (GetMsgInByMsgOut). If
 * that incoming message is a SPIKE message, the channel list of that
 * destinator is looped to find the parameters of the synapse
 * corresponding to that SPIKE message, after which finally the weight
 * field can be computed and set. This all requires four (mostly nested) loops.
 *
 * Instead, we now loop through the OUTPUT plane, and through the channel
 * list of each output element. For each synapse in that channel list, 
 * the source element can easily be identified. If the source belongs to the
 * input plane, the weight parameter is computed and set. This all
 * requires only two nested loops, BUT REQUIRES  THE DESTINATOR 
 * PLANE ARGUMENT TO BE SPECIFIED IN THE FUNCTION CALL.
 */


#include <math.h>
#include "newconn_ext.h"

/*
 * SetWeights2
 *
 * FUNCTION
 *     Function to set up weight values of a group of postsynaptic elements
 *     receiving input from a group of presynaptic elements; 
 *     called from PlanarWeight and VolumeWeight
 *
 * ARGUMENTS
 *     path      -- pathname of source elements
 *     destpath  -- pathname of destination elements (required)
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
 *     
 *
 * RETURN VALUE
 *     void              
 *
 * AUTHORS
 *     Matt Wilson, Dave Bilitch, Mike Vanier, Reinoud Maex
 */
 
static void SetWeights2(path, destpath, decay, scale, decayarg1, decayarg2,
		        mode, absrandom, randarg1, randarg2, volume)
     char *path, *destpath;
     short decay, mode, absrandom, volume;
     float scale, decayarg1, decayarg2, randarg1, randarg2;
{
  int   i,j;
  float x,y,z;
  float weight;
  float maxval, minval;
  Element       *source, *destination;
  ElementList   *list, *destlist;
  MsgIn         *mi;

  maxval = decayarg1;
  minval = decayarg2;

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

             if (decay)
		{
		  /*
		  ** set the weight according to the radial distance between
		  ** the source and dst and the velocity
		  */
		  if(volume) {
		    weight = (maxval - minval) *
		      exp(-scale * sqrt(sqr(x - destination->x) +
					sqr(y - destination->y) +
					sqr(z - destination->z))) + minval;
		  }
		  else {   /* planar */
		    weight = (maxval - minval) *
		      exp(-scale * sqrt(sqr(x - destination->x) +
					sqr(y - destination->y))) + minval;
		  }
		}
		else{ /* fixed */
		  /*
		  ** Give all synapses a fixed weight.
		  */
		  weight = scale;
		}

		/*
		** Add a random component to the weight.
		*/
		randomize_value(&weight, mode, absrandom, randarg1, randarg2);

		GetSynapseAddress(synchan, j)->weight = weight;
/*	     printf ("%d %f\n", j, weight); */
	fflush(stdout);
	    }
            printf(".");
    }
    printf("\n");
    FreeElementList(list);

}





/*
 * PlanarWeight2
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

int PlanarWeight2(argc,argv)
     int argc;
     char **argv;
{
  float 	 maxval;
  float 	 minval;
  char 		*path, *destpath;
  register float scale = 1.0;

  int     status;
  short   mode;      /* type of randomness: see below */
  short   decay;     /* flag for options: 0 = fixed, 1 = weights decay with distance */
  short   absrandom; /* flag for options: 0 = relative, 1 = absolute randomness */
  short   weightoptions, randoptions;  /* counters for options */ 
  float   randarg1, randarg2; /* variables for randomness options */

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
  
  initopt(argc, argv, "sourcepath destpath -fixed weight -decay decay_rate max_weight min_weight -uniform scale -gaussian stdev maxdev -exponential mid max -absoluterandom");

  while((status = G_getopt(argc, argv)) == 1){
    /* The following options determine the way weights are calculated: */
    if(strcmp(G_optopt, "-fixed") == 0){
      decay = 0;
      scale = Atof(optargv[1]);
      weightoptions++;
    }
    else if(strcmp(G_optopt, "-decay") == 0){
      decay  = 1;
      scale  = Atof(optargv[1]);
      maxval = Atof(optargv[2]);
      minval = Atof(optargv[3]);
      weightoptions++;
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
  
  if(weightoptions != 1){
    Error();
    printf("Must have exactly one of -fixed, -decay options.\n");
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

  SetWeights2(path, destpath, decay, scale, maxval, minval,
	     mode, absrandom, randarg1, randarg2, 0);

  OK();
  return(1);
}




/*
 * VolumeWeight2
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

int VolumeWeight2(argc,argv)
     int argc;
     char **argv;
{
  float 	 maxval;
  float 	 minval;
  char 		*path, *destpath;
  register float scale = 1.0;

  int     status;
  short   mode;      /* type of randomness: see below */
  short   decay;     /* flag for options: 0 = fixed, 1 = weights decay with distance */
  short   absrandom; /* flag for options: 0 = relative, 1 = absolute randomness */
  short   weightoptions, randoptions;  /* counters for options */ 
  float   randarg1, randarg2; /* variables for randomness options */

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
  
  initopt(argc, argv, "sourcepath destpath -fixed weight -decay decay_rate max_weight min_weight -uniform scale -gaussian stdev maxdev -exponential mid max -absoluterandom");

  while((status = G_getopt(argc, argv)) == 1){
    /* The following options determine the way weights are calculated: */
    if(strcmp(G_optopt, "-fixed") == 0){
      decay = 0;
      scale = Atof(optargv[1]);
      weightoptions++;
    }
    else if(strcmp(G_optopt, "-decay") == 0){
      decay  = 1;
      scale  = Atof(optargv[1]);
      maxval = Atof(optargv[2]);
      minval = Atof(optargv[3]);
      weightoptions++;
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
  
  if(weightoptions != 1){
    Error();
    printf("Must have exactly one of -fixed, -decay options.\n");
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

  SetWeights2(path, destpath, decay, scale, maxval, minval,
	     mode, absrandom, randarg1, randarg2, 1);

  OK();
  return(1);
}



