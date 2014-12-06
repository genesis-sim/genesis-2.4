static char rcsid[] = "$Id: delay2.c,v 1.3 2005/07/20 20:02:01 svitak Exp $";

/* 

delay3.c by Dave Beeman Oct 2014, based on delay2.c by Mike Vanier,
as revised by R. Maex bbf-uia okt96.

This version implements the volumedelay3 command with these additional
options beyond those for volumedelay2 (implemented in delay2.c):

 (1) an option "-pbc xlen ylen zlen" applies periodic boundary
     conditions to a box of dimensions (xlen, ylen, zlen). This is
     used when a network has been connected with volumeconnect3, and
     the "-pbc xlen ylen zlen" option was used, so that an edge cell
     will be connected to one on the opposite side.  Then if pbc is
     specified, a cell at a distance > xlen/2.0 is considered to be at
     distance - xlen, and likewise for ylen and zlen. If the "-radial"
     option is used with the volumedelay variants, then the delay will
     be proportional to the distance between the source (typically a
     spikegen) and the destination (typically a synchan in another
     cell). If the network has periodic bounary conditions, then a
     cell at one edge of the network might have a nearest neighbor
     that is a distance xlen, ylen, or zlen away. The "-pbc" option
     was introduced in volumedelay3 in order to correct for this
     translation when calculating the distance between connected cells.

 (2) The '-planar' option restricts distances to lie in the x-y plane, as in
     planardelay2. If the -pbc option is also used, a'zlen' parameter must
     be given, although it will be ignored.

*/

#include <math.h>
#include "newconn_ext.h"

/*
 * SetDelays3
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
 *     add       -- 0 = don't add delay; 1 = add delay to *previous value
 *     pbc       -- flag for Periodic Boundary Conditions
 *     xlen, ylen, zlen -- dimensions of the "box" for applying PBCs
 *
 * RETURN VALUE
 *     void              
 *
 * AUTHORS
 *     Matt Wilson, Dave Bilitch, Mike Vanier, Reinoud Maex
 * Modified by Dave Beeman Oct 2014
 */

static void SetDelays3(path, destpath, radial, scale, mode,
       absrandom, randarg1, randarg2, pbc, xlen, ylen, zlen, volume, add)
     char  *path, *destpath;
     short  radial, mode, absrandom, volume, add, pbc;
     float  scale, randarg1, randarg2, xlen, ylen, zlen;
{
  int          i, j;
  float        x, y, z;
  float        delay, xdist, ydist, zdist;
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
		  delay = sqrt(sqr(xdist) + sqr(ydist) + sqr(zdist))/scale;
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

	/*  printf("."); fflush(stdout); */

     } /* for destlist */

     printf("\n");
     FreeElementList(list);

} /* SetDelays3 */

/*
 * VolumeDelay3
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
 *     Matt Wilson, Dave Bilitch, Mike Vanier, Dave Beeman
 */

int VolumeDelay3(argc,argv)
     int    argc;
     char **argv;
{
  char   *path, *destpath;
  int     status;
  short   mode;      /* type of randomness: see below */
  short   radial;    /* flag for options: 0 = fixed, 1 = radial */
  short   absrandom; /* flag for options: 0 = relative, 1 = absolute randomness */
  short   pbc;       /* flag for Periodic Boundary Conditions */
  short   delayoptions, randoptions;  /* counters for options */ 
  float   scale;     /* = delay for -fixed option or conduction velocity 
		        for -radial option */ 
  float   randarg1, randarg2; /* variables for randomness options */
  int     add;       /* flag for options: 0 = don't add delay; *
		      *  1 = add delay to *previous value.     */
  float   xlen, ylen, zlen; /* dimensions of the "box" for applying PBCs */
  short   volume;    /* 0 = planardelay, 1 = volumedelay */    
  mode = radial = absrandom = delayoptions = randoptions = add = pbc = 0;
  scale = randarg1 = randarg2 = xlen = ylen = zlen = 0.0;
  volume = 1;        /* Assume that it is volumedelay, not planar */
  /* 
   * Random options:
   *
   * mode 0 --> no randomness
   * mode 1 --> uniform
   * mode 2 --> gaussian
   * mode 3 --> exponential
   */

  initopt(argc, argv, "sourcepath destpath -fixed delay -radial conduction_velocity -add -uniform scale -gaussian stdev maxdev -exponential mid max -absoluterandom -planar -pbc xlen ylen zlen");

 
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

  SetDelays3(path, destpath, radial, scale, mode, absrandom, randarg1, randarg2, volume, add, pbc, xlen, ylen, zlen);

  OK();
  return(1);
  }
