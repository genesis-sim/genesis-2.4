/*
** 2000-05-07 <mhucka@bbb.caltech.edu>
** This is a totally redone GIF display module.  We had problems with
** the previous code under X displays having more than 8 bit depths, and
** moreover, the previous code did not handle some of the more modern
** GIF extensions.  This version is taken from code in the XEmacs 21.1.8
** distribution, files src/glyphs-x.c, src/glyphs-eimage.c, src/dgif_lib.c,
** src/gif_io.h and src/gif_io.c.  The code is subject to the GNU General
** Public License, version 2.
**
** Portions of this file came from imgproc.h from XEmacs 21.1.8.
** Here is the original copyright notice:
** 
**  Image processing aux functions header file
**    Copyright (C) 1998 Jareth Hein
** 
** This file is a part of XEmacs
** 
** XEmacs is free software; you can redistribute it and/or modify it
** under the terms of the GNU General Public License as published by the
** Free Software Foundation; either version 2, or (at your option) any
** later version.
** 
** XEmacs is distributed in the hope that it will be useful, but WITHOUT
** ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
** FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
** for more details.
** 
** You should have received a copy of the GNU General Public License
** along with XEmacs; see the file COPYING.  If not, write to
** the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
** Boston, MA 02111-1307, USA.
*/

/* $Id: xgifload.c,v 1.3 2005/07/01 10:02:50 svitak Exp $
** $Log: xgifload.c,v $
** Revision 1.3  2005/07/01 10:02:50  svitak
** Misc fixes to address compiler warnings, esp. providing explicit types
** for all functions and cleaning up unused variables.
**
** Revision 1.2  2005/06/18 18:50:29  svitak
** Typecasts to quiet compiler warnings. From OSX Panther patch.
**
** Revision 1.1.1.1  2005/06/14 04:38:33  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.8  2001/05/30 15:57:09  mhucka
** Portability fix involving bcopy.
**
** Revision 1.7  2000/06/12 04:28:20  mhucka
** Removed nested comments within header, to make compilers happy.
**
** Revision 1.6  2000/06/07 05:25:38  mhucka
** #include of alloca.h is needed under IRIX.
**
** Revision 1.5  2000/05/26 22:24:59  mhucka
** Changed the bigendian flag to a #ifdef instead of #if.
**
** Revision 1.4  2000/05/19 18:22:37  mhucka
** Reimplementation of entire code, using code taken from XEmacs GIF handling
** elements.  This fixes problems in the old code with display depths > 8, and
** also adds support for more varieties of GIF files.
**
** Revision 1.3  2000/05/02 06:09:48  mhucka
** Fixed return value returned by ColorDicking(), which required modifying
** the FatalError macro and how it was called.
**
** Revision 1.2  1994/05/25 13:44:12  bhalla
** Uses SearchForNonScript instead of fopen for opening the text file. This
** lets it find files anywhere on the genesis search path
**
** Revision 1.1  1994/03/22  15:24:32  bhalla
** Initial revision
*/

#include <sys/types.h>
#include <sys/stat.h>
#if defined( Solaris ) || defined( irix )
#include <alloca.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xatom.h>
#include <X11/Shell.h>

#include "Xo/Xo_ext.h"
#include "gifrlib.h"
#include "xgifload.h"
#include "shell.h"
#include "Image.h"
#include "ImageP.h"
#include "../../sys/system_deps.h"

typedef unsigned char Extbyte;
typedef int Extcount;

static XImage *convert_EImage_to_XImage();

/*
** Most of the code for the GIF handling is located in dgif_lib.c and
** gif_io.c.  The functions here provide an interface between GENESIS
** and the code in those files.  2000-05-10 <mhucka@bbb.caltech.edu>
**
*/

#define Error(str) { fprintf(stderr, "xgifload: %s\n", str); }

/* From lisp.h of XEmacs 21.1.8. */
#define XREALLOC_ARRAY(ptr, type, len) \
    ((void) (ptr = (type *) realloc(ptr, (len) * sizeof (type))))

/* From lisp.h of XEmacs 21.1.8:
   "also generally useful if you want to avoid arbitrary size limits
   but don't need a full dynamic array.  Assumes that BASEVAR points
   to a malloced array of TYPE objects (or possibly a NULL pointer,
   if SIZEVAR is 0), with the total size stored in SIZEVAR.  This
   macro will realloc BASEVAR as necessary so that it can hold at
   least NEEDED_SIZE objects.  The reallocing is done by doubling,
   which ensures constant amortized time per element." */
#define DO_REALLOC(basevar, sizevar, needed_size, type)	do	\
{								\
  /* Avoid side-effectualness. */				\
  /* Dammit! Macros suffer from dynamic scope! */		\
  /* We demand inline functions! */				\
  size_t do_realloc_needed_size = (needed_size);		\
  size_t do_realloc_newsize = 0;				\
  while ((sizevar) < (do_realloc_needed_size)) {		\
    do_realloc_newsize = 2*(sizevar);				\
    if (do_realloc_newsize < 32)				\
      do_realloc_newsize = 32;					\
    (sizevar) = do_realloc_newsize;				\
  }								\
  if (do_realloc_newsize)					\
    XREALLOC_ARRAY (basevar, type, do_realloc_newsize);		\
} while (0)


