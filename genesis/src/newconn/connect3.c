
static char rcsid[] = "$Id: connect.c,v 1.2 2005/06/27 19:00:41 svitak Exp $";

/*
 connect3.c by Dave Beeman Sept 2014, based on connect.c by Mike Vanier.
 This implements the 'volumeconnect3' command, with some additional options
 similar to those in 'volumeweight3'.  There is no 'volumeconnect2' command.

 The new options are:

 -decay decay_rate maxval minval power 
    Rather than creating connections with a fixed probability, they are created
    with a probability (maxval - minval) * exp(-pow(scale*dist,power)) + minval
    where 'dist' is the distance from the source element (typically a spikegen
    in a soma compartment) to the destination element (typically a synchan in a
    dendritic compartment).

 -planar

    When the '-decay' option is also used, the distance along the z-axis
    is ignored, so that connection probabilities are based on the separation
    in the x-y plane. This option is useful when experimental data for
    connection probabilites are based upon the distance between parallel
    electrodes that lie normal to the cortical surface, along the z-axis.
    The connection regions continue to use the 3D coordinates, 

 -pbc xlen ylen zlen 

    Applies periodic boundary conditions to a box of dimensions xlen
    ylen zlen. Then, an edge cell will be connected to one on the
    opposite side.  Then, a cell at a distance > xlen/2.0 is
    considered to be at distance - xlen for the purpose of creating a
    connection.  This can also be used if the option "-decay
    decay_rate max_weight min_weight power" is specified.

*/

#include "newconn_ext.h"

/*
 * CoordsWithinVolumeRegionPBC
 *
 * FUNCTION
 *     Checks to see if a given x,y position pair is positioned within
 *     a single mask which specifies a region.  Uses the
 *     extended_mask_type which allows specification
 *     of rectangular or ellipsoidal regions.  Called by 
 *     IsElementWithinVolumeRegions function.
 *
 * ARGUMENTS
 *     int                        x, y   - the coordinates to test
 *     struct extended_mask_type *region - the mask
 *
 * RETURN VALUE
 *     short (boolean; either 0 or 1; returns 1 if coords are within region)
 *
 * AUTHOR
 *     Mike Vanier
 * Modified for Periodic Boundary Conditions, by Dave Beeman Sept 2014
 */

short CoordsWithinVolumeRegionPBC(x, y, z, xlen, ylen, zlen, region)
     double x, y, z, xlen, ylen, zlen;
     struct extended_mask_type *region;
{
  double x1, y1, x2, y2, z1, z2, xdist, ydist, zdist;
  x1 = region->x1;
  y1 = region->y1;
  x2 = region->x2;
  y2 = region->y2;
  z1 = region->z1;
  z2 = region->z2;

  if (region->shape == CONNECTION_MASK_BOX_SHAPE)
    { /* return 0 if any of the tests, with PBCs, fails. Note that adding
	 or subtracting a box length will make it satisfy one test, but
	 can cause it to fail another.
      */

      if  ( (x < x1) &&  (x + xlen > x2)) return 0;
      if ( (x > x2) &&  (x - xlen < x1)) return 0;
      if  ( (y < y1) &&  (y + ylen > y2)) return 0;
      if ( (y > y2) &&  (y - ylen < y1)) return 0;
      if  ( (z < z1) &&  (z + zlen > z2)) return 0;
      if ( (z > z2) &&  (z - zlen < z1)) return 0;
      return 1;
    }

  else if (region->shape == CONNECTION_MASK_ELLIPSOID_SHAPE)
    {
      if (x2 == 0. || y2 == 0. || z2 == 0.)
	return 0;

      /* Find magnitudes of distances from center of ellipsoid */
      xdist = fabs(x - x1);
      ydist = fabs(y - y1);
      zdist = fabs(z - z1);
  
      /* apply PBCs, by using the shortest distance */
      if(xdist > xlen/2.0) xdist = xdist - xlen;
      if(ydist > ylen/2.0) ydist = ydist - ylen;
      if(zdist > zlen/2.0) zdist = zdist - zlen;

      if ((sqr(xdist)/sqr(x2) + sqr(ydist)/sqr(y2) + sqr(zdist)/sqr(z2)) <= 1.0)
	return 1;
      else
	return 0;
    }

