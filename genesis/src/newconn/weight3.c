static char rcsid[] = "$Id: weight2.c,v 1.3 2005/07/20 20:02:01 svitak Exp $";

/*
 Initial version of weight3.c by Dave Beeman Sept 2014, based on weight2.c
 by Erik De Schutter 97/01/28,  BBF-UIA 1/97, and Renoud Maex  BBF-UIA 1996.

 This version implements the volumeweight3 command with these changes from
 volumeweight2:

 (1) The option '-decay decay_rate max_weight min_weight power' now has a 4th
     argument for the power of distance dependence of the exponential decay of
     weights. The weight is set to 

     (max_weight - min_weight) * exp(-pow(distance*decay_rate,power)) + min_weight,

     where 'distance' is the distance from the source element
     (typically a spikegen in a soma compartment) to the destination
     element (typically a synchan in a dendritic compartment). 
     volumeweight2 assumes a power of 1.

 (2) an option "-pbc xlen ylen zlen" to apply periodic boundary
     conditions to a box of dimensions xlen ylen zlen. This is only
     useful in the case where the connectivity is all-to-all, or when
     volumeconnect3 is used with the "-pbc" option, so that an edge
     cell will be connected to one on the opposite side.  Then if pbc
     is specified, a cell at a distance > xlen/2.0 is considered to be
     at distance - xlen, and likewise for ylen and zlen, for the
     purpose of applying a decay in the weights with distance. This
     option can only be used with "-decay decay_rate max_weight
     min_weight power".

 (3) The '-planar' option restricts distances to lie in the x-y plane, as in
     planarweight2. If the -pbc option is also used, a'zlen' parameter must
     be given, although it will be ignored.

 (4) The annoying dots are no longer sent to stdout.

 The code for planarweight2 has NOT been modified to create a "planarweight3".
 Instead, use the -planar option to volumeweight3.

*/

#include <math.h>
#include "newconn_ext.h"

/*
 * SetWeights3
 *
 * FUNCTION
 *     Function to set up weight values of a group of postsynaptic elements
 *     receiving input from a group of presynaptic elements; 
 *     called from PlanarWeight and VolumeWeight
 *
 * ARGUMENTS
 *     path      -- pathname of source elements
 *     destpath  -- pathname of destination elements (required)
 *     decay     -- flag: 0 means fixed weights, > 0 means decay with distance
 *                  When the '-decay' option is used with volumeweight3, the decay
                    argument will have the value of the 'power' argument to '-decay'
 *     scale     -- weight (if fixed) or decay_rate (if decay)
 *     decayarg1 -- max_weight argument to decay option
 *     decayarg2 -- min_weight argument to decay option
 *     power     -- power to raise (decay_rate*distance) in exonential decay
 *     mode      -- type of randomness to be added (see above)
 *     absrandom -- flag: 0 means relative randomness, 1 means absolute
 *     randarg1  -- argument for randomize_value call
 *     randarg2  -- argument for randomize_value call
 *     volume    -- flag: 0 means planar delays, 1 means volume delays 
 *                  (i.e. include z axis in calculating distances)
 *     pbc       -- flag: 1 = use Periodic Boundary Conditions, 0 = no PBCs
 *     xlen, ylwn, zlen -- dimensions of the "box" for applying PBCs.
 *
 * RETURN VALUE
 *     void              
 *
 * AUTHORS
 *     Matt Wilson, Dave Bilitch, Mike Vanier, Reinoud Maex
 *     Modified by Dave Beeman September 2014.    
 */
 