typedef struct gif_memory_storage
{
    Extbyte *bytes;		/* The data       */
    Extcount len;			/* How big is it? */
    int index;			/* Where are we?  */
} gif_memory_storage;


XImage *
XGIFLoad(fname, widget, width, height)
    char *fname;
    ImageWidget widget;
    int *width, *height;
{
    FILE *fp;
    GifFileType *giffile;
    unsigned char *eimage, *eip;
    unsigned long *pixtbl = NULL;
    int npixels = 0;
    ColorMapObject *cmo;
    int i, j, row, pass, interlace;

    /* interlaced gifs have rows in this order:
       0, 8, 16, ..., 4, 12, 20, ..., 2, 6, 10, ..., 1, 3, 5, ...  */
    static int InterlacedOffset[] = { 0, 4, 2, 1 };
    static int InterlacedJumps[] = { 8, 8, 4, 2 };
    XImage *img;


    /* First try to allocate the structure recording info about the file. */
    if ((giffile = GifSetup()) == NULL) {
	Error("Insufficent memory to instantiate GIF image");
	return NULL;
    }

    /* Next, try to open the file. */
    if (strcmp(fname, "-") == 0) {
	fp = stdin;
	fname = "<stdin>";
    }
    else {
	fp = SearchForNonScript(fname, "r");
    }
    if (!fp) {
	Error("File not found");
	return NULL;
    }
    DGifOpenFileHandle(giffile, fileno(fp));
    
    /* Slurp the image into memory, decoding along the way.  The result is
    ** the image in a simple one-byte-per-pixel format.  All the state is
    ** kept in the "giffile" structure.
    */
    DGifSlurp(giffile);

    /*
    ** Convert to intermediate "EImage" format.  This step is probably
    ** unnecessary, but I don't know enough about X and the XEmacs GIF code
    ** to understand how to go straight to an XImage, so I'm leaving this
    ** piece in.  It's doing something about interlacing that I don't have
    ** time to figure out right now.  2000-05-10 <mhucka@bbb.caltech.edu>
    */
    *height = giffile->SHeight;
    *width = giffile->SWidth;
    eimage = (unsigned char*) malloc((*width) * (*height) * 3);
    if (!eimage) {
	Error("Unable to allocate enough memory for image");
	return NULL;
    }	

    /* Write the data in EImage format (8bit RGB triples). */
    cmo = giffile->SColorMap;
    interlace = giffile->SavedImages[0].ImageDesc.Interlace;
    pass = 0;
    row = interlace ? InterlacedOffset[pass] : 0;
    eip = eimage;
    for (i = 0; i < *height; i++) {
	if (interlace) {
	    if (row >= *height) {
		row = InterlacedOffset[++pass];
		while (row >= *height) 
		    row = InterlacedOffset[++pass];
	    }
	}
	eip = eimage + (row * (*width) * 3);
	for (j = 0; j < *width; j++) {
	    unsigned char pixel =
		giffile->SavedImages[0].RasterBits[ (i * (*width)) + j ];
	    *eip++ = cmo->Colors[pixel].Red;
	    *eip++ = cmo->Colors[pixel].Green;
	    *eip++ = cmo->Colors[pixel].Blue;
	}
	row += interlace ? InterlacedJumps[pass] : 1;
    }

    img = convert_EImage_to_XImage(widget, *width, *height,
				   eimage, &pixtbl, &npixels);
    if (img == NULL) {
	if (pixtbl) {
	    free(pixtbl);
	}
	return NULL;
    } else {
	return img;
    }
}


static void
get_histogram(quant_table *qt, unsigned char *pic,
	      int width, int height, Colorbox* box)
{
  register unsigned char *inptr;
  register int red, green, blue;
  register unsigned int j, i;

  box->rmin = box->gmin = box->bmin = 999;
  box->rmax = box->gmax = box->bmax = -1;
  box->total = width * height;

  inptr = pic;
  for (i = 0; i < height; i++)
    {
      for (j = width; j-- > 0;)
	{
	  red = *inptr++ >> COLOR_SHIFT;
	  green = *inptr++ >> COLOR_SHIFT;
	  blue = *inptr++ >> COLOR_SHIFT;
	  if (red < box->rmin)
	    box->rmin = red;
	  if (red > box->rmax)
	    box->rmax = red;
	  if (green < box->gmin)
	    box->gmin = green;
	  if (green > box->gmax)
	    box->gmax = green;
	  if (blue < box->bmin)
	    box->bmin = blue;
	  if (blue > box->bmax)
	    box->bmax = blue;
	  qt->histogram[red][green][blue]++;
	}
    }
}


