static char rcsid[] = "$Id: connect.c,v 1.2 2005/06/27 19:00:41 svitak Exp $";

/*
** $Log: connect.c,v $
** Revision 1.2  2005/06/27 19:00:41  svitak
** Added explicit types to untyped functions and fixed return values as
** appropriate. Initialized values explicitly when it was unclear if
** they were being used uninitialized (may not apply to all files being
** checked in in this batch).
**
** Revision 1.1.1.1  2005/06/14 04:38:29  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.10  2000/07/09 23:57:28  mhucka
** New function "fileconnect" from Upi Bhalla.
**
** Revision 1.9  1997/07/18 19:54:20  dhb
** Merged in 1.7.1.1 changes
**
** Revision 1.8  1997/07/18 03:12:39  dhb
** Fix for getopt problem; getopt(), optopt and optind are now
** G_getopt(), G_optopt and G_optind.
**
** Revision 1.7.1.1  1997/07/18 15:49:57  dhb
** Changes from PSC: sanity checks to avoid division by zero.
**
** Revision 1.7  1996/07/22 21:49:34  dhb
** Changed from Mike Vanier:
**   Using double in place of floats in various circumstances
**   Indentation/whitespace changes
**
 * Revision 1.6  1995/07/13  19:36:17  dhb
 * VolumeConnect() wasn't setting the mask coordinates properly, missing
 * the z components.
 *
 * Revision 1.5  1995/07/08  19:20:49  dhb
 * Removed static qualifier from VolumeConnect() so external
 * modules can call the function.  This is done primarily
 * for the parlib rvolumeconnect command.
 *
 * Revision 1.4  1995/03/28  22:39:24  mvanier
 * Cosmetic changes.
 *
 * Revision 1.3  1995/03/20  20:11:38  mvanier
 * Removed all references to projections.
 *
 * Revision 1.2  1995/03/20  19:28:27  mvanier
 * Changed VolumeConnect to look for elements of class "spiking"
 * instead of class "projection".  Also got rid of redundant code
 * relating to old region_connect syntax.  Also cleaned up code.
 *
 * Revision 1.1  1995/01/11  23:09:02  dhb
 * Initial revision
 *
*/

#include "newconn_ext.h"

/*
 * CoordsWithinPlanarRegion
 *
 * FUNCTION
 *     Checks to see if a given x,y position pair is positioned within
 *     a single mask which specifies a region.  Uses the
 *     extended_mask_type which allows specification
 *     of rectangular or elliptical regions.  Called by 
 *     IsElementWithinPlanarRegions function.
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
 */

short CoordsWithinPlanarRegion(x, y, region)
     double x, y;
     struct extended_mask_type *region;
{
  double x1, y1, x2, y2;
  x1 = region->x1;
  y1 = region->y1;
  x2 = region->x2;
  y2 = region->y2;

