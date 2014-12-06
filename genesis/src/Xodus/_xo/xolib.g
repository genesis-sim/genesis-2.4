//genesis

/* $Id */
/* $Log: xolib.g,v $
/* Revision 1.1  2005/06/17 21:23:52  svitak
/* This file was relocated from a directory with the same name minus the
/* leading underscore. This was done to allow comiling on case-insensitive
/* file systems. Makefile was changed to reflect the relocations.
/*
/* Revision 1.1.1.1  2005/06/14 04:38:33  svitak
/* Import from snapshot of CalTech CVS tree of June 8, 2005
/*
/* Revision 1.9  1999/10/17 04:11:33  mhucka
/* Applied additional patches from Upi dated July 1998 that never seem to have
/* made it into DR 2.2.1.
/*
/* Revision 1.8  1998/01/14 22:11:09  venkat
/* Added addfunc statement for xsimplot command
/*
 * Revision 1.7  1997/05/29 00:56:40  venkat
 * Changed alias for xps from set_postscript setpostscript
 *
# Revision 1.6  1995/08/03  01:06:27  venkat
# Removed xon. xinit is now mapped to xinit_defunct() and added
# the xgetstat command
#
# Revision 1.5  1995/08/02  23:00:32  dhb
# Added hashfunc for XgEvetLoop() which appears in showjobs
# listing.
#
# Revision 1.4  1995/01/12  01:04:06  venkat
# Removed call to xon and code_g doesn't report error
#
# Revision 1.3  1994/12/06  00:37:16  dhb
# Nov 8 1994 changes from Upi Bhalla
# */

newclass widget
newclass gadget

// functions visible to the user

hashfunc XgEventLoop
hashfunc XgEventStep

// commands

/* Retained to avoid user calling the X11 xinit command */ 
addfunc xinit 			xinit_defunct

addfunc xraise			xoraise
addfunc xlower			xolower
addfunc xshow			xoshow_on_top
addfunc xshowontop		xoshow_on_top
addfunc xhide			xohide
addfunc xmap			xoshow
addfunc xcolorscale             xocolorscale
addfunc xupdate                 xoupdate
addfunc xps                     xops
addfunc setpostscript          xops // backwards compat hack
addfunc xflushevents            xoflushevents
addfunc xgetstat 		xogetstat int
addfunc xsimplot 		xosimplot

addfunc callfunc 				do_callfunc

/*Eliminating superfluous call since code_g arranges
  for this to be called at startup. So commented out. */
// xon