static Colorbox *
largest_box(quant_table *qt)
{
  register Colorbox *p, *b;
  register int size;

  b = NULL;
  size = -1;
  for (p = qt->usedboxes; p != NULL; p = p->next)
    if ((p->rmax > p->rmin || p->gmax > p->gmin ||
	 p->bmax > p->bmin) &&  p->total > size)
      size = (b = p)->total;
  return (b);
}


static void
shrinkbox(quant_table *qt, Colorbox* box)
{
  register int *histp, ir, ig, ib;

  if (box->rmax > box->rmin)
    {
      for (ir = box->rmin; ir <= box->rmax; ++ir)
	for (ig = box->gmin; ig <= box->gmax; ++ig)
	  {
	    histp = &(qt->histogram[ir][ig][box->bmin]);
	    for (ib = box->bmin; ib <= box->bmax; ++ib)
	      if (*histp++ != 0)
		{
		  box->rmin = ir;
		  goto have_rmin;
		}
	  }
    have_rmin:
      if (box->rmax > box->rmin)
	for (ir = box->rmax; ir >= box->rmin; --ir)
	  for (ig = box->gmin; ig <= box->gmax; ++ig)
	    {
	      histp = &(qt->histogram[ir][ig][box->bmin]);
	      ib = box->bmin;
	      for (; ib <= box->bmax; ++ib)
		if (*histp++ != 0)
		  {
		    box->rmax = ir;
		    goto have_rmax;
		  }
	    }
    }
 have_rmax:
  if (box->gmax > box->gmin)
    {
      for (ig = box->gmin; ig <= box->gmax; ++ig)
	for (ir = box->rmin; ir <= box->rmax; ++ir)
	  {
	    histp = &(qt->histogram[ir][ig][box->bmin]);
	    for (ib = box->bmin; ib <= box->bmax; ++ib)
	      if (*histp++ != 0)
		{
		  box->gmin = ig;
		  goto have_gmin;
		}
	  }
    have_gmin:
      if (box->gmax > box->gmin)
	for (ig = box->gmax; ig >= box->gmin; --ig)
	  for (ir = box->rmin; ir <= box->rmax; ++ir)
	    {
	      histp = &(qt->histogram[ir][ig][box->bmin]);
	      ib = box->bmin;
	      for (; ib <= box->bmax; ++ib)
		if (*histp++ != 0)
		  {
		    box->gmax = ig;
		    goto have_gmax;
		  }
	    }
    }
 have_gmax:
  if (box->bmax > box->bmin)
    {
      for (ib = box->bmin; ib <= box->bmax; ++ib)
	for (ir = box->rmin; ir <= box->rmax; ++ir)
	  {
	    histp = &(qt->histogram[ir][box->gmin][ib]);
	    for (ig = box->gmin; ig <= box->gmax; ++ig)
	      {
		if (*histp != 0)
		  {
		    box->bmin = ib;
		    goto have_bmin;
		  }
		histp += B_LEN;
	      }
	  }
    have_bmin:
      if (box->bmax > box->bmin)
	for (ib = box->bmax; ib >= box->bmin; --ib)
	  for (ir = box->rmin; ir <= box->rmax; ++ir)
	    {
	      histp = &(qt->histogram[ir][box->gmin][ib]);
	      ig = box->gmin;
	      for (; ig <= box->gmax; ++ig)
		{
		  if (*histp != 0)
		    {
		      box->bmax = ib;
		      goto have_bmax;
		    }
		  histp += B_LEN;
		}
	    }
    }
 have_bmax:
  ;
}


