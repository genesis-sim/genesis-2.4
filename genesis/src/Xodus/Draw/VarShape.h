/* $Id: VarShape.h,v 1.1.1.1 2005/06/14 04:38:32 svitak Exp $ */
/*
 * $Log: VarShape.h,v $
 * Revision 1.1.1.1  2005/06/14 04:38:32  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.5  2000/06/12 04:28:18  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.4  1997/08/12 18:19:51  dhb
 * Commented extraneous expression after #endif preprocessor
 * directive.  Causes compile problems on some systems.
 *
 * Revision 1.3  1994/12/06  00:17:22  dhb
 * Nov 8 1994 changes from Upi Bhalla
 *
 * Revision 1.2  1994/01/13  19:35:36  bhalla
 * *** empty log message ***
 *
 * Revision 1.2  1994/01/13  19:35:36  bhalla
 * *** empty log message ***
 * */
#ifndef _VarShape_h
#define _VarShape_h

/************************************************************
  
  Var Shape structure.
  Needs to have no dependencies on any X11 data structures

*/


#define XoVarShapeMode 1
#define XoVarGraphMode 2
#define XoVarSurfaceMode 3
#define XoVarContourMode 4

#define	XoVarColorVal	0
#define	XoVarMorphVal	1
#define	XoVarXOffsetVal	2
#define	XoVarYOffsetVal	3
#define	XoVarZOffsetVal	4
#define	XoVarTextVal	5
#define	XoVarTextColorVal	6
#define	XoVarLineThicknessVal	7

#define XoVarNValues 5
#define XoVarNVars 8

typedef struct {
	char	*shape;		/* the shape widget being used */
	int		fgindex;	/* for cases when color table lookups
						** are being used */
	int		textcolorindex;	/* for cases when color table lookups
							** are being used */
	double	value_limit[XoVarNValues];	/* New, for the var widget */
									/* Specify limits for var values */
	float	xox,xoy;	/* tranformed screen coords ox,oy for the */
	float	yox,yoy;	/* x,y,z offsets respectively */
	float	zox,zoy;
} varShape;

#endif /* _VarShape_h */

