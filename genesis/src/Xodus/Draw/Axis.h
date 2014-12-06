/* $Id: Axis.h,v 1.1.1.1 2005/06/14 04:38:33 svitak Exp $ */
/*
 * $Log: Axis.h,v $
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.6  2000/06/12 04:28:16  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.5  1995/03/22 18:54:06  venkat
 * Coalesced the added resource into one
 * XtNunitsjustify resource for backwards compatibility
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
#ifndef _Axis_h
#define _Axis_h

/************************************************************
  
  Axis Object.

*/


/* Resources: inherited from Pix ..

 Name                Class              RepType         Default Value
 ----                -----              -------         -------------
 destroyCallback     Callback           Pointer         NULL
 selectCallback      Callback           Pointer         NULL
 sensitive           Sensitive          Boolean         True

 x                   Position           Position        0
 y                   Position           Position        0
width               Width              Dimension       0
 height              Height             Dimension       0

 pixX	     Position		Position	0
 pixY	     Position		Position	0
 pixZ	     Position		Position	0

 fg		     Foreground         Pixel		BlackPixel

 Own resources
 mode		      Mode		String		"DrawLine"
 npts		      Npts		int		0
 xpts		      Xpts		Pointer		NULL
 ypts		      Xpts		Pointer		NULL
 zpts		      Xpts		Pointer		NULL
*/

#define XtNlinewidth	"linewidth"
#define XtNlinestyle	"linestyle"
#define XtNaxisTextmode		"axisTextmode"
#define XtNtext			"text"
#define XtNtextcolor	"textcolor"
#define XtNtextfont		"textfont"
#define XtNunits		"units"
#define XtNtitle		"title"
#define XtNunitoffset	"unitoffset"
#define XtNtitleoffset	"titleoffset"
#define XtNticklength	"ticklength"
#define XtNtickmode		"tickmode"
#define XtNlabeloffset	"labeloffset"
#define XtNrotatelabels	"rotatelabels"
#define XtNaxx			"axx"
#define XtNaxy			"axy"
#define XtNaxz			"axz"
#define XtNaxmin		"axmin"
#define XtNaxmax		"axmax"
#define XtNaxlength		"axlength"
#define XtNtickx		"tickx"
#define XtNticky		"ticky"
#define XtNtickz		"tickz"
#define XtNlabmin		"labmin"
#define XtNlabmax		"labmax"
#define XtNrefreshCallback		"refreshCallback"

#define XtNunitsjustify		"unitsjustify"
#define XtCUnitsJustify		"UnitsJustify"



#define XtCMode			"Mode"
#define XtCAxisTextmode	"AxisTextmode"
#define XtCLinewidth	"Linewidth"
#define XtCLinestyle	"Linestyle"
#define XtCAxLength		"AxLength"

#define XtRLinestyle	"Linestyle"
#define XtRAxisTextmode		"AxisTextmode"
#define XtRTickmode		"Tickmode"

typedef struct _AxisClassRec*       AxisObjectClass;
typedef struct _AxisRec*            AxisObject;
typedef struct _AxisRec*            Axis;

extern WidgetClass axisObjectClass;
#endif