static void
splitbox(quant_table *qt, Colorbox* ptr)
{
  int		hist2[B_LEN];
  int		first = 0, last = 0;
  register Colorbox	*new;
  register int	*iptr, *histp;
  register int	i, j;
  register int	ir,ig,ib;
  register int sum, sum1, sum2;
  enum { RED, GREEN, BLUE } axis;

  /*
   * See which axis is the largest, do a histogram along that
   * axis.  Split at median point.  Contract both new boxes to
   * fit points and return
   */
  i = ptr->rmax - ptr->rmin;
  if (i >= ptr->gmax - ptr->gmin  && i >= ptr->bmax - ptr->bmin)
    axis = RED;
  else if (ptr->gmax - ptr->gmin >= ptr->bmax - ptr->bmin)
    axis = GREEN;
  else
    axis = BLUE;
  /* get histogram along longest axis */
  switch (axis)
    {
    case RED:
      histp = &hist2[ptr->rmin];
      for (ir = ptr->rmin; ir <= ptr->rmax; ++ir)
	{
	  *histp = 0;
	  for (ig = ptr->gmin; ig <= ptr->gmax; ++ig)
	    {
	      iptr = &(qt->histogram[ir][ig][ptr->bmin]);
	      for (ib = ptr->bmin; ib <= ptr->bmax; ++ib)
		*histp += *iptr++;
	    }
	  histp++;
	}
      first = ptr->rmin;
      last = ptr->rmax;
      break;
    case GREEN:
      histp = &hist2[ptr->gmin];
      for (ig = ptr->gmin; ig <= ptr->gmax; ++ig)
	{
	  *histp = 0;
	  for (ir = ptr->rmin; ir <= ptr->rmax; ++ir)
	    {
	      iptr = &(qt->histogram[ir][ig][ptr->bmin]);
	      for (ib = ptr->bmin; ib <= ptr->bmax; ++ib)
		*histp += *iptr++;
	    }
	  histp++;
	}
      first = ptr->gmin;
      last = ptr->gmax;
      break;
    case BLUE:
      histp = &hist2[ptr->bmin];
      for (ib = ptr->bmin; ib <= ptr->bmax; ++ib)
	{
	  *histp = 0;
	  for (ir = ptr->rmin; ir <= ptr->rmax; ++ir)
	    {
	      iptr = &(qt->histogram[ir][ptr->gmin][ib]);
	      for (ig = ptr->gmin; ig <= ptr->gmax; ++ig)
		{
		  *histp += *iptr;
		  iptr += B_LEN;
		}
	    }
	  histp++;
	}
      first = ptr->bmin;
      last = ptr->bmax;
      break;
    }
  /* find median point */
  sum2 = ptr->total / 2;
  histp = &hist2[first];
  sum = 0;
  for (i = first; i <= last && (sum += *histp++) < sum2; ++i)
    ;
  if (i == first)
    i++;

  /* Create new box, re-allocate points */
  new = qt->freeboxes;
  qt->freeboxes = new->next;
  if (qt->freeboxes)
    qt->freeboxes->prev = NULL;
  if (qt->usedboxes)
    qt->usedboxes->prev = new;
  new->next = qt->usedboxes;
  qt->usedboxes = new;

  histp = &hist2[first];
  for (sum1 = 0, j = first; j < i; j++)
    sum1 += *histp++;
  for (sum2 = 0, j = i; j <= last; j++)
    sum2 += *histp++;
  new->total = sum1;
  ptr->total = sum2;

  new->rmin = ptr->rmin;
  new->rmax = ptr->rmax;
  new->gmin = ptr->gmin;
  new->gmax = ptr->gmax;
  new->bmin = ptr->bmin;
  new->bmax = ptr->bmax;
  switch (axis)
    {
    case RED:
      new->rmax = i-1;
      ptr->rmin = i;
      break;
    case GREEN:
      new->gmax = i-1;
      ptr->gmin = i;
      break;
    case BLUE:
      new->bmax = i-1;
      ptr->bmin = i;
      break;
    }
  shrinkbox (qt, new);
  shrinkbox (qt, ptr);
}


static C_cell *
create_colorcell(quant_table *qt, int num_colors, int red, int green, int blue)
{
  register int ir, ig, ib, i;
  register C_cell *ptr;
  int mindist, next_n;
  register int tmp, dist, n;

  ir = red >> (COLOR_DEPTH-C_DEPTH);
  ig = green >> (COLOR_DEPTH-C_DEPTH);
  ib = blue >> (COLOR_DEPTH-C_DEPTH);
  ptr = (C_cell *) malloc(sizeof (C_cell));
  *(qt->ColorCells + ir*C_LEN*C_LEN + ig*C_LEN + ib) = ptr;
  ptr->num_ents = 0;

  /*
   * Step 1: find all colors inside this cell, while we're at
   *	   it, find distance of centermost point to furthest corner
   */
  mindist = 99999999;
  for (i = 0; i < num_colors; ++i)
    {
      if (qt->rm[i]>>(COLOR_DEPTH-C_DEPTH) != ir  ||
	  qt->gm[i]>>(COLOR_DEPTH-C_DEPTH) != ig  ||
	  qt->bm[i]>>(COLOR_DEPTH-C_DEPTH) != ib)
	continue;
      ptr->entries[ptr->num_ents][0] = i;
      ptr->entries[ptr->num_ents][1] = 0;
      ++ptr->num_ents;
      tmp = qt->rm[i] - red;
      if (tmp < (MAX_COLOR/C_LEN/2))
	tmp = MAX_COLOR/C_LEN-1 - tmp;
      dist = tmp*tmp;
      tmp = qt->gm[i] - green;
      if (tmp < (MAX_COLOR/C_LEN/2))
	tmp = MAX_COLOR/C_LEN-1 - tmp;
      dist += tmp*tmp;
      tmp = qt->bm[i] - blue;
      if (tmp < (MAX_COLOR/C_LEN/2))
	tmp = MAX_COLOR/C_LEN-1 - tmp;
      dist += tmp*tmp;
      if (dist < mindist)
	mindist = dist;
    }

  /*
   * Step 3: find all points within that distance to cell.
   */
  for (i = 0; i < num_colors; ++i)
    {
      if (qt->rm[i] >> (COLOR_DEPTH-C_DEPTH) == ir  &&
	  qt->gm[i] >> (COLOR_DEPTH-C_DEPTH) == ig  &&
	  qt->bm[i] >> (COLOR_DEPTH-C_DEPTH) == ib)
	continue;
      dist = 0;
      if ((tmp = red - qt->rm[i]) > 0 ||
	  (tmp = qt->rm[i] - (red + MAX_COLOR/C_LEN-1)) > 0 )
	dist += tmp*tmp;
      if ((tmp = green - qt->gm[i]) > 0 ||
	  (tmp = qt->gm[i] - (green + MAX_COLOR/C_LEN-1)) > 0 )
	dist += tmp*tmp;
      if ((tmp = blue - qt->bm[i]) > 0 ||
	  (tmp = qt->bm[i] - (blue + MAX_COLOR/C_LEN-1)) > 0 )
	dist += tmp*tmp;
      if (dist < mindist)
	{
	  ptr->entries[ptr->num_ents][0] = i;
	  ptr->entries[ptr->num_ents][1] = dist;
	  ++ptr->num_ents;
	}
    }

  /*
   * Sort color cells by distance, use cheap exchange sort
   */
  for (n = ptr->num_ents - 1; n > 0; n = next_n)
    {
      next_n = 0;
      for (i = 0; i < n; ++i)
	if (ptr->entries[i][1] > ptr->entries[i+1][1])
	  {
	    tmp = ptr->entries[i][0];
	    ptr->entries[i][0] = ptr->entries[i+1][0];
	    ptr->entries[i+1][0] = tmp;
	    tmp = ptr->entries[i][1];
	    ptr->entries[i][1] = ptr->entries[i+1][1];
	    ptr->entries[i+1][1] = tmp;
	    next_n = i;
	  }
    }
  return (ptr);
}


