static char rcsid[] = "$Id: misc_connfuncs.c,v 1.4 2005/07/20 20:02:01 svitak Exp $";

/*
** $Log: misc_connfuncs.c,v $
** Revision 1.4  2005/07/20 20:02:01  svitak
** Added standard header files needed by some architectures.
**
** Revision 1.3  2005/06/29 17:20:50  svitak
** Initializations of some variables to quiet compiler.
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
** Revision 1.3  1997/07/18 03:12:39  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.2  1996/07/22 22:14:16  dhb
** Changes from Mike Vanier:
**   Looks like mostly indentation/whitespace and comment convention
**     changes
**
 * Revision 1.1  1995/01/11  23:09:02  dhb
 * Initial revision
 *
*/

#include <math.h>
#include "newconn_ext.h"

/*
 * These functions are included for backwards compatibility, mainly with
 * the piriform cortex simulation.
 */

/*
 * NormalizeWeights
 *
 * FUNCTION
 *
 *	Sets the cumulative synaptic weight on a synaptic channel to a given
 *	or randomly selected value.  The proportion of cumulative weight
 *	assigned to each synapse is the same as for the previously existing
 *	weights.
 *
 * ARGUMENTS
 *
 *	argc	- The number of command arguments
 *	argv	- The command arguements
 *
 * RETURN VALUE
 *	void
 *
 * SIDE EFFECTS
 *
 *	Clears the MARKERMASK flag for all elements and then sets it
 *	for all the selected source elements leaving them set after the
 *	command.
 *
 * AUTHORS
 *	Mike Vanier, David Bilitch, Matt Wilson
 */

/*
 * Note: I got rid of the -exponential option in NormalizeWeights because 
 * (a) it had a bug, and 
 * (b) it doesn't make sense in this context.
 *
 * -- Mike
 */

void NormalizeWeights(argc,argv)
     int    argc;
     char **argv;
{
  char 	       *src;
  char	       *dst;
  MsgIn*        msg;
  ElementList  *srclist;
  ElementList  *dstlist;
  Element      *dst_element;
  int		i, j;
  int		mode = -1;
  int           status;
  short         options = 0;
  double	mean = 0.0, sd = 0.0, min = 0.0, max = 0.0;	/* initializations to quiet compiler - svi */
  double	scale = 0.0;

  initopt(argc, argv, "sourcepath targetpath -fixed scale -uniform low high -gaussian mean sd min max");
  
  while ((status = G_getopt(argc, argv)) == 1)
    {
      if (strcmp(G_optopt, "-fixed") == 0)
	{
	  mode  = 0;
	  scale = Atod(optargv[1]);
	  options++;
	}
      else if (strcmp(G_optopt, "-uniform") == 0)
	{
	  mode  = 1;
	  min   = Atod(optargv[1]); /* range of uniform distribution */
	  max   = Atod(optargv[2]);
	  options++;
	}
      else if (strcmp(G_optopt, "-gaussian") == 0)
	{
	  mode = 2;
	  mean = Atod(optargv[1]);
	  sd   = Atod(optargv[2]); /* standard deviation */
	  min  = Atod(optargv[3]); 
	  max  = Atod(optargv[4]); 
	  options++;
	}
    }
  
  if (status < 0)
    {
      printoptusage(argc, argv);
      return;
    }
  
  if (options != 1)
    {
      Error();
      printf("normalizeweights: must have exactly one of -fixed, -uniform, or -gaussian options.\n");
      return;
    }
  
  src = optargv[1];
  dst = optargv[2];
  srclist = WildcardGetElement(src,1);
  dstlist = WildcardGetElement(dst,1);  
  
  
  ClearMarkers(RootElement());
  for (i = 0; i < srclist->nelements; i++)
    {
      /*
       * Set the marker used to indicate valid targets.
       */
      srclist->element[i]->flags |= MARKERMASK;
    }
  
  /*
   * Have each destination element normalize its synapses given the
   * scale factor.
   */
  
  for (i = 0; i < dstlist->nelements; i++) 
    {
      struct Synchan_type *channel;
      double newWeightSum = 0.0;
      double sumOfWeights;
      
      dst_element = dstlist->element[i];

      if (!CheckClass(dst_element, ClassID("synchannel"))) 
	{
	  Error();
	  printf("normalizeweights: object %s is not of class synchannel!\n",
		 dst_element->name);
	  continue;
	}
      
      channel = (struct Synchan_type *) dst_element;
      
      /*
       * Calculate the desired normalization factor.
       */
      
      switch (mode) 
	{
	case 0:			/* constant */
	  newWeightSum = scale;
	  break;
	case 1:			/* uniform */
	  newWeightSum = (double) frandom((float)min, (float)max);
	  break;
	case 2:			/* gaussian */
	  newWeightSum = (double) rangauss((float)mean, sqr((float)sd));
	  break;
	}
      
      if (newWeightSum < 0.0) newWeightSum = 0.0;
      
      /*
       * First calculate existing total weight of all projections to this
       * synchannel coming from the source path.
       */
      
      j = 0;
      sumOfWeights = 0.0;
      
      MSGLOOP(channel, msg)  
	{
	case SPIKE:
	  if (msg->src->flags & MARKERMASK) 
	    sumOfWeights += (double) (GetSynapseAddress(channel, j)->weight);
	  j++;
	  break;
	}
      
      /*
       * Now set the weight of each synapse to the same fraction of the
       * new total synaptic weight that it had relative to the old total
       * synaptic weight.
       */
      
      j = 0;
      MSGLOOP(channel, msg) 
	{
	case SPIKE:
	  if (msg->src->flags & MARKERMASK) 
	    {
	      Synapse *synapse_ptr = GetSynapseAddress(channel, j); 

	      synapse_ptr->weight = 
		synapse_ptr->weight / sumOfWeights * newWeightSum;
	    }
	  j++;
	  break;
	}
    }
  FreeElementList(srclist);
  FreeElementList(dstlist);
}




