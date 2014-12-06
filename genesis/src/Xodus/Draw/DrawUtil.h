/* $Id: DrawUtil.h,v 1.1.1.1 2005/06/14 04:38:33 svitak Exp $ */
/*
** $Log: DrawUtil.h,v $
** Revision 1.1.1.1  2005/06/14 04:38:33  svitak
** Import from snapshot of CalTech CVS tree of June 8, 2005
**
** Revision 1.1  1995/07/17 20:40:09  dhb
** Initial revision
**
*/

#ifndef _DrawUtil_h
#define _DrawUtil_h

extern double XoLength();
extern double XoVLength();
extern void XoVNormalize();
extern void XoUnitCross(
#ifdef ARGS_NEEDED
	float,float,float, /* the x coords */
	float,float,float, /* the y coords */
	float*,float*,float*
#endif
	); /* the returned z coords */

#endif