static int
map_colortable(quant_table *qt, int num_colors)
{
  register int *histp = &(qt->histogram[0][0][0]);
  register C_cell *cell;
  register int j, tmp, d2, dist;
  int ir, ig, ib, i;

  for (ir = 0; ir < B_LEN; ++ir)
    for (ig = 0; ig < B_LEN; ++ig)
      for (ib = 0; ib < B_LEN; ++ib, histp++)
	{
	  if (*histp == 0)
	    {
	      *histp = -1;
	      continue;
	    }
	  cell = *(qt->ColorCells +
		   (((ir>>(B_DEPTH-C_DEPTH)) << C_DEPTH*2) +
		    ((ig>>(B_DEPTH-C_DEPTH)) << C_DEPTH) +
		    (ib>>(B_DEPTH-C_DEPTH))));
	  if (cell == NULL )
	    cell = create_colorcell (qt, num_colors,
				     ir << COLOR_SHIFT,
				     ig << COLOR_SHIFT,
				     ib << COLOR_SHIFT);
	  if (cell == NULL) /* memory exhausted! punt! */
	    return -1;
	  dist = 9999999;
	  for (i = 0; i < cell->num_ents &&
		 dist > cell->entries[i][1]; ++i)
	    {
	      j = cell->entries[i][0];
	      d2 = qt->rm[j] - (ir << COLOR_SHIFT);
	      d2 *= d2;
	      tmp = qt->gm[j] - (ig << COLOR_SHIFT);
	      d2 += tmp*tmp;
	      tmp = qt->bm[j] - (ib << COLOR_SHIFT);
	      d2 += tmp*tmp;
	      if (d2 < dist)
		{
		  dist = d2;
		  *histp = j;
		}
	    }
	}
  return 0;
}