/*
 * AfferentDelay
 *
 * FUNCTION
 *     This function calculates an aggregate delay between elements
 *     separated by two paths, one fast-conducting and one slow-conducting,
 *     with the second path branching from the first at an angle of 45 degrees.
 *     This function exists only for backwards compatibility with the
 *     piriform cortex simulation.  I also removed the exponential randomness
 *     options because they're bogus (i.e. the implementation was wrong).
 *
 * ARGUMENTS
 *     int argc         - number of command arguments
 *     char* argv[]     - command arguments proper
 *
 * RETURN VALUE
 *     int              
 *
 * AUTHORS
 *     Matt Wilson, Dave Bilitch, Mike Vanier
 */

/* FIXME! This function is BOGUS since it uses projections! */

int AfferentDelay(argc,argv)
     int argc;
     char **argv;
{
/* initializations to quiet compiler - svi */
  Projection *projection;
  float	 x_dst,y_dst;
  float	 vx = 0.0, vy = 0.0, tx, ty;
  float	 tan_theta;
  float	 d;
  float	 angle;
  int	 i;
  int	 vxmode, vymode;
  float	 vxmean = 0.0, vxvar = 0.0, vymean = 0.0, vyvar = 0.0;
  float	 vxlower = 0.0, vxupper = 0.0, vylower = 0.0, vyupper = 0.0;
  float	 sd;
  ElementList *list;
  MsgIn*       mi;
  MsgOut*      mo;
  char 	*path;
  int	 nxtarg;
  
  if (argc < 4)
    {
      Error();
      printf("usage: %s path angle [-vx vx][-uniformvx low high][-gaussianvx mean sd low high]\n",argv[0]);
      printf("[-vy vy][-uniformvy low high][-gaussianvy mean sd low high]\n");
      return(0);
    }

  path      = argv[1];
  angle     = Atof(argv[2]);
  tan_theta = tan(2.0 * M_PI * angle / 360.0);
  vxmode    = -1;
  vymode    = -1;
  nxtarg    = 2;

  while (++nxtarg < argc)
    {
      /* 
       * get the velocities along the main tract 
       */
      
      if (strcmp(argv[nxtarg],"-vx") == 0)
	{
	  vxmode = 0;
	  if ((vx = Atof(argv[++nxtarg])) <= 0)
	    {
	      Error();
	      printf("illegal velocity vx = %e\n",vx);
	      return(0);
	    }
	  continue;
	} 
      if (strcmp(argv[nxtarg],"-uniformvx") == 0)
	{
	  vxmode  = 1;
	  vxlower = Atof(argv[++nxtarg]);
	  vxupper = Atof(argv[++nxtarg]);
	  if (vxlower <= 0 || vxupper <= 0)
	    {
	      Error();
	      printf("illegal velocity range = %e to %e\n",vxlower,vxupper);
	      return(0);
	    }
	  continue;
	} 
      if (strcmp(argv[nxtarg],"-gaussianvx") == 0)
	{
	  vxmode  = 2;
	  vxmean  = Atof(argv[++nxtarg]);
	  sd      = Atof(argv[++nxtarg]);
	  vxlower = Atof(argv[++nxtarg]);
	  vxupper = Atof(argv[++nxtarg]);
	  vxvar   = sd * sd;
	  if (vxlower <= 0 || vxupper <= 0)
	    {
	      Error();
	      printf("illegal velocity range = %e to %e\n",vxlower,vxupper);
	      return(0);
	    }
	  continue;
	} 
      
      /* 
       * Get the velocities along the collaterals.
       */

      if (strcmp(argv[nxtarg],"-vy") == 0)
	{
	  vymode = 0;
	  if ((vy = Atof(argv[++nxtarg])) <= 0)
	    {
	      Error();
	      printf("illegal velocity vy = %e\n",vy);
	      return(0);
	    }
	  continue;
	} 

      if (strcmp(argv[nxtarg],"-uniformvy") == 0)
	{
	  vymode = 1;
	  vylower = Atof(argv[++nxtarg]);
	  vyupper = Atof(argv[++nxtarg]);
	  if (vylower <= 0 || vyupper <= 0)
	    {
	      Error();
	      printf("illegal velocity range = %e to %e\n",vylower,vyupper);
	      return(0);
	    }
	  continue;
	} 

      if (strcmp(argv[nxtarg],"-gaussianvy") == 0)
	{
	  vymode  = 2;
	  vymean  = Atof(argv[++nxtarg]);
	  sd      = Atof(argv[++nxtarg]);
	  vyvar   = sd*sd;
	  vylower = Atof(argv[++nxtarg]);
	  vyupper = Atof(argv[++nxtarg]);
	  if (vylower <= 0 || vyupper <= 0)
	    {
	      Error();
	      printf("illegal velocity range = %e to %e\n",vylower,vyupper);
	      return(0);
	    }
	  continue;
      } 
      
      Error();
      printf("%s : invalid option '%s'\n",argv[0],argv[nxtarg]);
      return(0);
    }

  if (vxmode == -1 || vymode == -1)
    {
      Error();
      printf("incomplete specification of afferent velocity parameters\n");
      return(0);
    }

  list = WildcardGetElement(path,1);
  for (i = 0; i < list->nelements; i++)
    {
      projection = (Projection *)list->element[i];
      
      switch (vxmode)
	{
	case 0:			/* constant */
	  break;
	case 1:			/* uniform  */
	  vx = frandom(vxlower,vxupper);
	  break;
	case 2:			/* gaussian */
	  do 
	    {
	      vx = rangauss(vxmean,vxvar);
	    } 
	  while (vx < vxlower || vx > vxupper);
	  break;
        }

	switch (vymode)
	  {
	  case 0:			/* constant */
	    break;
	  case 1:			/* uniform  */
	    vy = frandom(vylower,vyupper);
	    break;
	  case 2:			/* gaussian */
	    do 
	      {
		vy = rangauss(vymean,vyvar);
	      } 
	    while (vy < vylower || vy > vyupper);
	    break;
	  }

	if (vx <= 0) 
	  vx = 0;
	if (vy <= 0) 
	  vy = 0;

	MSGOUTLOOP(projection, mo) 
	  {
	    struct Synchan_type *synchan;
	    int	i;

	    synchan = (struct Synchan_type *) mo->dst;
	    if (!CheckClass((struct element_type *)synchan, ClassID("synchannel")))
		continue;
	    mi = (MsgIn*) GetMsgInByMsgOut(mo->dst, mo);
	    if (mi->type != SPIKE) 
	      continue;

	    for (i = 0; i < synchan->nsynapses; i++) 
	      {
		if (mi == synchan->synapse[i].mi)
		  {
		    x_dst = mo->dst->x;
		    y_dst = mo->dst->y;
		    d = MIN(x_dst, y_dst/tan_theta);

		    /*
		     * Calculate time delay along the main fiber tract.
		     */

		    tx = (x_dst - d)/vx;

		    /*
		     * Calculate time delay along the angled collateral.
		     */

		    ty = sqrt(y_dst * y_dst + d * d) / vy;
		    synchan->synapse[i].delay = tx + ty;
		    break;
		  }
	      }
	  }
      printf(".");
      fflush(stdout);
    }

  printf("\n");
  FreeElementList(list);
  OK();
  return(1);
}