  /* This should never be reached. */
  Error();
  printf("Invalid shape specifier"); 
  return 0;
}


/*
 * IsElementWithinVolumeRegionsPBC
 *
 * FUNCTION
 *     Checks to see if a given element is positioned within
 *     an array of masks which specify a region.  Uses the
 *     extended_mask_type which allows specification
 *     of rectangular or ellipsoidal regions.  Also can use
 *     either relative or absolute coordinates.
 *
 * ARGUMENTS
 *     Element *element           - the element to test
 *     struct extended_mask_type  *region - the array of masks
 *     int      nregions          - the number of mask regions
 *     short    relative          - flag: origin is absolute or relative to 
 *                                  position of source
 *     double   srcx, srcy, srcz  - coordinates of source (irrelevant if absolute
 *                                  coordinates used)
 *                            
 *
 * RETURN VALUE
 *     short (boolean; either 0 or 1; returns 1 if element is within regions)
 *
 * AUTHOR
 *     Mike Vanier
 * Modified for Periodic Boundary Conditions, by Dave Beeman Sept 2014
 * This version is identical to IsElementWithinVolumeRegions, except that
 * it calls CoordsWithinVolumeRegionPBC instead of CoordsWithinVolumeRegion
 * and passes the xlen, ylen, and zlen parameters.
 *
 */

short IsElementWithinVolumeRegionsPBC(element, region, nregions, 
				 relative, srcx, srcy, srcz, xlen, ylen, zlen)
     Element                    *element;
     struct extended_mask_type  *region;
     int                         nregions;
     short                       relative;
     double                      srcx, srcy, srcz, xlen, ylen, zlen;
{
  int    i;
  double x, y, z;
  short  val = 0; 

  x = element->x;
  y = element->y;
  z = element->z;

  if (relative)  /* origin relative to src element */
    {
      x -= srcx;
      y -= srcy;
      z -= srcz;
    }

  /*
   * Go through all region masks.  If a mask is a "hole", return 0 
   * (not in region).  Otherwise set val to 1.  After going through 
   * all regions, return val.  Note that this means that if the mask 
   * is a "hole" it overrules those masks which are not holes that 
   * the element may be in.
   */

  val = 0;

  for (i = 0; i < nregions; i++)
    {
      if (CoordsWithinVolumeRegionPBC(x, y, z, xlen, ylen, zlen, &region[i]))
	{
	  if (region[i].type == CONNECTION_MASK_HOLE)
	    return 0;
	  else
	    val = 1;
	}
    }
  
  return val;
}


/*
 * VolumeConnect3
 *
 * FUNCTION
 *     Connects the elements of one tree to another
 *     using specified source and destination elements
 *     and the specified source and destination masks.
 *     The destination mask may be given relative to the source 
 *     element location. This is the "business end" of 
 *     do_volumeconnect, which calls this function.
 *
 * ARGUMENTS
 *     char    *source_path
 *     struct extended_mask_type *src_region
 *     int 	nsrc_regions
 *     char    *dest_path
 *     struct extended_mask_type *dst_region
 *     int 	ndst_regions
 *     double	pconnect         - connection probability
 *     short	relative         - 1 if coords relative to source element,
 *                                 else 0
 *
 * RETURN VALUE
 *     void
 *
 * NOTES
 *     VolumeConnect is now used externally from other libraries
 *     (currently only the parlib).
 *
 * AUTHORS
 *     Mike Vanier, Dave Bilitch, Matt Wilson
 *
 * Modified by Dave Beeman Sept 2014 to add options:
 *
 * "-decay decay_rate maxval minval power" - probablility decay as
 *     exp(-pow(decay_rate*distance, power))
 *
 * "-planar" - treat distances as lying in the x-y plane, for the purposes
 *     of calculating connection probabilities with "-decay"
 *
 * "-pbc xlen ylen zlen" - apply periodic boundary conditions for a
 *     box of dimensions xlen, ylen, zlen.
 */

