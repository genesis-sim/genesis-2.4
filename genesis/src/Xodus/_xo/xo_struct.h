/* $Id */
/*
 * $Log: xo_struct.h,v $
 * Revision 1.1  2005/06/17 21:23:52  svitak
 * This file was relocated from a directory with the same name minus the
 * leading underscore. This was done to allow comiling on case-insensitive
 * file systems. Makefile was changed to reflect the relocations.
 *
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.3  2000/06/12 04:28:22  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.2  1994/02/02 20:26:05  bhalla
 * *** empty log message ***
 * */

#ifndef _xo_struct_h
#define _xo_struct_h

#ifdef FOO
typedef struct {
	char*	g;	/* Genesis field name */
	char*	x;	/* Xodus  field name */
} Gen2Xo;

/* This is also defined in Xo, since the widgets use it */
typedef struct {
	int event; /* bits in this get set according to the event */
	XtPointer	ret; /* Special info being returned by widget */
	float	x,y,z;	/* coords of event */
} XoEventInfo;
#endif

#endif