/*
 * AfferentWeight
 *
 * FUNCTION
 *     Calculates weights of projections along a path which decline
 *     exponentially in strength with distance.  The path goes along
 *     straight for a while and then turns an angle of theta and
 *     continues again.  The function first calculates the decay
 *     of the weights along the first part of the path and then uses 
 *     that value to calculate the decay of weights along the second 
 *     part of the path.  This function exists only for backwards 
 *     compatibility with the piriform cortex simulation.
 *
 * ARGUMENTS
 *     int argc         - number of command arguments
 *     char* argv[]     - command arguments proper
 *
 * RETURN VALUE
 *     int              
 *
 * AUTHORS
 *     Matt Wilson, Dave Bilitch, Mike Vanier
 */

/* FIXME! This function is BOGUS since it uses projections! */

int AfferentWeight(argc,argv)
     int argc;
     char **argv;
{
  Projection *projection;
  float	x_dst,y_dst;
  float	tx;
  float	tan_theta;
  float	d;
  float	angle;
  int	i;
  ElementList	*list;
  MsgIn*	mi;
  MsgOut*	mo;
  char 	*path;
  float 	scale;
  float	mrate;
  float	crate;
  float	minval;
  
  if (argc < 6)
    {
      Error();
      printf("usage: %s path angle scale mainrate collrate [minval]\n",
	     argv[0]);
      return (0);
    }
  
  path  = argv[1];
  angle = Atof(argv[2]);
  tan_theta = tan(2.0 * M_PI * angle / 360.0);

  /* 
   * Get the max weight.
   */

  scale = Atof(argv[3]);

  /* 
   * Get the lambda of the collaterals.
   */

  mrate = Atof(argv[4]);
  crate = Atof(argv[5]);

  if (argc > 6)
    minval = Atof(argv[6]);
  else 
    minval = 0;

  list = WildcardGetElement(path,1);

  for (i = 0; i < list->nelements; i++)
    {
      projection = (Projection *)list->element[i];

      MSGOUTLOOP(projection, mo) 
	{
	  struct Synchan_type *synchan;
	  int i;
	
	  if (!CheckClass(mo->dst, ClassID("synchannel")))
	    continue;

	  synchan = (struct Synchan_type*) mo->dst;
	  mi      = (MsgIn*) GetMsgInByMsgOut(mo->dst, mo);

	  if (mi->type != SPIKE) 
	    continue;
	
	  for (i = 0; i < synchan->nsynapses; i++)
	    if (synchan->synapse[i].mi == mi)
	      {
		x_dst = mo->dst->x;
		y_dst = mo->dst->y;
		d = MIN(x_dst, y_dst / tan_theta);

		/*
		 * Calculate the weight distribution according to the 
		 * distance along the main fiber tract.
		 */

		tx = (scale - minval) * exp(mrate * (x_dst - d)) + minval;

		/*
		 * Calculate the weight distribution according to the 
		 * distance along the angled collateral.
		 */

		synchan->synapse[i].weight = (tx - minval) *
		  exp(crate * sqrt(y_dst * y_dst + d * d)) + minval;
		break;
	      }
	}
      printf(".");
      fflush(stdout);
    }

  printf("\n");
  FreeElementList(list);
  OK();
  return(1);
}