void VolumeConnect3(source_path, src_region, nsrc_regions,
	    dest_path,   dst_region, ndst_regions,
	    pconnect, relative, planar, decay, scale, maxval, minval, power,
	    pbc, xlen, ylen, zlen)

     char 		          *source_path;
     struct extended_mask_type *src_region;
     int 			   nsrc_regions;
     char 		          *dest_path;
     struct extended_mask_type *dst_region;
     int 			   ndst_regions;
     double			   pconnect;
     short			   relative, planar; 
     short decay;
     float scale, maxval, minval, power;
     short pbc;
     double xlen, ylen, zlen;
{
  Element	       *source,  *dest;
  ElementList	       *dstlist, *srclist;
  double 		srcx, srcy, srcz;
  short debug = 0; /* Flag to turn on debugging output */
  int			i, j;
  float xdist, ydist, zdist, dist;
  srclist = WildcardGetElement(source_path,0);
  dstlist = WildcardGetElement(dest_path,0);

  if(debug) {
     printf ("relative, decay, scale, maxval minval, power, pconnect\n");
     printf ("%d %d %f %f %f %f %f\n", relative, decay, scale, maxval, minval, power, pconnect);
	printf ("pbc, xlen, ylen, zlen\n");
	printf ("%d %f %f %f\n", pbc, xlen, ylen, zlen);
  }
  for (i = 0; i < srclist->nelements; i++)
    {
      source = srclist->element[i];

      /* Make sure it's a spiking object. */   

      if (!CheckClass(source, ClassID("spiking")))
	  continue;

      /* 
       * Is it within the bounding regions? 
       * Note that the relative position arguments of the following
       * function are set to zero since the source's position 
       * is absolute.
       */

      if (!IsElementWithinVolumeRegions(
             source, 
             src_region, 
             nsrc_regions,
             0, 0.0, 0.0, 0.0))
	continue;

      /* Get the location of the source. */

      srcx = source->x;
      srcy = source->y;
      srcz = source->z;
      
      /* 
       * Go through all of the destination elements to get the
       * destination synchans. PBCs are not used for source elements.
       */

      for (j = 0; j < dstlist->nelements; j++)
	{
	  dest = dstlist->element[j];

	  /* Make sure it's a synchan or related object. */

	  if (!CheckClass(dest, ClassID("synchannel")))
	    {
	      Error();
	      printf("Destination element is not of class synchannel.\n");
	      continue;
	    }

	    /* Is it within the bounding regions? */

	  if(pbc)
	    {
	      if (!IsElementWithinVolumeRegionsPBC(
		 dest, 
		 dst_region, ndst_regions, relative, 
		 srcx, srcy, srcz, xlen, ylen, zlen))
	      continue;
	    }
	  else /* use the non-PBC function in connect.c */
	    {
	      if (!IsElementWithinVolumeRegions(
		 dest, dst_region, ndst_regions, relative, srcx, srcy, srcz))
	      continue;
	    }

	    /* 
	     * The dest element is in the destination region so make the 
	     * connection (using messages) with a likelihood based 
	     * on the connection probability.
	     */

	  /* This replaces the original code for a fixed probability  */
	  if(decay)
	    {
		  /*
		  ** set connection probability according to the
		  **  radial distance between the source and dst. 
		  */
		xdist = fabs(srcx - dest->x);
		ydist = fabs(srcy - dest->y);
		zdist = fabs(srcz - dest->z);
		if(planar) zdist = 0.0;
		if(pbc) { /* with Periodic Boundary Conditions, the
			     dest x, y, z may have been put in the
			     region by adding or subtracting a box
			     length in the test.  Use the shortest distance.
			  */
				if(xdist > xlen/2.0) xdist = xdist - xlen;
				if(ydist > ylen/2.0) ydist = ydist - ylen;
				if(zdist > zlen/2.0) zdist = zdist - zlen;
			}
		dist = sqrt(sqr(xdist) + sqr(ydist) + sqr(zdist));
		pconnect = (maxval - minval) * exp(-pow(scale*dist,power)) + minval;
	    /* else fixed - use the default pconnect to give all
	       connections a fixed probability
            */
	  } /* end if(decay) */

	if (pconnect >= 1.0 || urandom() <= pconnect)
	    {
	      AddMsg(source, dest, SPIKE, 0, NULL);
	      /* printf("%f ", pconnect); */
	    }

	}  /* end loop over dstlist */

      if (IsSilent() < 2) 
	{
	  /*	  printf("."); */
	  fflush(stdout);
	}
    } /* end loop over srclist */
	
  if (IsSilent() < 2)
    printf("\n");

  FreeElementList(dstlist);
  FreeElementList(srclist);
}

