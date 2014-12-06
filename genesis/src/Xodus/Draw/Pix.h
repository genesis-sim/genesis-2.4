/* $Id: Pix.h,v 1.1.1.1 2005/06/14 04:38:33 svitak Exp $ */
/*
 * $Log: Pix.h,v $
 * Revision 1.1.1.1  2005/06/14 04:38:33  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.5  2000/06/12 04:28:17  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.4  1994/06/13 23:09:56  bhalla
 * Added XO_AFFECTS_DISPLAY define for pixflags that require redraws
 *
 * Revision 1.3  1994/04/25  18:03:12  bhalla
 * Got rid of a bunch of obsolete pixflags options such as
 * XO_DONT_REDRAW etc
 *
 * Revision 1.2  1994/01/13  19:35:36  bhalla
 * *** empty log message ***
 *
 * Revision 1.2  1994/01/13  19:35:36  bhalla
 * *** empty log message ***
 * */
#ifndef _Pix_h
#define _Pix_h

/************************************************************
  
  Pix Object.

*/


/* Resources:

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
 pixRadius	     Dimension		Dimension	0             

 fg		     Foreground         Pixel		BlackPixel
*/

#define XtNpixX		"pixX"
#define XtNpixY		"pixY"
#define XtNpixZ		"pixZ"
#define XtNpixflags		"pixflags"
/*
#define XtNrefresh	"refresh"
#define XtCRefresh	"Refresh"
*/
#define XtCPixflags		"Pixflags"

typedef struct _PixClassRec*       PixObjectClass;
typedef struct _PixRec*            PixObject;
typedef struct _PixRec*            Pix;

extern WidgetClass pixObjectClass;
extern void XoProjectAndDrawPix();
extern void XoUndrawByClearing();

/*
Flag bits:
        1   visible = decides if it is displayed at all
        2   clickable = sensitive to click events
        3   is_selected = flag indicating it is selected ?
        4   is_icon = flag indicates it is subordinate to another pix
        5   update_all_on_set =
                tell entire draw to update if field in pix is set
        6   update_self_on_set = update own image on set, no global
                redraw
        7   update_with_undraw = When updating self only, first undraw
                self.
        8   update_all_on_step = entire draw updates on PROCESS
        9   update_self_on_step = only itself updates
        10  move_sensitive = will handle move events. Should normally
                be unset since it is expensive.
        11  resizable = flag indicating if can be scaled. This can
                be set and unset any time to freeze a specific size.
                dependent on next 2 settings.
        12  pixelcoords = flag indicating if real or pixel coords to use
        13  pctcoords = flag indicating that coords are % of window sz
                (if pixelcoords && pctcoords, use % of full screen)
        14  hlt1 = Default highlight, inverts pixel
        15  hlt2 = pix specific highlight
        16  hlt3 = pix specific highlight
                (if none of the highlights, then the pix does not hlt)
        17  is_flat = can only display in X-Y coords. Skips fancier
                transforms.
*/
#define XO_VISIBLE_NOT					0x01
#define XO_CLICKABLE_NOT				0x02
#define XO_UPDATE_SELF_ON_SET_NOT		0x020
#define XO_UPDATE_SELF_ON_STEP_NOT		0x0100
#define XO_RESIZABLE_NOT				0x0400
#define XO_HLT1_NOT						0x02000

#define XO_VISIBLE					0x01
#define XO_CLICKABLE				0x02
#define XO_IS_SELECTED				0x04
#define XO_IS_ICON					0x08
#define XO_UPDATE_ALL_ON_SET		0x010
#define XO_UPDATE_SELF_ON_SET		0x020
#define XO_UPDATE_WITH_UNDRAW		0x040
#define XO_UPDATE_ALL_ON_STEP		0x080
#define XO_UPDATE_SELF_ON_STEP		0x0100
#define XO_MOVE_SENSITIVE			0x0200
#define XO_RESIZABLE				0x0400
#define XO_PIXELCOORDS				0x0800
#define XO_PCTCOORDS				0x01000
#define XO_HLT1						0x02000
#define XO_HLT2						0x04000
#define XO_HLT3						0x08000
#define XO_IS_FLAT					0x010000
#define XO_PIXELOFFSET				0x020000
#define XO_PCTOFFSET				0x040000
#define XO_FLUSH_NOT				0x080000
#define XO_TRANSLATE_NOT			0x0100000

#define XO_AFFECTS_DISPLAY			XO_VISIBLE | XO_IS_SELECTED | \
	XO_PIXELCOORDS | XO_PCTCOORDS | XO_HLT1 | XO_HLT2 | XO_HLT3 | \
	XO_IS_FLAT | XO_PIXELOFFSET | XO_PCTOFFSET | XO_TRANSLATE_NOT

#define XO_MAX_SEL_DIST		10000

#endif