/*
 * ExponentialSum
 *
 * FUNCTION
 *     This function returns the sum of all the weights projecting
 *     from a given source, assuming that they were set by 
 *     planarweight with the -decay option with no randomness
 *     and with the exact same options.  This function is only included 
 *     for backwards compatibility with the piriform simulation.
 *     
 * ARGUMENTS
 *     int argc         - number of command arguments
 *     char* argv[]     - command arguments proper
 *          
 *
 * RETURN VALUE
 *     int              
 *
 * AUTHORS
 *     Matt Wilson, Dave Bilitch, Mike Vanier
 */

float ExponentialSum(argc,argv)
     int argc;
     char **argv;
{
  float        minval, maxval;
  float        x, y, rate;
  float	       px, py, sum;
  ElementList *list;
  Element     *element;
  char 	      *path;
  int	       i;
  
  initopt(argc, argv, "path rate maxvalue minvalue x y");

  if (G_getopt(argc, argv) != 0)
    {
      printoptusage(argc, argv);
      return(0);
    }
  
  path   = optargv[1];
  rate   = Atof(optargv[2]);
  maxval = Atof(optargv[3]);
  minval = Atof(optargv[4]);
  px     = Atof(optargv[5]);
  py     = Atof(optargv[6]);
  
  list = WildcardGetElement(path,1);
  sum  = 0;
  for (i = 0; i < list->nelements; i++)
    {
      element = list->element[i];
      x = element->x;
      y = element->y;

      /*
       * This assumes that the weights were set according to the radial distance 
       * between the source and destination and the propagation velocity.
       */

      sum += (maxval - minval) * 
	exp(rate * sqrt(pow((x - px), 2.0) + pow((y - py), 2.0))) + minval;
    }
  
  FreeElementList(list);
  return(sum);
}

