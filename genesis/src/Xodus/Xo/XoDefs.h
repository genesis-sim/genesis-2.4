/* $Id: XoDefs.h,v 1.1.1.1 2005/06/14 04:38:32 svitak Exp $ */
/*
 * $Log: XoDefs.h,v $
 * Revision 1.1.1.1  2005/06/14 04:38:32  svitak
 * Import from snapshot of CalTech CVS tree of June 8, 2005
 *
 * Revision 1.4  2000/06/12 04:28:20  mhucka
 * Removed nested comments within header, to make compilers happy.
 *
 * Revision 1.3  1995/06/02 19:47:32  venkat
 * Resource-converters-clean-up
 *
 * Revision 1.2  1995/03/24  23:46:53  venkat
 * Defined XoDefaultBackground macro
 *
 * Revision 1.1  1994/01/13  18:34:14  bhalla
 * Initial revision
 * */
#ifndef _XoDefs_h
#define _XoDefs_h

#define XoBut1 0x01
#define XoBut2 0x02
#define XoBut3 0x04
#define XoPress 0x08
#define XoRelease 0x010
#define XoMove 0x020
#define XoEnter 0x040
#define XoLeave 0x080
#define XoDouble 0x100
#define XoKeyPress 0x200
#define XoUpdateFields 0x400
#define XoDrag 0x800
#define XoDrop 0x1000
#define XoShift 0x2000
#define XoControl 0x4000
#define XoEventMask	0xfff8
#define XoButtonMask	0x07
#define XoCvtDone(type, value) 				\
 {							\
	if(toVal->addr != NULL){			\
		if(toVal->size < sizeof(type)){		\
		  toVal->size = sizeof(type);		\
		  return ((Boolean)False);		\
		}					\
		else {					\
		  *(type *)(toVal->addr) = (value);		\
		}					\
	}						\
	else {						\
		static type staticval;			\
		staticval = (value);			\
		toVal->addr = (XtPointer) &staticval;	\
	}						\
	toVal->size = sizeof(type);			\
	return ((Boolean)True);				\
 }

#define XO_MINIMUM_MOVE 5	/* minimum # of pixels before widgets will
 						** recognize an event as a move */

#define XO_DOUBLE_CLICK_TIME 400	/* max time in msec between clicks
									** for a double-click event */
#define XtRXoPixel "XoPixel"
#define XoDefaultBackground "LightSteelBlue"

typedef struct {
	int		event; /* bits in this get set according to the event */
	long	key;	/* key that was hit */
	XtPointer ret; /* Special info being returned by widget */
	float	x,y,z; /* coords of event */
} XoEventInfo;

#endif