/*
 * CONNECTION COMMANDS FOR USE IN THE INTERPRETER.
 */

/*
 * do_volumeconnect3
 *
 * FUNCTION
 *     Command to connect the elements of one tree to another
 *     using specified source and destination elements
 *     and the specified source and destination masks.
 *     The destination mask may be given relative to the source 
 *     element location.  This is the volume analog of do_planarconnect,
 *     which only uses the x and y dimensions as a basis for 
 *     connecting elements.
 *
 * ARGUMENTS
 *     int argc	        - number of command arguments
 *     char* argv[]	- command arguments proper
 *
 * RETURN VALUE
 *     void
 *
 * AUTHORS
 *     Mike Vanier, Dave Beeman
 */

void do_volumeconnect3(argc,argv)
     int argc;
     char **argv;
{
  char 			     *source_name;
  char 			     *dest_name;
  int 			      src_count,dst_count;
  struct extended_mask_type  *src_mask, *dst_mask;
  double		      pconnect = 1.0;
  short			      relative = 0;
  short			      planar = 0;
  int			      status;

  src_mask = NULL;
  dst_mask = NULL;
  src_count = 0;
  dst_count = 0;
  short   decay = 0;
  short   pbc = 0;
  double   xlen, ylen, zlen;
  float   scale, maxval, minval, power;
  xlen = ylen = zlen = 0.0;

  initopt(argc, argv, "source-path dest-path -relative -planar -sourcemask {box,ellipsoid} x1 y1 z1 x2 y2 z2 -sourcehole {box,ellipsoid} x1 y1 z1 x2 y2 z2 -destmask {box,ellipsoid} x1 y1 z1 x2 y2 z2 -desthole {box,ellipsoid} x1 y1 z1 x2 y2 z2 -probability p -decay decay_rate maxval minval power -pbc xlen ylen zlen");
  
  while ((status = G_getopt(argc, argv)) == 1)
    {
      if (strcmp(G_optopt, "-relative") == 0)
	relative = 1;
      if (strcmp(G_optopt, "-planar") == 0)
	planar = 1;
      else if (strcmp(G_optopt, "-sourcemask") == 0)
	src_count++;
      else if (strcmp(G_optopt, "-sourcehole") == 0)
	src_count++;
      else if (strcmp(G_optopt, "-destmask") == 0)
	dst_count++;
      else if (strcmp(G_optopt, "-desthole") == 0)
	dst_count++;
      else if (strcmp(G_optopt, "-probability") == 0)
	pconnect = Atod(optargv[1]);
      else if (strcmp(G_optopt, "-decay") == 0) {
	decay = 1;
        scale  = Atof(optargv[1]);
        maxval = Atof(optargv[2]);
        minval = Atof(optargv[3]);
        power = Atof(optargv[4]);
      }
      else if(strcmp(G_optopt, "-pbc") == 0){
        pbc = 1;
        xlen = Atof(optargv[1]);
        ylen = Atof(optargv[2]);
        zlen = Atof(optargv[3]);
      }
    }
  
  if (status < 0)
    {
      printoptusage(argc, argv);
      return;
    }
  
  if (src_count < 1 || dst_count < 1)
    {
      Error();
      printf("Must have at least one source and one destination mask\n");
      return;
    }
  
  /*
   * Allocate memory for the appropriate number of source and destination 
   * masks.
   */

  src_mask = (struct extended_mask_type*) 
    malloc(src_count*sizeof(struct extended_mask_type));

  if (src_mask == NULL)
    {
      Error();
      perror(argv[0]);
      return;
    }
  
  dst_mask = (struct extended_mask_type*)
    malloc(dst_count*sizeof(struct extended_mask_type));

  if (dst_mask == NULL)
    {
      Error();
      perror(argv[0]);
      free(src_mask);
      return;
    }
  
  /*
   * Set up the source and destination masks.
   * Re-initialize the argument list to extract the necessary parameters.
   */

  initopt(argc, argv, "source-path dest-path -relative -planar -sourcemask {box,ellipsoid} x1 y1 z1 x2 y2 z2 -sourcehole {box,ellipsoid} x1 y1 z1 x2 y2 z2 -destmask {box,ellipsoid} x1 y1 z1 x2 y2 z2 -desthole {box,ellipsoid} x1 y1 z1 x2 y2 z2 -probability p  -decay decay_rate maxval minval power -pbc xlen ylen zlen");
  
  /* src_count and dst_count count the number of regions of each type */

  src_count = 0;
  dst_count = 0;

  while (G_getopt(argc, argv) == 1)
    {
      /*
       * "mask" is a generic mask pointer which we set to be the
       * address of the current src_mask or dst_mask.
       */

      struct extended_mask_type* mask;      
      mask = NULL;

      if (strcmp(G_optopt, "-sourcemask") == 0)
	{
	  src_mask[src_count].type = CONNECTION_MASK;
	  mask = src_mask + src_count;
	  src_count++;
	}
      else if (strcmp(G_optopt, "-destmask") == 0)
	{
	  dst_mask[dst_count].type = CONNECTION_MASK;
	  mask = dst_mask + dst_count;
	  dst_count++;
	}
      else if (strcmp(G_optopt, "-sourcehole") == 0)
	{
	  src_mask[src_count].type = CONNECTION_MASK_HOLE;
	  mask = src_mask + src_count;
	  src_count++;
	}
      else if (strcmp(G_optopt, "-desthole") == 0)
	{
	  dst_mask[dst_count].type = CONNECTION_MASK_HOLE;
	  mask = dst_mask + dst_count;
	  dst_count++;
	}
      
      if (mask != NULL)
	{
	  /* 
	   * Choose between box-type mask and ellipsoid-type mask. 
	   * You can use a shorter designation than "box" or 
	   * "ellipsoid" (for example "b" or "e") as long as
	   * all the letters of the shorter word match the
	   * corresponding letters of one of the longer words
	   * ("box" or "ellipsoid").
	   */

	  if (strncmp(optargv[1], "box", strlen(optargv[1])) == 0)
	    {
	      mask->shape = CONNECTION_MASK_BOX_SHAPE;
	    }
	  else if (strncmp(optargv[1], "ellipsoid", strlen(optargv[1])) == 0)
	    { 
	      mask->shape = CONNECTION_MASK_ELLIPSOID_SHAPE;
	    }
	  else
	    {
	      Error();
	      printf("Only \"box\" and \"ellipsoid\" mask shapes allowed.\n");
	      return;
	    }

	  /* Set the parameters of the mask. */

	  mask->x1 = Atod(optargv[2]); 
	  mask->y1 = Atod(optargv[3]); 
	  mask->z1 = Atod(optargv[4]); 
	  mask->x2 = Atod(optargv[5]); 
	  mask->y2 = Atod(optargv[6]); 
	  mask->z2 = Atod(optargv[7]); 
	}
    }

  /*
   * When all the other options have been dispensed with, what remains 
   * are the source and dest pathnames.
   */
  
  source_name = optargv[1];
  dest_name   = optargv[2];
  
  VolumeConnect3(source_name, src_mask, src_count, 
	dest_name, dst_mask, dst_count,
	pconnect, relative, planar, decay, scale, maxval, minval, power,
	pbc, xlen, ylen, zlen);
  
  free(src_mask);
  free(dst_mask);
  OK();
}