  if (region->shape == CONNECTION_MASK_BOX_SHAPE)
    {
      if (x >= x1 && x <= x2 &&
	  y >= y1 && y <= y2)
	return 1;
      else
	return 0;
    }
  else if (region->shape == CONNECTION_MASK_ELLIPSE_SHAPE)
    {
      if ((sqr(x-x1)/sqr(x2) +
	   sqr(y-y1)/sqr(y2)) <= 1.0)
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
 * CoordsWithinVolumeRegion
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
 */

short CoordsWithinVolumeRegion(x, y, z, region)
     double x, y, z;
     struct extended_mask_type *region;
{
  double x1, y1, x2, y2, z1, z2;
  x1 = region->x1;
  y1 = region->y1;
  x2 = region->x2;
  y2 = region->y2;
  z1 = region->z1;
  z2 = region->z2;

  if (region->shape == CONNECTION_MASK_BOX_SHAPE)
    {
      if (x >= x1 && x <= x2 &&
	  y >= y1 && y <= y2 &&
	  z >= z1 && z <= z2)
	return 1;
      else
	return 0;
    }
  else if (region->shape == CONNECTION_MASK_ELLIPSOID_SHAPE)
    {
      if (x2 == 0. || y2 == 0. || z2 == 0.)
	return 0;
      if ((sqr(x-x1)/sqr(x2) +
	   sqr(y-y1)/sqr(y2) +
	   sqr(z-z1)/sqr(z2)) <= 1.0)
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
 * IsElementWithinPlanarRegions
 *
 * FUNCTION
 *     Checks to see if a given element is positioned within
 *     an array of masks which specify a region.  Uses the
 *     extended_mask_type which allows specification
 *     of rectangular or elliptical regions.  Also can use
 *     either relative or absolute coordinates.
 *
 * ARGUMENTS
 *     Element *element     - the element to test
 *     struct extended_mask_type  *region - the array of masks
 *     int      nregions    - the number of mask regions
 *     short    relative    - flag: origin is absolute or relative to 
 *                                  position of source
 *     double   srcx, srcy  - coordinates of source (irrelevant if absolute
 *                            coordinates used)
 *                            
 *
 * RETURN VALUE
 *     short (boolean; either 0 or 1; returns 1 if element is within regions)
 *
 * AUTHOR
 *     Mike Vanier
 */

short IsElementWithinPlanarRegions(element, region, nregions, 
				   relative, srcx, srcy)
     Element                    *element;
     struct extended_mask_type  *region;
     int                         nregions;
     short                       relative;
     double                      srcx, srcy;
{
  int    i;
  double x, y;
  short  val = 0; 

  x = element->x;
  y = element->y;

  if (relative)  /* origin relative to src element */
    {
      x -= srcx;
      y -= srcy;
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
      if (CoordsWithinPlanarRegion(x, y, &region[i]))
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
 * IsElementWithinVolumeRegions
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
 */

short IsElementWithinVolumeRegions(element, region, nregions, 
				   relative, srcx, srcy, srcz)
     Element                    *element;
     struct extended_mask_type  *region;
     int                         nregions;
     short                       relative;
     double                      srcx, srcy, srcz;
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
      if (CoordsWithinVolumeRegion(x, y, z, &region[i]))
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
 * PlanarConnect
 *
 * FUNCTION
 *     Connects the elements of one tree to another
 *     using specified source and destination element lists
 *     and the specified source and destination masks.
 *     The destination mask may be given relative to the source 
 *     element location. This is the "business end" of 
 *     do_planarconnect, which calls this function.
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
 * AUTHORS
 *     Mike Vanier, Dave Bilitch, Matt Wilson
 */


static void PlanarConnect(source_path, src_region, nsrc_regions,
			  dest_path,   dst_region, ndst_regions,
			  pconnect, relative)
     char 		          *source_path;
     struct extended_mask_type *src_region;
     int 			   nsrc_regions;
     char 		          *dest_path;
     struct extended_mask_type *dst_region;
     int 			   ndst_regions;
     double			   pconnect;
     short			   relative; 
{
  Element	       *source,  *dest;
  ElementList	       *srclist, *dstlist;
  double 		srcx, srcy;
  int			i, j;

  srclist = WildcardGetElement(source_path,0);
  dstlist = WildcardGetElement(dest_path,0);

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

      if (!IsElementWithinPlanarRegions(
             source, 
             src_region, 
             nsrc_regions,
             0, 0.0, 0.0))
	continue;

      /* Get the location of the source. */

      srcx = source->x;
      srcy = source->y;

      /* 
       * Go through all of the destination elements
       * to get the destination synchans.
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

          if (!IsElementWithinPlanarRegions(
		 dest, 
		 dst_region, 
		 ndst_regions,
		 relative, 
		 srcx, srcy))
	      continue;

	    /* 
	     * The dest element is in the destination region so make the 
	     * connection (using messages) with a likelihood based 
	     * on the connection probability.
	     */
	  
	  if (pconnect >= 1.0 || urandom() <= pconnect)
	    {
	      AddMsg(source, dest, SPIKE, 0, NULL);
	    }
	}
      
      if (IsSilent() < 2) 
	{
	  printf(".");
	  fflush(stdout);
	}
    }
	
  if (IsSilent() < 2)
    printf("\n");

  FreeElementList(dstlist);
  FreeElementList(srclist);
}




/*
 * VolumeConnect
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
 */

void VolumeConnect(source_path, src_region, nsrc_regions,
			  dest_path,   dst_region, ndst_regions,
			  pconnect, relative)
     char 		          *source_path;
     struct extended_mask_type *src_region;
     int 			   nsrc_regions;
     char 		          *dest_path;
     struct extended_mask_type *dst_region;
     int 			   ndst_regions;
     double			   pconnect;
     short			   relative; 
{
  Element	       *source,  *dest;
  ElementList	       *dstlist, *srclist;
  double 		srcx, srcy, srcz;
  int			i, j;

  srclist = WildcardGetElement(source_path,0);
  dstlist = WildcardGetElement(dest_path,0);
  
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
       * Go through all of the destination elements
       * to get the destination synchans.
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

          if (!IsElementWithinVolumeRegions(
		 dest, 
		 dst_region, 
		 ndst_regions,
		 relative, 
		 srcx, srcy, srcz))
	      continue;

	    /* 
	     * The dest element is in the destination region so make the 
	     * connection (using messages) with a likelihood based 
	     * on the connection probability.
	     */
	  
	  if (pconnect >= 1.0 || urandom() <= pconnect)
	    {
	      AddMsg(source, dest, SPIKE, 0, NULL);
	    }
	}
      
      if (IsSilent() < 2) 
	{
	  printf(".");
	  fflush(stdout);
	}
    }
	
  if (IsSilent() < 2)
    printf("\n");

  FreeElementList(dstlist);
  FreeElementList(srclist);
}




/*
 * CONNECTION COMMANDS FOR USE IN THE INTERPRETER.
 */

/*
 * do_planarconnect
 *
 * FUNCTION
 *     Command to connect the elements of one tree to another
 *     using specified source and destination objects and the 
 *     specified source and destination masks.  The destination 
 *     mask may be given relative to the source element location.  
 *
 * ARGUMENTS
 *     int argc	        - number of command arguments
 *     char* argv[]	- command arguments proper
 *
 * RETURN VALUE
 *     void
 *
 * AUTHORS
 *     Mike Vanier, Dave Bilitch, Matt Wilson
 */

void do_planarconnect(argc,argv)
     int argc;
     char **argv;
{
  char 			     *source_name;
  char 			     *dest_name;
  int 			      src_count,dst_count;
  struct extended_mask_type  *src_mask, *dst_mask;
  double		      pconnect = 1.0;
  short			      relative = 0;
  int			      status;

  src_mask = NULL;
  dst_mask = NULL;
  src_count = 0;
  dst_count = 0;
  
  initopt(argc, argv, "source-path dest-path -relative -sourcemask {box,ellipse} x1 y1 x2 y2 -sourcehole {box,ellipse} x1 y1 x2 y2 -destmask {box,ellipse} x1 y1 x2 y2 -desthole {box,ellipse} x1 y1 x2 y2 -probability p");
  
  while ((status = G_getopt(argc, argv)) == 1)
    {
      if (strcmp(G_optopt, "-relative") == 0)
	relative = 1;
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

  initopt(argc, argv, "source-path dest-path -relative -sourcemask {box,ellipse} x1 y1 x2 y2 -sourcehole {box,ellipse} x1 y1 x2 y2 -destmask {box,ellipse} x1 y1 x2 y2 -desthole {box,ellipse} x1 y1 x2 y2 -probability p");
  
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
	   * Choose between box-type mask and ellipse-type mask. 
	   * You can use a shorter designation than "box" or 
	   * "ellipse" (for example "b" or "e") as long as
	   * all the letters of the shorter word match the
	   * corresponding letters of one of the longer words
	   * ("box" or "ellipse").
	   */

	  if (strncmp(optargv[1], "box", strlen(optargv[1])) == 0)
	    {
	      mask->shape = CONNECTION_MASK_BOX_SHAPE;
	    }
	  else if (strncmp(optargv[1], "ellipse", strlen(optargv[1])) == 0)
	    { 
	      mask->shape = CONNECTION_MASK_ELLIPSE_SHAPE;
	    }
	  else
	    {
	      Error();
	      printf("Only \"box\" and \"ellipse\" mask shapes allowed.\n");
	      return;
	    }

	  /* Set the parameters of the mask. */

	  mask->x1 = Atod(optargv[2]); 
	  mask->y1 = Atod(optargv[3]); 
	  mask->x2 = Atod(optargv[4]); 
	  mask->y2 = Atod(optargv[5]); 
	}
    }

  /*
   * When all the other options have been dispensed with, what remains 
   * are the source and dest pathnames.
   */
  
  source_name = optargv[1];
  dest_name   = optargv[2];
  
  PlanarConnect(source_name, src_mask, src_count, 
		dest_name,   dst_mask, dst_count,
		pconnect, relative);
  
  free(src_mask);
  free(dst_mask);
  OK();
}




/*
 * do_volumeconnect
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
 *     Mike Vanier
 */

void do_volumeconnect(argc,argv)
     int argc;
     char **argv;
{
  char 			     *source_name;
  char 			     *dest_name;
  int 			      src_count,dst_count;
  struct extended_mask_type  *src_mask, *dst_mask;
  double		      pconnect = 1.0;
  short			      relative = 0;
  int			      status;

  src_mask = NULL;
  dst_mask = NULL;
  src_count = 0;
  dst_count = 0;
  
  initopt(argc, argv, "source-path dest-path -relative -sourcemask {box,ellipsoid} x1 y1 z1 x2 y2 z2 -sourcehole {box,ellipsoid} x1 y1 z1 x2 y2 z2 -destmask {box,ellipsoid} x1 y1 z1 x2 y2 z2 -desthole {box,ellipsoid} x1 y1 z1 x2 y2 z2 -probability p");
  
  while ((status = G_getopt(argc, argv)) == 1)
    {
      if (strcmp(G_optopt, "-relative") == 0)
	relative = 1;
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

  initopt(argc, argv, "source-path dest-path -relative -sourcemask {box,ellipsoid} x1 y1 z1 x2 y2 z2 -sourcehole {box,ellipsoid} x1 y1 z1 x2 y2 z2 -destmask {box,ellipsoid} x1 y1 z1 x2 y2 z2 -desthole {box,ellipsoid} x1 y1 z1 x2 y2 z2 -probability p");
  
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
  
  VolumeConnect(source_name, src_mask, src_count, 
		dest_name,   dst_mask, dst_count,
		pconnect, relative);
  
  free(src_mask);
  free(dst_mask);
  OK();
}

/*
 * do_fileconnect
 *
 * FUNCTION
 *     Command to connect the elements of one tree to another
 *     using specified source and destination object lists and one big
 *     file with weights for all NxM possible connections.
 *     This is extremely general and extremely inefficient, as
 *     almost all possible connections will have weight 0, ie., be
 *     nonexistent.
 *     Specified weights can be scaled with the optional 'wtscale'
 *     argument.
 *     The command ignores negative weights as well as zeros.
 *     Negative weights are often used to indicate inhibitory input,
 *     so the command provides a way around that: use a negative
 *     wtscale argument. If a file has +ve as well as -ve weights,
 *     it would be read twice: first with the paths of excitatory
 *     receptor channels and a +ve wtscale, then with the paths of
 *     the inhibitory ones and a -ve wtscale.
 *
 *     The file is organised as a series of receptive fields of the
 *     destination plane. In other words, all the incoming connection
 *     weights for destination 0 are first set up, then all the 
 *     weights for destination 1, and so on. Line breaks and spaces
 *     in the file are ignored. Weights are separated by whitespace.
 *
 *
 * ARGUMENTS
 *     int argc	        - number of command arguments
 *     char* argv[]	- command arguments proper
 *
 * RETURN VALUE
 *     void
 *
 * AUTHORS
 *     Upi Bhalla.
 *     Based loosely on the PlanarConnect code by 
 *     Mike Vanier, Dave Bilitch, Matt Wilson
 */

void do_fileconnect(argc,argv)
		 int argc;
		 char **argv;
{
	double		w = 1.0; /* weight scale factor */
	Element		*source,	*dest;
	ElementList	*srclist, *dstlist;
	int			i, j, k;
	int			status;
	float		wt;
	FILE*		fp;
	float		EPSILON = 1e-20;
	struct Synchan_type*	synchan;
	
	initopt(argc, argv, "source-path dest-path file -wtscale w");
	while ((status = G_getopt(argc, argv)) == 1) {
			if (strcmp(G_optopt, "-wtscale") == 0)
			w = Atod(optargv[1]);
	}

	if (status < 0) {
		printoptusage(argc, argv);
		return;
	}
	
	srclist = WildcardGetElement(optargv[1],0);
	dstlist = WildcardGetElement(optargv[2],0);
	if ((fp = fopen(optargv[3], "r")) == NULL) {
		Error();
	printf("%s failed to open file %s for reading\n",
		optargv[0], optargv[3]);
		return;
	}

	if (srclist->nelements == 0) {
		Warning();
		printf("%s has zero elements on src path '%s'\n",
			optargv[0], optargv[1]);
		return;
	}

	if (dstlist->nelements == 0) {
		Warning();
		printf("%s has zero elements on dest path '%s'\n",
			optargv[0], optargv[2]);
		return;
	}

	k = 0;
	for (j = 0; j < dstlist->nelements; j++) {
		dest = dstlist->element[j];

	/* Make sure it's a synchan or related object. */
		if (!CheckClass(dest, ClassID("synchannel"))) {
			Error();
			printf("Destination element is not of class synchannel.\n");
			continue;
			}
		for (i = 0; i < srclist->nelements; i++) {
			if (!fscanf(fp, "%f", &wt))
				break;
			source = srclist->element[i];
			/* Make sure it's a spiking object. */	 
			if (!CheckClass(source, ClassID("spiking")))
				continue;
			wt *= w;
			if (wt > EPSILON) { /* Only wts > zero make synapses */
				AddMsg(source, dest, SPIKE, 0, NULL);
				synchan = (struct Synchan_type*) dest;
				GetSynapseAddress(
					dest, synchan->nsynapses - 1)->weight = wt;
				k++;
			}
		}
		if (IsSilent() < 2) {
			printf(".");
			fflush(stdout);
		}
	}
	if (IsSilent() < 2)
		printf("\nNumber of connections = %d\n", k);

	FreeElementList(dstlist);
	FreeElementList(srclist);
	fclose(fp);
}