static void SetWeights3(path, destpath, decay, scale, decayarg1, decayarg2, power,
	 mode, absrandom, randarg1, randarg2, pbc, xlen, ylen, zlen, volume)
     char *path, *destpath;
     short decay, mode, absrandom, volume, pbc;
     float scale, decayarg1, decayarg2, power, randarg1, randarg2, xlen, ylen, zlen;
{
  short debug = 0; /* Flag to turn on debugging output */
  int   i,j;
  float x,y,z;
  float weight;
  float maxval, minval;
  float xdist, ydist, zdist, dist;
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
		  ** the source and dst. 
		  */
		xdist = fabs(x - destination->x);
		ydist = fabs(y - destination->y);
		zdist = fabs(z - destination->z);
		if(!volume) zdist = 0.0;
		if(pbc) { /* with Periodic Boundary Conditions, the
			     destination x, y, z may have been translated
			     by a box length. Use the shortest distance. */
				if(xdist > xlen/2.0) xdist = xdist - xlen;
				if(ydist > ylen/2.0) ydist = ydist - ylen;
				if(zdist > zlen/2.0) zdist = zdist - zlen;
			}
		dist = sqrt(sqr(xdist) + sqr(ydist) + sqr(zdist));
		weight = (maxval - minval) * exp(-pow(scale*dist,power)) + minval;
	        } /* end if(decay) */

		else{ /* fixed */
		  /*
		  ** Give all synapses a fixed weight.
		  */
		  weight = scale;
		}

		/*
		** Add a random component to the weight. (Do nothing if mode = 0)
		*/
		randomize_value(&weight, mode, absrandom, randarg1, randarg2);

		GetSynapseAddress(synchan, j)->weight = weight;
	fflush(stdout);
	    }  /* end of for loop over destination synchan synapses */
       }  /* end of for loop over destination elements */
    FreeElementList(list);

    if(debug)
      {
	printf ("decay, scale, decayarg1, decayarg2, volume\n");
	printf ("%d %f %f %f %d\n", decay, scale, decayarg1, decayarg2, volume);
	printf ("mode, pbc, xlen, ylen, zlen\n");
	printf ("%d %d %f %f %f\n",  mode, pbc, xlen, ylen, zlen);
      }
}
            

/*
 * VolumeWeight3
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
 *
 * Modified Nov by Dave Beeman to allow use of option
 * '-pbc xlen ylen zlen' for periodic boundary conditions.
 * '-decay decay_rate max_weight min_weight power'
 * now has a 4th argument for the power of of the exponential
 * decay in weight with distance.
 * '-planar' restricts distances to lie in the x-y plane as in planarweight2.
 */

int VolumeWeight3(argc,argv)
     int argc;
     char **argv;
{
  float 	 maxval;
  float 	 minval;
  char 		*path, *destpath;
  float scale = 1.0;

  int     status;
  short   volume;    /* 0 = planarweight, 1 = volumeweight */
  short   mode;      /* type of randomness: see below */
  short   decay;     /* flag for options: 0 = fixed weights */
  /* 1 = exponential decay  with distance */
  /* > 1 = exponential decay as pow(distance, decay) */
  short   absrandom; /* flag for options: 0 = relative, 1 = absolute randomness */
  short   weightoptions, randoptions;  /* counters for options */ 
  float   randarg1, randarg2; /* variables for randomness options */
  float power;
  short   pbc;  /* flag = 1 to use Periodic Boundary Conditions */
  float   xlen, ylen, zlen; /* 'box' dimensions for PBCs */
  mode = decay = absrandom = weightoptions = randoptions = pbc = 0;
  randarg1 = randarg2 = maxval = minval = power = xlen = ylen = zlen = 0.0;
  volume = 1; /* assume that it is a volumeweight, not planar */
  /* 
   * Random options:
   *
   * mode 0 --> no randomness
   * mode 1 --> uniform
   * mode 2 --> gaussian
   * mode 3 --> exponential
   */
  
  initopt(argc, argv, "sourcepath destpath -fixed weight -decay decay_rate max_weight min_weight power -uniform scale -gaussian stdev maxdev -exponential mid max -absoluterandom -planar -pbc xlen ylen zlen");

  while((status = G_getopt(argc, argv)) == 1){
    /* The following options determine the way weights are calculated: */
    if(strcmp(G_optopt, "-fixed") == 0){
      decay = 0;
      scale = Atof(optargv[1]);
      weightoptions++;
    }
    else if(strcmp(G_optopt, "-decay") == 0){
      decay = 1;
      scale  = Atof(optargv[1]);
      maxval = Atof(optargv[2]);
      minval = Atof(optargv[3]);
      power = Atof(optargv[4]);
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

    else if(strcmp(G_optopt, "-planar") == 0){
      volume = 0;
    }

    else if(strcmp(G_optopt, "-pbc") == 0){
      pbc = 1;
      xlen = Atof(optargv[1]);
      ylen = Atof(optargv[2]);
      zlen = Atof(optargv[3]);
    }
  }

  if(status < 0 || optargc > 3){
    printoptusage(argc, argv);
    return(0);
  } /* end while looo */
  
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

  SetWeights3(path, destpath, decay, scale, maxval, minval, power,
	     mode, absrandom, randarg1, randarg2, pbc, xlen, ylen, zlen, volume);

  OK();
  return(1);
}