quant_table *
build_EImage_quantable(unsigned char *eimage, int width, int height, int num_colors)
{
  quant_table *qt;
  Colorbox *box_list, *ptr;
  int i,res;

  qt = (quant_table*) malloc(sizeof(quant_table));
  if (qt == NULL) return NULL;
  BZERO(qt, sizeof(quant_table));

  /*
   * STEP 1:  create empty boxes
   */
  qt->usedboxes = NULL;
  box_list = qt->freeboxes = (Colorbox *) malloc(num_colors * sizeof(Colorbox));
  qt->freeboxes[0].next = &(qt->freeboxes[1]);
  qt->freeboxes[0].prev = NULL;
  for (i = 1; i < num_colors-1; ++i)
    {
      qt->freeboxes[i].next = &(qt->freeboxes[i+1]);
      qt->freeboxes[i].prev = &(qt->freeboxes[i-1]);
    }
  qt->freeboxes[num_colors-1].next = NULL;
  qt->freeboxes[num_colors-1].prev = &(qt->freeboxes[num_colors-2]);

  /*
   * STEP 2: get histogram, initialize first box
   */
  ptr = qt->freeboxes;
  qt->freeboxes = ptr->next;
  if (qt->freeboxes)
    qt->freeboxes->prev = NULL;
  ptr->next = qt->usedboxes;
  qt->usedboxes = ptr;
  if (ptr->next)
    ptr->next->prev = ptr;
  get_histogram (qt, eimage, width, height, ptr);

  /*
   * STEP 3: continually subdivide boxes until no more free
   * boxes remain or until all colors assigned.
   */
  while (qt->freeboxes != NULL)
    {
      ptr = largest_box(qt);
      if (ptr != NULL)
	splitbox (qt, ptr);
      else
	qt->freeboxes = NULL;
    }

  /*
   * STEP 4: assign colors to all boxes
   */
  for (i = 0, ptr = qt->usedboxes; ptr != NULL; ++i, ptr = ptr->next)
    {
      qt->rm[i] = ((ptr->rmin + ptr->rmax) << COLOR_SHIFT) / 2;
      qt->gm[i] = ((ptr->gmin + ptr->gmax) << COLOR_SHIFT) / 2;
      qt->bm[i] = ((ptr->bmin + ptr->bmax) << COLOR_SHIFT) / 2;
      qt->um[i] = ptr->total;
    }
  qt->num_active_colors = i;

  /* We're done with the boxes now */
  free(box_list);
  qt->freeboxes = qt->usedboxes = NULL;

  /*
   * STEP 5: scan histogram and map all values to closest color
   */
  /* 5a: create cell list as described in Heckbert */
  qt->ColorCells = (C_cell **) malloc(C_LEN*C_LEN*C_LEN*sizeof (C_cell*));
  BZERO(qt->ColorCells, C_LEN*C_LEN*C_LEN*sizeof (C_cell*));
  /* 5b: create mapping from truncated pixel space to color
     table entries */
  res = map_colortable (qt, num_colors);

  /* 5c: done with ColorCells */
  for (i = 0; i < C_LEN*C_LEN*C_LEN; i++)
    if (qt->ColorCells[i]) free(qt->ColorCells[i]);
  free(qt->ColorCells);
  
  if (res)
    {
      /* we failed in memory allocation, so clean up an leave */
      free(qt);
      return NULL;
    }
  
  return qt;
}

/* The following came from objects-x.c from XEmacs 21.1.8.
**
** Copyright (C) 1993, 1994 Free Software Foundation, Inc.
** Copyright (C) 1995 Board of Trustees, University of Illinois.
** Copyright (C) 1995 Tinker Systems.
** Copyright (C) 1995, 1996 Ben Wing.
** Copyright (C) 1995 Sun Microsystems, Inc.
**
** Replacement for XAllocColor() that tries to return the nearest
** available color if the colormap is full.  Original was from FSFmacs,
** but rewritten by Jareth Hein <jareth@camelot-soft.com> 97/11/25
** Modified by Lee Kindness <lkindness@csl.co.uk> 31/08/99 to handle previous
** total failure which was due to a read/write colorcell being the nearest
** match - tries the next nearest...
**
** Return value is 1 for normal success, 2 for nearest color success,
** 3 for Non-deallocable sucess.
*/

