/* $Id: xtext_struct.h,v 1.1 2005/06/17 21:23:51 svitak Exp $ */
/*
 * $Log: xtext_struct.h,v $
 * Revision 1.1  2005/06/17 21:23:51  svitak
 * This file was relocated from a directory with the same name minus the
 * leading underscore. This was done to allow comiling on case-insensitive
 * file systems. Makefile was changed to reflect the relocations.
 *
 * Revision 1.1.1.1  2005/06/14 04:38:32  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.3  2000/06/12 04:28:21  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.2  1994/02/02 20:30:34  bhalla
 * added font, filename and hiddentext field
 * */
#ifndef _xtext_struct_h
#define _xtext_struct_h

/*
 * #include "struct_defs.h"
 */
struct xtext_type {
	XWIDG_TYPE

    char *initialtext; /* user settable text string */
	int editable;
	char *filename;
	char *font;
	/* private data */
	char	*hiddentext; /* contains the actual text string */
};


#endif
