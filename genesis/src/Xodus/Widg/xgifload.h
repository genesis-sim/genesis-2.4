/*
** 2000-05-07 <mhucka@bbb.caltech.edu>
** This is a totally redone GIF display module.  We had problems with
** the previous code under X displays having more than 8 bit depths, and
** moreover, the previous code did not handle some of the more modern
** GIF extensions.  This version is taken from code in the XEmacs 21.1.8
** distribution, files src/glyphs-x.c, src/glyphs-eimage.c, src/dgif_lib.c,
** src/gif_io.h, src/gif_io.c, src/imgproc.c and src/imgproc.h.
** This code is subject to the GNU General Public License, version 2.
**
** The contents of this file came from imgproc.h from XEmacs 21.1.8.
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

/* $Id: xgifload.h,v 1.1.1.1 2005/06/14 04:38:33 svitak Exp $
** $Log: xgifload.h,v $
** Revision 1.1.1.1  2005/06/14 04:38:33  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.1  2000/05/19 18:24:08  mhucka
** New file for use with xgifload.c.
**
*/

#define	MAX_CMAP_SIZE	256
#define	COLOR_DEPTH	8
#define	MAX_COLOR	256

#define	B_DEPTH		5		/* # bits/pixel to use */
#define	B_LEN		(1L<<B_DEPTH)

#define	C_DEPTH		2
#define	C_LEN		(1L<<C_DEPTH)	/* # cells/color to use */

#define	COLOR_SHIFT	(COLOR_DEPTH-B_DEPTH)

typedef	struct colorbox {
	struct	colorbox *next, *prev;
	int	rmin, rmax;
	int	gmin, gmax;
	int	bmin, bmax;
	int	total;
} Colorbox;

typedef struct {
	int	num_ents;
	int	entries[MAX_CMAP_SIZE][2];
} C_cell;

typedef struct {
  unsigned short rm[MAX_CMAP_SIZE], gm[MAX_CMAP_SIZE], bm[MAX_CMAP_SIZE]; /* map values */
  int um[MAX_CMAP_SIZE]; /* usage counts for each mapentry */
  int histogram[B_LEN][B_LEN][B_LEN]; 
  int num_active_colors;
  Colorbox *freeboxes; /* used and freed internally */
  Colorbox *usedboxes; /* used and freed internally */
  C_cell **ColorCells; /* used and freed internally */
} quant_table;

#define QUANT_GET_COLOR(qt,r,g,b) (qt->histogram[r>>COLOR_SHIFT][g>>COLOR_SHIFT][b>>COLOR_SHIFT])

quant_table *build_EImage_quantable(unsigned char *eimage, int width, int height, int num_colors);