int
allocate_nearest_color (Display *disp, Colormap colormap, Visual *visual,
		        XColor *color_def)
{
  int status;

  if (visual->class == DirectColor || visual->class == TrueColor)
    {
      if (XAllocColor (disp, colormap, color_def) != 0)
	{
	  status = 1;
	}
      else
	{
	  /* We're dealing with a TrueColor/DirectColor visual, so play games
	     with the RGB values in the XColor struct. */
	  /* ### JH: I'm not sure how a call to XAllocColor can fail in a
	     TrueColor or DirectColor visual, so I will just reformat the
	     request to match the requirements of the visual, and re-issue
	     the request.  If this fails for anybody, I wanna know about it
	     so I can come up with a better plan */

	  unsigned long rshift,gshift,bshift,rbits,gbits,bbits,junk;
	  junk = visual->red_mask;
	  rshift = 0;
	  while ((junk & 0x1) == 0) {
	    junk = junk >> 1;
	    rshift ++;
	  }
	  rbits = 0;
	  while (junk != 0) {
	    junk = junk >> 1;
	    rbits++;
	  }
	  junk = visual->green_mask;
	  gshift = 0;
	  while ((junk & 0x1) == 0) {
	    junk = junk >> 1;
	    gshift ++;
	  }
	  gbits = 0;
	  while (junk != 0) {
	    junk = junk >> 1;
	    gbits++;
	  }
	  junk = visual->blue_mask;
	  bshift = 0;
	  while ((junk & 0x1) == 0) {
	    junk = junk >> 1;
	    bshift ++;
	  }
	  bbits = 0;
	  while (junk != 0) {
	    junk = junk >> 1;
	    bbits++;
 	  }

	  color_def->red = color_def->red >> (16 - rbits);
	  color_def->green = color_def->green >> (16 - gbits);
	  color_def->blue = color_def->blue >> (16 - bbits);
	  if (XAllocColor (disp, colormap, color_def) != 0)
	    status = 1;
	  else
  	    {
  	      int rd, gr, bl;	
	      /* ### JH: I'm punting here, knowing that doing this will at
		 least draw the color correctly.  However, unless we convert
		 all of the functions that allocate colors (graphics
		 libraries, etc) to use this function doing this is very
		 likely to cause problems later... */

	      if (rbits > 8)
		rd = color_def->red << (rbits - 8);
	      else
		rd = color_def->red >> (8 - rbits);
	      if (gbits > 8)
		gr = color_def->green << (gbits - 8);
	      else
		gr = color_def->green >> (8 - gbits);
	      if (bbits > 8)
		bl = color_def->blue << (bbits - 8);
	      else
		bl = color_def->blue >> (8 - bbits);
	      color_def->pixel = (rd << rshift) | (gr << gshift) | (bl << bshift);
	      status = 3;
	    }
	}
    }
  else
    {
      XColor *cells = NULL;
      /* JH: I can't believe there's no way to go backwards from a
	 colormap ID and get its visual and number of entries, but X
	 apparently isn't built that way... */
      int no_cells = visual->map_entries;
      status = 0;

      if (XAllocColor (disp, colormap, color_def) != 0)
	status = 1;
      else while( status != 2 )
	{
	  /* If we got to this point, the colormap is full, so we're
	     going to try and get the next closest color.  The algorithm used
	     is a least-squares matching, which is what X uses for closest
	     color matching with StaticColor visuals. */
	  int nearest;
	  long nearest_delta, trial_delta;
	  int x;

	  if( cells == NULL )
	      {
		  cells = (XColor *) alloca(no_cells * sizeof(XColor));
		  for (x = 0; x < no_cells; x++)
		      cells[x].pixel = x;

		  /* read the current colormap */
		  XQueryColors (disp, colormap, cells, no_cells);
	      }

	  nearest = 0;
	  /* I'm assuming CSE so I'm not going to condense this. */
	  nearest_delta = ((((color_def->red >> 8) - (cells[0].red >> 8))
			    * ((color_def->red >> 8) - (cells[0].red >> 8)))
			   +
			   (((color_def->green >> 8) - (cells[0].green >> 8))
			    * ((color_def->green >> 8) - (cells[0].green >> 8)))
			   +
			   (((color_def->blue >> 8) - (cells[0].blue >> 8))
			    * ((color_def->blue >> 8) - (cells[0].blue >> 8))));
	  for (x = 1; x < no_cells; x++)
	    {
	      trial_delta = ((((color_def->red >> 8) - (cells[x].red >> 8))
			      * ((color_def->red >> 8) - (cells[x].red >> 8)))
			     +
			     (((color_def->green >> 8) - (cells[x].green >> 8))
			      * ((color_def->green >> 8) - (cells[x].green >> 8)))
			     +
			     (((color_def->blue >> 8) - (cells[x].blue >> 8))
			      * ((color_def->blue >> 8) - (cells[x].blue >> 8))));

	      /* less? Ignore cells marked as previously failing */
	      if( (trial_delta < nearest_delta) && (cells[x].pixel != -1) )
		{
		  nearest = x;
		  nearest_delta = trial_delta;
		}
	    }
	  color_def->red = cells[nearest].red;
	  color_def->green = cells[nearest].green;
	  color_def->blue = cells[nearest].blue;
	  if (XAllocColor (disp, colormap, color_def) != 0)
	      status = 2;
	  else
	      /* LSK: Either the colour map has changed since
	       * we read it, or the colour is allocated
	       * read/write... Mark this cmap entry so it's
	       * ignored in the next iteration.
	       */
	      cells[nearest].pixel = -1;
	}
    }
  return status;
}


/* The following came from glyphs-x.c from XEmacs 21.1.8.
**
** Convert from a series of RGB triples to an XImage formated for the
** proper display.
*/
static XImage *
convert_EImage_to_XImage (widget, width, height, pic, pixtbl, npixels)
    ImageWidget widget;
    int width;
    int height;
    unsigned char *pic;
    unsigned long **pixtbl;
    int *npixels;
{
    Display *disp;
    Visual *vis;
    Colormap cmap;
    XImage *outimg;
    int depth, bitmap_pad, byte_cnt, i, j;
    int rd,gr,bl,q;
    unsigned char *data, *ip, *dp;
    quant_table *qtable = NULL;
    union {
	unsigned int val;
	char cp[4];
    } conv;

    disp = (Display *)XgDisplay();
    vis = (Visual *)XoGetUsedVisual();
    cmap = XoGetUsedColormap();
    depth = XoGetUsedDepth();

    switch (vis->class) {
    case PseudoColor:
	/* We can work with this visual class.
	   Quantize the image and get a histogram while we're at it.
	   Do this first to save memory */
	qtable = build_EImage_quantable(pic, width, height, 256);
	if (qtable == NULL) return NULL;
	break;

    case GrayScale:
	Error("Support for GIF display on GrayScale visuals not implemented.");
	return NULL;
	/* NOTREACHED */

    case StaticColor:
	Error("Support for GIF display on StaticColor visuals not implemented.");
	return NULL;
	/* NOTREACHED */
	break;
	
    case StaticGray:
	Error("Support for GIF display on StaticGray visuals not implemented.");
	return NULL;
	/* NOTREACHED */
	break;
    }

    if (depth > 16) 
	bitmap_pad = 32;
    else if (depth > 8)
	bitmap_pad = 16;
    else
	bitmap_pad = 8;

    outimg = XCreateImage(disp, vis, depth, ZPixmap, 0, 0, width, height,
			  bitmap_pad, 0);
    if (!outimg) {
	Error("Unable to create X image");
	return NULL;
    }

    byte_cnt = outimg->bits_per_pixel >> 3;

    data = (unsigned char *) malloc(outimg->bytes_per_line * height);
    if (!data) {
	Error("Unable to allocate memory for image structure");
	XDestroyImage(outimg);
	return NULL;
    }
    outimg->data = (char *) data;
  
    if (vis->class == PseudoColor) {
	unsigned long pixarray[256];
	int pixcount, n;
	/* use our quantize table to allocate the colors */
	pixcount = 32;
	*pixtbl = (unsigned long *) malloc(pixcount * sizeof(unsigned long));
	*npixels = 0;

	/* ### should implement a sort by popularity to assure proper allocation */
	n = *npixels;
	for (i = 0; i < qtable->num_active_colors; i++) {
	    XColor color;
	    int res;
	
	    color.red = qtable->rm[i] ? qtable->rm[i] << 8 : 0;
	    color.green = qtable->gm[i] ? qtable->gm[i] << 8 : 0;
	    color.blue = qtable->bm[i] ? qtable->bm[i] << 8 : 0;
	    color.flags = DoRed | DoGreen | DoBlue;
	    res = allocate_nearest_color(disp, cmap, vis, &color);
	    if (res > 0 && res < 3)
	    {
		DO_REALLOC(*pixtbl, pixcount, n+1, unsigned long);
		(*pixtbl)[n] = color.pixel;
		n++;
	    }
	    pixarray[i] = color.pixel;
	}
	*npixels = n;
	ip = pic;
	for (i = 0; i < height; i++) {
	    dp = data + (i * outimg->bytes_per_line);
	    for (j = 0; j < width; j++) {
		rd = *ip++;
		gr = *ip++;
		bl = *ip++;
		conv.val = pixarray[QUANT_GET_COLOR(qtable,rd,gr,bl)];
#ifdef BIGENDIAN
		if (outimg->byte_order == MSBFirst)
		    for (q = 4-byte_cnt; q < 4; q++) *dp++ = conv.cp[q];
		else
		    for (q = 3; q >= 4-byte_cnt; q--) *dp++ = conv.cp[q];
#else
		if (outimg->byte_order == MSBFirst)
		    for (q = byte_cnt-1; q >= 0; q--) *dp++ = conv.cp[q];
		else
		    for (q = 0; q < byte_cnt; q++) *dp++ = conv.cp[q];
#endif
	    }
	}
	free(qtable);
    } else {
	unsigned long rshift,gshift,bshift,rbits,gbits,bbits,junk;
	junk = vis->red_mask;
	rshift = 0;
	while ((junk & 0x1) == 0) {
	    junk = junk >> 1;
	    rshift ++;
	}
	rbits = 0;
	while (junk != 0) {
	    junk = junk >> 1;
	    rbits++;
	}
	junk = vis->green_mask;
	gshift = 0;
	while ((junk & 0x1) == 0) {
	    junk = junk >> 1;
	    gshift ++;
	}
	gbits = 0;
	while (junk != 0) {
	    junk = junk >> 1;
	    gbits++;
	}
	junk = vis->blue_mask;
	bshift = 0;
	while ((junk & 0x1) == 0) {
	    junk = junk >> 1;
	    bshift ++;
	}
	bbits = 0;
	while (junk != 0) {
	    junk = junk >> 1;
	    bbits++;
	}
	ip = pic;
	for (i = 0; i < height; i++) {
	    dp = data + (i * outimg->bytes_per_line);
	    for (j = 0; j < width; j++) {
		if (rbits > 8)
		    rd = *ip++ << (rbits - 8);
		else
		    rd = *ip++ >> (8 - rbits);
		if (gbits > 8)
		    gr = *ip++ << (gbits - 8);
		else
		    gr = *ip++ >> (8 - gbits);
		if (bbits > 8)
		    bl = *ip++ << (bbits - 8);
		else
		    bl = *ip++ >> (8 - bbits);

		conv.val = (rd << rshift) | (gr << gshift) | (bl << bshift);
#ifdef BIGENDIAN
		if (outimg->byte_order == MSBFirst)
		    for (q = 4-byte_cnt; q < 4; q++) *dp++ = conv.cp[q];
		else
		    for (q = 3; q >= 4-byte_cnt; q--) *dp++ = conv.cp[q];
#else
		if (outimg->byte_order == MSBFirst)
		    for (q = byte_cnt-1; q >= 0; q--) *dp++ = conv.cp[q];
		else
		    for (q = 0; q < byte_cnt; q++) *dp++ = conv.cp[q];
#endif
	    }
	}
    }  
    return outimg;
}
